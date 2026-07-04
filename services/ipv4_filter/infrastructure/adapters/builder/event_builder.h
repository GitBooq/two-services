// event/event_builder.h
/*
Transforms LogEntry(from net_log_filter lib) to Event
*/
#pragma once

#include <net_logger/net_logger.h>
#include <string_view>

#include "event.h"
#include "core/interfaces/i_event_builder.h"

class EventBuilder : public IEventBuilder {
public:
  explicit EventBuilder(std::string_view src_service_name)
      : src_service_name_(src_service_name) {}

  dto::Event Build(const net::logger::LogEntry &log_entry);

private:
  std::string src_service_name_;
};