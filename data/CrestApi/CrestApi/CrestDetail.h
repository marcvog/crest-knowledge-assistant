#ifndef CREST_DETAIL_H
#define CREST_DETAIL_H

#include <array>
#include <string_view>

namespace Crest {
namespace Detail {

inline static const std::string s_CREST_CLIENT_VERSION = "6.0";

// Define the enum inside the namespace
enum UrlPathIndex {
  CREST_ROOT,
  GLOBALTAGS,
  GLOBALTAGMAPS,
  TAGS,
  IOVS,
  PAYLOADS,
  MONITORING_PAYLOADS,
  META,
  DATA,
  SIZE,
  FOLDERS,
  RUNINFO,
  RUNINFO_LIST,
  ADMIN,
  MGMT,
  MGMT_INFO,
  ACTUATOR_INFO,
  COUNT  // Helps in defining array size and future-proofing
};

// Declare the array in the header with constexpr for internal linkage
constexpr std::array<std::string_view, UrlPathIndex::COUNT> urlPaths = {
    "/api-v6.0",
    "/globaltags",
    "/globaltagmaps",
    "/tags",
    "/iovs",
    "/payloads",
    "/monitoring/payloads",
    "/meta",
    "/data",
    "/size",
    "/folders",
    "/runinfo",
    "/runinfo/list",
    "/admin",
    "/mgmt",
    "/mgmt/info",
    "/actuator/info"};

enum IovMethodIndex {
  METHOD_GROUPS,
  METHOD_IOVS,
  METHOD_COUNT  // Helps in defining array size and future-proofing
};

// Declare the array in the header with constexpr for internal linkage
constexpr std::array<std::string_view, IovMethodIndex::METHOD_COUNT> iovMethod =
    {"GROUPS", "IOVS"};

}  // namespace Detail
}  // namespace Crest

#endif  // CREST_DETAIL_H
