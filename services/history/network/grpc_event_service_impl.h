// network/grpc_event_service_impl.h
/*

*/
#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>

#include "event_service.grpc.pb.h"

namespace event_service {
/*
Implements event service from gRPC protobuf contract:
  service EventService {
    rpc SaveEvent(SaveEventsRequest) returns (SaveEventsResponse);
    rpc GetEvents(GetEventsRequest) returns (GetEventsResponse);
    rpc GetStats(GetStatsRequest) returns (GetStatsResponse);
  }
*/
class GrpcEventServiceImpl : public EventService::Service {
public:
  // Receive SaveEvent RPC -> Save Event(s) to DB Use Case
  grpc::Status SaveEvent(grpc::ServerContext *context,
                         const event_service::SaveEventsRequest *request,
                         event_service::SaveEventsResponse *response) override;

  // Receive GetEvents RPC -> Collect Events from DB Use Case -> Send Back to
  // Caller
  grpc::Status GetEvents(grpc::ServerContext *context,
                         const event_service::GetEventsRequest *request,
                         event_service::GetEventsResponse *response) override;

  // Receive GetStats RPC -> Collect Stats from DB Use Case -> Send Back to
  // Caller
  grpc::Status GetStats(grpc::ServerContext *context,
                        const event_service::GetStatsRequest *request,
                        event_service::GetStatsResponse *response) override;

private:
  static grpc::Status Validate(const event_service::SaveEventsRequest *request);
};

} // namespace event_service
