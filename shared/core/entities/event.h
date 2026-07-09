/*
Event model
Contains data to send to server
*/
#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <stdexcept>
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

  [[nodiscard]] static std::string StatusToStr(Status status);
  [[nodiscard]] static Status StrToStatus(const std::string &status);

  std::string source_service;
  std::string timestamp_utc;
  Status status;
  Payload payload;
};

struct EventFilter {
  enum class Type { SRC_SRV = 0, STATUS, LIMIT, FROM, TO, OFFSET };
  static const inline std::map<std::string, Type> kMap{
      {"source_service", Type::SRC_SRV},
      {"status", Type::STATUS},
      {"limit", Type::LIMIT},
      {"from", Type::FROM},
      {"to", Type::TO},
      {"offset", Type::OFFSET}};

  std::optional<std::string> source_service;
  std::optional<Event::Status> status;
  std::optional<std::size_t> limit;
  std::optional<std::string> from;
  std::optional<std::string> to;
  std::optional<std::size_t> offset;
};

inline std::string Event::StatusToStr(Status status) {
  switch (status) {
  case Status::UNSPECIFIED:
    return "unspecified";
  case Status::SUCCESS:
    return "success";
  case Status::ERROR:
    return "error";
  }
  throw std::invalid_argument("Invalid status in Event::StatusToStr().");
}

inline Event::Status Event::StrToStatus(const std::string &status) {
  if (status == "unspecified") {
    return Status::UNSPECIFIED;
  }
  if (status == "success") {
    return Status::SUCCESS;
  }
  if (status == "error") {
    return Status::ERROR;
  }
  throw std::invalid_argument("Invalid status str in Event::StrToStatus().");
}

} // namespace dto