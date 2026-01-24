#pragma once

#include "../logging/log_record.h"

class Sink {
public:
  virtual ~Sink() = default;

  virtual const std::string name() const = 0;
  
  virtual void write(const LogRecord& record) = 0;
  virtual void flush() = 0;
};
