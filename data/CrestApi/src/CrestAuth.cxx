// CrestAuth.cxx
#include <CrestApi/CrestAuth.h>
#include <CrestApi/CrestLogger.h>
#include <CrestApi/JwtUtils.h>

#include <fstream>
#include <iostream>
#include <sstream>

namespace Crest {

CrestAuth::CrestAuth()
    : m_tokenFilePath(std::getenv("HOME") +
                      std::string("/.client-exchange-token.json")),
      m_tokenEndpoint(
          "https://auth.cern.ch/auth/realms/cern/protocol/openid-connect/"
          "token"),
      m_apiEndpoint("https://auth.cern.ch/auth/realms/cern/api-access/token"),
      m_clientId("crest-client"),
      m_audience("crest-server"),
      m_exchangeTokenExpiresAt(0) {}

bool CrestAuth::loadTokens() {
  std::ifstream file(m_tokenFilePath);
  if (!file.is_open()) {
    Logger::log(LogLevel::ERROR,
                "Failed to open token file: " + m_tokenFilePath);
    return false;
  }

  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  if (content.empty()) {
    Logger::log(LogLevel::ERROR, "Token file is empty");
    return false;
  }

  file.clear();
  file.seekg(0);

  nlohmann::json js;
  try {
    file >> js;
  } catch (const std::exception& e) {
    Logger::log(LogLevel::ERROR,
                "Failed to parse token JSON: " + (std::string)e.what());
    return false;
  }

  m_exchangeToken = js.value("access_token", "");
  m_refreshToken = js.value("refresh_token", "");
  try {
    m_exchangeTokenExpiresAt = JwtUtils::extractExp(m_exchangeToken, 30);
  } catch (const std::exception& e) {
    Logger::log(LogLevel::ERROR,
                "Failed to extract 'exp' from JWT: " + (std::string)e.what());
    m_exchangeTokenExpiresAt = std::time(nullptr) + 300;  // fallback
  }

  Logger::log(LogLevel::DEBUG, "Loaded exchange and refresh tokens from file");
  return true;
}

bool CrestAuth::saveTokens(const nlohmann::json& tokenJson) const {
  std::ofstream file(m_tokenFilePath);
  if (!file.is_open()) {
    Logger::log(LogLevel::ERROR,
                "Failed to open token file for writing: " + m_tokenFilePath);
    return false;
  }

  file << tokenJson.dump();
  Logger::log(LogLevel::DEBUG, "Saved exchanged token to file");
  return true;
}

bool CrestAuth::isExchangeTokenExpired() const {
  return std::time(nullptr) >= m_exchangeTokenExpiresAt;
}

std::string CrestAuth::getAccessToken() const {
  return m_finalAccessToken;
}

bool CrestAuth::refreshExchangeToken() {
  std::ostringstream oss;
  oss << "grant_type=refresh_token"
      << "&refresh_token=" << m_refreshToken << "&client_id=" << m_clientId;

  std::string response = postRequest(m_tokenEndpoint, oss.str());
  if (response.empty())
    return false;

  auto js = nlohmann::json::parse(response, nullptr, false);
  if (js.is_discarded() || js.contains("error")) {
    Logger::log(LogLevel::ERROR,
                "Failed to refresh exchange token: " + js.dump());
    return false;
  }

  m_exchangeToken = js["access_token"];
  m_refreshToken = js.value("refresh_token", m_refreshToken);
  try {
    m_exchangeTokenExpiresAt = JwtUtils::extractExp(m_exchangeToken, 30);
  } catch (const std::exception& e) {
    Logger::log(LogLevel::ERROR,
                "Failed to extract 'exp' from JWT: " + (std::string)e.what());
    m_exchangeTokenExpiresAt = std::time(nullptr) + 300;  // fallback
  }
  Logger::log(LogLevel::DEBUG, "Successfully refreshed exchange token");
  return saveTokens(js);
}

bool CrestAuth::exchangeTokenForAccess() {
  CURL* curl = curl_easy_init();
  if (!curl) {
    Logger::log(LogLevel::ERROR, "Failed to initialize CURL");
    return false;
  }

  if (m_exchangeToken.empty()) {
    Logger::log(LogLevel::ERROR, "Exchange token is empty");
    curl_easy_cleanup(curl);
    return false;
  }

  std::ostringstream oss;
  oss << "grant_type=urn:ietf:params:oauth:grant-type:token-exchange"
      << "&subject_token=" << m_exchangeToken
      << "&subject_token_type=urn:ietf:params:oauth:token-type:access_token"
      << "&client_id=" << m_clientId << "&audience=" << m_audience;

  std::string response = postRequest(m_tokenEndpoint, oss.str());
  curl_easy_cleanup(curl);

  if (response.empty())
    return false;

  auto js = nlohmann::json::parse(response, nullptr, false);
  if (js.is_discarded() || !js.contains("access_token")) {
    Logger::log(LogLevel::ERROR, "Token exchange failed: " + js.dump(2));
    return false;
  }

  m_finalAccessToken = js["access_token"];

  Logger::log(LogLevel::DEBUG, "Token exchanged successfully");

  return true;
}

std::string CrestAuth::postRequest(const std::string& url,
                                   const std::string& postFields) const {
  CURL* curl = curl_easy_init();
  if (!curl)
    return "";

  std::string response;
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(
      headers, "Content-Type: application/x-www-form-urlencoded");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::string errMsg = curl_easy_strerror(res);
    Logger::log(LogLevel::ERROR, "curl_easy_perform failed: " + errMsg);
  }

  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return response;
}

