/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_PL_TAGINFO_DTO_HPP
#define CREST_PL_TAGINFO_DTO_HPP

#include <CrestApi/CrestException.h>

#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class PayloadTagInfoDto {
 private:
  std::string tagname;
  int niovs;
  double totvolume;
  double avgvolume;

 public:
  // Ctor
  PayloadTagInfoDto(const std::string &tagname, int niovs, double totvolume,
                    double avgvolume)
      : tagname(tagname),
        niovs(niovs),
        totvolume(totvolume),
        avgvolume(avgvolume) {}

  // Default Ctor
  PayloadTagInfoDto() : tagname(""), niovs(0), totvolume(0.0), avgvolume(0.0) {}

  // getters
  const std::string &getTagName() const { return tagname; }
  int getNiovs() const { return niovs; }
  double getTotVolume() const { return totvolume; }
  double getAvgVolume() const { return avgvolume; }

  // setters
  void setTagName(const std::string &tagname) { this->tagname = tagname; }
  void setNiovs(int niovs) { this->niovs = niovs; }
  void setTotVolume(double totvolume) { this->totvolume = totvolume; }
  void setAvgVolume(double avgvolume) { this->avgvolume = avgvolume; }

  json toJson() const;
  static PayloadTagInfoDto fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_PL_TAGINFO_DTO_HPP
