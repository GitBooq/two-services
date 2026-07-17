// tests/unit/get_events_use_case_test.cpp

#include "get_events_use_case.h"
#include "mocks/client/mock_event_provider.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;
using testing::Return;

class GetEventsUseCaseTest : public testing::Test {
protected:
  void SetUp() override {
    mock_provider_ = std::make_shared<MockEventProvider>();
    use_case_ = std::make_unique<GetEventsUseCase>(mock_provider_);
  }

  std::shared_ptr<MockEventProvider> mock_provider_;
  std::unique_ptr<GetEventsUseCase> use_case_;
};

TEST_F(GetEventsUseCaseTest, ExecuteCallsProviderGetEventsImpl) {
  dto::EventFilter filter;

  EXPECT_CALL(*mock_provider_, GetEventsImpl(_))
      .Times(1)
      .WillOnce(Return(std::nullopt));

  use_case_->Execute({filter});
}