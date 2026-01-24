#include <iostream>
#include <memory>
#include "../third_party/httplib.h"
#include "../third_party/nlohmann/json.hpp"
#include "../include/errors/http_error.h"
#include "logging/log_level.h"
#include "logging/log_record.h"
#include "logging/logger.h"
#include "sink/file_sink.h"

using json = nlohmann::json;

int main() {
	httplib::Server server;

	auto fileSink = std::make_shared<FileSink>("log.txt");
	Logger logger(fileSink);

	server.Get("/health", [](const httplib::Request&, httplib::Response& res) {
		json health {
			{"status", "ok"}
		};
		res.set_content(health.dump(), "application/json");
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

	std::cout << "Starting server on http://localhost:8080\n";
	server.listen("0.0.0.0", 8080);
	return 0;
}
