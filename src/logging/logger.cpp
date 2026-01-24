#include "logger.h"

Logger::Logger(const std::shared_ptr<Sink>& sink)
  : sinks_{sink} {}

void Logger::addSink(const std::shared_ptr<Sink>& sink) {
  sinks_.push_back(sink);
}

void Logger::log(const LogRecord& record) {
  for (const auto& sink : sinks_) {
    sink->write(record);
  }
}
