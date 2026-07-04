// core/interfaces/i_stats_provider.h
/*
    Stats provider interface
*/
#pragma once

#include "stats.h"

#include <optional>

namespace event_service {

class IStatsProvider {
public:
  virtual ~IStatsProvider() = default;

  [[nodiscard]] virtual std::optional<dto::Stats> GetStats() const = 0;
};
} // namespace event_service