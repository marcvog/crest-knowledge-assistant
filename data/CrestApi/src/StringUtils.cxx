#include <CrestApi/CrestException.h>
#include <CrestApi/StringUtils.h>

#include <algorithm>
#include <sstream>

using namespace Crest::StringUtils;

std::string Crest::StringUtils::makeUrl(const std::string &prefix,
                                        const std::string &host,
                                        const std::string &port,
                                        const std::string &address) {
  std::string str = prefix;
  str += "://";
  str += host;
  str += ':';
  str += port;
  str += address;
  return str;
}

// Helper function to parse the port from a string view
std::optional<int> Crest::StringUtils::parsePort(std::string_view portStr) {
  if (portStr.empty())
    return std::nullopt;
  try {
    int port = std::stoi(std::string(portStr));
    return port;
  } catch (const std::invalid_argument &e) {
    return std::nullopt;  // Not a valid port number
  }
}

// Function to parse the URL
ParsedUrl Crest::StringUtils::parseUrl(std::string_view url) {
  ParsedUrl result;

  // Find the protocol part, e.g., "http"
  size_t protocolEnd = url.find("://");
  if (protocolEnd != std::string_view::npos) {
    result.protocol = url.substr(0, protocolEnd);
    url.remove_prefix(protocolEnd + 3);  // Remove "://"
  }

  // Find the host part and possible port delimiter ':'
  size_t hostEnd = url.find_first_of(":/");
  result.host = url.substr(0, hostEnd);
  url.remove_prefix(hostEnd);  // Update url to start after the host

  // Check for port
  if (!url.empty() && url[0] == ':') {
    size_t portEnd = url.find('/', 1);
    std::string_view portStr = url.substr(1, portEnd - 1);
    result.port = parsePort(portStr);
    url.remove_prefix(portEnd);  // Update url to start after the port
  } else {
    // Assign default port based on protocol if not explicitly defined
    if (result.protocol == "http") {
      result.port = 80;
    } else if (result.protocol == "https") {
      result.port = 443;
    }
  }

  // The remaining part is the apipath
  if (!url.empty() && url[0] == '/') {
    result.apipath = url;
  }

  return result;
}

const std::string Crest::StringUtils::appendEndpoint(
    const std::string &url, const std::string &endpoint) {
  std::string sanitisedPath(url);

  auto caretPosition = sanitisedPath.find('^');
  if (caretPosition != std::string::npos)
    sanitisedPath.replace(caretPosition, 1, "%5E");

  std::string sanitisedEndpoint(endpoint);
  auto caretPosition2 = sanitisedEndpoint.find('^');
  if (caretPosition2 != std::string::npos)
    sanitisedEndpoint.replace(caretPosition2, 1, "%5E");

  std::string result = sanitisedPath;
  if (result.back() != '/' && sanitisedEndpoint.front() != '/') {
    result += '/';
  }
  result += sanitisedEndpoint;
  return result;
}

const std::string Crest::StringUtils::urlEncode(const std::string &value) {
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex << std::uppercase;

  // If the character is alphanumeric or one of the allowed characters, add it
  // to the escaped string Otherwise, escape it using the URL encoding format: %
  // followed by hex value of the character, e.g. %20 for space, %25 for percent
  // sign, etc.
  for (unsigned char c : value) {
    if (std::isalnum(c) || (c == '-') || (c == '_') || (c == '.') ||
        (c == '~')) {
      escaped << c;
    } else {
      escaped << '%' << std::setw(2) << (int)c;
    }
  }

  return escaped.str();
}

const std::string Crest::StringUtils::removeCR(const std::string &str) {
  // Copy the input string
  std::string str2 = str;
  // Replace all '\n' with '|'
  std::replace(str2.begin(), str2.end(), '\n', '|');

  // Erase all occurrences of '\r'
  str2.erase(std::remove(str2.begin(), str2.end(), '\r'), str2.end());

  return str2;
}

std::string Crest::StringUtils::parseXMLOutput(
    const std::string_view xmlBuffer) {
  bool copy = true;

  std::string plainString = "";

  // remove all xml tags
  for (long unsigned int i = 0; i < xmlBuffer.length(); i++) {
    char convertc = xmlBuffer[i];

    if (convertc == '<')
      copy = false;
    else if (convertc == '>') {
      copy = true;
      continue;
    }

    if (copy)
      plainString += convertc;
  }

  return plainString;
}

// Add trim function to std::string
std::string &Crest::StringUtils::trim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
  return s;
}

std::string Crest::StringUtils::getWarningString(
    const std::string &message, const std::source_location &location) {
  std::string function_name = location.function_name();
  std::string response =
      "ERROR " + function_name + " | CREST Server response : " + message;
  return response;
}

const std::vector<std::string> Crest::StringUtils::split(
    const std::string &s, const std::string &delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

const std::string Crest::StringUtils::extractName(
    const std::string &full_name) {
  std::string name = "";
  std::vector<std::string> v = Crest::StringUtils::split(full_name, "(");
  std::string name1 = v[0];

  v = Crest::StringUtils::split(name1, " ");
  name = v[v.size() - 1];

  return name;
}

const std::string Crest::StringUtils::getCurrentMethod(
    const std::source_location &location) {
  std::string function = location.function_name();
  return extractName(function);
}

/**
 * getJson method is used to convert the string to the json object.
 * @param str - string to be converted to the json object.
 * @param location - location, where this metod was called.
 * @return json object.
 */
nlohmann::json Crest::StringUtils::getJson(
    const std::string &str, const std::source_location &location) {
  const std::string method = StringUtils::getCurrentMethod(location);

  nlohmann::json js = nullptr;
  try {
    js = nlohmann::json::parse(str);
  } catch (nlohmann::json::parse_error &e) {
    std::string str2 = StringUtils::parseXMLOutput(
        str);  // to remove HTML tags use this function
    std::string str3 =
        StringUtils::removeCR(str2);  // to remove carridge return
    throw CrestException("ERROR in " + method +
                         " | CREST Server response : " + str3);
  } catch (const std::exception &e) {
    throw CrestException("ERROR in CrestApi::" + method + ": " + e.what());
  }
  return js;
}

/**
 * toJson method is used to convert the string to the json object.
 * @param str - string to be converted to the json object.
 * @return json object.
 */
nlohmann::json Crest::StringUtils::toJson(const std::string &str) {
  nlohmann::json js = nullptr;
  try {
    js = nlohmann::json::parse(str);
  } catch (nlohmann::json::parse_error &e) {
    // method name is undefined
    std::string wh = e.what();
    throw CrestException("ERROR in JSON conversion: " + wh +
                         " | In string: " + str);
  } catch (const std::exception &e) {
    std::string wh = e.what();
    throw CrestException("ERROR in JSON: " + wh + " | In string: " + str);
  }
  return js;
}

std::string Crest::StringUtils::getDateAndTime() {
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];

  localtime_r(&now, &tstruct);
  strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
  return buf;
}
