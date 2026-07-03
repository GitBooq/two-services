/*
Event model
Contains data to send to server
*/
#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace dto {

struct Payload {
  std::string raw_line;
  std::optional<std::string> parsed_ip;
  std::string filter_decision;
  std::optional<std::string> reject_reason;
};

struct Event {
  enum class Status : uint8_t { UNSPECIFIED /*ALL*/, SUCCESS, ERROR };

  [[nodiscard]] std::string StatusToStr() const;

  std::string source_service;
  std::string timestamp_utc;
  Status status;
  Payload payload;
};

struct EventFilter {
  std::optional<std::string> source_service;
  std::optional<Event::Status> status;
  std::optional<std::size_t> limit;
  std::optional<std::string> from;
  std::optional<std::string> to;
  std::optional<std::size_t> offset;
};

inline std::string Event::StatusToStr() const {
  switch (status) {
  case Status::UNSPECIFIED:
    return "unspecified";
  case Status::SUCCESS:
    return "success";
  case Status::ERROR:
    return "error";
  }
}

} // namespace dto