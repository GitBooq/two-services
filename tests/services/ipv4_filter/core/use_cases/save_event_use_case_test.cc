#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks/client/mock_event_builder.h"
#include "mocks/client/mock_event_saver.h"
#include "save_event_use_case.h"

using ::testing::_;
using ::testing::Exactly;
using ::testing::Return;

class SaveEventUseCaseTest : public ::testing::Test {
protected:
  void SetUp() override {
    builder_ = std::make_shared<MockEventBuilder>();
    saver_ = std::make_shared<MockEventSaver>();
    use_case_ = std::make_unique<SaveEventUseCase>(builder_, saver_);
  }

  std::shared_ptr<MockEventBuilder> builder_;
  std::shared_ptr<MockEventSaver> saver_;
  std::unique_ptr<SaveEventUseCase> use_case_;
};

TEST_F(SaveEventUseCaseTest, ExecuteCallsBuildForEachLogAndSaveBatchOnce) {
  std::stringstream input;
  input << "1\n2\n3\n";

  SaveEventUseCase::Request request{.filter= {}, .source_service_name = {}};

  EXPECT_CALL(*builder_, Build(_))
      .Times(Exactly(3))
      .WillRepeatedly(Return(dto::Event{}));
  EXPECT_CALL(*saver_, SaveBatch(_)).Times(Exactly(1)).WillOnce(Return(true));

  bool result = use_case_->Execute(request, input);

  EXPECT_TRUE(result);
}

TEST_F(SaveEventUseCaseTest, ExecuteWithEmptyInputCallsSaveBatchWithEmptySpan) {
  std::stringstream input("");

  SaveEventUseCase::Request request{.filter= {}, .source_service_name = {}};

  EXPECT_CALL(*builder_, Build(_)).Times(Exactly(0));
  // empty span match IsEmpty()
  EXPECT_CALL(*saver_, SaveBatch(testing::IsEmpty()))
      .Times(Exactly(1))
      .WillOnce(Return(true));

  bool result = use_case_->Execute(request, input);

  EXPECT_TRUE(result);
}
