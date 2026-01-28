#include "file_sink.h"
#include "../logging/log_record.h"
#include "../../include/errors/http_error.h"
#include <chrono>
#include <fstream>
#include <mutex>
#include <thread>

FileSink::FileSink(const std::string& path)
  : running_(true), file_(path, std::ios::app) {
  worker_ = std::thread(&FileSink::loop, this); // Are all of these needed
  if (!file_.is_open()) {
    throw HttpError(500, "Failed to open log file");
  }
}

FileSink::~FileSink() {
  shutdown();
  file_.close();
  worker_.join();
}

std::string FileSink::name() const {
  return "FileSink";
}

void FileSink::shutdown() {
  running_.store(false);
  cv_.notify_one();
}

void FileSink::write(const LogRecord& log) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    buffer_.push_back(log);
  }
  cv_.notify_one();
}

void FileSink::loop() {
  while (running_ || !buffer_.empty()) {
    std::vector<LogRecord> local;

    {
      std::unique_lock<std::mutex> lock(mutex_);

      cv_.wait_for(
        lock,
        std::chrono::seconds(1),
        [&] {
          return !buffer_.empty() || !running_;
        }
      );

      local.swap(buffer_);
    }

    for (const auto& log : local) {
      file_ << log.timestamp << " " << log.service << " " << static_cast<int>(log.level) << " " << log.message << "\n";
    }

    file_.flush();
  }
}

