// core/interfaces/i_event_builder.h
#pragma once

#include "event.h"

#include "net_logger/types.h" // for LogEntry

class IEventBuilder {
public:
  virtual ~IEventBuilder() = default;

  // Build one Event from one LogEntry
  virtual std::optional<dto::Event>
  Build(const net::logger::LogEntry &log_entry) = 0;
};