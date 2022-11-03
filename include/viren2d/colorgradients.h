#ifndef __VIREN2D_COLORGRADIENTS_H__
#define __VIREN2D_COLORGRADIENTS_H__

#include <vector>
#include <tuple>

#include <viren2d/colors.h>
#include <viren2d/primitives.h>
#include <viren2d/imagebuffer.h>


namespace viren2d {

class ColorGradient {
public:
  virtual ~ColorGradient() {}
  ColorGradient() = default;
  ColorGradient(const ColorGradient &) = default;
  ColorGradient &operator=(const ColorGradient &) = default;
  ColorGradient(ColorGradient &&) = default;
  ColorGradient &operator=(ColorGradient &&) = default;

  /// Adds a color stop to the gradient.
  /// Args:
  ///   offset: The offset, in [0,1], is the location along the gradient's
  ///     control vector.
  bool AddColorStop(double offset, const Color &color);

  const std::vector<std::tuple<double, Color>> &GetColorStops() const {
    return color_stops_;
  }

protected:
  //TODO doc
  std::vector<std::tuple<double, Color>> color_stops_;
};


/// Linear color blending along a line.
///
/// The control vector (for adding color stops) is from the line's start point
/// to its end point.
class LinearColorGradient : public ColorGradient {
public:
  LinearColorGradient(const Vec2d &start, const Vec2d &end)
    : ColorGradient(), start_point_(start), end_point_(end)
  {}

  const Vec2d &StartPoint() const { return start_point_; }
  const Vec2d &EndPoint() const { return end_point_; }

private:
  /// Position of the start point.
  Vec2d start_point_;

  /// Position of the end point.
  Vec2d end_point_;
};


/// Radial gradient between two circles.
///
/// The control vector (for adding color stops) is from any point on the start
/// circle to the corresponding point on the end circle.
class RadialColorGradient : public ColorGradient {
public:
  //TODO c'tor
  //FIXME naming consistency Get... vs variable names

  const Vec2d &StartCenter() const { return start_center_; }
  const Vec2d &EndCenter() const { return end_center_; }
  double StartRadius() const { return start_radius_; }
  double EndRadius() const { return end_radius_; }

private:
  /// Center of the start circle.
  Vec2d start_center_;

  /// Radius of the start circle.
  double start_radius_;

  /// Center of the end circle.
  Vec2d end_center_;

  /// Radius of the end circle.
  double end_radius_;
};


ImageBuffer DrawColorGradient(
    const ColorGradient &gradient,
    int width, int height, int channels = 3,
    const Color &background_color = Color::White);

} // namespace viren2d

#endif // __VIREN2D_COLORGRADIENTS_H__
