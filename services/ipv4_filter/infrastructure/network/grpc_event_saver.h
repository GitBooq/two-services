// infrastructure/network/grpc_event_saver.h

#include "event_service.grpc.pb.h"
#include "i_event_saver.h"

#include <grpcpp/grpcpp.h>
#include <memory>

namespace event_service {

class GrpcEventSaver : public IEventSaver {
  // timeout in ms
  explicit GrpcEventSaver(const std::shared_ptr<grpc::Channel> &channel,
                          std::size_t timeout = kDefaultTimeout);

  [[nodiscard]] bool Save(const dto::Event &event) const override;
  [[nodiscard]] bool
  SaveBatch(std::span<const dto::Event> events) const override;

private:
  [[nodiscard]] bool DoSaveRequest(const SaveEventsRequest &request) const;
  static void HandleSaveEventsResponse(const grpc::Status &status,
                                       const SaveEventsResponse &response);
  static void AddEventToRequest(SaveEventsRequest &request,
                                const dto::Event &event);

  static constexpr std::size_t kDefaultTimeout = 3000;

  std::unique_ptr<EventService::Stub> stub_;
  std::size_t timeout_; //< timeout in ms
};

} // namespace event_service