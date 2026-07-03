#pragma once

#include <string>

namespace core {

struct Event {
  std::string source_service;
  std::string timestamp_utc;
  enum class Status { SUCCESS, ERROR } status;
};

} // namespace core