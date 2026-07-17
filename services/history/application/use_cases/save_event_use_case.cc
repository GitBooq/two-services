// application/use_cases/save_event_use_case.cc
#include "save_event_use_case.h"

#include <utility>

SaveEventUseCase::SaveEventUseCase(std::shared_ptr<IEventRepository> repository)
    : repository_(std::move(repository)) {}

shared::Result<>
SaveEventUseCase::Execute(const std::vector<dto::Event> &events) {
  return repository_->SaveEvents(events);
}