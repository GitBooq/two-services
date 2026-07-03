#pragma once

#include <net_logger/net_logger.h>
#include <span>

#include "event.h"
#include "event_saver.h"

class EventBuilder;

class EventPipeline {
public:
  EventPipeline(EventBuilder &builder, event_service::IEventSaver &saver);

  // logEntryBatch is logs processed by filter (ipv4filter in this case)
  void Process(std::span<const net::logger::LogEntry> logEntryBatch);

private:
  EventBuilder &builder_;
  event_service::IEventSaver &saver_;
  std::vector<dto::Event> events_;
};
