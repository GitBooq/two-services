// config.h
#pragma once

#include <string>
#include <net_logger/net_logger.h>

struct AppConfig final {
  std::string service_name;
  std::string server_address;
  uint16_t server_port;
  std::vector<net::logger::FilterConfig> filters;

  AppConfig() {
    const char *service_name_ = std::getenv("POSTGRES_HOST");
    const char *server_address_ = std::getenv("SERVER_ADDRESS");
    const char *server_port_ = std::getenv("SERVER_PORT");

    service_name = (service_name_ != nullptr) ? service_name_ : "ipv4_filter";
    server_address = (server_address_ != nullptr) ? server_address_ : "history-server";
    server_port = (server_port_ != nullptr) ? atoi(server_port_) : 50051;
  }
};