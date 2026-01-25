#pragma once

#include "../querying/query_params.h"
#include "../logging/log_record.h"
#include <string>
#include <vector>

class Source {
public:
  virtual ~Source() = default;
  
  virtual std::string name() const = 0;

  virtual std::vector<LogRecord> query(const QueryParams& record) = 0;
};
