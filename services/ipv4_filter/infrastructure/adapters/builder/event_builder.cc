#include "event_builder.h"
#include "details/ip_v4_address.h"
#include "utils.h"


dto::Event EventBuilder::Build(const net::logger::LogEntry &log_entry) {
  dto::Event event;
  event.source_service = src_service_name_;
  event.timestamp_utc = builder_utils::formatTimestamp(std::chrono::system_clock::now());
  event.status = log_entry.parsed_ip.has_value() ? dto::Event::Status::SUCCESS
                                                 : dto::Event::Status::ERROR;

  dto::Payload payload;
  payload.raw_line = log_entry.raw_line;
  payload.parsed_ip = builder_utils::IPv4AddressToString(log_entry.parsed_ip->ToUint32());
  payload.filter_decision = builder_utils::getFilterDecision(log_entry.reason);
  payload.reject_reason = builder_utils::filterRejectReasonToStr(log_entry.reason);

  event.payload = std::move(payload);

  return event;
}
