// main.cc

#include <absl/strings/string_view.h>
#include <algorithm>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <net_logger/net_logger.h>
#include <sstream>
#include <string>

#include "config.h"
#include "event.h"
#include "event_builder.h"
#include "get_events_use_case.h"
#include "get_stats_use_case.h"
#include "grpc_service.h"
#include "save_event_use_case.h"

namespace {

// Retry only idempotent methods
// Transparent retries still can be made
// clang-format off
[[maybe_unused]] constexpr absl::string_view kRetryPolicy = 
    "{\"methodConfig\" : [{"
    "   \"name\" : [{\"service\": \"event_service.EventService\",\"method\":\"GetEvents\"}],"
    "   \"waitForReady\": true,"
    "   \"retryPolicy\": {"
    "     \"maxAttempts\": 4,"
    "     \"initialBackoff\": \"0.1s\","
    "     \"maxBackoff\": \"1s\","
    "     \"backoffMultiplier\": 2.0,"
    "     \"retryableStatusCodes\": [\"UNAVAILABLE\"]"
    "    }"
    "},"
    "{"
    "   \"name\" : [{\"service\": \"event_service.EventService\",\"method\":\"GetStats\"}],"
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

// CLI

std::optional<dto::EventFilter>
ParseEventFilter(const std::vector<std::string> &args) {
  dto::EventFilter filter;

  for (size_t i = 1; i < args.size(); ++i) {
    const auto &arg = args[i].substr(2); // skip '--'

    auto filter_type_it = filter.kMap.find(arg);
    if (filter_type_it == filter.kMap.cend()) {
      std::cerr << "Unknown parameter: " << arg << std::endl;
      return std::nullopt;
    }

    const auto &[string, type] = *filter_type_it;

    if (i + 1 >= args.size()) {
      std::cerr << "Missing value for parameter: " << string << std::endl;
      continue;
    }
    ++i;
    switch (type) {
    case dto::EventFilter::Type::FROM:
    case dto::EventFilter::Type::TO:
      std::cerr << "Unsupported filter param\n";
      return std::nullopt;
    case dto::EventFilter::Type::LIMIT:
      filter.limit = std::stoi(args[i]);
      break;
    case dto::EventFilter::Type::OFFSET:
      filter.offset = std::stoi(args[i]);
      break;
    case dto::EventFilter::Type::STATUS:
      filter.status = dto::Event::StrToStatus(args[i]);
      break;
    case dto::EventFilter::Type::SRC_SRV:
      filter.source_service = args[i];
      break;
    }
  }

  return filter;
}

[[maybe_unused]] void PrintHelp() {
  std::cout << R"(Available commands:
  Save <file_path>               - Save events from file to server
  Get [options]                  - Get events with optional filters
  Stats                          - Get statistics
  Help                           - Show this help message
  Exit                           - Exit program

GetEvents options:
  --limit <number>               - Limit number of events
  --offset <number>              - Offset for pagination
  --status <status>              - Filter by status (success/error)
  --source_service <service>     - Filter by source service

Examples:
  Save path/to/log.txt
  Get --limit 10 --offset 1
  Get --status <success>
  Get --source_service ipv4_filter
)";
}

[[maybe_unused]] void
HandleSaveEvent(const std::vector<std::string> &args,
                const std::shared_ptr<SaveEventUseCase> &save_event_use_case,
                const AppConfig &cfg) {
  if (args.size() < 2) {
    std::cerr << "Error: Missing file path. Usage: SaveEvent <file_path>\n";
    return;
  }

  const std::string &file_path = args[1];

  std::ifstream input(file_path);
  if (!input.is_open()) {
    std::cerr << "Can't open file: " << file_path << "\n";
    return;
  }

  SaveEventUseCase::Request request{.filter =
                                        net::logger::CreateFilter(cfg.filters),
                                    .source_service_name = cfg.service_name};

  auto result = save_event_use_case->Execute(request, input);
  if (result) {
    std::cout << "Events saved successfully.\n";
  } else {
    std::cout << "Failed to save events.\n";
  }
}

