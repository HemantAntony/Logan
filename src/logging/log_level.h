#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

enum class LogLevel {
  Debug,
  Info,
  Warn,
  Error,
  Fatal
};

inline std::string_view logLevelToString(LogLevel level) {
  switch (level) {
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info:  return "INFO";
    case LogLevel::Warn:  return "WARN";
    case LogLevel::Error: return "ERROR";
    case LogLevel::Fatal: return "FATAL";
  }
  return "UNKNOWN";
}

inline std::optional<LogLevel> stringToLogLevel(const std::string& level) {
  std::string normalised = level;
  std::transform(normalised.begin(), normalised.end(), normalised.begin(), [](unsigned char c) {
    return std::toupper(c);
  });
  static const std::unordered_map<std::string, LogLevel> map = {
    {"DEBUG", LogLevel::Debug},
    {"INFO",  LogLevel::Info},
    {"WARN",  LogLevel::Warn},
    {"ERROR", LogLevel::Error},
    {"FATAL", LogLevel::Fatal}
  };

  auto it = map.find(normalised);
  if (it == map.end()) {
    return std::nullopt;
  }
  return it->second;
}
