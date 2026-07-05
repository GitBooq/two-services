// services/history/main.cc

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "grpc_event_service_impl.h"

int main() {

  // using namespace datatransfer;
  // auto port = helpers::GetServerPortOr50051();
  std::string server_address("0.0.0.0:50051");

  // application::ConsoleMessageHandler handler;
  event_service::GrpcEventServiceImpl service{/*handler*/};

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "History service server listening on " << server_address << std::endl;

  server->Wait();
  return 0;
}