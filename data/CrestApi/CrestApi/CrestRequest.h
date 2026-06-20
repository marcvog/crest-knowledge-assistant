/*
   Copyright (C) 2019-2025 CERN for the benefit of the ATLAS collaboration
 */

#ifndef CRESTAPI_REQUEST_H
#define CRESTAPI_REQUEST_H

#include <CrestApi/CrestAuth.h>
#include <curl/curl.h>

#include <cstdint>
#include <iosfwd>
#include <map>
#include <nlohmann/json.hpp>
#include <source_location>
#include <string>

namespace Crest {

// The action to be performed
enum class Action { GET = 0, POST = 1, PUT = 2, DELETE = 3 };
class urlParameters {
 private:
  std::string m_str;

 public:
  void add(const std::string &key, const std::string &value) {
    if (m_str.empty()) {
      m_str = key;
      m_str += '=';
      m_str += value;
    } else {
      m_str += '&';
      m_str += key;
      m_str += '=';
      m_str += value;
    }
  }

  const std::string &getParams() const { return m_str; }

  void reset() { m_str.clear(); }
};

class CrestRequest {

 private:
  std::string m_prefix = "http://";
  std::string m_host;
  std::string m_port;
  std::string m_url;

  CrestAuth m_auth;

  char *m_CREST_PROXY = NULL;
  const char *m_CREST_PROXY_VAR = "SOCKS_PROXY";

  // Auxiliary methods:

  bool isJson(const std::string &str);

  int checkErrors(const nlohmann::json &js, const char *method);
  std::string parseXMLOutput(const std::string_view xmlBuffer);
  std::string removeCR(const std::string &str);

  void getProxyPath();

 public:
  CrestRequest();
  CrestRequest(const std::string &prefix, const std::string &host,
               const std::string &port);
  CrestRequest(const std::string &url);

  ~CrestRequest();

  // Setters
  void setHost(const std::string &host);
  void setPort(const std::string &port);
  void setPrefix(const std::string &prefix);
  void setUrl(const std::string &url);

  // Getters
  std::string getUrl();

  /**
   * Initializes authentication and authorization for CREST requests.
   *
   * This method reads the specified execution mode (e.g., "JWT",
   * "CLIENT_CREDENTIALS") and sets up access credentials accordingly. It:
   * - Requests an access token via an exchange token if mode="JWT"
   * - Requests an access token via client id and secret if
   * mode="CLIENT_CREDENTIALS"
   *
   * If the mode is nullptr (i.e. environment variable not set), no special
   * behavior is applied.
   *
   * @param mode - A C-style string indicating the execution mode (e.g., from
   * the CREST_AUTH_MODE environment variable). If nullptr, the default behavior
   * is applied.
   *
   * @throws std::runtime_error if authentication fails critically (e.g.,
   * missing token file or failed token exchange).
   */
  void initAuth(const char *mode);

  /**
   * Creates an Authorization header using the current access token from
   * CrestAuth.
   * @return A curl_slist pointer containing the Authorization header, or
   * nullptr if no token is available.
   */
  struct curl_slist *createAuthHeader() const;

  /**
   * General auxillary method to make request to the CREST Server. This method
   * is used by other methods realizing the requests with the concrete kinds of
   * data (iovs|payloads|tags…).
   *
   * @param current_path - URL request path
   * @param action - Action (GET|POST|DELETE)
   * @param js - a JSON object which has to be send or changed with this
   * request. If this argument has to be void it has to be set as nullptr.
   * @param method_name - the name of method which called this method. This
   * parameter is used in the error messages.
   *
   */
  std::string performRequest(
      const std::string &current_path, Action action, nlohmann::json &js,
      const std::string &header_params = "",
      const std::source_location &location = std::source_location::current());

  std::vector<char> getPayloadRequest(const std::string &current_path);

  std::string uploadPayload(const std::string &current_path,
                            const std::string &tag, uint64_t endtime,
                            const nlohmann::json &js,
                            const std::string &objectType,
                            const std::string &compressionType,
                            const std::string &version,
                            const std::vector<std::string> &files);

  void checkResult(CURLcode res, long response_code, const std::string &st,
                   const char *method_name);
};
}  // namespace Crest

#endif  // CRESTAPI_REQUEST_H
