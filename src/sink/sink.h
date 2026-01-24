#pragma once

#include "../logging/log_record.h"

class Sink {
public:
  virtual ~Sink() = default;
  virtual void write(const LogRecord& record) = 0;
};
