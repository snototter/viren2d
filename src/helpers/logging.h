#ifndef __VIREN2D_LOGGING_H__
#define __VIREN2D_LOGGING_H__

// TODO add cmake options to set log levels & disable debug:
// https://github.com/gabime/spdlog/wiki/0.-FAQ#how-to-remove-all-debug-statements-at-compile-time-
// different log levels (nice concise summary) - https://stackoverflow.com/questions/2031163/when-to-use-the-different-log-levels
//TODO Decide whether to trace each method invocation - or debug-log the "most" important (preference: trace each call, so we can actually trace what's going on if it should ever break)

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#endif  // __VIREN2D_LOGGING_H__
