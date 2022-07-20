#ifndef __VIREN2D_PRIMITIVES_H__
#define __VIREN2D_PRIMITIVES_H__

#include <stdexcept>
#include <string>
#include <ostream>
#include <initializer_list>
#include <cmath>

#include <werkzeugkiste/geometry/vector.h>


namespace viren2d {
//------------------------------------------------- Reuse vector class from werkzeugkiste library
typedef werkzeugkiste::geometry::Vec2d Vec2d;
typedef werkzeugkiste::geometry::Vec2d Size2d;
typedef werkzeugkiste::geometry::Vec3d Vec3d;
typedef werkzeugkiste::geometry::Vec4d Vec4d;

typedef werkzeugkiste::geometry::Vec2i Vec2i;
typedef werkzeugkiste::geometry::Vec3i Vec3i;


//-------------------------------------------------  Ellipse
/**
 * @brief Ellipse for visualization.
 * //TODO doc - major/minor will always be set correctly if you use the c'tors
 */
struct Ellipse {
  double cx;            ///< Center coordinate in x direction.
  double cy;            ///< Center coordinate in y direction.
  double major_axis;    ///< Length of major axis.
  double minor_axis;    ///< Length of minor axis.
  double rotation;      ///< Clockwise rotation in degrees.
  double angle_from;    ///< Starting angle of contour/fill in degrees.
  double angle_to;      ///< Ending angle of contour/fill in degrees.
  bool include_center;  ///< Whether to include the center point in drawing/filling (only used if angle_from/angle_to are set).

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add Python bindings - not allowed
  //DONE [x] add Python test (tests/test_xxx.py)
  /** Default c'tor creates an invalid ellipse. */
  Ellipse()
    : cx(0.0), cy(0.0), major_axis(0.0), minor_axis(0.0),
      rotation(0.0), angle_from(0.0), angle_to(360.0),
      include_center(true)
  {}

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add Python bindings
  //DONE [x] add Python test (tests/test_xxx.py)
  /** Creates an ellipse. */
  Ellipse(double center_x, double center_y,
          double major, double minor,
          double rotation_angle = 0.0,
          double draw_angle1 = 0.0,
          double draw_angle2 = 360.0,
          bool center_included = true);

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add Python bindings
  //DONE [x] add Python test (tests/test_xxx.py)
  //TODO doc (from py)
  Ellipse(const Vec2d &center, const Vec2d &size,
          double rotation_angle = 0.0,
          double draw_angle1 = 0.0,
          double draw_angle2 = 360.0,
          bool center_included = true);


  // Nothing special about the ellipse class, so we can have
  // the default copy/assignment/move c'tors/operators:
  Ellipse(const Ellipse &other) = default;
  Ellipse& operator=(const Ellipse &other) = default;
  Ellipse(Ellipse&&) = default;
  Ellipse& operator=(Ellipse &&) = default;


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /**
   * @brief Construct from an initializer list with 4 to 7 elements.
   * Minimum: {cx, cy, major, minor}
   * Maximum: {cx, cy, major, minor, rotation, draw_angle1, draw_angle2}.
   * This c'tor will always set include_center to true.
   */
  Ellipse(std::initializer_list<double> values);

  Vec2d Center() const;
  Vec2d Axes() const;

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add C++ demo
  /** @brief Translate the center point by "offset" pixels in each dimension. */
  Ellipse &operator+=(double offset);

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add C++ demo
  /** @brief Translate the center point by "offset" pixels in each dimension. */
  Ellipse &operator-=(double offset);

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add C++ demo
  /** @brief Translate the center point by "offset" pixels. */
  Ellipse &operator+=(const Vec2d &offset);

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add C++ demo
  /** @brief Translate the center point by "offset" pixels. */
  Ellipse &operator-=(const Vec2d &offset);


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add Python bindings
  //DONE [x] add Python test (tests/test_xxx.py)
  /** @brief Returns true if this rectangle can be drawn. */
  bool IsValid() const;


  /** @brief Returns a human-readable string representation. */
  std::string ToString() const;


  /** @brief Overloaded stream operator. */
  friend std::ostream &operator<<(std::ostream &os, const Ellipse &e) {
    os << e.ToString();
    return os;
  }


  static Ellipse FromEndpoints(
      const Vec2d &pt1, const Vec2d &pt2, double width,
      double angle_from = 0.0, double angle_to = 360.0,
      bool include_center = true);
};


//-------------------------------------------------  Comparison operators
bool operator==(const Ellipse& lhs, const Ellipse& rhs);
bool operator!=(const Ellipse& lhs, const Ellipse& rhs);



//-------------------------------------------------  Rectangle
/// Rectangle for visualization.
///
/// Note that it is defined by its CENTER coordinates
/// width, height, angle (clockwise rotation in degrees),
/// and a corner radius (for rounded rectangles).
struct Rect {
  /// Center coordinate in x direction.
  double cx;

