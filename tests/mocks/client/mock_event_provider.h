// tests/mocks/mock_event_provider.h

#pragma once

#include "i_event_provider.h"

#include <gmock/gmock.h>

class MockEventProvider : public event_service::IEventProvider {
public:
  MOCK_METHOD(std::optional<std::vector<dto::Event>>, GetEventsImpl,
              (const dto::EventFilter &filter), (const, override));
};
