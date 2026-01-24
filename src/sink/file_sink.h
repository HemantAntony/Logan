#pragma once

#include "sink.h"
#include <fstream>

class FileSink : public Sink {
public:
  explicit FileSink(const std::string& path);
  // ~FileSink();

  void write(const LogRecord& record) override;

private:
  std::ofstream file_;
};
