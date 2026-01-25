#pragma once

#include "source.h"
#include "../logging/log_record.h"
#include "../querying/query_params.h"
#include <fstream>

class FileSource : public Source {
public:
  explicit FileSource(const std::string& path);
  ~FileSource() override;

  std::string name() const override;

  std::vector<LogRecord> query(const QueryParams& params) override;

private:
  std::ifstream file_;
};
