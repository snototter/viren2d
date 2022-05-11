#ifndef __VIREN2D_LOGGING_H__
#define __VIREN2D_LOGGING_H__

#include <ostream>
#include <string>

namespace viren2d {

//TODO use consistent logging style: "Class::Method: parameters"
//TODO doc

enum class LogLevel : unsigned char {
  Off = 0,
  Trace,
  Debug,
  Info,
  Warn,
  Error
};


LogLevel LogLevelFromString(const std::string &lvl);

/** Returns the string representation. */
std::string LogLevelToString(LogLevel lvl);

/** Overloaded ostream operator. */
std::ostream &operator<<(std::ostream &os, LogLevel lvl);

//TODO
//returns true if the desired log level is >= the compilation time lowest level
// if false, you won't get any message at this, because the log level was disabled during compile time
bool SetLogLevel(LogLevel lvl);

bool SetLogLevel(const std::string &lvl);

} // namespace viren2d

#endif // __VIREN2D_LOGGING_H__
