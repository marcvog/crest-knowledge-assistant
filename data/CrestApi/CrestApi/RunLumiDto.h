/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_RUN_LUMI_DTO_HPP
#define CREST_RUN_LUMI_DTO_HPP

#include <CrestApi/CrestException.h>

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

using json = nlohmann::json;

namespace Crest {

/**
 * @brief The RunLumiDto class
 * It contains CTP information of run,lb,start and end time of each lumi block.
 *
 * @param runNumber The run number.
 * @param lb The lumi block number.
 * @param starttime The start time of the lumi block, in milliseconds since
 * epoch.
 * @param endtime The end time of the lumi block, in milliseconds since epoch.
 */

class RunLumiDto {
 private:
  uint64_t runNumber;
  uint64_t lb;
  uint64_t starttime;
  uint64_t endtime;

 public:
  // Ctor
  RunLumiDto(uint64_t runNumber, uint64_t lb, uint64_t starttime,
             uint64_t endtime)
      : runNumber(runNumber), lb(lb), starttime(starttime), endtime(endtime) {};
  // Default Ctor
  RunLumiDto();
  const uint64_t &getRunNumber() const { return runNumber; }
  const uint64_t &getLumiBlock() const { return lb; }
  const uint64_t &getStartTime() const { return starttime; }
  const uint64_t &getEndTime() const { return endtime; }
  // Define setters
  void setRunNumber(uint64_t runNumber) { this->runNumber = runNumber; }
  void setLumiBlock(uint64_t lb) { this->lb = lb; }
  void setStartTime(uint64_t starttime) { this->starttime = starttime; }
  void setEndTime(uint64_t endtime) { this->endtime = endtime; }

  json toJson() const;
  static RunLumiDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_RUN_LUMI_DTO_HPP
