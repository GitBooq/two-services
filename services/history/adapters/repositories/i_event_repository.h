// adapters/repositories/i_event_repository.h
#pragma once

#include "event.h"
#include "result.h"
#include "stats.h"

#include <optional>
#include <span>
#include <vector>

class IEventRepository {
public:
  virtual ~IEventRepository() = default;

  virtual shared::Result<> SaveEvents(std::span<const dto::Event> events) = 0;

  virtual shared::Result<std::vector<dto::Event>>
  GetEvents(const std::optional<dto::EventFilter> &filter) = 0;

  virtual shared::Result<dto::Stats> GetStats() = 0;
};