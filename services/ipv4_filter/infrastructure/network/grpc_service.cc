#include "grpc_service.h"
#include "event.h"
#include "event_service.pb.h"
#include "grpc_helpers.h"
#include <optional>

namespace event_service {

GrpcService::GrpcService(const std::shared_ptr<grpc::Channel> &channel,
                         std::size_t timeout)
    : stub_(EventService::NewStub(channel)), timeout_(timeout) {}

////////////////////////////////////////////////////////////////////////
///////////////////////////SaveEvents///////////////////////////////////
////////////////////////////////////////////////////////////////////////

bool GrpcService::Save(const dto::Event &event) const {
  SaveEventsRequest request;
  AddEventToRequest(request, event);

  return DoSaveRequest(request);
}

bool GrpcService::SaveBatch(std::span<const dto::Event> events) const {
  SaveEventsRequest request;
  for (const auto &event : events) {
    AddEventToRequest(request, event);
  }

  return DoSaveRequest(request);
}

void GrpcService::HandleSaveEventsResponse(const grpc::Status &status,
                                           const SaveEventsResponse &response) {
  if (!status.ok()) {
    std::cout << absl::StrFormat("gRPC error. Code: %d Message: %s",
                                 status.error_code(), status.error_message())
              << std::endl;
    return;
  }

  if (!response.success()) {
    std::cout << absl::StrFormat("Receiver rejected message: %s",
                                 response.message())
              << std::endl;
    return;
  }

  std::cout << absl::StrFormat("Ok. Response message: %s", response.message())
            << std::endl;
}

void GrpcService::AddEventToRequest(SaveEventsRequest &request,
                                    const dto::Event &event) {
  /* ok pattern: protobuf handles this raw ptrs (arena buffer) */
  auto *eventReq = request.add_event();
  eventReq->set_source_service(event.source_service);
  eventReq->set_timestamp_utc(event.timestamp_utc);
  eventReq->set_status(proto::ToProtoEventStatus(event.status));

  auto *reqPayload = eventReq->mutable_payload();
  const auto &evPayload = event.payload;
  reqPayload->set_raw_line(evPayload.raw_line);
  if (evPayload.parsed_ip.has_value()) {
    reqPayload->set_parsed_ip(evPayload.parsed_ip.value());
  }
  reqPayload->set_filter_decision(
      proto::ToProtoFilterDecision(evPayload.filter_decision));
  if (evPayload.reject_reason.has_value()) {
    reqPayload->set_reject_reason(evPayload.reject_reason.value());
  }
}

bool GrpcService::DoSaveRequest(const SaveEventsRequest &request) const {
  SaveEventsResponse response;
  grpc::ClientContext context;
  context.set_deadline(std::chrono::system_clock::now() +
                       std::chrono::milliseconds(timeout_));

  grpc::Status status = stub_->SaveEvent(&context, request, &response);
  HandleSaveEventsResponse(status, response);
  return status.ok() && response.success();
}

////////////////////////////////////////////////////////////////////////
///////////////////////////GetEvents////////////////////////////////////
////////////////////////////////////////////////////////////////////////

std::optional<std::vector<dto::Event>>
GrpcService::GetEventsImpl(const dto::EventFilter &filter) const {
  GetEventsRequest request;
  GrpcService::AddFilterToRequest(request, filter);

  return DoGetEventsRequest(request);
}

void GrpcService::AddFilterToRequest(GetEventsRequest &request,
                                     const dto::EventFilter &filter) {
  if (filter.source_service.has_value()) {
    request.set_source_service(filter.source_service.value());
  }
  if (filter.status.has_value()) {
    request.set_status(proto::ToProtoEventStatus(filter.status.value()));
  }
  if (filter.limit.has_value()) {
    request.set_limit(static_cast<uint64_t>(filter.limit.value()));
  }
  if (filter.from.has_value()) {
    *request.mutable_from() = proto::ToTimestampFromStr(filter.from.value());
  }
  if (filter.to.has_value()) {
    *request.mutable_to() = proto::ToTimestampFromStr(filter.to.value());
  }
  if (filter.offset.has_value()) {
    request.set_offset(static_cast<uint32_t>(filter.offset.value()));
  }
}

std::optional<std::vector<dto::Event>>
GrpcService::DoGetEventsRequest(const GetEventsRequest &request) const {
  GetEventsResponse response;
  grpc::ClientContext context;
  context.set_deadline(std::chrono::system_clock::now() +
                       std::chrono::milliseconds(timeout_));

  grpc::Status status = stub_->GetEvents(&context, request, &response);
  HandleGetEventsResponse(status, response);
  if (!status.ok() || !response.success()) {
    return std::nullopt;
  }

  std::vector<dto::Event> events;
  events.reserve(response.events_size());
  for (const auto &protoEvent : response.events()) {
    auto event = proto::ToDtoEvent(protoEvent);
    events.push_back(std::move(event));
  }
  return events;
}

void GrpcService::HandleGetEventsResponse(const grpc::Status &status,
                                          const GetEventsResponse &response) {
  if (!status.ok()) {
    std::cout << absl::StrFormat("gRPC error. Code: %d Message: %s",
                                 status.error_code(), status.error_message())
              << std::endl;
    return;
  }

  if (!response.success()) {
    std::cout << absl::StrFormat("Receiver rejected message: %s",
                                 response.message())
              << std::endl;
    return;
  }

  std::cout << absl::StrFormat("Ok. Response message: %s", response.message())
            << std::endl;
}

////////////////////////////////////////////////////////////////////////
///////////////////////////GetStats/////////////////////////////////////
////////////////////////////////////////////////////////////////////////

std::optional<dto::Stats> GrpcService::GetStats() const {
  return DoGetStatsRequest(GetStatsRequest{});
}

std::optional<dto::Stats>
GrpcService::DoGetStatsRequest(const GetStatsRequest &request) const {
  GetStatsResponse response;
  grpc::ClientContext context;
  context.set_deadline(std::chrono::system_clock::now() +
                       std::chrono::milliseconds(timeout_));

  grpc::Status status = stub_->GetStats(&context, request, &response);
  HandleGetStatsResponse(status, response);
  if (!status.ok() || !response.success()) {
    return std::nullopt;
  }

  return proto::ToDtoStats(response.stats());
}

void GrpcService::HandleGetStatsResponse(const grpc::Status &status,
                                         const GetStatsResponse &response) {
  if (!status.ok()) {
    std::cout << absl::StrFormat("gRPC error. Code: %d Message: %s",
                                 status.error_code(), status.error_message())
              << std::endl;
    return;
  }

  if (!response.success()) {
    std::cout << absl::StrFormat("Receiver rejected message: %s",
                                 response.message())
              << std::endl;
    return;
  }

  std::cout << absl::StrFormat("Ok. Response message: %s", response.message())
            << std::endl;
}

} // namespace event_service