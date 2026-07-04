/*
Starts processing the input stream using the net-log-filter lib.

Creates a filter based on the configuration, calls ProcessStream(), and
passes each received LogEntry batch to the EventPipeline.
*/

#pragma once

#include <net_logger/net_logger.h>

struct Config;
class EventPipeline;

class FilterRunner {
public:
  FilterRunner(const Config &config, EventPipeline &pipeline);

  void Run(std::istream &input);

private:
  EventPipeline &pipeline_;
  net::logger::CompositeFilter filter_;
};