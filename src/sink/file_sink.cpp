#include "file_sink.h"
#include "../logging/log_record.h"
#include "../../include/errors/http_error.h"
#include <iostream>

FileSink::FileSink(const std::string& path)
  : file_(path, std::ios::app) {
  if (!file_.is_open()) {
    throw HttpError(500, "Failed to open log file");
  }
}

FileSink::~FileSink() {
  file_.close();
}

void FileSink::write(const LogRecord& record) {
  file_ << record.timestamp << " " << record.service << " " << static_cast<int>(record.level) << " " << record.message << "\n";
  if (++recordCount_ >= BUFFER_LIMIT) {
    file_.flush();
    recordCount_ = 0;
  }
}

void FileSink::flush() {
  file_.flush();
  recordCount_ = 0;
}
