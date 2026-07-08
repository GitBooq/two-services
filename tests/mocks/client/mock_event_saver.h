// tests/mocks/mock_event_saver.h
#pragma once

#include "i_event_saver.h"

#include <gmock/gmock.h>

class MockEventSaver : public event_service::IEventSaver {
public:
  MOCK_METHOD(bool, Save, (const dto::Event &), (const, override));

  MOCK_METHOD(bool, SaveBatch, (std::span<const dto::Event>), (const, override));
};
