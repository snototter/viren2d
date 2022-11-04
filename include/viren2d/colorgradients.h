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
  ///
  /// Args:
  ///   offset: The offset, in [0,1], is the location along the gradient's
  ///     control vector.
  ///   color: The color at the specified offset.
  ///
  /// Returns:
  ///   True if the color stop was added, false if the inputs were invalid (for
  ///   example, out-of-range) which will be detailed in a logged warning
  ///   message.
  bool AddColorStop(double offset, const Color &color);


  /// Adds a grayscale stop to the gradient.
  ///
  /// Args:
  ///   offset: The offset, in [0,1], is the location along the gradient's
  ///     control vector.
  ///   intensity: The intensity/grayscale value at the specified offset.
  ///
  /// Returns:
  ///   True if the color stop was added, false if the inputs were invalid (for
  ///   example, out-of-range) which will be detailed in a logged warning
  ///   message.
  inline bool AddGrayscaleStop(double offset, double intensity) {
    return AddColorStop(offset, Color(intensity, intensity, intensity, 1.0));
  }


  /// Returns the defined color stops along the gradient's control vector.
  inline const std::vector<std::pair<double, Color>> &ColorStops() const {
    return color_stops;
  }


  /// Returns true if this color gradient can be rendered, i.e. if it has
  /// at least 2 color stops.
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
  // TODO doc
  LinearColorGradient(const Vec2d &start, const Vec2d &end)
    : ColorGradient(), start_point(start), end_point(end)
  {}


  // TODO doc
  inline const Vec2d &StartPoint() const { return start_point; }


  // TODO doc
  inline const Vec2d &EndPoint() const { return end_point; }


  /// Returns a human readable representation.
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
  // TODO doc
  RadialColorGradient(
      const Vec2d &center_start, double radius_start,
      const Vec2d &center_end, double radius_end)
    : ColorGradient(),
      start_center(center_start), start_radius(radius_start),
      end_center(center_end), end_radius(radius_end)
  {}


  // TODO doc
  inline const Vec2d &StartCenter() const { return start_center; }


  // TODO doc
  inline const Vec2d &EndCenter() const { return end_center; }


  // TODO doc
  inline double StartRadius() const { return start_radius; }


  // TODO doc
  inline double EndRadius() const { return end_radius; }


  /// Returns a human readable representation.
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


/// Returns a color gradient rendered onto an image of the specified size.
///
/// Args:
///   gradient: The color gradient to be rendered.
///   width: Width of the output image.
///   height: Height of the output image.
///   channels: Number of output channels, must be either 3 or 4.
///   background_color: The color used to initialize the image buffer before
///     rendering the gradient.
ImageBuffer DrawColorGradient(
    const ColorGradient &gradient,
    int width, int height, int channels = 3,
    const Color &background_color = Color::White);

} // namespace viren2d

#endif // __VIREN2D_COLORGRADIENTS_H__
