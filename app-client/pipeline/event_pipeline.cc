#include "event_pipeline.h"
#include "event_builder.h"
#include "event_saver.h"

EventPipeline::EventPipeline(EventBuilder &builder,
                             event_service::IEventSaver &saver)
    : builder_(builder), saver_(saver) {}

/* works w/ logs batch
  transform to events
  pass to eventSaver
*/
void EventPipeline::Process(
    std::span<const net::logger::LogEntry> logEntryBatch) {
  events_.clear();
  events_.reserve(logEntryBatch.size());

  for (auto &&logEntry : logEntryBatch) {
    auto event = builder_.Build(logEntry);
    events_.push_back(std::move(event));
  }

  saver_.SaveBatch(events_);

  // log error/success
}