size_t CrestAuth::WriteCallback(void* contents, size_t size, size_t nmemb,
                                void* userp) {
  size_t totalSize = size * nmemb;
  std::string* out = static_cast<std::string*>(userp);
  out->append(static_cast<char*>(contents), totalSize);
  return totalSize;
}

bool CrestAuth::setupExchangeCredentials() {
  Logger::log(LogLevel::DEBUG,
              "Setting up authorization - requesting access token...");

  bool tokensLoaded = loadTokens();
  if (!tokensLoaded) {
    Logger::log(LogLevel::ERROR,
                "No valid token file found.\nExecute: auth-get-user-token -c "
                "crest-client -o ~/.client-exchange-token.json");
    return false;
  }

  Logger::log(LogLevel::DEBUG, "Checking if exchange token is expired...");
  if (isExchangeTokenExpired()) {
    Logger::log(LogLevel::DEBUG,
                "Exchange token is expired. Attempting refresh...");
    if (!refreshExchangeToken()) {
      Logger::log(LogLevel::ERROR,
                  "Exchange token is expired and refresh failed.\nExecute: "
                  "auth-get-user-token -c crest-client -o "
                  "~/.client-exchange-token.json");
      return false;
    }
  }

  if (!exchangeTokenForAccess()) {
    Logger::log(LogLevel::ERROR, "Failed to obtain access token from exchange");
    return false;
  }

  Logger::log(LogLevel::DEBUG, "Setup complete — access token obtained");
  return true;
}

bool CrestAuth::setupClientCredentials() {
  const char* clientId = std::getenv("CREST_CLIENT_ID");
  const char* clientSecret = std::getenv("CREST_CLIENT_SECRET");

  if (!clientId || !clientSecret) {
    Logger::log(
        LogLevel::ERROR,
        "Missing CREST_CLIENT_ID or CREST_CLIENT_SECRET environment variable.");
    return false;
  }

  std::ostringstream oss;
  oss << "grant_type=client_credentials"
      << "&client_id=" << clientId << "&client_secret=" << clientSecret
      << "&subject_token_type=urn:ietf:params:oauth:token-type:access_token"
      << "scope=openid"
      << "&audience=" << m_audience;

  std::string response = postRequest(m_apiEndpoint, oss.str());
  if (response.empty()) {
    Logger::log(LogLevel::ERROR,
                "No response received from client credentials request.");
    return false;
  }

  auto js = nlohmann::json::parse(response, nullptr, false);
  if (js.is_discarded() || !js.contains("access_token")) {
    Logger::log(LogLevel::ERROR,
                "Failed to obtain access token using client credentials: " +
                    js.dump(2));
    return false;
  }

  m_finalAccessToken = js["access_token"];

  Logger::log(LogLevel::DEBUG,
              "Access token acquired via client credentials flow");

  return true;
}
}  // namespace Crest
