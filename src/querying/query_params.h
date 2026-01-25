#pragma once

#include "../logging/log_level.h"
#include <string>
#include <optional>

struct QueryParams {
  std::optional<LogLevel> level;
  std::optional<std::string> service;
  std::optional<int64_t> from;
  std::optional<int64_t> to;
};
