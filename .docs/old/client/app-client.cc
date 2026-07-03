#include <net_logger/net_logger.h>

#include "client_helpers.h"
#include "log_entry.h"
#include "log_message.h"
#include "log_processor.h"
#include "sender/grpc_message_sender.h"

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <string>

namespace {

// clang-format off
[[maybe_unused]] constexpr absl::string_view kRetryPolicy = 
    "{\"methodConfig\" : [{"
    "   \"name\" : [{\"service\": \"datatransfer.DataTransfer\"}],"
    "   \"waitForReady\": true,"
    "   \"retryPolicy\": {"
    "     \"maxAttempts\": 4,"
    "     \"initialBackoff\": \"0.1s\","
    "     \"maxBackoff\": \"1s\","
    "     \"backoffMultiplier\": 2.0,"
    "     \"retryableStatusCodes\": [\"UNAVAILABLE\"]"
    "    }"
    "}]}";
// clang-format on

} // namespace

constexpr std::string APPNAME = "app-client";

int main(int argc, char **argv) {
  using namespace client;

  if (argc != 2) {
    std::cout << std::format("Usage: {} [path_to_logs]\n", APPNAME);
    return 1;
  }
  std::ifstream input(argv[1]);
  if (!input.is_open()) {
    std::cerr << "Can't open file\n";
    return 1;
  }
  std::stringstream buffer;
  buffer << input.rdbuf();

  try {
    auto filter = net::logger::CreateFilter(
        {{"type", "subnet", "value", "192.168.1.0/24"},
         {"type", "range", "value", "10.0.0.1-10.0.0.100"}});

    auto processor = core::logProcessor::LogProcessor();
    processor.Process(buffer, filter);
    auto entries = processor.entries(); // ACCEPTED/REJECTED

    //SaveEvent, GetEvents, GetStats -- Клиент
    //SaveEvent, SendEvents, SendStats -- Сервер
    // отправляем SaveEvent <EventMessage> -- Ожидаем подтверждение от сервера
    // отправляем GetEvents -- ожидаем EventMessage'S от сервера
    // отправляем GetStats -- ожидаем статистику

    std::string target = GetTargetServerString(GetServerNameOrLocalhost(),
                                               GetServerPortOr50051());
    auto channel_args = grpc::ChannelArguments();
    channel_args.SetServiceConfigJSON(std::string(kRetryPolicy));

    auto channel = grpc::CreateCustomChannel(
        target, grpc::InsecureChannelCredentials(), channel_args);

    datatransfer::GrpcMessageSender sender(channel);

    for (const auto &msg : log_messages) {
      sender.Send(msg);
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "Unknown exception caught" << std::endl;
    return 1;
  }

  return 0;
}