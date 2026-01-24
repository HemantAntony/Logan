#pragma once

#include "log_level.h"
#include <string>

struct LogRecord {
  LogLevel level;
  int64_t timestamp;
  std::string service;
  std::string message;
};
