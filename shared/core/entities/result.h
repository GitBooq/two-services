// shared/result.h
#pragma once

#include <optional>
#include <string>
#include <utility>

namespace shared {

template <typename T = void> struct Result {
  bool success{false};
  std::string error_message;
  std::optional<T> data;

  Result() = default;

  Result(bool success, std::string error_message,
         std::optional<T> data = std::nullopt)
      : success(success), error_message(std::move(error_message)),
        data(std::move(data)) {}

  static Result<T> Ok(T value) {
    Result<T> result;
    result.success = true;
    result.data = std::move(value);
    return result;
  }

  static Result<T> Ok() {
    Result<T> result;
    result.success = true;
    return result;
  }

  static Result<T> NotOk(std::string error_message) {
    Result<T> result;
    result.success = false;
    result.error_message = std::move(error_message);
    return result;
  }
};

// specialisation for Result w/o data
template <> struct Result<void> {
  bool success{false};
  std::string error_message;

  static Result<void> Ok() {
    Result<void> result;
    result.success = true;
    return result;
  }

  static Result<void> NotOk(std::string error_message) {
    Result<void> result;
    result.success = false;
    result.error_message = std::move(error_message);
    return result;
  }
};

} // namespace shared