#include <algorithm>
#include <utility>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

#include <werkzeugkiste/strings/strings.h>

#include <viren2d/styles.h>

#include <helpers/math_utils.h>
#include <helpers/enum.h>

//TODO logging

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
const LineStyle LineStyle::Invalid = LineStyle(-1, Color::Invalid);
const LineStyle LineStyle::Default = LineStyle(-42, Color::Invalid);


LineStyle::LineStyle()
  : line_width(2),
    color(Color(NamedColor::Azure)),
    dash_pattern({}),
    line_cap(LineCap::Butt),
    line_join(LineJoin::Miter)
{}


LineStyle::LineStyle(std::initializer_list<double> values) {
  if (values.size() < 2) {
    *this = LineStyle();
    if (values.size() > 0) {
      line_width = values.begin()[0];
    }
  } else {
    std::stringstream s;
    s << "LineStyle c'tor requires 0, or 1 elements in initializer_list, "
      << "but got " << values.size() << ".";
    throw std::invalid_argument(s.str());
  }//TODO initializer lists for other styles, too
}

LineStyle::LineStyle(double width, const Color &col,
                     const std::vector<double> &dash,
                     LineCap cap, LineJoin join)
  : line_width(width), color(col), dash_pattern(dash),
    line_cap(cap), line_join(join)
{}


bool LineStyle::IsValid() const {
  return (line_width > 0.0) && color.IsValid();
}


bool LineStyle::IsSpecialInvalid() const {
  return *this == Invalid;
}


bool LineStyle::IsSpecialDefault() const {
  return *this == Default;
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
  if (IsSpecialDefault()) {
    return "LineStyle::Default";
  }

  if (IsSpecialInvalid()) {
    return "LineStyle::Invalid";
  }

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


bool operator==(const LineStyle &lhs, const LineStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const LineStyle &lhs, const LineStyle &rhs) {
  return !(lhs == rhs);
}



//-------------------------------------------------  ArrowStyle
const ArrowStyle ArrowStyle::Invalid = ArrowStyle(-1, Color::Invalid, -1, -1);
const ArrowStyle ArrowStyle::Default = ArrowStyle(-42, Color::Invalid, -42, -42);

ArrowStyle::ArrowStyle()
  : LineStyle(),
    tip_length(0.2), tip_angle(20),
    tip_closed(false), double_headed(false) {
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


bool ArrowStyle::IsSpecialInvalid() const {
  return *this == ArrowStyle::Invalid;
}


bool ArrowStyle::IsSpecialDefault() const {
  return *this == ArrowStyle::Default;
}

std::string ArrowStyle::ToString() const {
  if (IsSpecialInvalid()) {
    return "ArrowStyle::Invalid";
  }

  if (IsSpecialDefault()) {
    return "ArrowStyle::Default";
  }

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


bool operator==(const ArrowStyle &lhs, const ArrowStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const ArrowStyle &lhs, const ArrowStyle &rhs) {
  return !(lhs == rhs);
}



//-------------------------------------------------  TextStyle
const TextStyle TextStyle::Default = TextStyle(-42, std::string());

TextStyle::TextStyle()
  : font_size(16),
    font_family("monospace"),
    font_color(Color::Black),
    font_bold(false),
    font_italic(false) {
}


TextStyle::TextStyle(unsigned int size,
                     const std::string &family,
                     const Color &color,
                     bool bold, bool italic)
  : font_size(size), font_family(family),
    font_color(color), font_bold(bold),
    font_italic(italic) {
}


bool TextStyle::IsValid() const {
  return !(font_family.empty())
      && (font_size > 0)
      && font_color.IsValid();
}


bool TextStyle::IsSpecialDefault() const {
  return *this == Default;
}



bool TextStyle::Equals(const TextStyle &other) const {
  return (font_size == other.font_size)
      && (font_family.compare(other.font_family) == 0)
      && (font_color == other.font_color)
      && (font_bold == other.font_bold)
      && (font_italic == other.font_italic); //TODO check line_spacing
}


std::string TextStyle::ToString() const {
  if (IsSpecialDefault()) {
    return "TextStyle::Default";
  }

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


bool operator==(const TextStyle &lhs, const TextStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const TextStyle &lhs, const TextStyle &rhs) {
  return !(lhs == rhs);
}



TextAnchor TextAnchorFromString(const std::string &anchor) {
  std::string slug = werkzeugkiste::strings::Lower(anchor);
  slug.erase(std::remove_if(slug.begin(), slug.end(), [](char ch) -> bool {
      return ::isspace(ch) || (ch == '-') || (ch == '_');
    }), slug.end());

  // We support:
  // * Standard corner terminology (bottom right, top, center, ...)
  // * Eight principal compass directions (south, north-west, east, ...)
  //TODO support abbreviations? n,e,s,w; t,r,b,l;
  if (slug.compare("center") == 0) {
    return TextAnchor::Center;
  } else if ((slug.compare("right") == 0)
             || (slug.compare("east") == 0)) {
    return TextAnchor::Right;
  } else if ((slug.compare("bottomright") == 0)
             || (slug.compare("southeast") == 0)) {
    return TextAnchor::BottomRight;
  } else if ((slug.compare("bottom") == 0)
             || (slug.compare("south") == 0)) {
    return TextAnchor::Bottom;
  } else if ((slug.compare("bottomleft") == 0)
             || (slug.compare("southwest") == 0)) {
    return TextAnchor::BottomLeft;
  } else if ((slug.compare("left") == 0)
             || (slug.compare("west") == 0)) {
    return TextAnchor::Left;
  } else if ((slug.compare("topleft") == 0)
             || (slug.compare("northwest") == 0)) {
    return TextAnchor::TopLeft;
  } else if ((slug.compare("top") == 0)
             || (slug.compare("north") == 0)) {
    return TextAnchor::Top;
  } else if ((slug.compare("topright") == 0)
             || (slug.compare("northeast") == 0)) {
    return TextAnchor::TopRight;
  }

  std::stringstream s;
  s << "Could not deduce TextAnchor from string \""
    << anchor << "\".";
  throw std::invalid_argument(s.str());
}


std::string TextAnchorToString(TextAnchor anchor) {
  switch (anchor) {
    case TextAnchor::Center:
      return "center";

    case TextAnchor::Top:
      return "top";

    case TextAnchor::TopRight:
      return "top-right";

    case TextAnchor::Right:
      return "right";

    case TextAnchor::BottomRight:
      return "bottom-right";

    case TextAnchor::Bottom:
      return "bottom";

    case TextAnchor::BottomLeft:
      return "bottom-left";

    case TextAnchor::Left:
      return "left";

    case TextAnchor::TopLeft:
      return "top-left";

    default:
      throw std::invalid_argument("Incomplete text anchor: you must specify both horizontal & vertical alignment");
  }
}


std::ostream &operator<<(std::ostream &os, TextAnchor anchor) {
  os << TextAnchorToString(anchor);
  return os;
}



} // namespace viren2d
