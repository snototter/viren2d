#include <sstream>
#include <iomanip>
#include <type_traits>
#include <stdexcept>
#include <cstdlib>
#include <cassert>
#include <cstring> // memcpy
#include <algorithm> // std::swap

#include <werkzeugkiste/strings/strings.h>
#include <werkzeugkiste/geometry/utils.h>

#include <viren2d/primitives.h>

#include <helpers/logging.h>


namespace wgu = werkzeugkiste::geometry;

namespace viren2d {
//---------------------------------------------------- Ellipse
Ellipse::Ellipse(double center_x, double center_y,
        double major, double minor, double rotation_angle,
        double draw_angle1, double draw_angle2,
        bool center_included)
  : cx(center_x), cy(center_y),
    major_axis(major), minor_axis(minor),
    rotation(rotation_angle),
    angle_from(draw_angle1),
    angle_to(draw_angle2),
    include_center(center_included) {
}


Ellipse::Ellipse(const Vec2d &center, const Vec2d &size,
        double rotation_angle,
        double draw_angle1, double draw_angle2,
        bool center_included)
  : cx(center.X()),
    cy(center.Y()),
    major_axis(size.Width()),
    minor_axis(size.Height()),
    rotation(rotation_angle),
    angle_from(draw_angle1),
    angle_to(draw_angle2),
    include_center(center_included)
{}


Ellipse::Ellipse(std::initializer_list<double> values) {
  if (values.size() < 4 || values.size() > 7) {
    std::ostringstream s;
    s << "Ellipse c'tor requires 4 to 7 entries in initializer_list, "
      << "but got " << values.size() << ".";
    throw std::invalid_argument(s.str());
  }

  const auto val = values.begin();
  cx = val[0];
  cy = val[1];
  major_axis = val[2];
  minor_axis = val[3];

  if (values.size() > 4)
    rotation = val[4];

  if (values.size() > 5)
    angle_from = val[5];

  if (values.size() > 6)
    angle_to = val[6];

  include_center = true;
}


Vec2d Ellipse::Center() const {
  return Vec2d{cx, cy};
}


Vec2d Ellipse::Axes() const {
  return Vec2d{major_axis, minor_axis};
}


Ellipse &Ellipse::operator+=(double offset) {
  cx += offset;
  cy += offset;
  return *this;
}


Ellipse &Ellipse::operator-=(double offset) {
  cx -= offset;
  cy -= offset;
  return *this;
}


Ellipse &Ellipse::operator+=(const Vec2d &offset) {
  cx += offset.X();
  cy += offset.Y();
  return *this;
}


Ellipse &Ellipse::operator-=(const Vec2d &offset) {
  cx -= offset.X();
  cy -= offset.Y();
  return *this;
}


bool Ellipse::IsValid() const {
  return (major_axis > 0.0) && (minor_axis > 0.0)
      && (major_axis >= minor_axis)
      && !wgu::IsClose(angle_from, angle_to);
}


std::string Ellipse::ToString() const {
  std::ostringstream s;
  s << "Ellipse(cx=" << std::fixed << std::setprecision(1)
    << cx << ", cy=" << cy << ", mj=" << major_axis
    << ", mn=" << minor_axis << "; rot=" << rotation
    << ", a1=" << angle_from << "°, a2=" << angle_to << "°";

  if (!wgu::IsEpsZero(angle_from) || !wgu::IsClose(angle_to, 360.0)) {
    s << ", " << (include_center ? "w/" : "w/o") << " center";
  }

  if (!IsValid())
    s << ", invalid";

  s << ")";
  return s.str();
}


Ellipse Ellipse::FromEndpoints(
      const Vec2d &pt1, const Vec2d &pt2, double width,
      double angle_from, double angle_to, bool include_center) {
  const auto dirvec = pt1.DirectionVector(pt2);
  const double major = dirvec.Length();
  const auto center = pt1 + major / 2.0 * dirvec.UnitVector();
  const double rotation = wgu::AngleDegFromDirectionVec(dirvec);
  return Ellipse(
      center, {major, width}, rotation,
      angle_from, angle_to, include_center);
}


bool operator==(const Ellipse& lhs, const Ellipse& rhs) {
  return wgu::IsClose(lhs.cx, rhs.cx)
      && wgu::IsClose(lhs.cy, rhs.cy)
      && wgu::IsClose(lhs.major_axis, rhs.major_axis)
      && wgu::IsClose(lhs.minor_axis, rhs.minor_axis)
      && wgu::IsClose(lhs.rotation, rhs.rotation)
      && wgu::IsClose(lhs.angle_from, rhs.angle_from)
      && wgu::IsClose(lhs.angle_to, rhs.angle_to)
      && (lhs.include_center == rhs.include_center);
}


bool operator!=(const Ellipse& lhs, const Ellipse& rhs) {
  return !(lhs == rhs);
}


//---------------------------------------------------- Rectangle

Rect::Rect(std::initializer_list<double> values) {
  if (values.size() < 4 || values.size() > 6) {
    std::ostringstream s;
    s << "Rect c'tor requires 4 to 6 entries in initializer_list, "
      << "but got " << values.size() << ".";
    throw std::invalid_argument(s.str());
  }

  const auto val = values.begin();
  cx = val[0];
  cy = val[1];
  width = val[2];
  height = val[3];

  if (values.size() > 4)
    rotation = val[4];

  if (values.size() > 5)
    radius = val[5];
}


Vec2d Rect::Center() const {
  return Vec2d{cx, cy};
}


Rect &Rect::operator+=(double offset) {
  cx += offset;
  cy += offset;
  return *this;
}


Rect &Rect::operator-=(double offset) {
  cx -= offset;
  cy -= offset;
  return *this;
}


Rect &Rect::operator+=(const Vec2d &offset) {
  cx += offset.X();
  cy += offset.Y();
  return *this;
}


Rect &Rect::operator-=(const Vec2d &offset) {
  cx -= offset.X();
  cy -= offset.Y();
  return *this;
}


Vec2d Rect::Size() const {
  return Vec2d(width, height);
}


double Rect::left() const {
  return cx - HalfWidth();
}


double Rect::right() const {
  return cx + HalfWidth();
}


double Rect::top() const {
  return cy - HalfHeight();
}


double Rect::bottom() const {
  return cy + HalfHeight();
}


//Vec2d Rect::TopLeft() const {
//  return Vec2d(left(), top());
//}


//Vec2d Rect::TopRight() const {
//  return Vec2d(right(), top());
//}


//Vec2d Rect::BottomLeft() const {
//  return Vec2d(left(), bottom());
//}


//Vec2d Rect::BottomRight() const {
//  return Vec2d(right(), bottom());
//}


bool Rect::IsValid() const {
  if ((radius > 0.5) && (radius < 1.0)) {
    return false;
  }
  return (width > 0.0) && (height > 0.0)
      && (radius <= std::min(HalfHeight(), HalfWidth()))
      && (radius >= 0.0);
}


std::string Rect::ToString() const {
  std::ostringstream s;
  s << "Rect(cx=" << std::fixed << std::setprecision(1)
    << cx << ", cy=" << cy << ", w=" << width << ", h=" << height
    << "; rot=" << rotation << "°, radius=" << radius;

  if (!IsValid())
    s << ", invalid";

  s << ")";
  return s.str();
}


Rect Rect::FromLTWH(double left, double top, double width, double height,
                    double corner_radius) {
  return Rect(left + width / 2.0, top + height / 2.0,
              width, height, 0.0, corner_radius);
}


Rect Rect::FromLRTB(double left, double right, double top, double bottom, double corner_radius) {
  const double w = right - left;
  const double h = bottom - top;
  return Rect(left + w / 2.0, top + h / 2.0,
              w, h, 0.0, corner_radius);
}


Rect Rect::FromCWH(double cx, double cy, double width, double height,
                   double rotation, double corner_radius) {
  return Rect(cx, cy, width, height, rotation, corner_radius);
}


bool operator==(const Rect& lhs, const Rect& rhs) {
  return wgu::IsClose(lhs.cx, rhs.cx)
      && wgu::IsClose(lhs.cy, rhs.cy)
      && wgu::IsClose(lhs.width, rhs.width)
      && wgu::IsClose(lhs.height, rhs.height)
      && wgu::IsClose(lhs.rotation, rhs.rotation)
      && wgu::IsClose(lhs.radius, rhs.radius);
}

bool operator!=(const Rect& lhs, const Rect& rhs) {
  return !(lhs == rhs);
}

} // namespace viren2d
