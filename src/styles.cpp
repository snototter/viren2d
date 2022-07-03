#include <algorithm>
#include <utility>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

#include <werkzeugkiste/geometry/utils.h>
#include <werkzeugkiste/strings/strings.h>

#include <viren2d/styles.h>
#include <helpers/logging.h>
#include <helpers/enum.h>


//TODO logging

namespace wkg = werkzeugkiste::geometry;
namespace wks = werkzeugkiste::strings;

namespace viren2d {
namespace helpers {

/// Returns desired_fill if the given marker is fillable.
/// Otherwise, returns true if the marker is fillable and
/// false if it's not.
bool AdjustMarkerFill(Marker marker, bool desired_fill) {
  switch (marker) {
    case Marker::Circle:
    case Marker::Cross:
    case Marker::Plus:
    case Marker::Star:
      return false;

    case Marker::Point:
      return true;

    case Marker::Diamond:
    case Marker::Enneagon:
    case Marker::Enneagram:
    case Marker::Heptagon:
    case Marker::Heptagram:
    case Marker::Hexagon:
    case Marker::Hexagram:
    case Marker::Octagon:
    case Marker::Octagram:
    case Marker::Pentagon:
    case Marker::Pentagram:
    case Marker::RotatedSquare:
    case Marker::Square:
    case Marker::TriangleDown:
    case Marker::TriangleLeft:
    case Marker::TriangleRight:
    case Marker::TriangleUp:
      return desired_fill;
  }

  std::string s("Marker '");
  s += MarkerToChar(marker);
  s += "' is not mapped in `AdjustMarkerFill`!";
  throw std::logic_error(s);
}
} // namespace helpers


//-------------------------------------------------  Line Cap & Join
std::string LineCapToString(LineCap cap) {
  switch (cap) {
    case LineCap::Butt:
      return "Butt";
    case LineCap::Round:
      return "Round";
    case LineCap::Square:
      return "Square";
  }

  std::ostringstream s;
  s << "LineCap (" << static_cast<int>(cap)
    << ") is not mapped in `LineCapToString`!";
  throw std::logic_error(s.str());
}


LineCap LineCapFromString(const std::string &cap) {
  const auto lower = wks::Trim(wks::Lower(cap));
  if (lower.compare("butt") == 0) {
    return LineCap::Butt;
  } else if (lower.compare("square") == 0) {
    return LineCap::Square;
  } else if (lower.compare("round") == 0) {
    return LineCap::Round;
  }

  std::string s("Could not deduce `LineCap` from string representation \"");
  s += cap;
  s += "\"!";
  throw std::logic_error(s);
}


std::ostream &operator<<(std::ostream &os, LineCap cap) {
  os << LineCapToString(cap);
  return os;
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

  std::ostringstream s;
  s << "LineJoin (" << static_cast<int>(join)
    << ") is not mapped in `LineJoinToString`!";
  throw std::logic_error(s.str());
}


LineJoin LineJoinFromString(const std::string &join) {
  const auto lower = wks::Lower(join);
  if (lower.compare("miter") == 0) {
    return LineJoin::Miter;
  } else if (lower.compare("bevel") == 0) {
    return LineJoin::Bevel;
  } else if (lower.compare("round") == 0) {
    return LineJoin::Round;
  }

  std::string s("Could not deduce `LineJoin` from string representation \"");
  s += join;
  s += "\"!";
  throw std::logic_error(s);
}


std::ostream &operator<<(std::ostream &os, LineJoin join) {
  os << LineJoinToString(join);
  return os;
}


//-------------------------------------------------  MarkerStyle
Marker MarkerFromChar(char m) {
  if (m == '.') {
    return Marker::Point;
  } else if (m == 'o') {
    return Marker::Circle;
  } else if (m == 'd') {
    return Marker::Diamond;
  } else if (m == '+') {
    return Marker::Plus;
  } else if (m == 'x') {
    return Marker::Cross;
  } else if (m == 's') {
    return Marker::Square;
  } else if (m == 'r') {
    return Marker::RotatedSquare;
  } else if (m == '^') {
    return Marker::TriangleUp;
  } else if (m == 'v') {
    return Marker::TriangleDown;
  } else if (m == '<') {
    return Marker::TriangleLeft;
  } else if (m == '>') {
    return Marker::TriangleRight;
  } else if (m == '*') {
    return Marker::Star;
  } else if (m == '5') {
    return Marker::Pentagram;
  } else if (m == 'p') {
    return Marker::Pentagon;
  } else if (m == '6') {
    return Marker::Hexagram;
  } else if (m == 'h') {
    return Marker::Hexagon;
  } else if (m == '7') {
    return Marker::Heptagram;
  } else if (m == 'H') {
    return Marker::Heptagon;
  } else if (m == '8') {
    return Marker::Octagram;
  } else if (m == '0') {
    return Marker::Octagon;
  } else if (m == '9') {
    return Marker::Enneagram;
  } else if (m == 'n') {
    return Marker::Enneagon;
  }

  std::string s("Could not deduce Marker from char '");
  s += m;
  s += "'!";
  throw std::logic_error(s);
}


char MarkerToChar(Marker marker) {
  switch (marker) {
    case Marker::Circle:
      return 'o';

    case Marker::Cross:
      return 'x';

    case Marker::Diamond:
      return 'd';

    case Marker::Enneagon:
      return 'n';

    case Marker::Enneagram:
      return '9';

    case Marker::Heptagon:
      return 'H';

    case Marker::Heptagram:
      return '7';

    case Marker::Hexagon:
      return 'h';

    case Marker::Hexagram:
      return '6';

    case Marker::Octagon:
      return '0';

    case Marker::Octagram:
      return '8';

    case Marker::Pentagon:
      return 'p';

    case Marker::Pentagram:
      return '5';

    case Marker::Plus:
      return '+';

    case Marker::Point:
      return '.';

    case Marker::RotatedSquare:
      return 'r';

    case Marker::Star:
      return '*';

    case Marker::Square:
      return 's';

    case Marker::TriangleUp:
      return '^';

    case Marker::TriangleDown:
      return 'v';

    case Marker::TriangleLeft:
      return '<';

    case Marker::TriangleRight:
      return '>';
  }

  std::ostringstream s;
  s << "Marker value ("
    << static_cast<int>(marker)
    << ") has not been mapped to char representation "
       "in `MarkerToChar`!";
  throw std::logic_error(s.str());
}


std::ostream &operator<<(std::ostream &os, Marker marker) {
  os << "'" << MarkerToChar(marker) << "'";
  return os;
}


std::vector<Marker> ListMarkers() {
  SPDLOG_TRACE("ListMarkers().");
  std::vector<Marker> lst;
  typedef ContinuousEnumIterator<Marker,
    Marker::Point, Marker::Enneagon> MarkerIterator;

  for (Marker m: MarkerIterator()) {
    lst.push_back(m);
  }

  return lst;
}


MarkerStyle::MarkerStyle()
  : marker(Marker::Circle),
    size(10.0), thickness(3.0),
    color(NamedColor::Azure),
    filled(helpers::AdjustMarkerFill(marker, false)),
    cap(LineCap::Butt),
    join(LineJoin::Miter)
{}


MarkerStyle::MarkerStyle(Marker type, double marker_size, double marker_thickness,
                         const Color &marker_color, bool fill,
                         viren2d::LineCap line_cap, viren2d::LineJoin line_join)
  : marker(type),
    size(marker_size),
    thickness(marker_thickness),
    color(marker_color),
    filled(helpers::AdjustMarkerFill(type, fill)),
    cap(line_cap), join(line_join)
{}


MarkerStyle::MarkerStyle(char type, double marker_size, double marker_thickness,
                         const Color &marker_color, bool fill,
                         LineCap line_cap, LineJoin line_join)
  : MarkerStyle(MarkerFromChar(type), marker_size, marker_thickness,
                marker_color, fill, line_cap, line_join)
{}


bool MarkerStyle::Equals(const MarkerStyle &other) const {
  return (marker == other.marker)
      && wkg::eps_equal(size, other.size)
      && wkg::eps_equal(thickness, other.thickness)
      && (color == other.color)
      && (filled == other.filled);
}


bool MarkerStyle::IsValid() const {
  if ((size <= 0.0) || !color.IsValid()) {
    return false;
  }

  if (!helpers::AdjustMarkerFill(marker, filled)) {
    return thickness > 0.0;
  }
  return true;
}


bool MarkerStyle::IsFilled() const {
  // User could have changed the `filled` flag
  // since creation, thus always check whether
  // the shape actually supports it:
  return helpers::AdjustMarkerFill(marker, filled);
}


std::string MarkerStyle::ToString() const {
  //TODO maybe change most "measurement" types of viren2d::XYStyles to int?
  //  who wants to use 1.5px line width anyhow... --> on the
  //  other hand, we might need the higher precision, once
  //  we switch to svg output (need to think about it!).

  std::ostringstream s;
  s << "MarkerStyle('" << MarkerToChar(marker)
    << std::fixed << std::setprecision(1)
    << "', sz=" << size
    << ", t=" << thickness
    << ", " << color;

  if (filled) {
    s << ", filled";
  }

  if (!IsValid()) {
    s << ", invalid";
  }

  s << ")";
  return s.str();
}


bool operator==(const MarkerStyle &lhs, const MarkerStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const MarkerStyle &lhs, const MarkerStyle &rhs) {
  return !(lhs == rhs);
}


//-------------------------------------------------  LineStyle
const LineStyle LineStyle::Invalid = LineStyle(-1, Color::Invalid);


LineStyle::LineStyle()
  : width(2),
    color(Color(NamedColor::Azure)),
    dash_pattern({}),
    dash_offset(0.0),
    cap(LineCap::Butt),
    join(LineJoin::Miter)
{}

//FIXME remove!
LineStyle::LineStyle(std::initializer_list<double> values) {
  if (values.size() < 2) {
    *this = LineStyle();
    if (values.size() > 0) {
      width = values.begin()[0];
    }
  } else {
    std::ostringstream s;
    s << "LineStyle c'tor requires 0, or 1 elements in initializer_list, "
      << "but got " << values.size() << ".";
    throw std::invalid_argument(s.str());
  }
}

LineStyle::LineStyle(
    double width, const Color &col,
    const std::vector<double> &dash,
    double offset, LineCap cap, LineJoin join)
  : width(width), color(col), dash_pattern(dash),
    dash_offset(offset), cap(cap), join(join)
{}


bool LineStyle::IsValid() const {
  // Dash strokes must be positive. Otherwise, it would put the
  // drawing context into an error state, see also:
  // https://cairographics.org/manual/cairo-cairo-t.html#cairo-set-dash
  // The following check is slightly stricter, because we don't
  // allow *any* 0-length strokes (I don't see an actual benefit
  // of having a dash pattern where "some" strokes can be 0)
  for (double d : dash_pattern) {
    if (d <= 0.0) {
      return false;
    }
  }
  return (width > 0.0) && color.IsValid();
}


bool LineStyle::IsSpecialInvalid() const {
  return *this == Invalid;
}


bool LineStyle::IsDashed() const {
  return dash_pattern.size() > 0;
}


double LineStyle::CapOffset() const {
  switch (cap) {
    case LineCap::Butt:
      return 0.0;

    case LineCap::Round:
    case LineCap::Square:
      return width / 2.0;
  }

  std::string s("LineCap::");
  s += LineCapToString(cap);
  s += " is not mapped in `CapOffset`!";
  throw std::runtime_error(s);
}


double LineStyle::JoinOffset(double interior_angle, double miter_limit) const {
  // For a diagram of how to compute the miter length, see
  //   https://github.com/freedesktop/cairo/blob/9bb1cbf7249d12dd69c8aca3825711645da20bcb/src/cairo-path-stroke.c#L432
  const double miter_length = width / std::max(1e-6, std::sin(wkg::deg2rad(interior_angle / 2.0)));
  if (((miter_length / width) > miter_limit)  // Cairo would switch to BEVEL
      || (join == LineJoin::Round)
      || (join == LineJoin::Bevel)) {
    return width / 2.0;
  } else {
    return miter_length / 2.0;
  }
}


std::string LineStyle::ToString() const {
  if (IsSpecialInvalid()) {
    return "LineStyle::Invalid";
  }

  std::ostringstream s;
  s << "LineStyle(" << std::fixed << std::setprecision(1)
    << width << "px, " << color.ToString() << ", "
    << (dash_pattern.empty() ? "solid" : "dashed");
  if (!IsValid())
    s << ", invalid";
  s << ")";
  return s.str();
}

std::string LineStyle::ToDetailedString() const {
  if (IsSpecialInvalid()) {
    return "LineStyle::Invalid";
  }

  std::ostringstream s;
  s << "LineStyle(" << std::fixed << std::setprecision(1)
    << width << "px, " << color.ToRGBaString() << ", ";

  s << "[" << std::fixed << std::setprecision(1);
  for (std::size_t idx = 0; idx < dash_pattern.size(); ++idx) {
    if (idx > 0) {
      s << ", ";
    }
    s << dash_pattern[idx];
  }
  s << ']';

  if (dash_pattern.size() > 0) {
    s << "+(" << std::fixed << std::setprecision(2)
      << dash_offset << ')';
  }

  s << ", cap=" << LineCapToString(cap)
    << ", join=" << LineJoinToString(join);

  if (!IsValid())
    s << ", invalid";
  s << ")";

  return s.str();
}


bool LineStyle::Equals(const LineStyle &other) const {
  if (!wkg::eps_equal(width, other.width)) {
    return false;
  }

  if (color != other.color) {
    return false;
  }

  if (dash_pattern.size() != other.dash_pattern.size()) {
    return false;
  }

  for (size_t i = 0; i < other.dash_pattern.size(); ++i) {
    if (!wkg::eps_equal(dash_pattern[i], other.dash_pattern[i])) {
      return false;
    }
  }

  if (!wkg::eps_equal(dash_offset, other.dash_offset)) {
    return false;
  }

  if (cap != other.cap) {
    return false;
  }

  if (join != other.join) {
    return false;
  }

  return true;
}


bool operator==(const LineStyle &lhs, const LineStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const LineStyle &lhs, const LineStyle &rhs) {
  return !(lhs == rhs);
}



//-------------------------------------------------  ArrowStyle
ArrowStyle::ArrowStyle()
  : LineStyle(),
    tip_length(0.2), tip_angle(20),
    tip_closed(false), double_headed(false) {
  // Change default cap, round looks nicer:
  cap = LineCap::Round;
}


ArrowStyle::ArrowStyle(
    double width, const Color &col,
    double tip_len, double angle,
    bool fill, bool two_heads,
    const std::vector<double> &dash,
    double offset, LineCap cap, LineJoin join)
  : LineStyle(width, col, dash, offset, cap, join),
    tip_length(tip_len), tip_angle(angle),
    tip_closed(fill), double_headed(two_heads)
{}


ArrowStyle::ArrowStyle(
    const LineStyle &line_style, double tip_len,
    double angle, bool fill, bool two_heads)
  : LineStyle(line_style),
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
  std::ostringstream s;
  s << "ArrowStyle(lw=" << std::fixed << std::setprecision(1)
    << width << ", tip=" << tip_length
    << ", angle=" << tip_angle << "°, "
    << (tip_closed ? "filled" : "open") << ", "
    << (double_headed ? "double-headed, " : "")
    << color.ToHexString() << ", "
    << (dash_pattern.empty() ? "solid" : "dashed");

