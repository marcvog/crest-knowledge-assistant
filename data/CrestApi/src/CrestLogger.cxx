#include "CrestApi/CrestLogger.h"

#include "CrestApi/StringUtils.h"

namespace Crest {

LogLevel Logger::currentLogLevel = LogLevel::INFO;

void Logger::setLogLevel(LogLevel level) {
  currentLogLevel = level;
}

void Logger::setLogLevelFromEnv() {
  const char *envLogLevel = std::getenv("CREST_LOG_LEVEL");
  if (envLogLevel) {
    std::string levelStr(envLogLevel);
    if (levelStr == "VERBOSE")
      currentLogLevel = LogLevel::VERBOSE;
    else if (levelStr == "DEBUG")
      currentLogLevel = LogLevel::DEBUG;
    else if (levelStr == "INFO")
      currentLogLevel = LogLevel::INFO;
    else if (levelStr == "WARNING")
      currentLogLevel = LogLevel::WARNING;
    else if (levelStr == "ERROR")
      currentLogLevel = LogLevel::ERROR;
    else if (levelStr == "FATAL")
      currentLogLevel = LogLevel::FATAL;
    else if (levelStr == "ALWAYS")
      currentLogLevel = LogLevel::ALWAYS;
    else
      currentLogLevel = LogLevel::NIL;  // default to NIL
  }
}

void Logger::log(LogLevel level, const std::string &message) {
  if ((currentLogLevel != LogLevel::NIL) && (level >= currentLogLevel)) {
    switch (level) {
      case LogLevel::VERBOSE:
        std::cout << StringUtils::getDateAndTime() << " [VERBOSE] " << message
                  << std::endl;
        break;
      case LogLevel::DEBUG:
        std::cout << StringUtils::getDateAndTime() << " [DEBUG] " << message
                  << std::endl;
        break;
      case LogLevel::INFO:
        std::cout << StringUtils::getDateAndTime() << " [INFO] " << message
                  << std::endl;
        break;
      case LogLevel::WARNING:
        std::cout << StringUtils::getDateAndTime() << " [WARNING] " << message
                  << std::endl;
        break;
      case LogLevel::ERROR:
        std::cout << StringUtils::getDateAndTime() << " [ERROR] " << message
                  << std::endl;
        break;
      case LogLevel::FATAL:
        std::cout << StringUtils::getDateAndTime() << " [FATAL] " << message
                  << std::endl;
        break;
      case LogLevel::ALWAYS:
        std::cout << StringUtils::getDateAndTime() << " [ALWAYS] " << message
                  << std::endl;
        break;
      default:
        break;
    }
  }
}

}  // namespace Crest
