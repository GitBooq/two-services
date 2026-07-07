// core/interfaces/i_event_saver.h
/*
    Events saver interface
*/
#pragma once

#include "event.h"

#include <span>

namespace event_service {

class IEventSaver {
public:
  virtual ~IEventSaver() noexcept = default;

  [[nodiscard]] virtual bool Save(const dto::Event &) const = 0;
  [[nodiscard]] virtual bool
  SaveBatch(std::span<const dto::Event> events) const = 0;
};
} // namespace event_service