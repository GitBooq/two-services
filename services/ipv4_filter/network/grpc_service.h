// network/grpc_service.h
/*
Low level gRPC client.
Creates gRPC connection, performs RPC requests, returns RPC responses
*/
#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>

#include "event_service.grpc.pb.h"
#include "i_event_provider.h"
#include "i_event_saver.h"
#include "i_stats_provider.h"

namespace event_service {

class GrpcService : public IEventSaver,
                    public IEventProvider,
                    public IStatsProvider {
public:
  // timeout in ms
  explicit GrpcService(const std::shared_ptr<grpc::Channel> &channel,
                       std::size_t timeout = kDefaultUnaryRPCTimeoutSec);

  [[nodiscard]] bool Save(const dto::Event &event) const override;
  [[nodiscard]] bool
  SaveBatch(std::span<const dto::Event> events) const override;
  [[nodiscard]] std::optional<dto::Stats> GetStats() const override;

private:
  [[nodiscard]] bool SaveBatchStream(std::span<const dto::Event> &events) const;
  [[nodiscard]] std::optional<std::vector<dto::Event>>
  GetEventsImpl(const dto::EventFilter &filter) const override;

  [[nodiscard]] bool DoSaveRequest(const SaveEventRequest &request) const;
  [[nodiscard]] std::optional<std::vector<dto::Event>>
  DoGetEventsRequest(const GetEventsRequest &request) const;
  [[nodiscard]] std::optional<dto::Stats>
  DoGetStatsRequest(const GetStatsRequest &request) const;

  static void HandleSaveEventsResponse(const grpc::Status &status,
                                       const SaveEventsResponse &response);
  static void HandleGetEventsResponse(const grpc::Status &status,
                                      const GetEventsResponse &response);
  static void HandleGetStatsResponse(const grpc::Status &status,
                                     const GetStatsResponse &response);

  static void AddEventToRequest(SaveEventRequest &request,
                                const dto::Event &event);
  static void AddFilterToRequest(GetEventsRequest &request,
                                 const std::optional<dto::EventFilter> &filter);

  static constexpr std::size_t kDefaultUnaryRPCTimeoutSec = 3000;
  static constexpr std::size_t kDefaultStreamingTimeoutMin = 30;

  std::unique_ptr<EventService::Stub> stub_;
  std::size_t timeout_; //< timeout in ms
};
} // namespace event_service
