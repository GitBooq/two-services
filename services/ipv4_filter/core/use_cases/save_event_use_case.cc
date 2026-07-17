// services/ipv4_filter/core/use_cases/save_event_use_case.cc

#include "save_event_use_case.h"
#include "event.h"

#include <utility>

SaveEventUseCase::SaveEventUseCase(
    std::shared_ptr<IEventBuilder> builder,
    std::shared_ptr<event_service::IEventSaver> saver)
    : builder_(std::move(builder)), saver_(std::move(saver)) {}

bool SaveEventUseCase::Execute(const Request &request, std::istream& input) {
  std::vector<dto::Event> events;

  net::logger::ProcessStream(
      input, request.filter,
      [this, &events](std::span<const net::logger::LogEntry> batch) {
        for (const auto &entry : batch) {
          const auto &event = builder_->Build(entry);
          // skip events from empty logs (raw_line == "")
          if (event.has_value()) {
            events.push_back(*event);
          }
        }
      });

  return saver_->SaveBatch(events);
}
