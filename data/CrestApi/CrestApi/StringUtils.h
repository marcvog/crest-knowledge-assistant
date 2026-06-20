#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <iostream>
#include <optional>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

#include "nlohmann/json.hpp"

namespace Crest {
namespace StringUtils {

struct ParsedUrl {
  std::string protocol;
  std::string host;
  std::string apipath;
  std::optional<int> port;  // Port is optional
};
// Concatenates parts of a URL with slashes
/**
 * This method concatenates parts of a URL with slashes.
 * @param prefix - the prefix of the URL (http or https).
 * @param host - the host name.
 * @param port - the port number.
 * @param address - the address. Can be empty.
 */
std::string makeUrl(const std::string &prefix, const std::string &host,
                    const std::string &port, const std::string &address);

/**
 * Append an endpoint to a URL.
 * It will sanitize the URL by adding a slash if needed.
 * @param url - the URL.
 * @param endpoint - the endpoint to append.
 * @return the URL with the endpoint appended.
 */
const std::string appendEndpoint(const std::string &url,
                                 const std::string &endpoint);

/**
 * This method parses a Port string and returns the port if any.
 * @param portStr - the port string.
 * @return the port number or std::nullopt if the string is empty or not a valid
 * port number.
 */
std::optional<int> parsePort(std::string_view portStr);

/**
 * This method parses a URL and returns the protocol, host, and port.
 * @param url - the URL string.
 * @return a ParsedUrl struct with the protocol, host, and port.
 */
ParsedUrl parseUrl(std::string_view url);

// Encodes special characters in a URL component
const std::string urlEncode(const std::string &value);

// remove CR
/**
 * This method removes all end of line and carriage return symbols from a
 * string. (It is an auxiliary method to clear the CREST Server response.)
 * @param str - the text (a std::string ) to be cleared.
 */
const std::string removeCR(const std::string &str);

/**
 * This method removes all XML/HTML tags from a string.
 * (It is an auxiliary method to clear the CREST Server response.)
 * @param xmlBuffer - the text (a std::string ) to be cleared.
 * @return the plain text without tags.
 */
std::string parseXMLOutput(const std::string_view xmlBuffer);

std::string &trim(std::string &s);

std::string getWarningString(const std::string &message,
                             const std::source_location &location);

const std::vector<std::string> split(const std::string &s,
                                     const std::string &delimiter);

const std::string extractName(const std::string &full_name);

const std::string getCurrentMethod(
    const std::source_location &location = std::source_location::current());

/**
 * Auxiliary method to convert string in to JSON object.
 * @param str - string (std::string)
 * @param location - location, where this metod was called.
 * @return - JSON object as nlohmann::json
 *
 */
nlohmann::json getJson(
    const std::string &str,
    const std::source_location &location = std::source_location::current());

/**
 * Auxiliary method is used to convert the string to the json object.
 * @param str - string to be converted to the json object.
 * @return json object.
 */
nlohmann::json toJson(const std::string &str);

/**
 * Auxiliary method to get the current date and time as string.
 * @return date and time as std::string.
 */
std::string getDateAndTime();

// Other helper functions as needed

}  // namespace StringUtils
}  // namespace Crest

#endif  // STRING_UTILS_H
