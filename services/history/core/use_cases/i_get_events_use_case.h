// core/use_cases/i_get_events_use_case.h
#pragma once

#include "event.h"

#include <optional>
#include <vector>

class IGetEventsUseCase {
public:
  virtual ~IGetEventsUseCase() = default;
  virtual std::vector<dto::Event>
  Execute(const std::optional<dto::EventFilter> &filter) = 0;
};