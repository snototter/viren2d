#include <algorithm>
#include <utility>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

#include <werkzeugkiste/geometry/utils.h>

#include <viren2d/styles.h>

//TODO logging

namespace wgu = werkzeugkiste::geometry;

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

  std::ostringstream s;
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

  std::ostringstream s;
  s << "LineJoin [" << static_cast<int>(join)
    << "] is not yet supported in LineJoinToString()!";
  throw std::runtime_error(s.str());
}


//-------------------------------------------------  LineStyle
const LineStyle LineStyle::Invalid = LineStyle(-1, Color::Invalid);
//const LineStyle LineStyle::Default = LineStyle(-42, Color::Invalid);


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
    std::ostringstream s;
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


//bool LineStyle::IsSpecialDefault() const {
//  return *this == Default;
//}


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

  std::ostringstream s;
  s << "LineCap::" << LineCapToString(line_cap)
    << " is not yet supported in CapOffset()!";
  throw std::runtime_error(s.str());
}


double LineStyle::JoinOffset(double interior_angle, double miter_limit) const {
  // For a diagram of how to compute the miter length, see
  //   https://github.com/freedesktop/cairo/blob/9bb1cbf7249d12dd69c8aca3825711645da20bcb/src/cairo-path-stroke.c#L432
  const double miter_length = line_width / std::max(1e-6, std::sin(wgu::deg2rad(interior_angle / 2.0)));
  if (((miter_length / line_width) > miter_limit)  // Cairo would switch to BEVEL
      || (line_join == LineJoin::Round)
      || (line_join == LineJoin::Bevel)) {
    return line_width / 2.0;
  } else {
    return miter_length / 2.0;
  }
}


std::string LineStyle::ToString() const {
//  if (IsSpecialDefault()) {
//    return "LineStyle::Default";
//  }

  if (IsSpecialInvalid()) {
    return "LineStyle::Invalid";
  }
//TODO check if shortened tostring (color and linestyle) makes more sense; then apply for all classes
  std::ostringstream s;
  s << "LineStyle(" << std::fixed << std::setprecision(1)
    << line_width << "px, " << color.ToString() << ", "
    << (dash_pattern.empty() ? "solid" : "dashed");
  if (!IsValid())
    s << ", invalid";
  s << ")";
  return s.str();
}


bool LineStyle::Equals(const LineStyle &other) const {
  if (!wgu::eps_equal(line_width, other.line_width))
    return false;

  if (color != other.color)
    return false;

  if (dash_pattern.size() != other.dash_pattern.size())
    return false;

  for (size_t i = 0; i < other.dash_pattern.size(); ++i)
    if (!wgu::eps_equal(dash_pattern[i], other.dash_pattern[i]))
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
//const ArrowStyle ArrowStyle::Invalid = ArrowStyle(-1, Color::Invalid, -1, -1);
//const ArrowStyle ArrowStyle::Default = ArrowStyle(-42, Color::Invalid, -42, -42);

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


std::string ArrowStyle::ToString() const {
  std::ostringstream s;
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
  if (!wgu::eps_equal(tip_length, other.tip_length))
    return false;

  if (!wgu::eps_equal(tip_angle, other.tip_angle))
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
//const TextStyle TextStyle::Default = TextStyle(-42, std::string());

TextStyle::TextStyle()
  : font_size(16),
    font_family("monospace"),
    color(Color::Black),
    bold(false),
    italic(false),
    line_spacing(1.2),
    alignment(HorizontalAlignment::Left)
{}


TextStyle::TextStyle(unsigned int size,
                     const std::string &family,
                     const Color &font_color,
                     bool font_bold, bool font_italic,
                     double spacing,
                     HorizontalAlignment align)
  : font_size(size), font_family(family),
    color(font_color), bold(font_bold),
    italic(font_italic),
    line_spacing(spacing),
    alignment(align)
{}


bool TextStyle::IsValid() const {
  return !(font_family.empty())
      && (font_size > 0)
      && color.IsValid();
}


//bool TextStyle::IsSpecialDefault() const {
//  return *this == Default;
//}



bool TextStyle::Equals(const TextStyle &other) const {
  return (font_size == other.font_size)
      && (font_family.compare(other.font_family) == 0)
      && (color == other.color)
      && (bold == other.bold)
      && (italic == other.italic); //TODO check line_spacing
}


std::string TextStyle::ToString() const {
//  if (IsSpecialDefault()) {
//    return "TextStyle::Default";
//  }

  std::ostringstream s;
  s << "TextStyle(\"" << font_family << "\", "
    << font_size << "px";

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
    label_position(BoundingBoxLabelPosition::Top),
    label_padding(5, 5), clip_label(true)
{}


BoundingBox2DStyle::BoundingBox2DStyle(const LineStyle &contour,
                                       const TextStyle &label_style,
                                       const Color &bounding_box_fill_color,
                                       const Color &label_box_color,
                                       BoundingBoxLabelPosition label_pos,
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
