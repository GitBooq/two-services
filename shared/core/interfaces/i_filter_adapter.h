// shared/core/interfaces/i_filter_adapter.h
#pragma once

#include <istream>
#include <net_logger/net_logger.h>
#include <net_logger/types.h>
#include <vector>

class IFilterAdapter {
public:
  virtual ~IFilterAdapter() = default;

  virtual std::vector<net::logger::LogEntry> Process(std::istream &input) = 0;
};