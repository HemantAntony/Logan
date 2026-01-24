#include "file_sink.h"
#include "../logging/log_record.h"
#include <iostream>

FileSink::FileSink(const std::string& path)
  : file_(path, std::ios::app) {}

void FileSink::write(const LogRecord& record) {
  std::cout << "Writing " << (int) record.level << " " << record.timestamp << " " << record.service << " " << record.message << std::endl;
}
