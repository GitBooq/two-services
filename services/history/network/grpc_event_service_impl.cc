#include <grpcpp/support/status.h>

#include "event.h"
#include "event_service.pb.h"
#include "grpc_event_service_impl.h"
#include "utils.h"

namespace event_service {

grpc::Status
GrpcEventServiceImpl::SaveEvent([[maybe_unused]] grpc::ServerContext *context,
                                const event_service::SaveEventsRequest *request,
                                event_service::SaveEventsResponse *response) {
  // 1. Log Request
  // TODO
  // 2. Validate input data
  auto status = Validate(request);
  if (!status.ok()) {
    return status;
  }
  // 3. proto -> domain conversion
  std::vector<dto::Event> events;
  events.reserve(request->event_size());
  for (const auto &event : request->event()) {
    events.push_back(ToDtoEvent(event));
  }
  // 4. Call Use Case
  // TODO auto result = m_saveEventUseCase->execute(events);
  // 5. Log success/error
  // TODO
  // 6. Set Response & Return Status
  response->set_success(true);
  response->set_message("Event(s) received and saved successfully.");
  return status;
}

grpc::Status GrpcEventServiceImpl::Validate(
    const event_service::SaveEventsRequest *request) {
  if (0 == request->event_size()) {
    return {grpc::INVALID_ARGUMENT, "At least one event is required."};
  }
  for (const auto &event : request->event()) {
    if (event.source_service() != "ipv4_filter") {
      return {grpc::INVALID_ARGUMENT, "Unknown source_service."};
    }
    if (event.timestamp_utc().empty()) {
      return {grpc::INVALID_ARGUMENT, "timestamp_utc is required."};
    }
    if (event.status() == EventStatus::UNSPECIFIED) {
      return {grpc::INVALID_ARGUMENT, "status must be SUCCESS or ERROR"};
    }
    const auto &payload = event.payload();
    if (payload.raw_line().empty()) {
      return {grpc::INVALID_ARGUMENT, "payload raw_line is required"};
    }
    if (payload.filter_decision() ==
        FilterDecision::FILTER_DECISION_UNSPECIFIED) {
      return {grpc::INVALID_ARGUMENT,
              "filter_decision must be 'accepted' or 'rejected'."};
    }
  }
  return {grpc::OK, ""};
}

grpc::Status
GrpcEventServiceImpl::GetEvents([[maybe_unused]] grpc::ServerContext *context,
                                const event_service::GetEventsRequest *request,
                                event_service::GetEventsResponse *response) {
  // 1. Log Request
  // TODO
  // 2. proto -> domain conversion
  auto event_filter = FromProtoEventFilter(request->event_filter());
  // 3. Call Use Case
  // TODO: vector<events> events = m_getEventsUseCase->execute(event_filter);
  // 4. domain -> proto conversion
  // TODO: auto proto_events = ToProtoEvents
  // 5. Set Response & Return Status
  // TODO: response->...
  (void)response;
  return {grpc::OK, ""};
}

grpc::Status GrpcEventServiceImpl::GetStats([[maybe_unused]] grpc::ServerContext *context,
                                  const event_service::GetStatsRequest *request,
                                  event_service::GetStatsResponse *response) {
  // 1. Log Request
  // TODO
  // 2. Call Use Case
  // TODO: auto stats = get stats use case
  (void)request;
  // 3. domain -> proto conversion
  // TODO dto stats -> proto stats
  // 4. Set Response & Return Status
  // TODO: response->...
  (void)response;
  return {grpc::OK, ""};
}

} // namespace event_service