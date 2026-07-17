// core/use_cases/i_get_stats_use_case.h
#pragma once

#include "result.h"
#include "stats.h"

class IGetStatsUseCase {
public:
  virtual ~IGetStatsUseCase() = default;
  virtual shared::Result<dto::Stats> Execute() = 0;
};