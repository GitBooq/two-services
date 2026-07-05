#include "utils.h"

namespace utils::builder {

std::string FormatTimestamp(const utils::builder::TimePoint &timestamp) {
  auto time_t = std::chrono::system_clock::to_time_t(timestamp);
  auto *tm = std::localtime(&time_t);

  std::ostringstream oss;
  oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S"); // 2026-05-10 15:30:01
  return oss.str();
}

std::optional<std::string>
FilterRejectReasonToStr(net::logger::RejectReason reason) {
  switch (reason) {
  case net::logger::RejectReason::InvalidFormat:
    return "Invalid Format";
  case net::logger::RejectReason::InvalidIPAddress:
    return "InvalidIPAddress";
  case net::logger::RejectReason::FilterRejected:
    return "FilterRejected";
  case net::logger::RejectReason::None:
  default:
    return std::nullopt;
  }
}

std::string GetFilterDecision(net::logger::RejectReason reason) {
  switch (reason) {
  case net::logger::RejectReason::FilterRejected:
    return "rejected";
  case net::logger::RejectReason::None:
    return "accepted";
  default:
    return "";
  }
}

// Convert from network byte order addr
// to string representation (xxx.xxx.xxx.xxx)
std::string IPv4AddressToString(uint32_t addr) {
  uint8_t octet1 = (addr >> 24) & 0xFF;
  uint8_t octet2 = (addr >> 16) & 0xFF;
  uint8_t octet3 = (addr >> 8) & 0xFF;
  uint8_t octet4 = addr & 0xFF;

  return std::to_string(octet1) + "." + std::to_string(octet2) + "." +
         std::to_string(octet3) + "." + std::to_string(octet4);
}

} // namespace utils::builder

