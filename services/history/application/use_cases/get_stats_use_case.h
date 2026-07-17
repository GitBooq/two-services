// application/use_cases/get_stats_use_case.h
#pragma once

#include "i_event_repository.h"
#include "i_get_stats_use_case.h"
#include "stats.h"

#include <memory>

class GetStatsUseCase : public IGetStatsUseCase {
public:
  explicit GetStatsUseCase(std::shared_ptr<IEventRepository> repository);

  shared::Result<dto::Stats> Execute() override;

private:
  std::shared_ptr<IEventRepository> repository_;
};