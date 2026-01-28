#pragma once

#include "../logging/log_record.h"
#include "sink.h"
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <thread>
#include <vector>

class FileSink : public Sink {
public:
  explicit FileSink(const std::string& path);
  ~FileSink() override;

  std::string name() const override;
  
  void shutdown();
  void write(const LogRecord& log) override;

private:
  void loop();

  std::vector<LogRecord> buffer_;
  std::ofstream file_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::thread worker_;
  std::atomic<bool> running_;
};
