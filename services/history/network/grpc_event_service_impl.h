// network/grpc_event_service_impl.h
/*

*/
#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>

#include "event_service.grpc.pb.h"
#include "i_get_events_use_case.h"
#include "i_get_stats_use_case.h"
#include "i_save_event_use_case.h"

namespace event_service {
/*
Implements event service from gRPC protobuf contract:
  service EventService {
    rpc SaveEvent(SaveEventRequest) returns (SaveEventsResponse);
    rpc SaveEventsStream(stream SaveEventsRequest) returns (SaveEventsResponse);
    rpc GetEvents(GetEventsRequest) returns (GetEventsResponse);
    rpc GetStats(GetStatsRequest) returns (GetStatsResponse);
  }
*/
class GrpcEventServiceImpl : public EventService::Service {
public:
  explicit GrpcEventServiceImpl(
      std::shared_ptr<ISaveEventUseCase> save_event_use_case,
      std::shared_ptr<IGetEventsUseCase> get_events_use_case,
      std::shared_ptr<IGetStatsUseCase> get_stats_use_case);

  // Receive SaveEvent RPC -> Save Event(s) to DB Use Case
  grpc::Status SaveEvent([[maybe_unused]] grpc::ServerContext *context,
                         const event_service::SaveEventRequest *request,
                         event_service::SaveEventsResponse *response) override;

  // Streaming RPC - Save events one by one
  grpc::Status SaveEventsStream(grpc::ServerContext *context,
                                grpc::ServerReader<SaveEventsRequest> *reader,
                                SaveEventsResponse *response) override;

  // Receive GetEvents RPC -> Collect Events from DB Use Case -> Send Back to
  // Caller
  grpc::Status GetEvents([[maybe_unused]] grpc::ServerContext *context,
                         const event_service::GetEventsRequest *request,
                         event_service::GetEventsResponse *response) override;

  // Receive GetStats RPC -> Collect Stats from DB Use Case -> Send Back to
  // Caller
  grpc::Status
  GetStats([[maybe_unused]] grpc::ServerContext *context,
           [[maybe_unused]] const event_service::GetStatsRequest *request,
           event_service::GetStatsResponse *response) override;

private:
  static grpc::Status ValidateEvent(const Event &event);
  static grpc::Status Validate(const event_service::SaveEventRequest *request);
  static grpc::Status Validate(const event_service::SaveEventsRequest *request);

  std::shared_ptr<ISaveEventUseCase> save_event_use_case_;
  std::shared_ptr<IGetEventsUseCase> get_events_use_case_;
  std::shared_ptr<IGetStatsUseCase> get_stats_use_case_;
};

} // namespace event_service
