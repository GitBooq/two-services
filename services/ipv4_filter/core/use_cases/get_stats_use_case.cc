// services/ipv4_filter/core/use_cases/get_stats_use_case.cc
#include "get_stats_use_case.h"

GetStatsUseCase::GetStatsUseCase(
    std::shared_ptr<event_service::IStatsProvider> provider)
    : provider_(std::move(provider)) {}

GetStatsUseCase::Result GetStatsUseCase::Execute() {
  auto stats = provider_->GetStats(); // trivially copyable
  return Result{stats};
}
