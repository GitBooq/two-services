#include <grpcpp/support/status.h>

#include <utility>

#include "event.h"
#include "event_service.pb.h"
#include "grpc_event_service_impl.h"
#include "utils.h"

namespace event_service {

GrpcEventServiceImpl::GrpcEventServiceImpl(
    std::shared_ptr<ISaveEventUseCase> save_event_use_case,
    std::shared_ptr<IGetEventsUseCase> get_events_use_case,
    std::shared_ptr<IGetStatsUseCase> get_stats_use_case)
    : save_event_use_case_(std::move(save_event_use_case)),
      get_events_use_case_(std::move(get_events_use_case)),
      get_stats_use_case_(std::move(get_stats_use_case)) {}

grpc::Status
GrpcEventServiceImpl::SaveEvent([[maybe_unused]] grpc::ServerContext *context,
                                const event_service::SaveEventRequest *request,
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
  auto result = save_event_use_case_->Execute(events);
  // 5. Log success/error
  // TODO
  // 6. Set Response & Return Status
  response->set_success(result.success);
  response->set_message(result.success
                            ? "Event(s) received and saved successfully."
                            : result.error_message);

  return status;
}

grpc::Status GrpcEventServiceImpl::SaveEventsStream(
    grpc::ServerContext *context, grpc::ServerReader<SaveEventsRequest> *reader,
    SaveEventsResponse *response) {

  try {
    SaveEventsRequest request;
    int total_events = 0;
    int saved_events = 0;
    std::vector<dto::Event> batch;
    const size_t BATCH_SIZE = 1000;

    // Read events from stream
    while (reader->Read(&request)) {
      auto status = Validate(&request);
      if (!status.ok()) {
        response->set_success(false);
        response->set_message("Validation failed at event " +
                              std::to_string(total_events) + ": " +
                              status.error_message());
        return grpc::Status::OK;
      }

      // proto -> domain conv
      dto::Event dto_event = ToDtoEvent(request.event());
      batch.push_back(dto_event);
      total_events++;

      // Save to db using batch
      if (batch.size() >= BATCH_SIZE) {
        auto result = save_event_use_case_->Execute(batch);
        if (result.success) {
          saved_events += batch.size();
        } else {
          response->set_success(false);
          response->set_message("Failed to save batch at event " +
                                std::to_string(total_events) + ": " +
                                result.error_message);
          return grpc::Status::OK;
        }
        batch.clear();

        // Log
        if (total_events % 10000 == 0) {
          std::cout << "Stream progress: " << total_events
                    << " events processed, " << saved_events << " saved"
                    << std::endl;
        }
      }

      // Client can cancel sending
      if (context->IsCancelled()) {
        response->set_success(false);
        response->set_message("Cancelled by client at " +
                              std::to_string(total_events) + " events");
        return grpc::Status::CANCELLED;
      }
    }

    // Save other events if any
    if (!batch.empty()) {
      auto result = save_event_use_case_->Execute(batch);
      if (result.success) {
        saved_events += batch.size();
      } else {
        response->set_success(false);
        response->set_message("Failed to save final batch: " +
                              result.error_message);
        return grpc::Status::OK;
      }
    }

    // response
    response->set_success(true);
    response->set_message("Saved " + std::to_string(saved_events) + "/" +
                          std::to_string(total_events) + " events");

    std::cout << "Stream completed: " << saved_events << "/" << total_events
              << " events saved\n";

    return grpc::Status::OK;

  } catch (const std::exception &e) {
    response->set_success(false);
    response->set_message(std::string("Fatal error: ") + e.what());
    return grpc::Status::OK;
  }
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
  auto result = get_events_use_case_->Execute(event_filter);
  if (!result.success) {
    response->set_success(false);
    response->set_message(result.error_message);
    return grpc::Status::OK;
  }

  if (!result.data.has_value() || result.data->empty()) {
    response->set_success(true);
    response->set_message("No events found");
    return grpc::Status::OK;
  }
  // 4. domain -> proto conversion; add events to response
  const auto &events = result.data.value();
  for (const auto &event : events) {
    auto *proto_event = response->add_events();
    *proto_event = ToProtoEvent(event);
  }
  // 5. Set Response & Return Status
  response->set_success(true);
  response->set_message("GetEvents successful.");
  return grpc::Status::OK;
}

grpc::Status GrpcEventServiceImpl::GetStats(
    [[maybe_unused]] grpc::ServerContext *context,
    [[maybe_unused]] const event_service::GetStatsRequest *request,
    event_service::GetStatsResponse *response) {
  // 1. Log Request
  // TODO
  // 2. Call Use Case
  auto result = get_stats_use_case_->Execute();
  if (!result.success || !result.data.has_value()) {
    response->set_success(false);
    response->set_message(result.error_message);
    return grpc::Status::OK;
  }
  // 3. domain -> proto conversion
  Stats proto_stats = ToProtoStats(result.data.value());
  // 4. Set Response & Return Status
  *response->mutable_stats() = std::move(proto_stats);
  response->set_success(true);
  response->set_message("GetStats successful.");
  return grpc::Status::OK;
}

grpc::Status GrpcEventServiceImpl::Validate(const SaveEventRequest *request) {
  if (request->event_size() == 0) {
    return {grpc::INVALID_ARGUMENT, "At least one event is required."};
  }

  for (const auto &event : request->event()) {
    auto status = ValidateEvent(event);
    if (!status.ok()) {
      return status;
    }
  }

  return {grpc::OK, ""};
}

grpc::Status GrpcEventServiceImpl::Validate(const SaveEventsRequest *request) {
  if (!request->has_event()) {
    return {grpc::INVALID_ARGUMENT, "Event is required."};
  }

  return ValidateEvent(request->event());
}

grpc::Status GrpcEventServiceImpl::ValidateEvent(const Event &event) {
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

  return {grpc::OK, ""};
}

} // namespace event_service