#pragma once

#include "event.h"
#include "event_service.pb.h"
#include "stats.h"

#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/util/time_util.h>
#include <stdexcept>

namespace event_service::proto {

inline EventStatus ToProtoEventStatus(dto::Event::Status status) {
  switch (status) {
  case dto::Event::Status::UNSPECIFIED:
    return event_service::UNSPECIFIED;
  case dto::Event::Status::SUCCESS:
    return event_service::SUCCESS;
  case dto::Event::Status::ERROR:
    return event_service::ERROR;
  }
  return event_service::UNSPECIFIED;
}

inline dto::Event::Status
FromProtoEventStatus(event_service::EventStatus status) {
  switch (status) {
  case event_service::UNSPECIFIED:
    return dto::Event::Status::UNSPECIFIED;
  case event_service::SUCCESS:
    return dto::Event::Status::SUCCESS;
  case event_service::ERROR:
    return dto::Event::Status::ERROR;
  case EventStatus_INT_MIN_SENTINEL_DO_NOT_USE_:
  case EventStatus_INT_MAX_SENTINEL_DO_NOT_USE_:
    break;
  }
  return dto::Event::Status::UNSPECIFIED;
}

// TODO: don't like it
inline FilterDecision
ToProtoFilterDecision(const std::string &filter_decision) {
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
inline std::string FromProtoFilterDecision(const FilterDecision &decision) {
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

inline google::protobuf::Timestamp
ToTimestampFromStr(const std::string &timestamp_utc_str) {
  google::protobuf::Timestamp timestamp;

  if (!google::protobuf::util::TimeUtil::FromString(timestamp_utc_str,
                                                    &timestamp)) {
    throw std::invalid_argument("Bad string format.");
  }

  return timestamp;
}

inline std::string
FromTimestampToStr(const google::protobuf::Timestamp &timestamp) {
  return google::protobuf::util::TimeUtil::ToString(timestamp);
}

inline dto::Event ToDtoEvent(const event_service::Event &proto_event) {
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

inline dto::Stats ToDtoStats(const event_service::Stats &proto_stats) {
  dto::Stats stats;
  stats.events_total = proto_stats.total_events();
  stats.events_success = proto_stats.success_events();
  stats.events_error = proto_stats.error_events();

  return stats;
}

} // namespace event_service::proto