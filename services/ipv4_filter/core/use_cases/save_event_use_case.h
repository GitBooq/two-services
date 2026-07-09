// services/ipv4_filter/core/use_cases/save_event_use_case.h
#pragma once

#include "i_event_builder.h"
#include "i_event_saver.h"

#include <memory>
#include <net_logger/net_logger.h>

/*
    Send Event to Server for Saving
*/
class SaveEventUseCase {
public:
  struct Request {
    net::logger::CompositeFilter filter;
    std::string source_service_name;
  };

  SaveEventUseCase(std::shared_ptr<IEventBuilder> builder,
                   std::shared_ptr<event_service::IEventSaver> saver);

  bool Execute(const Request &request, std::istream& input);

private:
  std::shared_ptr<IEventBuilder> builder_;
  std::shared_ptr<event_service::IEventSaver> saver_;
};