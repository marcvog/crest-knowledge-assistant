#pragma once

#ifndef CRESTAPI_LOGGER_H
#define CRESTAPI_LOGGER_H

#include <iostream>
#include <string>

namespace Crest {

// Message level definitions
enum class LogLevel {
  NIL = 0,
  VERBOSE,
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FATAL,
  ALWAYS,
  NUM_LEVELS
};

class Logger {
 public:
  /**
   * @brief Set the log level
   */
  static void setLogLevel(LogLevel level);
  /**
   * @brief Set the log level from the environment variable CREST_LOG_LEVEL
   *      Possible values are NIL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL,
   * ALWAYS
   *
   */
  static void setLogLevelFromEnv();
  /**
   * @brief Log a message with the given log level
   */
  static void log(LogLevel level, const std::string &message);

 private:
  static LogLevel currentLogLevel;
};
}  // namespace Crest
#endif  // CRESTAPI_LOGGER_H