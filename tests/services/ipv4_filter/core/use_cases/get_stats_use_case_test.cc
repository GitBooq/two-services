// tests/unit/get_stats_use_case_test.cc

#include "get_stats_use_case.h"
#include "mocks/mock_stats_provider.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::Return;

class GetStatsUseCaseTest : public testing::Test {
protected:
  void SetUp() override {
    mock_provider_ = std::make_shared<MockStatsProvider>();
    use_case_ = std::make_unique<GetStatsUseCase>(mock_provider_);
  }

  std::shared_ptr<MockStatsProvider> mock_provider_;
  std::unique_ptr<GetStatsUseCase> use_case_;
};

TEST_F(GetStatsUseCaseTest, ExecuteCallsProviderGetStats) {
  EXPECT_CALL(*mock_provider_, GetStats())
      .Times(1)
      .WillOnce(Return(std::nullopt));

  use_case_->Execute();
}