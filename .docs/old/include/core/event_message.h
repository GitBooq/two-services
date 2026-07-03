#pragma once

#include "event.h"
#include "log_entry.h"

namespace core {

struct EventMessage {
  Event event;
  LogEntry payload;
};

} // namespace core