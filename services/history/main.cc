// services/history/main.cc

#include <cstdlib>
#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <pqxx/pqxx>
#include <string>

#include "get_events_use_case.h"
#include "get_stats_use_case.h"
#include "grpc_event_service_impl.h"
#include "postgres_events_repo.h"
#include "save_event_use_case.h"

namespace {

struct PgSettings final {
  std::string host;
  std::string port;
  std::string dbname;
  std::string user;
  std::string password;

  PgSettings() {
    const char *host_ = std::getenv("POSTGRES_HOST");
    const char *port_ = std::getenv("POSTGRES_PORT");
    const char *dbname_ = std::getenv("POSTGRES_DB");
    const char *user_ = std::getenv("POSTGRES_USER");
    const char *password_ = std::getenv("POSTGRES_PASSWORD");

    host = (host_ != nullptr) ? host_ : "localhost";
    port = (port_ != nullptr) ? port_ : "5432";
    dbname = (dbname_ != nullptr) ? dbname_ : "mydb";
    user = (user_ != nullptr) ? user_ : "admin";
    password = (password_ != nullptr) ? password_ : "admin";
  }
};

std::string MakeDbConnectionString(const PgSettings &env) {
  return std::string("host=" + env.host + " port=" + env.port +
                     " dbname=" + env.dbname + " user=" + env.user +
                     " password=" + env.password);
}

std::string GetServerPortOr50051() {
  const auto* const chars = std::getenv("SERVER_PORT");
  return (nullptr != chars) ? chars : "50051";
}

} // namespace

int main() {
  try {
    auto port = GetServerPortOr50051();
    std::string server_address("0.0.0.0:" + port);

    PgSettings env;
    std::string conn_str = MakeDbConnectionString(env);
    std::cout << "Connecting to: " << env.host << ":" << env.port << "..."
              << "\n";
    auto db_conn = std::make_shared<pqxx::connection>(conn_str);
    std::cout << "Connected to: " << db_conn->dbname() << "\n";

    auto postgres_repo = std::make_shared<PostgresEventsRepo>(db_conn);
    auto save_event_handler = std::make_shared<SaveEventUseCase>(postgres_repo);
    auto get_events_handler = std::make_shared<GetEventsUseCase>(postgres_repo);
    auto get_stats_handler = std::make_shared<GetStatsUseCase>(postgres_repo);
    event_service::GrpcEventServiceImpl service{
        save_event_handler, get_events_handler, get_stats_handler};

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "History service server listening on " << server_address
              << "\n";

    server->Wait();
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
}