#include "querier.h"
#include "../source/source.h"
#include "../logging/log_record.h"
#include <memory>
#include <vector>

Querier::Querier(const std::shared_ptr<Source>& source)
  : sources_{source} {}

void Querier::addSource(const std::shared_ptr<Source>& source) {
  sources_.push_back(source);
}

std::vector<LogRecord> Querier::query(const QueryParams& params) {
  std::vector<LogRecord> result;
  for (const auto& source : sources_) {
    auto partial = source->query(params);
    result.insert(result.end(), std::make_move_iterator(partial.begin()), std::make_move_iterator(partial.end()));
  }
  return result;
}
