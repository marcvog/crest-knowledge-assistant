/*
  Copyright (C) 2020-2024 CERN for the benefit of the ATLAS collaboration
*/

#include <CrestApi/PayloadDto.h>

namespace Crest {

// Function to serialize the object to JSON

json PayloadDto::toJson() const {
  json payloadDto = {};
  payloadDto["hash"] = hash;
  payloadDto["version"] = version;
  payloadDto["objectType"] = objectType;
  payloadDto["objectName"] = objectName;
  payloadDto["compressionType"] = compressionType;
  payloadDto["checkSum"] = checkSum;
  payloadDto["size"] = size();
  if (insertionTime.has_value())
    payloadDto["insertionTime"] = insertionTime.value();
  return payloadDto;
}

// Function to deserialize the object from JSON

PayloadDto PayloadDto::fromJson(const json &j) {
  PayloadDto payloadDto;
  payloadDto.hash = j.value("hash", "");
  payloadDto.version = j.value("version", "");
  payloadDto.objectType = j.value("objectType", "");
  payloadDto.objectName = j.value("objectName", "");
  payloadDto.compressionType = j.value("compressionType", "");
  payloadDto.checkSum = j.value("checkSum", "");
  payloadDto.size(j.value("size", 0));
  if (j.contains(std::string{"insertionTime"}))
    payloadDto.insertionTime = j.value("insertionTime", "");
  return payloadDto;
}

}  // namespace Crest
