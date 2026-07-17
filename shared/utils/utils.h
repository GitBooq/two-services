#include <chrono>
#include <cstdint>

#include "event.h"
#include "event_service.pb.h"
#include "net_logger/types.h"
#include "stats.h"

#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/util/time_util.h>

namespace utils::builder {

using TimePoint = std::chrono::system_clock::time_point;
std::string FormatTimestamp(const TimePoint &timestamp);

std::optional<std::string>
FilterRejectReasonToStr(net::logger::RejectReason reason);

std::string GetFilterDecision(net::logger::RejectReason reason);

std::string IPv4AddressToString(uint32_t addr);

} // namespace utils::builder

namespace event_service {

EventStatus ToProtoEventStatus(dto::Event::Status status);

dto::Event::Status FromProtoEventStatus(EventStatus status);

FilterDecision ToProtoFilterDecision(const std::string &filter_decision);

std::string FromProtoFilterDecision(const FilterDecision &decision);

google::protobuf::Timestamp
ToTimestampFromStr(const std::string &timestamp_utc_str);

std::string FromTimestampToStr(const google::protobuf::Timestamp &timestamp);

dto::Event ToDtoEvent(const event_service::Event &proto_event);

dto::Stats ToDtoStats(const event_service::Stats &proto_stats);

std::optional<dto::EventFilter>
FromProtoEventFilter(const std::optional<EventFilter> &event_filter);

std::optional<EventFilter>
ToProtoEventFilter(const std::optional<dto::EventFilter> &event_filter);

Event ToProtoEvent(const dto::Event &event);

Stats ToProtoStats(const dto::Stats &stats);

} // namespace event_service