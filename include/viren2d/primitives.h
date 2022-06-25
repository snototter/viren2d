#ifndef __VIREN2D_PRIMITIVES_H__
#define __VIREN2D_PRIMITIVES_H__

#include <stdexcept>
#include <string>
#include <ostream>
#include <initializer_list>
#include <cmath>

#include <werkzeugkiste/geometry/vector.h>


namespace viren2d {
//---------------------------------------------------- Image buffer
// TODO refactor imagebuffer into separate compilation unit
/**
 * @brief An ImageBuffer holds 8-bit images.
 *
 * Usage: either copy existing image data via @see CreateCopy(), or
 *        use existing memory via @see CreateSharedBuffer(). The
 *        latter does NOT take ownership of the memory (cleaning up
 *        remains the caller's responsibility).
 */
struct ImageBuffer {
public:
  unsigned char *data;  ///< Pointer to the image data.
  int width;            ///< Width of the image in pixels.
  int height;           ///< Height of the image in pixels.
  int channels;         ///< Number of channels.
  int stride;           ///< Stride (number of bytes) per row.

  bool OwnsData() const { return owns_data; }

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /** Creates an empty ImageBuffer. */
  ImageBuffer();


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /** @brief Allocates memory to hold a W x H x CH image. */
  ImageBuffer(int w, int h, int ch);

  ~ImageBuffer();

  //TODO [ ] add C++ test (tests/xxx_test.cpp) - fixme doc: copies data if other.owns_data; otherwise, *this will also be a shared buffer!
  ImageBuffer(const ImageBuffer &other); // copy c'tor

  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  ImageBuffer(ImageBuffer &&other) noexcept; // move c'tor

  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  ImageBuffer &operator=(const ImageBuffer &other); // copy assignment

  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  ImageBuffer &operator=(ImageBuffer &&other) noexcept; // move assignment


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /**
   * @brief Reuses the given image data.
   *
   * This ImageBuffer will point to the given image data. It will
   * NOT take ownership.
   */
  void CreateSharedBuffer(unsigned char *buffer, int width, int height, int channels, int stride);


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /**
   * @brief Copies the given image data.
   */
  void CreateCopy(unsigned char const *buffer, int width, int height, int channels, int stride);

  ImageBuffer CreateCopy() const;


  void SwapChannels(int ch1, int ch2);

  // TODO doc - sets owns_data = true; obviously, call it only if you want this buffer to free the memory!
  void TakeOwnership();


  /**
   * TODO documentation!
   *
   * Only the following conversions are supported:
   * * From single-channel to 1-, 3-, or 4-channel output.
   * * From 3-channel to 3- or 4-channel output, *i.e.* adding
   *   an alpha channel.
   * * From 4-channel to 3- or 4-channel output, *i.e.* removing
   *   the alpha channel.
   *
   * Other configurations are **not** supported.
   */
  ImageBuffer ToChannels(int output_channels) const;


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add Python bindings
  //TODO [ ] add Python test (tests/test_xxx.py)
  /** @brief Returns true if this buffer points to a valid memory location. */
  bool IsValid() const;

  /** @brief Returns a readable representation. */
  std::string ToString() const;

  /** @brief Overloaded stream operator. */
  friend std::ostream &operator<<(std::ostream &os, const ImageBuffer &buffer) {
    os << buffer.ToString();
    return os;
  }

private:
  bool owns_data;      ///< Flag indicating if we own the memory (i.e. if we need to clean up).

