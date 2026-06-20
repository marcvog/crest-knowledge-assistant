////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2026 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "CrestApi/RunLumiSetDto.h"

using namespace Crest;

TEST(CrestDtoTest, RunLumiDtoTest) {
  // parameters for RunLumiDto:
  uint64_t run_number_a = 10;
  uint64_t lb_a = 2000;
  uint64_t start_time_a = 101010;
  uint64_t end_time_a = 202020;

  // parameters (to update data):
  uint64_t run_number_b = 50;
  uint64_t lb_b = 5000;
  uint64_t start_time_b = 501010;
  uint64_t end_time_b = 502020;

  // RunLumiDto A:
  RunLumiDto rldtoA(run_number_a, lb_a, start_time_a, end_time_a);

  std::cout << "RunLumiDto A = " << std::endl
            << rldtoA.toJson().dump(4) << std::endl;

  EXPECT_EQ(rldtoA.getRunNumber(), run_number_a);
  EXPECT_EQ(rldtoA.getLumiBlock(), lb_a);
  EXPECT_EQ(rldtoA.getStartTime(), start_time_a);
  EXPECT_EQ(rldtoA.getEndTime(), end_time_a);

  // RunLumiDto B:
  nlohmann::json rljs_a = {{"runNumber", run_number_a},
                           {"lb", lb_a},
                           {"starttime", start_time_a},
                           {"endtime", end_time_a}};

  RunLumiDto rldtoB;
  rldtoB = rldtoB.fromJson(rljs_a);

  EXPECT_EQ(rldtoB.getRunNumber(), run_number_a);
  EXPECT_EQ(rldtoB.getLumiBlock(), lb_a);
  EXPECT_EQ(rldtoB.getStartTime(), start_time_a);
  EXPECT_EQ(rldtoB.getEndTime(), end_time_a);

  // JSON check:
  nlohmann::json patchA =
      nlohmann::json::diff(rldtoA.toJson(), rldtoB.toJson());
  std::cout << "patch = " << patchA << std::endl;
  EXPECT_TRUE(patchA.empty())
      << "The RunLumiDtos in test are not identical." << std::endl;

  // update the parameters with setters:
  rldtoB.setRunNumber(run_number_b);
  rldtoB.setLumiBlock(lb_b);
  rldtoB.setStartTime(start_time_b);
  rldtoB.setEndTime(end_time_b);

  // check parapeters:
  EXPECT_EQ(rldtoB.getRunNumber(), run_number_b);
  EXPECT_EQ(rldtoB.getLumiBlock(), lb_b);
  EXPECT_EQ(rldtoB.getStartTime(), start_time_b);
  EXPECT_EQ(rldtoB.getEndTime(), end_time_b);
}

TEST(CrestDtoTest, RunLumiSetDtoTest) {
  // parameters for RunLumiDto:
  uint64_t run_number[] = {10, 50, 70};
  uint64_t lb[] = {2000, 5000, 7000};
  uint64_t start_time[] = {101010, 501010, 701010};
  uint64_t end_time[] = {202020, 402020, 602020};

  // RunLumiSetDto A:
  std::vector<RunLumiDto> res;
  for (int i = 0; i <= 1; ++i) {
    RunLumiDto rlumi(run_number[i], lb[i], start_time[i], end_time[i]);
    res.push_back(rlumi);
  }
  RunLumiSetDto rlumiSetA(res);

  std::cout << "RunLumiSetDto A = " << std::endl
            << rlumiSetA.toJson().dump(4) << std::endl;

  EXPECT_EQ(rlumiSetA.size(), 2);
  EXPECT_EQ(rlumiSetA.getFormat(), "RunLumiSetDto");

  // add a new RunLumiDto:
  RunLumiDto rlumiN(run_number[2], lb[2], start_time[2], end_time[2]);
  rlumiSetA.add(rlumiN);

  // check parameters:
  std::vector<RunLumiDto> data = rlumiSetA.getResources();
  for (int i = 0; i <= 2; ++i) {
    EXPECT_EQ(data[i].getRunNumber(), run_number[i]);
    EXPECT_EQ(data[i].getLumiBlock(), lb[i]);
    EXPECT_EQ(data[i].getStartTime(), start_time[i]);
    EXPECT_EQ(data[i].getEndTime(), end_time[i]);
  }

  // JSON check:
  nlohmann::json resB_js = nlohmann::json::array();
  for (int i = 0; i <= 2; ++i) {
    nlohmann::json rl_js = {{"runNumber", run_number[i]},
                            {"lb", lb[i]},
                            {"starttime", start_time[i]},
                            {"endtime", end_time[i]}};
    resB_js.push_back(rl_js);
  }

  nlohmann::json rlumi_js = {
      {"size", 2}, {"format", "RunLumiSetDto"}, {"resources", resB_js}};

  RunLumiSetDto rlumiSetB;
  rlumiSetB = rlumiSetB.fromJson(rlumi_js);

  // JSON comparison:
  nlohmann::json patch =
      nlohmann::json::diff(rlumiSetA.toJson(), rlumiSetB.toJson());
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty())
      << "The  RunLumiSetDtos in test are not identical.";

  // parameter check:

  std::vector<RunLumiDto> dataB = rlumiSetB.getResources();
  for (int i = 0; i <= 2; ++i) {
    EXPECT_EQ(data[i].getRunNumber(), dataB[i].getRunNumber());
    EXPECT_EQ(data[i].getLumiBlock(), dataB[i].getLumiBlock());
    EXPECT_EQ(data[i].getStartTime(), dataB[i].getStartTime());
    EXPECT_EQ(data[i].getEndTime(), dataB[i].getEndTime());
  }
}
