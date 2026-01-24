#pragma once

#include "sink.h"
#include <fstream>

class FileSink : public Sink {
public:
  explicit FileSink(const std::string& path);
  ~FileSink() override;

  const std::string name() const override;

  void write(const LogRecord& record) override;
  void flush() override;

private:
  static constexpr int BUFFER_LIMIT = 100;

  std::ofstream file_;
  int recordCount_ = 0;
};
