#ifndef __VIREN2D_COLORGRADIENTS_H__
#define __VIREN2D_COLORGRADIENTS_H__

#include <ostream>
#include <string>
#include <vector>
#include <utility>

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

  // TODO doc
  inline bool AddGrayscaleStop(double offset, double intensity) {
    return AddColorStop(offset, Color(intensity, intensity, intensity, 1.0));
  }


  // TODO doc
  inline const std::vector<std::pair<double, Color>> &ColorStops() const {
    return color_stops;
  }

  // TODO doc
  bool IsValid() const;


  /// Returns a human readable representation.
  virtual std::string ToString() const;


  /// Overloaded stream operator.
  friend std::ostream &operator<<(std::ostream &os, const ColorGradient &buffer) {
    os << buffer.ToString();
    return os;
  }

protected:
  //TODO doc
  std::vector<std::pair<double, Color>> color_stops;
};


/// Linear color blending along a line.
///
/// The control vector (for adding color stops) is from the line's start point
/// to its end point.
class LinearColorGradient : public ColorGradient {
public:
  LinearColorGradient(const Vec2d &start, const Vec2d &end)
    : ColorGradient(), start_point(start), end_point(end)
  {}

  inline const Vec2d &StartPoint() const { return start_point; }
  inline const Vec2d &EndPoint() const { return end_point; }

  std::string ToString() const override;

private:
  /// Position of the start point.
  Vec2d start_point;

  /// Position of the end point.
  Vec2d end_point;
};


/// Radial gradient between two circles.
///
/// The control vector (for adding color stops) is from any point on the start
/// circle to the corresponding point on the end circle.
class RadialColorGradient : public ColorGradient {
public:
  RadialColorGradient(
      const Vec2d &center_start, double radius_start,
      const Vec2d &center_end, double radius_end)
    : ColorGradient(),
      start_center(center_start), start_radius(radius_start),
      end_center(center_end), end_radius(radius_end)
  {}

  inline const Vec2d &StartCenter() const { return start_center; }
  inline const Vec2d &EndCenter() const { return end_center; }
  inline double StartRadius() const { return start_radius; }
  inline double EndRadius() const { return end_radius; }

  std::string ToString() const override;

private:
  /// Center of the start circle.
  Vec2d start_center;

  /// Radius of the start circle.
  double start_radius;

  /// Center of the end circle.
  Vec2d end_center;

  /// Radius of the end circle.
  double end_radius;
};


ImageBuffer DrawColorGradient(
    const ColorGradient &gradient,
    int width, int height, int channels = 3,
    const Color &background_color = Color::White);

} // namespace viren2d

#endif // __VIREN2D_COLORGRADIENTS_H__