namespace event_service {

EventStatus ToProtoEventStatus(dto::Event::Status status) {
  switch (status) {
  case dto::Event::Status::UNSPECIFIED:
    return UNSPECIFIED;
  case dto::Event::Status::SUCCESS:
    return SUCCESS;
  case dto::Event::Status::ERROR:
    return ERROR;
  }
  return UNSPECIFIED;
}

dto::Event::Status FromProtoEventStatus(EventStatus status) {
  switch (status) {
  case UNSPECIFIED:
    return dto::Event::Status::UNSPECIFIED;
  case SUCCESS:
    return dto::Event::Status::SUCCESS;
  case ERROR:
    return dto::Event::Status::ERROR;
  case EventStatus_INT_MIN_SENTINEL_DO_NOT_USE_:
  case EventStatus_INT_MAX_SENTINEL_DO_NOT_USE_:
    break;
  }
  return dto::Event::Status::UNSPECIFIED;
}

// TODO: don't like it
FilterDecision ToProtoFilterDecision(const std::string &filter_decision) {
  if (filter_decision == "accepted") {
    return FilterDecision::ACCEPTED;
  }
  if (filter_decision == "rejected") {
    return FilterDecision::REJECTED;
  }
  return FilterDecision::FILTER_DECISION_UNSPECIFIED;
}

/* Payload.filter_decision is std::string
  TODO: maybe return Payload w/ filter_decision field set only?
  TODO: rework dto structures if time
*/
std::string FromProtoFilterDecision(const FilterDecision &decision) {
  switch (decision) {
  case ACCEPTED:
    return "accepted";
  case REJECTED:
    return "rejected";
  case FILTER_DECISION_UNSPECIFIED:
  case FilterDecision_INT_MIN_SENTINEL_DO_NOT_USE_:
  case FilterDecision_INT_MAX_SENTINEL_DO_NOT_USE_:
    break;
  }
  return "";
}

google::protobuf::Timestamp
ToTimestampFromStr(const std::string &timestamp_utc_str) {
  google::protobuf::Timestamp timestamp;

  if (!google::protobuf::util::TimeUtil::FromString(timestamp_utc_str,
                                                    &timestamp)) {
    throw std::invalid_argument("Bad string format.");
  }

  return timestamp;
}

std::string FromTimestampToStr(const google::protobuf::Timestamp &timestamp) {
  return google::protobuf::util::TimeUtil::ToString(timestamp);
}

dto::Event ToDtoEvent(const event_service::Event &proto_event) {
  dto::Event event;
  event.source_service = proto_event.source_service();
  event.timestamp_utc = proto_event.timestamp_utc();
  event.status = FromProtoEventStatus(proto_event.status());

  auto &payload = event.payload;
  const auto &proto_payload = proto_event.payload();
  payload.raw_line = proto_payload.raw_line();
  if (proto_payload.has_parsed_ip()) {
    payload.parsed_ip = proto_payload.parsed_ip();
  }
  payload.filter_decision =
      FromProtoFilterDecision(proto_payload.filter_decision());
  if (proto_payload.has_reject_reason()) {
    payload.reject_reason = proto_payload.reject_reason();
  }

  return event;
}

dto::Stats ToDtoStats(const event_service::Stats &proto_stats) {
  dto::Stats stats;
  stats.events_total = proto_stats.total_events();
  stats.events_success = proto_stats.success_events();
  stats.events_error = proto_stats.error_events();

  return stats;
}

/*message EventFilter {
    optional string source_service = 1;
    optional EventStatus status = 2;
    optional uint32 limit = 3;
    optional google.protobuf.Timestamp from = 4;
    optional google.protobuf.Timestamp to = 5;
    optional uint32 offset = 6;
}
    struct EventFilter {
  std::optional<std::string> source_service;
  std::optional<Event::Status> status;
  std::optional<std::size_t> limit;
  std::optional<std::string> from;
  std::optional<std::string> to;
  std::optional<std::size_t> offset;
};*/
std::optional<dto::EventFilter>
FromProtoEventFilter(const std::optional<EventFilter> &proto_event_filter) {
  if (!proto_event_filter.has_value()) {
    return std::nullopt;
  }

  dto::EventFilter dto_event_filter;
  if (proto_event_filter->has_source_service()) {
    dto_event_filter.source_service = proto_event_filter->source_service();
  }
  if (proto_event_filter->has_status()) {
    dto_event_filter.status =
        FromProtoEventStatus(proto_event_filter->status());
  }
  if (proto_event_filter->has_limit()) {
    dto_event_filter.limit = proto_event_filter->limit();
  }
  if (proto_event_filter->has_from()) {
    dto_event_filter.from = FromTimestampToStr(proto_event_filter->from());
  }
  if (proto_event_filter->has_to()) {
    dto_event_filter.to = FromTimestampToStr(proto_event_filter->to());
  }
  if (proto_event_filter->has_offset()) {
    dto_event_filter.offset = proto_event_filter->offset();
  }

  return dto_event_filter;
}

std::optional<EventFilter>
ToProtoEventFilter(const std::optional<dto::EventFilter> &dto_event_filter) {
  if (!dto_event_filter.has_value()) {
    return std::nullopt;
  }

  EventFilter proto_event_filter;
  if (dto_event_filter->source_service.has_value()) {
    proto_event_filter.set_source_service(
        dto_event_filter->source_service.value());
  }
  if (dto_event_filter->status.has_value()) {
    proto_event_filter.set_status(
        ToProtoEventStatus(dto_event_filter->status.value()));
  }
  if (dto_event_filter->limit.has_value()) {
    proto_event_filter.set_limit(
        static_cast<uint64_t>(dto_event_filter->limit.value()));
  }
  if (dto_event_filter->from.has_value()) {
    *proto_event_filter.mutable_from() =
        ToTimestampFromStr(dto_event_filter->from.value());
  }
  if (dto_event_filter->to.has_value()) {
    *proto_event_filter.mutable_to() =
        ToTimestampFromStr(dto_event_filter->to.value());
  }
  if (dto_event_filter->offset.has_value()) {
    proto_event_filter.set_offset(
        static_cast<uint32_t>(dto_event_filter->offset.value()));
  }

  return proto_event_filter;
}

} // namespace event_service