  if (!IsValid()) {
    s << ", invalid";
  }
  s << ")";
  return s.str();
}


std::string ArrowStyle::ToDetailedString() const {
  std::ostringstream s;
  s << "ArrowStyle(base=" << LineStyle::ToDetailedString()
    << ", " << std::fixed << std::setprecision(1)
    << ", tip=" << tip_length
    << ", angle=" << tip_angle << "°, "
    << (tip_closed ? "filled" : "open") << ", "
    << (double_headed ? "double-headed, " : "single-headed")
    << ')';

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
  if (!wkg::eps_equal(tip_length, other.tip_length))
    return false;

  if (!wkg::eps_equal(tip_angle, other.tip_angle))
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
TextStyle::TextStyle()
  : size(16),
    family("monospace"),
    color(Color::Black),
    bold(false),
    italic(false),
    line_spacing(1.2),
    alignment(HorizontalAlignment::Left)
{}


TextStyle::TextStyle(unsigned int font_size,
                     const std::string &font_family,
                     const Color &font_color,
                     bool font_bold, bool font_italic,
                     double spacing,
                     HorizontalAlignment align)
  : size(font_size), family(font_family),
    color(font_color), bold(font_bold),
    italic(font_italic),
    line_spacing(spacing),
    alignment(align)
{}


bool TextStyle::IsValid() const {
  return !(family.empty()) && (size > 0)
      && color.IsValid();
}


bool TextStyle::Equals(const TextStyle &other) const {
  return (size == other.size)
      && (family.compare(other.family) == 0)
      && (color == other.color)
      && (bold == other.bold)
      && (italic == other.italic)
      && wkg::eps_equal(line_spacing, other.line_spacing);
}


std::string TextStyle::ToString() const {
  std::ostringstream s;
  s << "TextStyle(\"" << family << "\", "
    << size << "px";

  if (bold) {
    s << ", bold";
  }

  if (italic) {
    s << ", italic";
  }

  s << ", ls=" << std::setprecision(2) << line_spacing
    << ", " << alignment
    << ", " << color;

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


//-------------------------------------------------  BoundingBoxStyle
BoundingBox2DStyle::BoundingBox2DStyle()
  : line_style(), text_style(),
    box_fill_color(Color::Same.WithAlpha(0.1)),
    text_fill_color(Color::Same.WithAlpha(0.5)),
    label_position(LabelPosition::Top),
    label_padding(5, 5), clip_label(true)
{}


BoundingBox2DStyle::BoundingBox2DStyle(const LineStyle &contour,
                                       const TextStyle &label_style,
                                       const Color &bounding_box_fill_color,
                                       const Color &label_box_color,
                                       LabelPosition label_pos,
                                       const Vec2d &text_padding,
                                       bool clip_lbl)
  : line_style(contour), text_style(label_style),
    box_fill_color(bounding_box_fill_color),
    text_fill_color(label_box_color),
    label_position(label_pos),
    label_padding(text_padding), clip_label(clip_lbl)
{}


bool BoundingBox2DStyle::IsValid() const {
  return line_style.IsValid()
      && text_style.IsValid();
}


Color BoundingBox2DStyle::BoxFillColor() const {
  if (box_fill_color.IsSpecialSame()) {
    return line_style.color.WithAlpha(box_fill_color.alpha);
  } else {
    // Otherwise, it's either a valid, user-selected color or
    // invalid (and we won't draw it).
    return box_fill_color;
  }
}


Color BoundingBox2DStyle::TextFillColor() const {
  if (text_fill_color.IsSpecialSame()) {
    return line_style.color.WithAlpha(text_fill_color.alpha);
  } else {
    // Otherwise, it's either a valid, user-selected color or
    // invalid (and we won't draw it).
    return text_fill_color;
  }
}


bool BoundingBox2DStyle::Equals(const BoundingBox2DStyle &other) const {
  return (line_style == other.line_style)
      && (text_style == other.text_style)
      && (box_fill_color == other.box_fill_color)
      && (text_fill_color == other.text_fill_color)
      && (label_position == other.label_position)
      && (clip_label == other.clip_label);
}

//TODO add "to detailed string"
std::string BoundingBox2DStyle::ToString() const {
  std::ostringstream s;
  s << "BoundingBox2DStyle("
    << line_style << ", " << text_style
    << ", box fill=" << box_fill_color
    << ", text fill=" << text_fill_color
    << ", label at " << label_position;

  if (clip_label) {
    s << ", clipped";
  }

  s << ")";
  return s.str();
}


bool operator==(const BoundingBox2DStyle &lhs, const BoundingBox2DStyle &rhs) {
  return lhs.Equals(rhs);
}


bool operator!=(const BoundingBox2DStyle &lhs, const BoundingBox2DStyle &rhs) {
  return !(lhs == rhs);
}

} // namespace viren2d
