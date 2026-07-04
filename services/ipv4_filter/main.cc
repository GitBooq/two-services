// main.cc
// stub

//


#include <iostream>


#include <fstream>
#include <net_logger/net_logger.h>
#include <sstream>
#include <string>

int main() {}

#if 0
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

std::string GetTargetServerString(const std::string &host, uint16_t port) {
  return host + std::string(":") + std::to_string(port);
}

} // namespace

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: client [path_to_file]\n";
    return 1;
  }

  std::ifstream input(argv[1]);
  if (!input.is_open()) {
    std::cerr << "Can't open file\n";
    return 1;
  }

  std::stringstream buffer;
  buffer << input.rdbuf();

  Config cfg;
  cfg.service_name = "ipv4_filter";
  cfg.server_address = "localhost";
  cfg.server_port = 50051;
  cfg.filters = {{"type", "subnet", "value", "192.168.1.0/24"},
                 {"type", "range", "value", "10.0.0.1-10.0.0.100"}};

  EventBuilder eventBuilder{cfg.service_name};

  std::string target =
      GetTargetServerString(cfg.server_address, cfg.server_port);
  auto channel_args = grpc::ChannelArguments();
  channel_args.SetServiceConfigJSON(std::string(kRetryPolicy));

  auto channel = grpc::CreateCustomChannel(
      target, grpc::InsecureChannelCredentials(), channel_args);
  event_service::GrpcService grpc{channel};

  auto pipeline = EventPipeline(eventBuilder, grpc);
  auto filter_runner = FilterRunner(cfg, pipeline);
  filter_runner.Run(buffer);

  auto events = grpc.GetEvents(/*filter*/);
  auto stats = grpc.GetStats();

  (void)stats;

  return 0;
}
#endif