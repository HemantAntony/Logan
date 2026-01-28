#include "console_sink.h"
#include <iostream>

std::string ConsoleSink::name() const {
  return "ConsoleSink";
}

void ConsoleSink::write(const LogRecord& record) {
  std::cout << "Record: " << record.timestamp << " " << record.service << " " << (int) record.level
            << " " << record.message << std::endl;
}
