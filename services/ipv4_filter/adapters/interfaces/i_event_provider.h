// core/interfaces/i_event_provider.h
/*
    Events provider interface

GetEvents' Filters
  Support:
      from
      to
      limit
      offset
      status
      source_service
*/
#pragma once

#include "event.h"

#include <vector>

namespace event_service {

class IEventProvider {
public:
  virtual ~IEventProvider() = default;

  // NVI
  [[nodiscard]] std::optional<std::vector<dto::Event>>
  GetEvents(const dto::EventFilter &filter = {}) const {
    return GetEventsImpl(filter);
  }

private:
  [[nodiscard]] virtual std::optional<std::vector<dto::Event>>
  GetEventsImpl(const dto::EventFilter &filter) const = 0;
};
} // namespace event_service