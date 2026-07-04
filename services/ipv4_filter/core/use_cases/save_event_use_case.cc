// services/ipv4_filter/core/use_cases/save_event_use_case.cc

#include "save_event_use_case.h"
#include "event.h"

#include <utility>

SaveEventUseCase::SaveEventUseCase(
    std::shared_ptr<IEventBuilder> builder,
    std::shared_ptr<event_service::IEventSaver> saver)
    : builder_(std::move(builder)), saver_(std::move(saver)) {}

SaveEventUseCase::Result SaveEventUseCase::Execute(const Request &request) {
  Result result;

  std::vector<dto::Event> events;

  net::logger::ProcessStream(
      *request.input, request.filter,
      [this, &events, &result](std::span<const net::logger::LogEntry> batch) {
        for (const auto &entry : batch) {
          events.push_back(builder_->Build(entry));
          ++result.logs_processed;
        }
      });

  if (events.empty()) {
    return result;
  }

  result.events_created = static_cast<int>(events.size());

  auto save_result = saver_->SaveBatch(events);
  result.events_sent = save_result.sent_count;
  result.errors = save_result.errors;

  return result;
}
