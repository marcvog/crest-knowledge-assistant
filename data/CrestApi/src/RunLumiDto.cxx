/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/RunLumiDto.h>

namespace Crest {

RunLumiDto::RunLumiDto() : runNumber(0), lb(0), starttime(0), endtime(0) {}

json RunLumiDto::toJson() const {
  json rl = {};
  rl["runNumber"] = runNumber;
  rl["lb"] = lb;
  rl["starttime"] = starttime;
  rl["endtime"] = endtime;
  return rl;
}

RunLumiDto RunLumiDto::fromJson(const json &j) {
  RunLumiDto rl;
  rl.runNumber = j.value<uint64_t>("runNumber", 0);
  rl.lb = j.value<uint64_t>("lb", 0);
  rl.starttime = j.value<uint64_t>("starttime", 0);
  rl.endtime = j.value<uint64_t>("endtime", 0);
  return rl;
}

}  // namespace Crest
