#include <utility>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

#include <iostream> // TODO remove after switching to spdlog

#include <viren2d/styles.h>
#include <viren2d/math.h>

namespace viren2d {

std::string LineCapToString(LineCap cap) {
  switch (cap) {
    case LineCap::Butt:
      return "Butt";
    case LineCap::Round:
      return "Round";
    case LineCap::Square:
      return "Square";
  }

  std::stringstream s;
  s << "LineCap [" << static_cast<int>(cap)
    << "] is not yet supported in LineCapToString()!";
  throw std::runtime_error(s.str());
}


std::string LineJoinToString(LineJoin join) {
  switch (join) {
    case LineJoin::Miter:
      return "Miter";
    case LineJoin::Round:
      return "Round";
    case LineJoin::Bevel:
      return "Bevel";
  }

  std::stringstream s;
  s << "LineJoin [" << static_cast<int>(join)
    << "] is not yet supported in LineJoinToString()!";
  throw std::runtime_error(s.str());
}


//-------------------------------------------------  LineStyle
bool LineStyle::IsValid() const {
  return (line_width > 0.0) && color.IsValid();
}


bool LineStyle::IsDashed() const {
  return dash_pattern.size() > 0;
}


double LineStyle::CapOffset() const {
  switch (line_cap) {
    case LineCap::Butt:
      return 0.0;

    case LineCap::Round:
    case LineCap::Square:
      return line_width / 2.0;
  }

  std::stringstream s;
  s << "LineCap::" << LineCapToString(line_cap)
    << " is not yet supported in CapOffset()!";
  throw std::runtime_error(s.str());
}


double LineStyle::JoinOffset(double interior_angle, double miter_limit) const {
  // For a diagram of how to compute the miter length, see
  //   https://github.com/freedesktop/cairo/blob/9bb1cbf7249d12dd69c8aca3825711645da20bcb/src/cairo-path-stroke.c#L432
  const double miter_length = line_width / std::max(1e-6, std::sin(deg2rad(interior_angle / 2.0)));
  if (((miter_length / line_width) > miter_limit)  // Cairo would switch to BEVEL
      || (line_join == LineJoin::Round)
      || (line_join == LineJoin::Bevel)) {
    return line_width / 2.0;
  } else {
    return miter_length / 2.0;
  }
}


std::string LineStyle::ToString() const {
  std::stringstream s;
  s << "LineStyle(w=" << std::fixed << std::setprecision(1)
    << line_width << ", " << color.ToString() << ", "
    << (dash_pattern.empty() ? "solid" : "dashed") << ")";
  return s.str();
}


bool LineStyle::Equals(const LineStyle &other) const {
  if (!eps_equal(line_width, other.line_width))
    return false;

  if (color != other.color)
    return false;

  if (dash_pattern.size() != other.dash_pattern.size())
    return false;

  for (size_t i = 0; i < other.dash_pattern.size(); ++i)
    if (!eps_equal(dash_pattern[i], other.dash_pattern[i]))
      return false;

  if (line_cap != other.line_cap)
    return false;

  if (line_join != other.line_join)
    return false;

  return true;
}


bool operator==(const LineStyle &lhs, const LineStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const LineStyle &lhs, const LineStyle &rhs) {
  return !(lhs == rhs);
}



//-------------------------------------------------  ArrowStyle

bool ArrowStyle::IsValid() const {
  // No need to check the boolean flags (tip_closed & double_headed)
  return (tip_length > 0.0) && (tip_angle > 0.0)
      && LineStyle::IsValid();
}


std::string ArrowStyle::ToString() const {
  std::stringstream s;
  s << "ArrowStyle(lw=" << std::fixed << std::setprecision(1)
    << line_width << ", tl=" << tip_length
    << ", ta=" << tip_angle << "°, "
    << (tip_closed ? "filled" : "open") << ", "
    << (double_headed ? "double-headed, " : "")
    << color.ToHexString() << ", "
    << (dash_pattern.empty() ? "solid" : "dashed") << ", "
    << ")";
  return s.str();
}


double ArrowStyle::TipLengthForShaft(double shaft_length) const {
  if (tip_length > 1.0) {
    return tip_length;
  } else {
    return tip_length * shaft_length;
  }
}


double ArrowStyle::TipLengthForShaft(const viren2d::Vec2d &from, const viren2d::Vec2d &to) const {
  if (tip_length > 1.0) {
    return tip_length;
  } else {
    return TipLengthForShaft(from.Distance(to));
  }
}


double ArrowStyle::TipOffset(double miter_limit) const {
  // The interior angle between the two line segments of
  // the arrow tip is 2 * the tip angle.
  return JoinOffset(2.0 * tip_angle, miter_limit);
}


bool ArrowStyle::Equals(const ArrowStyle &other) const {
  if (!eps_equal(tip_length, other.tip_length))
    return false;

  if (!eps_equal(tip_angle, other.tip_angle))
    return false;

  if (tip_closed != other.tip_closed)
    return false;

  auto other_base = static_cast<const LineStyle &>(other);
  return LineStyle::Equals(other_base);
}


bool operator==(const ArrowStyle &lhs, const ArrowStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const ArrowStyle &lhs, const ArrowStyle &rhs) {
  return !(lhs == rhs);
}


} // namespace viren2d
