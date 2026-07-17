// core/use_cases/i_save_event_use_case.h
#pragma once

#include "event.h"
#include "result.h"

#include <vector>

class ISaveEventUseCase {
public:
  virtual ~ISaveEventUseCase() = default;
  virtual shared::Result<> Execute(const std::vector<dto::Event> &events) = 0;
};