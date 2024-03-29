#ifndef __VIREN2D_COLORGRADIENTS_H__
#define __VIREN2D_COLORGRADIENTS_H__

#include <ostream>
#include <string>
#include <vector>
#include <utility>

#include <viren2d/colors.h>
#include <viren2d/primitives.h>
#include <viren2d/imagebuffer.h>

//FIXME remove stream colorizer

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


  /// Adds an intensity/grayscale stop to the gradient.
  ///
  /// Args:
  ///   offset: The offset, in [0,1], is the location along the gradient's
  ///     control vector.
  ///   intensity: The intensity/grayscale value, in [0,1], at the specified
  ///     offset.
  ///   alpha: The opacity, in [0, 1], at this stop.
  ///
  /// Returns:
  ///   True if the color stop was added, false if the inputs were invalid (for
  ///   example, out-of-range) which will be detailed in a logged warning
  ///   message.
  inline bool AddIntensityStop(
      double offset, double intensity, double alpha=1.0) {
    return AddColorStop(
          offset, Color(intensity, intensity, intensity, alpha));
  }


  /// Read-only access to the defined color stops along the
  /// gradient's control vector.
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


  /// Renders this gradient onto an image of the specified size.
  ///
  /// Args:
  ///   width: Width of the output image.
  ///   height: Height of the output image.
  ///   channels: Number of output channels, must be either 3 or 4.
  ///   background_color: The color used to initialize the image buffer before
  ///     rendering the gradient.
  ///
  /// Returns:
  ///   An ImageBuffer of type ImageBufferType::UInt8 with values in [0, 255].
  ImageBuffer Visualization(
      int width, int height, int channels = 3,
      const Color &background_color = Color::White) const;


  /// Renders this color gradient as a double-precision mask of the
  /// specified size.
  ///
  /// Args:
  ///   width: Width of the output mask in pixels.
  ///   height: Height of the output mask in pixels.
  ///   channels: Number of output channels. Must be either 1, 3, or 4. If a
  ///     single-channel mask is requested, only the red component of the
  ///     color stops will contribute to the output mask.
  ///   fill_color: The :class:`~viren2d.Color` used to initialize the
  ///     output mask before rendering the gradient.
  ///
  /// Returns:
  ///   An ImageBuffer of type ImageBufferType::Double with values in [0, 1].
  ImageBuffer Mask(
      int width, int height, int channels = 1,
      const Color &background_color = Color::Black.WithAlpha(0.0)) const;


protected:
  /// Color stops along fractions (i.e. within [0,1]) of the gradient's
  /// control vector.
  std::vector<std::pair<double, Color>> color_stops;
};


/// Linear color blending along a line.
///
/// The control vector (for adding color stops) is from the line's start point
/// to its end point. After initialization, the color stops along this control
/// vector have to be added via `AddColorStop` or `AddIntensityStop`.
class LinearColorGradient : public ColorGradient {
public:
  LinearColorGradient(const Vec2d &start, const Vec2d &end)
    : ColorGradient(), start_point(start), end_point(end)
  {}


  /// Read-only access to the start point.
  inline const Vec2d &StartPoint() const { return start_point; }


  /// Read-only access to the end point.
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
/// circle to the corresponding point on the end circle. After initialization,
/// the color stops along this control vector have to be added via
/// `AddColorStop` or `AddIntensityStop`.
class RadialColorGradient : public ColorGradient {
public:
  RadialColorGradient(
      const Vec2d &center_start, double radius_start,
      const Vec2d &center_end, double radius_end)
    : ColorGradient(),
      start_center(center_start), start_radius(radius_start),
      end_center(center_end), end_radius(radius_end)
  {}


  /// Read-only access to the center of the start circle.
  inline const Vec2d &StartCenter() const { return start_center; }


  /// Read-only access to the center of the end circle.
  inline const Vec2d &EndCenter() const { return end_center; }


  /// Returns the radius of the start circle.
  inline double StartRadius() const { return start_radius; }


  /// Returns the radius of the end circle.
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

} // namespace viren2d

#endif // __VIREN2D_COLORGRADIENTS_H__
