#include <algorithm>
#include <utility>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

#include <iostream> // TODO remove after switching to spdlog

#include <viren2d/styles.h>
#include <viren2d/math.h>
#include <viren2d/string_utils.h>

#include <helpers/enum.h>

namespace viren2d {
namespace {
static LineStyle default_line_style = LineStyle(2.0, Color(NamedColor::NavyBlue, 80),
                                                std::vector<double>(),
                                                LineCap::Butt, LineJoin::Miter);

static ArrowStyle default_arrow_style = ArrowStyle(default_line_style,
                                                   0.1, 20.0, false, false);

//TODO(snototter) change default font family
static TextStyle default_text_style = TextStyle(16, "xkcd",
                                                Color(NamedColor::MidnightBlue),
                                                false, false);
} // anonymous namespace

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
LineStyle::LineStyle()
  : line_width(default_line_style.line_width),
    color(default_line_style.color),
    dash_pattern(default_line_style.dash_pattern),
    line_cap(default_line_style.line_cap),
    line_join(default_line_style.line_join)
{}


LineStyle::LineStyle(double width, const Color &col,
                     const std::vector<double> &dash,
                     LineCap cap, LineJoin join)
  : line_width(width), color(col), dash_pattern(dash),
    line_cap(cap), line_join(join)
{}


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
  s << "LineStyle(lw=" << std::fixed << std::setprecision(1)
    << line_width << ", " << color.ToString() << ", "
    << (dash_pattern.empty() ? "solid" : "dashed");
  if (!IsValid())
    s << ", invalid";
  s << ")";
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


void SetDefaultLineStyle(const LineStyle &line_style) {
  default_line_style = line_style;
}

LineStyle GetDefaultLineStyle() {
  return default_line_style;
}



bool operator==(const LineStyle &lhs, const LineStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const LineStyle &lhs, const LineStyle &rhs) {
  return !(lhs == rhs);
}



//-------------------------------------------------  ArrowStyle
ArrowStyle::ArrowStyle() : LineStyle() {
  *this = default_arrow_style;
}


ArrowStyle::ArrowStyle(double width, const Color &col,
                       double tip_len, double angle,
                       bool fill, bool two_heads,
                       const std::vector<double> &dash,
                       LineCap cap, LineJoin join)
  : LineStyle(width, col, dash, cap, join),
    tip_length(tip_len), tip_angle(angle),
    tip_closed(fill), double_headed(two_heads)
{}


bool ArrowStyle::IsValid() const {
  // No need to check the boolean flags (tip_closed & double_headed)
  return (tip_length > 0.0)
      && (tip_angle > 0.0) && (tip_angle < 180.0)
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
    << (dash_pattern.empty() ? "solid" : "dashed");
  if (!IsValid())
    s << ", invalid";
  s << ")";
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


void SetDefaultArrowStyle(const ArrowStyle &arrow_style) {
  default_arrow_style = arrow_style;
}

ArrowStyle GetDefaultArrowStyle() {
  return default_arrow_style;
}


bool operator==(const ArrowStyle &lhs, const ArrowStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const ArrowStyle &lhs, const ArrowStyle &rhs) {
  return !(lhs == rhs);
}



//-------------------------------------------------  TextStyle
TextStyle::TextStyle() {
  *this = default_text_style;
}


TextStyle::TextStyle(unsigned int size,
                     const std::string &family,
                     const Color &color,
                     bool bold, bool italic)
  : font_size(size), font_family(family),
    font_color(color), font_bold(bold), font_italic(italic) {
}


TextStyle TextStyle::InvalidStyle() {
  TextStyle def;
  def.font_size = 0;
  def.font_family = std::string();
  def.font_color = Color::Invalid;
  return def;
}


bool TextStyle::IsValid() const {
  return !(font_family.empty())
      && (font_size > 0)
      && font_color.IsValid();
}


bool TextStyle::Equals(const TextStyle &other) const {
  return (font_size == other.font_size)
      && (font_family.compare(other.font_family) == 0)
      && (font_color == other.font_color)
      && (font_bold == other.font_bold)
      && (font_italic == other.font_italic);
}


std::string TextStyle::ToString() const {
  std::stringstream s;
  s << "TextStyle(\"" << font_family << "\", "
    << font_size << "px";

  if (font_bold) {
    s << ", bold";
  }
  if (font_italic) {
    s << ", italic";
  }
  if (!IsValid()) {
    s << ", invalid";
  }
  s << ")";
  return s.str();
}


void SetDefaultTextStyle(const TextStyle &text_style) {
  default_text_style = text_style;
}


TextStyle GetDefaultTextStyle() {
  return default_text_style;
}


bool operator==(const TextStyle &lhs, const TextStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const TextStyle &lhs, const TextStyle &rhs) {
  return !(lhs == rhs);
}



TextAnchor TextAnchorFromString(const std::string &anchor) {
  std::string slug = viren2d::strings::Lower(anchor);
  slug.erase(std::remove_if(slug.begin(), slug.end(), [](char ch) -> bool {
      return ::isspace(ch) || (ch == '-') || (ch == '_');
    }), slug.end());

  // We support:
  // * Standard corner terminology (bottom right, top, center, ...)
  // * Eight principal compass directions (south, north-west, east, ...)
  if (slug.compare("center") == 0) {
    return TextAnchor::CenterHorz | TextAnchor::CenterVert;
  } else if ((slug.compare("right") == 0)
             || (slug.compare("east") == 0)) {
    return TextAnchor::Right | TextAnchor::CenterVert;
  } else if ((slug.compare("bottomright") == 0)
             || (slug.compare("southeast") == 0)) {
    return TextAnchor::Right | TextAnchor::Bottom;
  } else if ((slug.compare("bottom") == 0)
             || (slug.compare("south") == 0)) {
    return TextAnchor::CenterHorz | TextAnchor::Bottom;
  } else if ((slug.compare("bottomleft") == 0)
             || (slug.compare("southwest") == 0)) {
    return TextAnchor::Left | TextAnchor::Bottom;
  } else if ((slug.compare("left") == 0)
             || (slug.compare("west") == 0)) {
    return TextAnchor::Left | TextAnchor::CenterVert;
  } else if ((slug.compare("topleft") == 0)
             || (slug.compare("northwest") == 0)) {
    return TextAnchor::Left | TextAnchor::Top;
  } else if ((slug.compare("top") == 0)
             || (slug.compare("north") == 0)) {
    return TextAnchor::CenterHorizontal | TextAnchor::Top;
  } else if ((slug.compare("topright") == 0)
             || (slug.compare("northeast") == 0)) {
    return TextAnchor::Right | TextAnchor::Top;
  }

  std::stringstream s;
  s << "Could not deduce TextAnchor from string \""
    << anchor << "\".";
  throw std::invalid_argument(s.str());
}


TextAnchor TextAnchorFromString(const char *anchor) {
  return TextAnchorFromString(std::string(anchor));
}

} // namespace viren2d
