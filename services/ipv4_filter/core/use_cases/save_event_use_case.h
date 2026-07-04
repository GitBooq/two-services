// services/ipv4_filter/core/use_cases/save_event_use_case.h
#pragma once

#include "i_event_builder.h"
#include "i_event_saver.h"
#include "i_filter_adapter.h"

#include <memory>
#include <net_logger/net_logger.h>
#include <utility>
#include <vector>

/*
    Send Event to Server for Saving
*/
class SaveEventUseCase {
public:
  struct Request {
    std::shared_ptr<std::istream> input;
    net::logger::CompositeFilter filter;
    std::string source_service_name;
  };

  struct Result {
    int logs_processed = 0;
    int events_created = 0;
    int events_sent = 0;
    std::vector<std::string> errors;
  };

  SaveEventUseCase(std::shared_ptr<IEventBuilder> builder,
                   std::shared_ptr<event_service::IEventSaver> saver);

  Result Execute(const Request &request);

private:
  std::shared_ptr<IEventBuilder> builder_;
  std::shared_ptr<event_service::IEventSaver> saver_;
};