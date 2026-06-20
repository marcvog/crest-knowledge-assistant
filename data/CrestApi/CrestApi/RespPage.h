/*
   Copyright (C) 2019-2024 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CREST_RESP_PAGE_HPP
#define CREST_RESP_PAGE_HPP

#include <CrestApi/CrestException.h>

#include <string>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Crest {

class RespPage {
 private:
  int size;
  int64_t totalElements;
  int totalPages;
  int number;

 public:
  RespPage() : size(0), totalElements(0), totalPages(0), number(0) {}

  RespPage(int size, int64_t totalElements, int totalPages, int number)
      : size(size),
        totalElements(totalElements),
        totalPages(totalPages),
        number(number) {}

  int getSize() const { return size; }
  int64_t getTotalElements() const { return totalElements; }
  int getTotalPages() const { return totalPages; }
  int getNumber() const { return number; }

  // Define setters
  void setSize(int size) { this->size = size; }
  void setTotalElements(int64_t totalElements) {
    this->totalElements = totalElements;
  }
  void setTotalPages(int totalPages) { this->totalPages = totalPages; }
  void setNumber(int number) { this->number = number; }

  json toJson() const;
  static RespPage fromJson(const json &j);
};

}  // namespace Crest

#endif  // CREST_RESP_PAGE_HPP
