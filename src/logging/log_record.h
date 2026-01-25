#pragma once

#include "log_level.h"
#include <string>

struct LogRecord {
  int64_t timestamp;
  std::string service;
  LogLevel level;
  std::string message;
};
