// services/ipv4_filter/core/use_cases/get_events_use_case.сс

#include "get_events_use_case.h"

#include <utility>

GetEventsUseCase::GetEventsUseCase(
    std::shared_ptr<event_service::IEventProvider> provider)
    : provider_(std::move(provider)) {}

GetEventsUseCase::Result GetEventsUseCase::Execute(const Request &request) {
  auto events = provider_->GetEvents(request.filter);
  return Result{std::move(events)};
}
