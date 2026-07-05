// core/use_cases/i_get_stats_use_case.h
#pragma once

#include "stats.h"

class IGetStatsUseCase {
public:
    virtual ~IGetStatsUseCase() = default;
    virtual dto::Stats Execute() = 0;
};