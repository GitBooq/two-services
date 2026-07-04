// services/ipv4_filter/core/use_cases/get_events_use_case.h
#pragma once

#include "i_event_provider.h"

#include <memory>
#include <net_logger/net_logger.h>

/*
    Get Events from Server 
*/
class GetEventsUseCase { // IEventProvider
public:
  struct Request {
  };
  
  struct Response {
  };

  GetEventsUseCase();

  Response Execute(const Request &request);

private:
  std::shared_ptr<event_service::IEventProvider> provider_;
};