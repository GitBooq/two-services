#include <gtest/gtest.h>

#include "utils.h"

using namespace net::logger;
using namespace utils::builder;

TEST(UtilsTests, FormatTimestampReturnsCorrectStr) {
  // 1993-10-01 03:13:37 UTC
  std::chrono::system_clock::time_point timestamp;

  std::tm tm = {};
  tm.tm_year = 1993 - 1900; // 1993
  tm.tm_mon = 10 - 1;       // 0-based
  tm.tm_mday = 01;
  tm.tm_hour = 03;
  tm.tm_min = 13;
  tm.tm_sec = 37;

  std::time_t time_t = std::mktime(&tm);
  timestamp = std::chrono::system_clock::from_time_t(time_t);

  std::string result = FormatTimestamp(timestamp);

  EXPECT_EQ(result, "1993-10-01 03:13:37");
}

TEST(UtilsTest, FilterRejectReasonToStrRetrunsCorrectStr) {
  auto ivalid_format = RejectReason::InvalidFormat;
  auto ivalid_ip = RejectReason::InvalidIPAddress;
  auto filter_rejected = RejectReason::FilterRejected;
  auto none = RejectReason::None;

  auto ivalid_format_str = FilterRejectReasonToStr(ivalid_format);
  auto ivalid_ip_str = FilterRejectReasonToStr(ivalid_ip);
  auto filter_rejected_str = FilterRejectReasonToStr(filter_rejected);
  auto none_nullopt = FilterRejectReasonToStr(none);

  EXPECT_EQ(ivalid_format_str, std::string("Invalid Format"));
  EXPECT_EQ(ivalid_ip_str, std::string("InvalidIPAddress"));
  EXPECT_EQ(filter_rejected_str,std::string("FilterRejected"));
  EXPECT_EQ(none_nullopt,std::nullopt);
}

TEST(UtilsTest, GetFilterDecisionRetrunsCorrectStr) {
  auto ivalid_format = RejectReason::InvalidFormat;
  auto ivalid_ip = RejectReason::InvalidIPAddress;
  auto filter_rejected = RejectReason::FilterRejected;
  auto none = RejectReason::None;

  auto decision_from_inv_format_str = GetFilterDecision(ivalid_format);
  auto decision_from_ivalid_ip_str = GetFilterDecision(ivalid_ip);
  auto decision_from_filter_rejected_str = GetFilterDecision(filter_rejected);
  auto decision_from_none = GetFilterDecision(none);

  EXPECT_EQ(decision_from_inv_format_str, std::string(""));
  EXPECT_EQ(decision_from_ivalid_ip_str, std::string(""));
  EXPECT_EQ(decision_from_filter_rejected_str,std::string("rejected"));
  EXPECT_EQ(decision_from_none,std::string("accepted"));
}

TEST(UtilsTest, IPv4AddressToStringReturnsCorrectStr) {
    const uint32_t big_end_addr = 0xFF000001; // 255 0 0 1

    auto addr_str = IPv4AddressToString(big_end_addr);

    EXPECT_EQ(addr_str, std::string("255.0.0.1"));
}