/*
Stats model
*/
#pragma once

#include <cstddef>

namespace dto {

struct Stats {
  std::size_t events_total;
  std::size_t events_success;
  std::size_t events_error;
};

} // namespace dto