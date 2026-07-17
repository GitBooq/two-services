// application/use_cases/save_event_use_case.h
#pragma once

#include "event.h"
#include "i_event_repository.h"
#include "i_save_event_use_case.h"

#include <memory>
#include <vector>

class SaveEventUseCase : public ISaveEventUseCase {
public:
  explicit SaveEventUseCase(std::shared_ptr<IEventRepository> repository);

  shared::Result<> Execute(const std::vector<dto::Event> &events) override;

private:
  std::shared_ptr<IEventRepository> repository_;
};