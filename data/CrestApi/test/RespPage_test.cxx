////////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2002-2025 CERN for the benefit of the ATLAS collaboration
////////////////////////////////////////////////////////////////////////////////

#include "CrestApi/RespPage.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "CrestApi/CrestApi.h"

using namespace Crest;

TEST(CrestDtoTest, RespPageTest) {
  int size = 5;
  int64_t totalElements = 15;
  int totalPages = 2;
  int number = 1;

  // RespPage A:
  RespPage respPageA;
  std::cout << "Empty RespPage = " << std::endl
            << respPageA.toJson().dump(4) << std::endl;

  EXPECT_EQ(respPageA.getSize(), 0);
  EXPECT_EQ(respPageA.getTotalElements(), 0);
  EXPECT_EQ(respPageA.getTotalPages(), 0);
  EXPECT_EQ(respPageA.getNumber(), 0);

  respPageA.setSize(size);
  respPageA.setTotalElements(totalElements);
  respPageA.setTotalPages(totalPages);
  respPageA.setNumber(number);

  std::cout << "Empty RespPage A = " << std::endl
            << respPageA.toJson().dump(4) << std::endl;

  EXPECT_EQ(respPageA.getSize(), size);
  EXPECT_EQ(respPageA.getTotalElements(), totalElements);
  EXPECT_EQ(respPageA.getTotalPages(), totalPages);
  EXPECT_EQ(respPageA.getNumber(), number);

  // RespPage B:
  nlohmann::json jsB = {{"size", size},
                        {"totalElements", totalElements},
                        {"totalPages", totalPages},
                        {"number", number}};

  RespPage respPageB;
  respPageB = respPageB.fromJson(jsB);

  // JSON check:
  nlohmann::json patch =
      nlohmann::json::diff(respPageA.toJson(), respPageB.toJson());
  std::cout << "patch = " << patch << std::endl;
  EXPECT_TRUE(patch.empty()) << "The RespPages in test are not identical.";

  // Parameter comparison:
  EXPECT_EQ(respPageA.getSize(), respPageB.getSize());
  EXPECT_EQ(respPageA.getTotalElements(), respPageA.getTotalElements());
  EXPECT_EQ(respPageA.getTotalPages(), respPageA.getTotalPages());
  EXPECT_EQ(respPageA.getNumber(), respPageA.getNumber());
}
