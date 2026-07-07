// application/use_cases/get_events_use_case.cc

#include "get_events_use_case.h"

GetEventsUseCase::GetEventsUseCase(std::shared_ptr<IEventRepository> repository)
    : repository_(std::move(repository)) {}

shared::Result<std::vector<dto::Event>>
GetEventsUseCase::Execute(const std::optional<dto::EventFilter> &filter) {
  return repository_->GetEvents(filter);
}