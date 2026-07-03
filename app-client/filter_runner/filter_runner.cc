#include "filter_runner.h"
#include "config.h"
#include "event_pipeline.h"

FilterRunner::FilterRunner(const Config &config, EventPipeline &pipeline)
    : pipeline_(pipeline) {
  filter_ = CreateFilter(config.filters);
}

void FilterRunner::Run(std::istream &input) {
  using net::logger::LogEntry;
  net::logger::ProcessStream(
      input, filter_,
      [this](std::span<const LogEntry> batch) { pipeline_.Process(batch); });
}
