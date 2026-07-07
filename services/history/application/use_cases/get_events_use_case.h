// application/use_cases/get_events_use_case.h
#pragma once

#include "event.h"
#include "i_event_repository.h"
#include "i_get_events_use_case.h"

#include <memory>
#include <optional>
#include <vector>

class GetEventsUseCase : public IGetEventsUseCase {
public:
  explicit GetEventsUseCase(std::shared_ptr<IEventRepository> repository);

  shared::Result<std::vector<dto::Event>>
  Execute(const std::optional<dto::EventFilter> &filter) override;

private:
  std::shared_ptr<IEventRepository> repository_;
};