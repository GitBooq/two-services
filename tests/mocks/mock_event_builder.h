// tests/mocks/mock_event_builder.h

#pragma once

#include "i_event_builder.h"

#include <gmock/gmock.h>

class MockEventBuilder : public IEventBuilder {
public:
  MOCK_METHOD(dto::Event, Build, (const net::logger::LogEntry &log_entry),
              (override));
};
