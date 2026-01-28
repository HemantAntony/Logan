#pragma once

#include "sink.h"

class ConsoleSink : public Sink {
public:
  ~ConsoleSink() override = default;

  std::string name() const override;

  void write(const LogRecord& record) override;
};
