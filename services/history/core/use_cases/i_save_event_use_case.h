// core/use_cases/i_save_event_use_case.h
#pragma once

#include "event.h"

#include <string>
#include <vector>

struct SaveEventResult {
  bool success;
  std::string error_message;
};

class ISaveEventUseCase {
public:
  virtual ~ISaveEventUseCase() = default;
  virtual SaveEventResult Execute(const std::vector<dto::Event> &events) = 0;
};