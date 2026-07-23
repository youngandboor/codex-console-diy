#include <unity.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <string>
#include <vector>

#include "protocol/FrameCodec.h"

using codex::AppendResult;
using codex::FrameAssembler;
using codex::FrameEncoder;
using codex::ReportBody;

void test_small_message_round_trip() {
  const std::string json = "{\"method\":\"sys.version\",\"id\":1}";
  std::vector<ReportBody> reports;
  TEST_ASSERT_TRUE(FrameEncoder::encode(json, [&](const ReportBody& report) {
    reports.push_back(report);
    return true;
  }));
  TEST_ASSERT_EQUAL_UINT32(1, reports.size());
  TEST_ASSERT_EQUAL_UINT8(codex::kMessageType, reports[0][0]);
  TEST_ASSERT_EQUAL_UINT8(json.size() + 1, reports[0][1]);

  FrameAssembler assembler;
  TEST_ASSERT_EQUAL_INT(static_cast<int>(AppendResult::kAccepted),
                        static_cast<int>(assembler.append(reports[0].data(), reports[0].size())));
  TEST_ASSERT_EQUAL_STRING_LEN((json + "\n").c_str(), assembler.data().data(), json.size() + 1);
}

void test_fragmented_message_round_trip() {
  const std::string json = "{\"method\":\"v.oai.thstatus\",\"params\":[" +
                           std::string(180, 'x') + "]}";
  std::vector<ReportBody> reports;
  TEST_ASSERT_TRUE(FrameEncoder::encode(json, [&](const ReportBody& report) {
    reports.push_back(report);
    return true;
  }));
  TEST_ASSERT_GREATER_THAN_UINT32(1, reports.size());

  FrameAssembler assembler;
  for (const ReportBody& report : reports) {
    TEST_ASSERT_EQUAL_INT(static_cast<int>(AppendResult::kAccepted),
                          static_cast<int>(assembler.append(report.data(), report.size())));
  }
  TEST_ASSERT_EQUAL_STRING_LEN((json + "\n").c_str(), assembler.data().data(), json.size() + 1);
}

void test_accepts_explicit_report_id() {
  ReportBody body{};
  const std::string json = "{\"method\":\"device.status\"}";
  body[0] = codex::kMessageType;
  body[1] = json.size();
  std::copy(json.begin(), json.end(), body.begin() + 2);

  std::array<uint8_t, codex::kReportBodySize + 1> raw{};
  raw[0] = codex::kReportId;
  std::copy(body.begin(), body.end(), raw.begin() + 1);

  FrameAssembler assembler;
  TEST_ASSERT_EQUAL_INT(static_cast<int>(AppendResult::kAccepted),
                        static_cast<int>(assembler.append(raw.data(), raw.size())));
  TEST_ASSERT_EQUAL_STRING_LEN(json.c_str(), assembler.data().data(), json.size());
}

void test_rejects_invalid_payload_length() {
  ReportBody report{};
  report[0] = codex::kMessageType;
  report[1] = codex::kPayloadSize + 1;
  FrameAssembler assembler;
  TEST_ASSERT_EQUAL_INT(static_cast<int>(AppendResult::kInvalid),
                        static_cast<int>(assembler.append(report.data(), report.size())));
}

void test_new_top_level_message_resynchronizes() {
  FrameAssembler assembler;
  ReportBody partial{};
  partial[0] = codex::kMessageType;
  partial[1] = 12;
  std::memcpy(partial.data() + 2, "{\"old\":true", 12);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(AppendResult::kAccepted),
                        static_cast<int>(assembler.append(partial.data(), partial.size())));

  ReportBody fresh{};
  const std::string json = "{\"method\":\"sys.version\"}";
  fresh[0] = codex::kMessageType;
  fresh[1] = json.size();
  std::copy(json.begin(), json.end(), fresh.begin() + 2);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(AppendResult::kAccepted),
                        static_cast<int>(assembler.append(fresh.data(), fresh.size())));
  TEST_ASSERT_EQUAL_STRING_LEN(json.c_str(), assembler.data().data(), json.size());
}

void test_overflow_clears_the_partial_message() {
  FrameAssembler assembler;
  ReportBody first{};
  first[0] = codex::kMessageType;
  first[1] = codex::kPayloadSize;
  first[2] = '{';
  std::fill(first.begin() + 3, first.end(), 'x');
  TEST_ASSERT_EQUAL_INT(static_cast<int>(AppendResult::kAccepted),
                        static_cast<int>(assembler.append(first.data(), first.size())));

  ReportBody continuation{};
  continuation[0] = codex::kMessageType;
  continuation[1] = codex::kPayloadSize;
  std::fill(continuation.begin() + 2, continuation.end(), 'x');

  AppendResult result = AppendResult::kAccepted;
  while (result == AppendResult::kAccepted) {
    result = assembler.append(continuation.data(), continuation.size());
  }
  TEST_ASSERT_EQUAL_INT(static_cast<int>(AppendResult::kOverflow),
                        static_cast<int>(result));
  TEST_ASSERT_EQUAL_UINT32(0, assembler.data().size());
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_small_message_round_trip);
  RUN_TEST(test_fragmented_message_round_trip);
  RUN_TEST(test_accepts_explicit_report_id);
  RUN_TEST(test_rejects_invalid_payload_length);
  RUN_TEST(test_new_top_level_message_resynchronizes);
  RUN_TEST(test_overflow_clears_the_partial_message);
  return UNITY_END();
}
