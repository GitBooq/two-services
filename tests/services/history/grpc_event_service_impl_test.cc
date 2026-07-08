// grpc_event_service_impl_test.cc
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "event.h"
#include "event_service.pb.h"
#include "utils.h"
#include "event_service.grpc.pb.h"
#include "grpc_event_service_impl.h"
#include "mocks/server/use_cases_mocks.h"

using namespace event_service;

dto::Event CreateTestEvent() {
  dto::Event event;
  event.source_service = "ipv4_filter";
  event.status = dto::Event::Status::SUCCESS;
  event.timestamp_utc = utils::builder::FormatTimestamp(std::chrono::system_clock::time_point{});
  auto& payload = event.payload;
  payload.raw_line = "0.0.0.0 - test";
  payload.parsed_ip = "0.0.0.0";
  payload.filter_decision = "accepted";
  payload.reject_reason = std::nullopt;

  return event;
}

class GrpcEventServiceImplTest : public ::testing::Test {
protected:
  void SetUp() override {
    mock_save_ = std::make_shared<MockSaveEventUseCase>();
    mock_get_events_ = std::make_shared<MockGetEventsUseCase>();
    mock_get_stats_ = std::make_shared<MockGetStatsUseCase>();

    service_ = std::make_unique<GrpcEventServiceImpl>(
        mock_save_, mock_get_events_, mock_get_stats_);
  }

  std::shared_ptr<MockSaveEventUseCase> mock_save_;
  std::shared_ptr<MockGetEventsUseCase> mock_get_events_;
  std::shared_ptr<MockGetStatsUseCase> mock_get_stats_;
  std::unique_ptr<GrpcEventServiceImpl> service_;
  grpc::ServerContext context_;
};

TEST_F(GrpcEventServiceImplTest, SaveEventSuccess) {
  SaveEventsRequest request;
  auto *event = request.add_event();
  event->set_timestamp_utc("2026-07-08T12:34:56Z");
  event->set_source_service("ipv4_filter");
  event->set_status(EventStatus::SUCCESS);

  auto *payload = event->mutable_payload();
  payload->set_raw_line("test log line");
  payload->set_filter_decision(FilterDecision::ACCEPTED);

  SaveEventsResponse response;

  shared::Result<> expected_result;
  expected_result.success = true;
  expected_result.error_message = "";

  EXPECT_CALL(*mock_save_, Execute(testing::_))
      .WillOnce(testing::Return(expected_result));

  auto status = service_->SaveEvent(&context_, &request, &response);

  EXPECT_TRUE(status.ok());
  EXPECT_TRUE(response.success());
  EXPECT_EQ(response.message(), "Event(s) received and saved successfully.");
}

TEST_F(GrpcEventServiceImplTest, SaveEventValidationError) {
  SaveEventsRequest request;
  SaveEventsResponse response;

  auto status = service_->SaveEvent(&context_, &request, &response);

  EXPECT_FALSE(status.ok());
  EXPECT_EQ(status.error_code(), grpc::INVALID_ARGUMENT);
  EXPECT_EQ(status.error_message(), "At least one event is required.");
}

TEST_F(GrpcEventServiceImplTest, GetEventsSuccess) {
  GetEventsRequest request;
  GetEventsResponse response;

  shared::Result<std::vector<dto::Event>> expected_result;
  expected_result.success = true;
  expected_result.data = std::vector<dto::Event>{};
  expected_result.data->push_back(CreateTestEvent());

  EXPECT_CALL(*mock_get_events_, Execute(testing::_))
      .WillOnce(testing::Return(expected_result));

  auto status = service_->GetEvents(&context_, &request, &response);

  EXPECT_TRUE(status.ok());
  EXPECT_TRUE(response.success());
  EXPECT_EQ(response.events_size(), 1);
}

TEST_F(GrpcEventServiceImplTest, GetStatsSuccess) {
  GetStatsRequest request;
  GetStatsResponse response;

  shared::Result<dto::Stats> expected_result;
  expected_result.success = true;
  expected_result.data = dto::Stats{};

  EXPECT_CALL(*mock_get_stats_, Execute())
      .WillOnce(testing::Return(expected_result));

  auto status = service_->GetStats(&context_, &request, &response);

  EXPECT_TRUE(status.ok());
  EXPECT_TRUE(response.success());
  EXPECT_TRUE(response.has_stats());
}

TEST_F(GrpcEventServiceImplTest, GetStatsError) {
  GetStatsRequest request;
  GetStatsResponse response;

  shared::Result<dto::Stats> expected_result;
  expected_result.success = false;
  expected_result.error_message = "Some error";
  expected_result.data = std::nullopt;

  EXPECT_CALL(*mock_get_stats_, Execute())
      .WillOnce(testing::Return(expected_result));

  auto status = service_->GetStats(&context_, &request, &response);

  EXPECT_TRUE(status.ok());
  EXPECT_FALSE(response.success());
  EXPECT_EQ(response.message(), "Some error");
}