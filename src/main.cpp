#include <iostream>
#include <memory>
#include <csignal>
#include <optional>
#include "../third_party/httplib.h"
#include "../third_party/nlohmann/json.hpp"
#include "../include/errors/http_error.h"
#include "logging/log_level.h"
#include "logging/log_record.h"
#include "logging/logger.h"
#include "sink/file_sink.h"
#include "sink/console_sink.h"
#include "querying/query_params.h"
#include "querying/querier.h"
#include "source/file_source.h"

#ifndef BUILD_DIR 							// To remove warnings. BUILD_DIR is set from CMakeLists.txt
#define BUILD_DIR "./build"
#endif

using json = nlohmann::json;

std::atomic<bool> shutdownRequested{false};

void handleSignal(int sig) {
	shutdownRequested.store(true, std::memory_order_relaxed);
}

void installSignalHandlers() {
	std::signal(SIGTERM, handleSignal);
	std::signal(SIGINT, handleSignal);
	std::signal(SIGQUIT, handleSignal);
}

int main() {
	installSignalHandlers();
	
	httplib::Server server;

	auto fileSink = std::make_shared<FileSink>(std::string(BUILD_DIR) + "/log.txt");
	auto consoleSink = std::make_shared<ConsoleSink>();
	Logger logger(fileSink);
	logger.addSink(consoleSink);

	auto fileSource = std::make_shared<FileSource>(std::string(BUILD_DIR) + "/log.txt");
	Querier querier(fileSource);

	server.Get("/health", [](const httplib::Request&, httplib::Response& res) {
		json health {
			{"status", "ok"}
		};
		res.set_content(health.dump(), "application/json");
	});

	server.Get("/log", [&querier](const httplib::Request& req, httplib::Response& res) {
		try {
			QueryParams params;

			if (req.has_param("service")) {
				params.service = req.get_param_value("service");
			}

			if (req.has_param("level")) {
				auto lvl = req.get_param_value("level");
				std::transform(lvl.begin(), lvl.end(), lvl.begin(), ::tolower);

				if (lvl == "info") params.level = LogLevel::Info;
				else if (lvl == "debug") params.level = LogLevel::Debug;
				else if (lvl == "warn") params.level = LogLevel::Warn;
				else if (lvl == "error") params.level = LogLevel::Error;
				else if (lvl == "fatal") params.level = LogLevel::Fatal;
				else throw HttpError(400, "Invalid level");
			}

			if (req.has_param("from")) {
				params.from = std::stoll(req.get_param_value("from"));
			}

			if (req.has_param("to")) {
				params.to = std::stoll(req.get_param_value("to"));
			}

			if (params.from && params.to && params.from > params.to) {
				throw HttpError(400, "\"From\" parameter should be less than \"To\" parameter");
			}

			std::vector<LogRecord> logs = querier.query(params);

			json response;
			response["success"] = true;
			response["count"] = logs.size();
			response["logs"] = json::array();

			for (const auto& log : logs) {
				response["logs"].push_back({
					{"timestamp", log.timestamp},
					{"service", log.service},
					{"level", logLevelToString(log.level)},
					{"message", log.message}
				});
			}

			res.set_content(response.dump(), "application/json");
		} catch (const HttpError& e) {
			res.status = e.status();
			res.set_content(
				json {
					{ "success", "false" },
					{ "error", e.what() }
				}.dump(),
				"application/json"
			);
		} catch (const std::exception& e) {
			res.status = 500;
			res.set_content(
				json{{"error", "Internal server error"}}.dump(),
				"application/json"
			);
		}
	});

	server.Post("/log", [&logger](const httplib::Request& req, httplib::Response& res) {
		try {
			if (req.get_header_value("Content-Type") != "application/json") {
				throw HttpError(415, "Expected application/json");
			}

			json body = json::parse(req.body);
			
			if (!body.contains("service")) {
				throw HttpError(400, "Service is required, but it is missing in body");
			} else if (!body.contains("level")) {
				throw HttpError(400, "Level is required, but it is missing in body");
			} else if (!body.contains("message")) {
				throw HttpError(400, "Message is required, but it is missing in body");
			} else if (!body.contains("timestamp")) {
				throw HttpError(400, "Timestamp is required, but it is missing in body");
			} else if (!body["timestamp"].is_number_integer()) {
				throw HttpError(400, "Timestamp should be an integer");
			}

			LogRecord record;
			record.timestamp = body["timestamp"].get<int64_t>();
			record.service = body["service"];
			record.message = body["message"];
			std::string level = body["level"];
			std::transform(level.begin(), level.end(), level.begin(), [](unsigned char c) {
				return std::tolower(c);
			});
			
			if (level == "info") {
				record.level = LogLevel::Info;
			} else if (level == "debug") {
				record.level = LogLevel::Debug;
			} else if (level == "warn") {
				record.level = LogLevel::Warn;
			} else if (level == "error") {
				record.level = LogLevel::Error;
			} else if (level == "fatal") {
				record.level = LogLevel::Fatal;
			} else {
				throw HttpError(400, "Invalid level");
			}

			logger.log(record);

		} catch (const HttpError& e) {
			res.status = e.status();
			res.set_content(
				json {
					{ "success", "false" },
					{ "error", e.what() }
				}.dump(),
				"application/json"
			);
		} catch (const std::exception& e) {
			res.status = 500;
			res.set_content(
				json{{"error", "Internal server error"}}.dump(),
				"application/json"
			);
		}
	});

	std::thread serverThread([&server]() {
		std::cout << "Starting server on http://localhost:8080\n";
		server.listen("0.0.0.0", 8080);
	});

	while (!shutdownRequested.load(std::memory_order_relaxed)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	server.stop();
	serverThread.join();

	logger.flush();

	return 0;
}
