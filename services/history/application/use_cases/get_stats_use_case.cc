// // application/use_cases/get_stats_use_case.cc

#include "get_stats_use_case.h"

#include <utility>

GetStatsUseCase::GetStatsUseCase(std::shared_ptr<IEventRepository> repository)
    : repository_(std::move(repository)) {}

shared::Result<dto::Stats> GetStatsUseCase::Execute() {
  return repository_->GetStats();
}