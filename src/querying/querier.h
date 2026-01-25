#pragma once

#include <vector>
#include <memory>
#include "query_params.h"
#include "../source/source.h"

class Querier {
public:
  Querier(const std::shared_ptr<Source>& source);
  void addSource(const std::shared_ptr<Source>& source);

  std::vector<LogRecord> query(const QueryParams& params);

private:
  std::vector<std::shared_ptr<Source>> sources_;
};
