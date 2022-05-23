#include <sstream>

#include <werkzeugkiste/strings/strings.h>

// Public viren2d headers
#include <viren2d/logging.h>
// Private viren2d headers
#include <helpers/logging.h>

namespace viren2d {
namespace  {
spdlog::level::level_enum ToSpdLogLevel(LogLevel lvl) {
  switch (lvl) {
    case LogLevel::Off:
      return spdlog::level::off;

    case LogLevel::Trace:
      return spdlog::level::trace;

    case LogLevel::Debug:
      return spdlog::level::debug;

    case LogLevel::Info:
      return spdlog::level::info;

    case LogLevel::Warn:
      return spdlog::level::warn;

    case LogLevel::Error:
      return spdlog::level::err;

    default: {
      std::ostringstream s;
      s << LogLevelToString(lvl)
        << " has not been mapped to spdlog::level::level_enum.";
      throw std::runtime_error(s.str());
    }
  }
}


/** Returns the corresponding spdlog preprocessor defined level. */
int ToSpdLogMacroLevel(LogLevel lvl) {
  switch (lvl) {
    case LogLevel::Off:
      return SPDLOG_LEVEL_OFF;

    case LogLevel::Trace:
      return SPDLOG_LEVEL_TRACE;

    case LogLevel::Debug:
      return SPDLOG_LEVEL_DEBUG;

    case LogLevel::Info:
      return SPDLOG_LEVEL_INFO;

    case LogLevel::Warn:
      return SPDLOG_LEVEL_WARN;

    case LogLevel::Error:
      return SPDLOG_LEVEL_ERROR;

    default: {
      std::ostringstream s;
      s << LogLevelToString(lvl)
        << " has not been mapped to its corresponding"
           "SPDLOG_LEVEL_xxx preprocessor definition.";
      throw std::runtime_error(s.str());
    }
  }
}

}  // anonymous namespace


LogLevel LogLevelFromString(const std::string &lvl) {
  SPDLOG_TRACE("LogLevelFromString(\"{:s}\").", lvl);

  std::string level = werkzeugkiste::strings::Lower(lvl);

  if (level.compare("off") == 0) {
    return LogLevel::Off;
  } else if (level.compare("trace") == 0) {
    return LogLevel::Trace;
  } else if (level.compare("debug") == 0) {
    return LogLevel::Debug;
  } else if ((level.compare("info") == 0)
             || (level.compare("information") == 0)) {
    return LogLevel::Info;
  } else if ((level.compare("warn") == 0)
             || (level.compare("warning") == 0)) {
    return LogLevel::Warn;
  } else if ((level.compare("error") == 0)
             || (level.compare("err") == 0)
             || (level.compare("failure") == 0)) {
    return LogLevel::Error;
  }

  std::ostringstream s;
  s << "Could not convert string \"" << lvl
    << "\" to LogLevel.";
  throw std::invalid_argument(s.str());
}


std::string LogLevelToString(LogLevel lvl) {
  switch(lvl) {
    case LogLevel::Off:
      return "off";

    case LogLevel::Trace:
      return "trace";

    case LogLevel::Debug:
      return "debug";

    case LogLevel::Info:
      return "info";

    case LogLevel::Warn:
      return "warn";

    case LogLevel::Error:
      return "error";

    default: {
      std::ostringstream s;
      s << "No string representation available for LogLevel ("
        << static_cast<int>(lvl) << ").";
      throw std::runtime_error(s.str());
    }
  }
}

std::ostream &operator<<(std::ostream &os, LogLevel lvl) {
  os << LogLevelToString(lvl);
  return os;
}


bool SetLogLevel(LogLevel lvl) {
  SPDLOG_DEBUG("viren2d: Changing log level to \"{:s}\".", lvl);
  spdlog::set_level(ToSpdLogLevel(lvl));

  if (lvl == LogLevel::Off) {
    // Disabling will always work
    return true;
  } else {
    // Enabling will only have an effect if the
    // library was compiled with an approriate
    // SPDLOG_ACTIVE_LEVEL
    if (SPDLOG_ACTIVE_LEVEL <= ToSpdLogMacroLevel(lvl)) {
      return true;
    } else {
      SPDLOG_ERROR("viren2d was compiled with log level \"{:s}\". "
                   "You will not see all desired logs!",
                   to_string_view(viren2d_ACTIVE_SPDLOG_LEVEL));
      return false;
    }
  }
}


bool SetLogLevel(const std::string &lvl) {
  return SetLogLevel(LogLevelFromString(lvl));
}

}  // namespace viren2d

