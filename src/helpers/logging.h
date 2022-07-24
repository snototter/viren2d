#ifndef __VIREN2D_HELPERS_LOGGING_H__
#define __VIREN2D_HELPERS_LOGGING_H__

// TODO if disabled, create a warning/error macro to log
// invalid inputs to drawing calls!

#ifdef viren2d_ENABLE_LOGGING
#  include <spdlog/spdlog.h>
#  include <spdlog/fmt/ostr.h>

// TODO add cmake options to set log levels & disable debug:
// https://github.com/gabime/spdlog/wiki/0.-FAQ#how-to-remove-all-debug-statements-at-compile-time-
// different log levels (nice concise summary) - https://stackoverflow.com/questions/2031163/when-to-use-the-different-log-levels
//TODO Decide whether to trace each method invocation - or debug-log the "most" important (preference: trace each call, so we can actually trace what's going on if it should ever break)

#  if defined(SPDLOG_ACTIVE_LEVEL)
#    undef SPDLOG_ACTIVE_LEVEL
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#  endif  // unset SPDLOG_ACTIVE_LEVEL

//#if defined(viren2d_LOG_LEVEL_TRACE)
//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
//#define viren2d_ACTIVE_SPDLOG_LEVEL spdlog::level::trace

//#elif defined(viren2d_LOG_LEVEL_DEBUG)
//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
//#define viren2d_ACTIVE_SPDLOG_LEVEL spdlog::level::debug

//#elif defined(viren2d_LOG_LEVEL_INFO)
//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
//#define viren2d_ACTIVE_SPDLOG_LEVEL spdlog::level::info

//#elif defined(viren2d_LOG_LEVEL_WARN)
//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_WARN
//#define viren2d_ACTIVE_SPDLOG_LEVEL spdlog::level::warn

//#elif defined(viren2d_LOG_LEVEL_ERROR)
//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_ERROR
//#define viren2d_ACTIVE_SPDLOG_LEVEL spdlog::level::err

//#elif defined(viren2d_LOG_LEVEL_OFF)
//#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
//#define viren2d_ACTIVE_SPDLOG_LEVEL spdlog::level::off

//#else
////TODO could make a default (document and test), or force the user to
////     provide it via rendering the library ill-formed:
//#error "No valid viren2d_LOG_LEVEL_xxx parameter has been specified."
//#endif  // viren2d_LOG_LEVEL definition

#  else  // viren2d_ENABLE_LOGGING
#    define SPDLOG_TRACE(...) (void)0
#    define SPDLOG_DEBUG(...) (void)0
#    define SPDLOG_INFO(...) (void)0
#    define SPDLOG_WARN(...) (void)0
#    define SPDLOG_CRITICAL(...) (void)0
#endif  // viren2d_ENABLE_LOGGING
#endif  // __VIREN2D_HELPERS_LOGGING_H__
