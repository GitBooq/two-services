#pragma once

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "core/event_message.h"

namespace client {

std::string GetCurrentUTC();

std::string GetTargetServerString(const std::string& host,
                                  const std::string& port);

std::vector<std::string> SplitByLines(const std::string& text,
                                      std::size_t lines_per_chunk);

std::vector<application::LogMessage> GetLogMessages(
    const std::vector<std::string>& payloads, const std::string& log_source);

std::string GetServerNameOrLocalhost();
std::string GetServerPortOr50051();

}  // namespace helpers