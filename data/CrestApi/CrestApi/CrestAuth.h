// CrestAuth.h
#ifndef CRESTAPI_AUTH_H
#define CRESTAPI_AUTH_H

#include <curl/curl.h>

#include <ctime>
#include <nlohmann/json.hpp>
#include <string>

namespace Crest {

/**
 * Class handling authentication for CREST requests.
 * It loads, refreshes, and exchanges tokens used to authorize access.
 */
class CrestAuth {
 private:
  std::string m_tokenFilePath;
  std::string m_tokenEndpoint;
  std::string m_apiEndpoint;
  std::string m_clientId;
  std::string m_audience;

  std::string m_exchangeToken;
  std::string m_refreshToken;
  std::string m_finalAccessToken;

  std::time_t m_exchangeTokenExpiresAt;

  /**
   * Save the full token JSON response to the configured token file.
   * @param tokenJson Full JSON response from the authentication service.
   * @return true if saving was successful, false otherwise.
   */
  bool saveTokens(const nlohmann::json& tokenJson) const;

  /**
   * Sends a form-encoded POST request using libcurl to the given URL.
   * @param url Target endpoint URL.
   * @param postFields POST body formatted as application/x-www-form-urlencoded.
   * @return The response body as a string, or empty string on failure.
   */
  std::string postRequest(const std::string& url,
                          const std::string& postFields) const;

  /**
   * libcurl write callback to accumulate received data into a string.
   * @param contents Raw data pointer.
   * @param size Size of each data chunk.
   * @param nmemb Number of data chunks.
   * @param userp Pointer to std::string used to collect output.
   * @return Total bytes handled (size * nmemb).
   */
  static size_t WriteCallback(void* contents, size_t size, size_t nmemb,
                              void* userp);

 public:
  /**
   * Default constructor. Initializes internal variables from default
   * environment-based settings or hardcoded defaults.
   */
  CrestAuth();

  /**
   * Loads the exchange and refresh tokens from a file on disk.
   * Sets internal state based on the saved token data.
   * @return true if loading and parsing succeeded; false if file missing or
   * malformed.
   */
  bool loadTokens();

  /**
   * Uses the current refresh token to request a new exchange token from the
   * authentication service. Stores the entire response to the token file and
   * updates internal state.
   * @return true if the refresh was successful, false otherwise.
   */
  bool refreshExchangeToken();

  /**
   * Exchanges the current exchange token for a final access token used for
   * authorization. This does not persist the access token to file.
   * @return true if access token was successfully acquired, false otherwise.
   */
  bool exchangeTokenForAccess();

  /**
   * Checks whether the current exchange token is expired based on the 'exp'
   * claim.
   * @return true if the exchange token is expired or invalid.
   */
  bool isExchangeTokenExpired() const;

  /**
   * Returns the currently cached access token. This method does not initiate a
   * new exchange.
   * @return Access token string if previously acquired; otherwise an empty
   * string.
   */
  std::string getAccessToken() const;

  /**
   * Initializes exchange credentials:
   * - Loads exchange and refresh tokens from file
   * - Refreshes the exchange token if expired
   * - Performs token exchange to obtain an access token
   * @return true if all steps succeed; throws std::runtime_error on fatal
   * errors.
   */
  bool setupExchangeCredentials();

  /**
   * Requests an access token using the OAuth2 client credentials flow.
   * Reads client ID and client secret from environment variables.
   * Stores the resulting access token in memory.
   *
   * Environment variables:
   * - CREST_CLIENT_ID
   * - CREST_CLIENT_SECRET
   *
   * @return true if the token was successfully obtained and stored, false
   * otherwise.
   */
  bool setupClientCredentials();
};

}  // namespace Crest

#endif  // CRESTAPI_AUTH_H
