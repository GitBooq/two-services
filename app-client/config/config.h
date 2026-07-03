// config/config.h

/*
App configuration storage:

    Filter config
    server address/port
    gRPC config
    batch size
    etc
*/
#pragma once
#include <string>

#include <net_logger/net_logger.h>

/*
struct FilterConfig {
  std::string type_;   ///< filter type
  std::string value_;  ///< ipv4addr
  ...
*/
struct Config {
  std::string service_name;
  std::string server_address = "localhost";
  uint16_t server_port = 50051;
  std::vector<net::logger::FilterConfig> filters;
  int connect_timeout_sec = 5;
  int events_poll_interval_sec = 30;
  int stats_poll_interval_sec = 60;
};