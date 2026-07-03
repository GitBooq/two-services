#pragma once

#include <string>

namespace core {

struct LogEntry {
  std::string raw_line;
  std::string parsed_ip;
  std::string filter_decision;
};

} // namespace core
