// tests/mocks/mock_stats_provider.h
#pragma once

#include "i_stats_provider.h"

#include <gmock/gmock.h>

class MockStatsProvider : public event_service::IStatsProvider {
public:
  MOCK_METHOD(std::optional<dto::Stats>, GetStats, (), (const, override));
};