  void Cleanup();
};


//DONE [x] add documentation
//DONE [x] add C++ test (tests/xxx_test.cpp)
//DONE [x] add Python bindings (via Painter)
/**
 * @brief Loads an image from disk.
 *
 * We use the stb/stb_image library for reading/decoding.
 * Supported formats as of now:
 *   JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM
 *
 * Check for stb updates if your format is missing:
 *   https://github.com/nothings/stb/blob/master/stb_image.h
 *
 * @param image_filename Path to image file
 *
 * @param force_num_channels can be used to force the number of
 *              loaded channels (e.g. load a JPEG as RGBA by
 *              specifying force_num_channels=4).
 *              Supported:
 *                0: load as-is
 *                1: load as grayscale
 *                2: load as grayscale + alpha channel
 *                3: load as rgb
 *                4: load as rgb + alpha channel
 *
 * @return ImageBuffer
 */
ImageBuffer LoadImage(const std::string &image_filename, int force_num_channels=0);


/// Saves the ImageBuffer to disk as either JPEG or PNG.
///
/// We use the stb/stb_image_write library for writing. Note
/// that PNG output is not optimal (20-50% larger file size
/// than using an optimized library).
/// I consider writing to disk only a nice-to-have feature,
/// thus I'm not including any other specialized third-party
/// libraries for that.
void SaveImage(const std::string &image_filename, const ImageBuffer &image);


//TODO(snototter) rgb(a)2gray (single output channel vs 3 channels for drawing)


//DONE [x] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo
/** @brief Converts a grayscale ImageBuffer to RGB. */
ImageBuffer Gray2RGB(const ImageBuffer &img);


//DONE [x] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo
/** @brief Converts a grayscale ImageBuffer to RGBA. */
ImageBuffer Gray2RGBA(const ImageBuffer &img);


//DONE [x] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo
/** @brief Converts a RGBA ImageBuffer to RGB. */
ImageBuffer RGBA2RGB(const ImageBuffer &img);


//DONE [x] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo
/** @brief Converts a RGB ImageBuffer to RGBA. */
ImageBuffer RGB2RGBA(const ImageBuffer &img);


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
/**
 * @brief Rectangle for visualization.
 *
 * Note that it is defined by its CENTER coordinates
 * width, height, angle (clockwise rotation in degrees),
 * and a corner radius (for rounded rectangles).
 */
struct Rect {
  double cx;        ///< Center coordinate in x direction.
  double cy;        ///< Center coordinate in y direction.
  double width;     ///< Width of rectangle.
  double height;    ///< Height of rectangle.
  double rotation;  ///< Clockwise rotation in degrees.
  double radius;    ///< Corner radius. Must be <= min(width, height)/2. TODO doc percentage if < 1

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add Python bindings - not allowed
  //DONE [x] add Python test (tests/test_xxx.py)
  /** Default c'tor creats an invalid rectangle. */
  Rect()
    : cx(0.0), cy(0.0), width(0.0), height(0.0),
      rotation(0.0), radius(0.0)
  {}


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add Python bindings
  //DONE [x] add Python test (tests/test_xxx.py)
  /** Create a rectangle. */
  Rect(double center_x, double center_y, double w, double h,
       double rot = 0.0, double corner_radius = 0.0)
    : cx(center_x), cy(center_y), width(w), height(h),
      rotation(rot), radius(corner_radius)
  {}


  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  //DONE [x] add Python bindings
  //DONE [x] add Python test (tests/test_xxx.py)
  /** Create a rectangle. */
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

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /**
   * @brief Construct from an initializer list with 4 to 6 elements.
   * Minimum: {cx, cy, w, h}
   * Maximum: {cx, cy, w, h, rotation, corner_radius}
   */
  Rect(std::initializer_list<double> values);


  Vec2d Center() const;

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /** @brief Translate the center point by "offset" pixels in each dimension. */
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
  inline double half_width() const {
    return width / 2.0;
  }

  //DONE [x] add documentation
  //DONE [x] add C++ test (tests/xxx_test.cpp)
  /** @brief Returns half the height. */
  inline double half_height() const {
    return height / 2.0;
  }


  //TODO doc & test & bind
  Vec2d Size() const;
//  Vec2d Center() const;
  double left() const;
  double right() const;
  double top() const;
  double bottom() const;
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
