#pragma once

#include "../logging/log_record.h"
#include <string>

class Sink {
public:
  virtual ~Sink() = default;

  virtual std::string name() const = 0;
  
  virtual void write(const LogRecord& record) = 0;
};
