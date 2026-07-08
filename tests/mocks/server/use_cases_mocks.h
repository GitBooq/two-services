// use_cases_mocks.h
#pragma once

#include "i_get_stats_use_case.h"
#include "i_save_event_use_case.h"

class MockSaveEventUseCase : public ISaveEventUseCase {
public:
  MOCK_METHOD(shared::Result<>, Execute, (const std::vector<dto::Event> &),
              (override));
};

class MockGetEventsUseCase : public IGetEventsUseCase {
public:
  MOCK_METHOD(shared::Result<std::vector<dto::Event>>, Execute, (const std::optional<dto::EventFilter> &filter), (override));
};

class MockGetStatsUseCase : public IGetStatsUseCase {
public:
  MOCK_METHOD(shared::Result<dto::Stats>, Execute, (), (override));
};