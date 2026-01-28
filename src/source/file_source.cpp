#include "file_source.h"
#include "../../include/errors/http_error.h"
#include <fstream>
#include <sstream>
#include <string>

FileSource::FileSource(const std::string& path)
  : file_(path) {
  if (!file_.is_open()) {
    throw HttpError(500, "Failed to open log file");
  }
}

FileSource::~FileSource() {
  file_.close();
}

std::string FileSource::name() const {
  return "FileSource";
}

std::vector<LogRecord> FileSource::query(const QueryParams& params) {
  std::vector<LogRecord> logs;

  file_.clear();
  file_.seekg(0, std::ios::beg);

  std::string line;
  while (std::getline(file_, line)) {
    if (line.empty()) continue;

    std::istringstream iss(line);

    int64_t timestamp;
    std::string service;
    int levelInt;
    std::string message;

    if (!(iss >> timestamp >> service >> levelInt)) {
      continue;                                             // Malformed
    }

    std::getline(iss >> std::ws, message);

    LogLevel level = static_cast<LogLevel>(levelInt);

    if (params.level && params.level.value() != level) {
      continue;
    }
    
    if (params.from && params.from.value() > timestamp) {
      continue;
    }

    if (params.to && params.to.value() < timestamp) {
      continue;
    }

    if (params.service && params.service.value() != service) {
      continue;
    }

    LogRecord log {
      timestamp,
      service,
      level,
      message
    };

    logs.push_back(std::move(log));
  }

  return logs;
}
