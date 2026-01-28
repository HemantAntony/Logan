#include "console_sink.h"
#include "../logging/log_record.h"
#include <iostream>

std::string ConsoleSink::name() const {
  return "ConsoleSink";
}

void ConsoleSink::write(const LogRecord& record) {
  std::cout << "Record: " << record.timestamp << " " << record.service << " " << logLevelToString(record.level)
            << " " << record.message << std::endl;
}
