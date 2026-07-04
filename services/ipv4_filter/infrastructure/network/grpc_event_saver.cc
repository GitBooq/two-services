// infrastructure/network/grpc_event_saver.cc

#include "grpc_event_saver.h"
#include "grpc_helpers.h"

namespace event_service {

GrpcEventSaver::GrpcEventSaver(const std::shared_ptr<grpc::Channel> &channel,
                               std::size_t timeout)
    : stub_(EventService::NewStub(channel)), timeout_(timeout) {}

[[nodiscard]] bool GrpcEventSaver::Save(const dto::Event &event) const {
  SaveEventsRequest request;
  AddEventToRequest(request, event);
  return DoSaveRequest(request);
}

[[nodiscard]] bool
GrpcEventSaver::SaveBatch(std::span<const dto::Event> events) const {
  SaveEventsRequest request;
  for (const auto &event : events) {
    AddEventToRequest(request, event);
  }
  return DoSaveRequest(request);
}

void GrpcEventSaver::HandleSaveEventsResponse(
    const grpc::Status &status, const SaveEventsResponse &response) {
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

void GrpcEventSaver::AddEventToRequest(SaveEventsRequest &request,
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

bool GrpcEventSaver::DoSaveRequest(const SaveEventsRequest &request) const {
  SaveEventsResponse response;
  grpc::ClientContext context;
  context.set_deadline(std::chrono::system_clock::now() +
                       std::chrono::milliseconds(timeout_));

  grpc::Status status = stub_->SaveEvent(&context, request, &response);
  HandleSaveEventsResponse(status, response);
  return status.ok() && response.success();
}

} // namespace event_service