void HandleGetEvents(
    const std::vector<std::string> &args,
    const std::shared_ptr<GetEventsUseCase> &get_events_use_case) {
  auto filter = ParseEventFilter(args);
  if (!filter.has_value()) {
    std::cerr << "Invalid filter params.\n";
    return;
  }

  GetEventsUseCase::Request request{filter.value()};
  auto result = get_events_use_case->Execute(request);

  if (result.events.has_value()) {
    const auto &events = result.events.value();
    std::cout << "Got " << events.size() << " events:\n";
    std::cout << std::format(
        "    {:<25} | {:<15} | {:<10} | {:<15} | {:<15} | {:<20} | {}\n",
        "TIMESTAMP_UTC", "SOURCE_SERVICE", "STATUS", "FILTER_DECISION",
        "PARSED_IP", "REJECT_REASON", "RAW_LINE");
    for (const auto &event : events) {
      const auto &payload = event.payload;
      std::cout << std::format(
          "  - {:<25} | {:<15} | {:<10} | {:<15} | {:<15} | {:<20} | {}\n",
          event.timestamp_utc, event.source_service,
          dto::Event::StatusToStr(event.status), payload.filter_decision,
          payload.parsed_ip ? *payload.parsed_ip : "None",
          payload.reject_reason ? *payload.reject_reason : "None",
          payload.raw_line);
    }
  } else {
    std::cout << "Failed to get events.\n";
  }
}

void HandleGetStats(
    const std::shared_ptr<GetStatsUseCase> &get_stats_use_case) {
  auto result = get_stats_use_case->Execute();

  if (result.stats.has_value()) {
    auto stats = result.stats.value();
    std::cout << "Stats:\n"
              << "  Total events: " << stats.events_total << "\n"
              << "  Success: " << stats.events_success << "\n"
              << "  Error: " << stats.events_error << "\n";
  } else {
    std::cout << "Failed to get stats.\n";
  }
}

} // namespace

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
  AppConfig cfg;
  cfg.filters = {{"type", "subnet", "value", "192.168.1.0/24"},
                 {"type", "range", "value", "10.0.0.1-10.0.0.100"}};

  std::string target =
      GetTargetServerString(cfg.server_address, cfg.server_port);
  auto channel_args = grpc::ChannelArguments();
  channel_args.SetServiceConfigJSON(std::string(kRetryPolicy));

  auto channel = grpc::CreateCustomChannel(
      target, grpc::InsecureChannelCredentials(), channel_args);

  auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(5);
  bool connected = channel->WaitForConnected(deadline);

  if (connected) {
    std::cout << "Connected to " << target << "\n";
  } else {
    std::cerr << "Failed to connect to " << target << " (timeout)\n";
    return 1;
  }

  auto grpc_service = std::make_shared<event_service::GrpcService>(channel);
  auto event_builder = std::make_shared<EventBuilder>(cfg.service_name);
  auto save_event_use_case =
      std::make_shared<SaveEventUseCase>(event_builder, grpc_service);
  auto get_events_use_case = std::make_shared<GetEventsUseCase>(grpc_service);
  auto get_stats_use_case = std::make_shared<GetStatsUseCase>(grpc_service);

  PrintHelp();

  std::string line;
  while (true) {
    std::cout << "\n> ";
    if (!std::getline(std::cin, line)) {
      break;
    }

    std::vector<std::string> args;
    std::string curr_arg;
    bool in_quotes = false;

    for (char c : line) {
      if (c == '"') {
        in_quotes = !in_quotes;
      } else if (c == ' ' && !in_quotes) {
        if (!curr_arg.empty()) {
          args.push_back(curr_arg);
          curr_arg.clear();
        }
      } else {
        curr_arg += c;
      }
    }
    if (!curr_arg.empty()) {
      args.push_back(curr_arg);
    }

    if (args.empty()) {
      continue;
    }

    // to lower case
    std::string command = args[0];
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    if (command == "help") {
      PrintHelp();
    } else if (command == "save") {
      HandleSaveEvent(args, save_event_use_case, cfg);
    } else if (command == "get") {
      HandleGetEvents(args, get_events_use_case);
    } else if (command == "stats") {
      HandleGetStats(get_stats_use_case);
    } else {
      std::cerr << "Unknown command: " << command << "\n";
      std::cout << "Type 'Help' for available commands.\n";
    }
  }

  return 0;
}
