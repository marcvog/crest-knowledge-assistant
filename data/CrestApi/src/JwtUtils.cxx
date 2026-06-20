#include <CrestApi/JwtUtils.h>

#include <algorithm>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>

namespace Crest {

std::string JwtUtils::base64urlDecode(const std::string& input) {
  std::string base64 = input;
  std::replace(base64.begin(), base64.end(), '-', '+');
  std::replace(base64.begin(), base64.end(), '_', '/');
  while (base64.length() % 4 != 0)
    base64 += '=';

  static constexpr unsigned char kDecodeTable[256] = {
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57,
      58, 59, 60, 61, 64, 64, 64, 0,  64, 64, 64, 0,  1,  2,  3,  4,  5,  6,
      7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
      25, 64, 64, 64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
      37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64};

  std::string output;
  size_t i = 0;
  while (i < base64.length()) {
    uint32_t val = 0;
    int valb = -8;
    for (int j = 0; j < 4 && i < base64.length(); ++j) {
      unsigned char c = base64[i++];
      if (kDecodeTable[c] == 64)
        throw std::runtime_error("Invalid base64url character");
      val = (val << 6) + kDecodeTable[c];
      valb += 6;
    }
    while (valb >= 0) {
      output.push_back(static_cast<char>((val >> valb) & 0xFF));
      valb -= 8;
    }
  }
  return output;
}

std::time_t JwtUtils::extractExp(const std::string& jwt, int graceSeconds) {
  size_t dot1 = jwt.find('.');
  size_t dot2 = jwt.find('.', dot1 + 1);
  if (dot1 == std::string::npos || dot2 == std::string::npos) {
    throw std::runtime_error("Invalid JWT format");
  }

  std::string payloadEncoded = jwt.substr(dot1 + 1, dot2 - dot1 - 1);
  std::string payloadJson = base64urlDecode(payloadEncoded);

  auto js = nlohmann::json::parse(payloadJson, nullptr, false);
  if (js.is_discarded()) {
    throw std::runtime_error("Failed to parse JWT payload");
  }

  if (!js.contains("exp") || !js["exp"].is_number()) {
    throw std::runtime_error("JWT payload does not contain valid 'exp'");
  }

  std::time_t rawExp = js["exp"];
  return rawExp - graceSeconds;
}

}  // namespace Crest
