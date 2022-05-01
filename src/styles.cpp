#include <utility>
#include <sstream>
#include <iomanip>
#include <iostream> // TODO remove after switching to spdlog
#include <stdexcept>

#include <viren2d/styles.h>
#include <viren2d/math.h>

namespace viren2d {

bool LineStyle::IsValid() const {
  return (line_width > 0.0) && color.IsValid();
}


std::string LineStyle::ToString() const {
  std::stringstream s;
  s << "LineStyle(w=" << std::fixed << std::setprecision(1)
    << line_width << ", " << color.ToString() << ", "
    << (dash_pattern.empty() ? "solid" : "dashed") << ")";
  return s.str();
}


bool operator==(const LineStyle &lhs, const LineStyle &rhs) {
  if (!eps_equal(lhs.line_width, rhs.line_width))
    return false;

  if (lhs.color != rhs.color)
    return false;

  if (lhs.dash_pattern.size() != rhs.dash_pattern.size())
    return false;

  for (size_t i = 0; i < lhs.dash_pattern.size(); ++i)
    if (!eps_equal(lhs.dash_pattern[i], rhs.dash_pattern[i]))
      return false;

  if (lhs.line_cap != rhs.line_cap)
    return false;

  if (lhs.line_join != rhs.line_join)
    return false;

  return true;
}


bool operator!=(const LineStyle &lhs, const LineStyle &rhs) {
  return !(lhs == rhs);
}


} // namespace viren2d
