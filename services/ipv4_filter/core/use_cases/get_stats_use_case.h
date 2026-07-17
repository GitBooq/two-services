// services/ipv4_filter/core/use_cases/get_stats_use_case.h
#pragma once

#include "i_stats_provider.h"

#include <memory>
#include <net_logger/net_logger.h>
#include <optional>

/*
    Get Stats from Server
*/
class GetStatsUseCase { // IEventProvider
public:
  struct Result {
    std::optional<dto::Stats> stats;
  };

  explicit GetStatsUseCase(std::shared_ptr<event_service::IStatsProvider> provider);

  Result Execute();

private:
  std::shared_ptr<event_service::IStatsProvider> provider_;
};