  /// Center coordinate in y direction.
  double cy;

  /// Width of the rectangle.
  double width;

  /// Height of the rectangle.
  double height;

  /// Clockwise rotation in degrees.
  double rotation;

  /// Corner radius.
  /// If `0 <= radius <= 0.5`, the actural corner radius
  /// will be computed as `radius * min(width, height)`.
  /// If `radius > 1`, it denotes the absolute corner radius in pixels.
  /// Otherwise, *i.e.* `0.5 < radius < 1`, the rectangle will be invalid.
  double radius;


  /// Default c'tor creates an invalid rectangle.
  Rect()
    : cx(0.0), cy(0.0), width(0.0), height(0.0),
      rotation(0.0), radius(0.0)
  {}


  /// Creates a custom rectangle.
  Rect(double center_x, double center_y, double w, double h,
       double rot = 0.0, double corner_radius = 0.0)
    : cx(center_x), cy(center_y), width(w), height(h),
      rotation(rot), radius(corner_radius)
  {}


  /// Creates a custom rectangle.
  Rect(const Vec2d &center, const Vec2d &size,
       double rot = 0.0, double corner_radius = 0.0)
    : cx(center.x()), cy(center.y()),
      width(size.width()), height(size.height()),
      rotation(rot), radius(corner_radius)
  {}


  // Nothing special about the rectangle class, so we can have
  // the default copy/assignment/move c'tors/operators:
  Rect(const Rect &other) = default;
  Rect& operator=(const Rect &other) = default;
  Rect(Rect&&) = default;
  Rect& operator=(Rect &&) = default;


  /// Creates a Rect from an initializer list with 4 to 6 elements.
  /// * Minimum: {cx, cy, w, h}
  /// * Maximum: {cx, cy, w, h, rotation, corner_radius}
  Rect(std::initializer_list<double> values);


  /// Translate the center point by "offset" pixels in each dimension.
  Rect &operator+=(double offset);

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /** @brief Translate the center point by "offset" pixels in each dimension. */
  Rect &operator-=(double offset);

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /** @brief Translate the center point by "offset" pixels. */
  Rect &operator+=(const Vec2d &offset);

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /** @brief Translate the center point by "offset" pixels. */
  Rect &operator-=(const Vec2d &offset);


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /** @brief Returns half the width. */
  inline double HalfWidth() const {
    return width / 2.0;
  }

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /** @brief Returns half the height. */
  inline double HalfHeight() const {
    return height / 2.0;
  }


  /// Returns the center position.
  Vec2d Center() const;

  /// Returns the retangle's size.
  Vec2d Size() const;

  /// Returns the x-coordinate of the left edge IFF rotation = 0.
  double left() const;

  /// Returns the x-coordinate of the right edge IFF rotation = 0.
  double right() const;

  /// Returns the y-coordinate of the top edge IFF rotation = 0.
  double top() const;

  /// Returns the y-coordinate of the bottom edge IFF rotation = 0.
  double bottom() const;

  //TODO convenient extensions:
  // topleft/bottomright, which respect the rotation (but not radius)!
//  Vec2d TopLeft() const;
//  Vec2d TopRight() const;
//  Vec2d BottomLeft() const;
//  Vec2d BottomRight() const;



  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add Python bindings
  //DONE [x] add Python test (tests/test_xxx.py)
  /** @brief Returns true if this rectangle can be drawn. */
  bool IsValid() const;


  /** @brief Returns a human-readable string representation. */
  std::string ToString() const;


  /** @brief Overloaded stream operator. */
  friend std::ostream &operator<<(std::ostream &os, const Rect &r) {
    os << r.ToString();
    return os;
  }

  static Rect FromLTWH(double left, double top, double width, double height,
                       double corner_radius = 0.0);

  static Rect FromLRTB(double left, double right, double top, double bottom,
                       double corner_radius = 0.0);

  static Rect FromCWH(double cx, double cy, double width, double height,
                      double rotation = 0.0, double corner_radius = 0.0);
};


//-------------------------------------------------  Comparison operators
bool operator==(const Rect& lhs, const Rect& rhs);
bool operator!=(const Rect& lhs, const Rect& rhs);

} // namespace viren2d

#endif // __VIREN2D_PRIMITIVES_H__
