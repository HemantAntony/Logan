#pragma once

#include <vector>
#include <memory>
#include "log_record.h"
#include "../sink/sink.h"

class Logger {
public:
  Logger(const std::shared_ptr<Sink>& sink);
  void addSink(const std::shared_ptr<Sink>& sink);
  
  void log(const LogRecord& record);
  void flush();

private:
  std::vector<std::shared_ptr<Sink>> sinks_;
};
