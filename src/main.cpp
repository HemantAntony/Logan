#include <iostream>
#include "../third_party/httplib.h"
#include "../third_party/nlohmann/json.hpp"
#include "../include/errors/http_error.h"

using json = nlohmann::json;

int main() {
	httplib::Server server;

	server.Get("/health", [](const httplib::Request&, httplib::Response& res) {
		json health {
			{"status", "ok"}
		};
		res.set_content(health.dump(), "application/json");
	});

	server.Post("/log", [](const httplib::Request& req, httplib::Response& res) {
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
			}

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
