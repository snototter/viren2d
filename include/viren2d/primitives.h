#ifndef __VIREN2D_PRIMITIVES_H__
#define __VIREN2D_PRIMITIVES_H__

#include <stdexcept>
#include <string>
#include <ostream>
#include <initializer_list>

#include <viren2d/math.h>

// FIXME add type conversions to new types (initializer lists; built-in types for more convenient/less cluttered use)

namespace viren2d {
//---------------------------------------------------- Image buffer
/**
 * @brief An ImageBuffer holds 8-bit images.
 *
 * Usage: either copy existing image data via @see CreateCopy(), or
 *        use existing memory via @see CreateSharedBuffer(). The
 *        latter does NOT take ownership of the memory (cleaning up
 *        remains the caller's responsibility).
 */
struct ImageBuffer {
  unsigned char *data;
  int width;
  int height;
  int channels;
  int stride;    /**< Stride (number of bytes) per row. */
  bool owns_data_; /**< Flag indicating if we own the memory (i.e. if we need to clean up). */


  ImageBuffer()
    : data(nullptr), width(0), height(0), channels(0), stride(0),
      owns_data_(false)
  {}

  /** @brief Allocates memory to hold a W x H x CH image. */
  ImageBuffer(int w, int h, int ch);

  ~ImageBuffer();
  ImageBuffer(const ImageBuffer &other); // copy c'tor
  ImageBuffer(ImageBuffer &&other) noexcept; // move c'tor
  ImageBuffer &operator=(const ImageBuffer &other); // copy assignment
  ImageBuffer &operator=(ImageBuffer &&other) noexcept; // move assignment


  /**
   * @brief Reuses the given image data.
   *
   * This ImageBuffer will point to the given image data. It will
   * NOT take ownership.
   */
  void CreateSharedBuffer(unsigned char *buffer, int width, int height, int channels, int stride);


  /**
   * @brief Copies the given image data.
   */
  void CreateCopy(unsigned char const *buffer, int width, int height, int channels, int stride);


  /**
   * @brief Flips the red and green components in-place.
   *
   * This is useful if you're working with OpenCV's BGR format images.
   * *Watch out* if you're using an ImageBuffer initialized via
   * @see CreateSharedBuffer().
   */
  void RGB2BGR();


  /** @brief Converts this image to RGB. */
  ImageBuffer ToRGB() const;


  /** @brief Converts this image to RGBA. */
  ImageBuffer ToRGBA() const;


  /** @brief Returns a readable representation. */
  std::string ToString() const;

private:
  void Cleanup();
};


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


/**
 * @brief Saves the ImageBuffer to disk as either JPEG or PNG.
 *
 * We use the stb/stb_image_write library for writing. Note
 * that PNG output is not optimal (20-50% larger file size
 * than using an optimized library).
 * I consider writing to disk only a nice-to-have feature,
 * thus I'm not including any other specialized third-party
 * libraries for that.
 */
void SaveImage(const std::string &image_filename, const ImageBuffer &image);


//TODO rgb(a)2gray (single output channel vs 3 channels for drawing)

/** @brief Converts a grayscale ImageBuffer to RGB. */
ImageBuffer Gray2RGB(const ImageBuffer &img);


/** @brief Converts a grayscale ImageBuffer to RGBA. */
ImageBuffer Gray2RGBA(const ImageBuffer &img);


/** @brief Converts a RGBA ImageBuffer to RGB. */
ImageBuffer RGBA2RGB(const ImageBuffer &img);


/** @brief Converts a RGB ImageBuffer to RGBA. */
ImageBuffer RGB2RGBA(const ImageBuffer &img);


//------------------------------------------------- Vectors/Coordinates
/** @brief Template class to represent a vector/coordinate. */
template<typename _Tp, int dim>
class Vec {
 public:
  Vec();

  Vec(_Tp x, _Tp y);
  Vec(_Tp x, _Tp y, _Tp z);
  Vec(_Tp x, _Tp y, _Tp z, _Tp w);
  Vec(std::initializer_list<_Tp> values);

  Vec(const Vec<_Tp, dim>& other);

  ~Vec() {}

  Vec(Vec<_Tp, dim> &&other) noexcept; // move c'tor
  Vec<_Tp, dim> &operator=(const Vec<_Tp, dim> &other); // copy assignment
  Vec<_Tp, dim> &operator=(Vec<_Tp, dim> &&other) noexcept; // move assignment

  /** @brief Allow casting each vector to its double-precision counterpart.
   *  Needed because we work with cairo, which heavily uses doubles.
   */
  explicit operator Vec<double, dim>() const;

  const _Tp& operator[](int i) const;
  _Tp& operator[](int i);

  const _Tp& x() const;
  const _Tp& y() const;
  const _Tp& z() const;
  const _Tp& w() const;
  _Tp& x();
  _Tp& y();
  _Tp& z();
  _Tp& w();
  void SetX(_Tp x);
  void SetY(_Tp y);
  void SetZ(_Tp z);
  void SetW(_Tp w);

  _Tp val[dim];

  // Arithmetics
  Vec<_Tp, dim> &operator+=(const Vec<_Tp, dim>& rhs);
  Vec<_Tp, dim> &operator+=(double value);
  Vec<_Tp, dim> &operator-=(const Vec<_Tp, dim>& rhs);
  Vec<_Tp, dim> &operator-=(double value);
  Vec<_Tp, dim> &operator*=(double scale);
  Vec<_Tp, dim> &operator/=(double scale);

  /** Returns a vector where each dimension is negated. */
  Vec<_Tp, dim> operator-() const;


  /** @brief Computes the dot product. */
  _Tp Dot(const Vec<_Tp, dim>& other) const;


  /** @brief Returns the vector's length. */
  Vec<_Tp, dim> Cross(const Vec<_Tp, dim>& other) const;


  /** @brief Returns the vector's length. */
  double Length() const;


  /** @brief Returns the squared vector's length. */
  double LengthSquared() const;


  /** @brief Computes the distance between this and the other. */
  double Distance(const Vec<_Tp, dim>& other) const;


  //TODO test
  /** @brief Returns the direction vector from 'this' to 'to'. */
  Vec<_Tp, dim> DirectionVector(const Vec<_Tp, dim>& to) const;

  //TODO test
  /** @brief Returns the unit vector. */
  Vec<_Tp, dim> UnitVector() const;


  /** @brief Returns a human-readable string representation. */
  std::string ToString() const;


  /** @brief Overloaded stream operator. */
  friend std::ostream &operator<<(std::ostream &os, const Vec<_Tp, dim> &vec) {
    os << vec.ToString();
    return os;
  }


  /** @brief Returns the class type name, e.g. "Vec2d". */
  static std::string TypeName();
};


//-------------------------------------------------  Comparison operators
// If you implement another operator, don't forget
// to add the corresponding explicit vector instantiation
// in primitives.cpp

template<typename _Tp, int dim>
bool operator==(const Vec<_Tp, dim>& lhs, const Vec<_Tp, dim>& rhs);

template<typename _Tp, int dim>
bool operator!=(const Vec<_Tp, dim>& lhs, const Vec<_Tp, dim>& rhs);


//-------------------------------------------------  Arithmetic operators
// If you implement another operator, don't forget
// to add the corresponding explicit vector instantiation
// in primitives.cpp

/** Vector addition. */
template<typename _Tp, int dim>
Vec<_Tp, dim> operator+(Vec<_Tp, dim> lhs, const Vec<_Tp, dim>& rhs);


/** Vector subtraction. */
template<typename _Tp, int dim>
Vec<_Tp, dim> operator-(Vec<_Tp, dim> lhs, const Vec<_Tp, dim>& rhs);


/** Multiply (rhs) by scalar. */
template<typename _Tp, int dim>
Vec<_Tp, dim> operator*(Vec<_Tp, dim> lhs, double rhs);


/** Multiply (lhs) by scalar. */
template<typename _Tp, int dim>
Vec<_Tp, dim> operator*(double lhs, Vec<_Tp, dim> rhs);


/** Divide (scale) by scalar. */
template<typename _Tp, int dim>
Vec<_Tp, dim> operator/(Vec<_Tp, dim> lhs, double rhs);


//-------------------------------------------------  Available specializations:
typedef Vec<double, 2> Vec2d;
typedef Vec<double, 3> Vec3d;
typedef Vec<double, 4> Vec4d;

typedef Vec<int, 2> Vec2i;
typedef Vec<int, 3> Vec3i;


//-------------------------------------------------  Vector Math
//TODO test
/** @brief Project point onto line. */
Vec2d ProjectPointOntoLine(const Vec2d &pt,
                           const Vec2d &line_from,
                           const Vec2d &line_to);

//-------------------------------------------------  Rectangle
/**
 * @brief Rectangle for visualization.
 *
 * Note that it is defined by its CENTER coordinates
 * width, height, angle (clockwise rotation in degrees),
 * and a corner radius (for rounded rectangles).
 */
struct Rect {
  double cx; /**< Center coordinate in x direction. */
  double cy; /**< Center coordinate in y direction. */
  double width;  /**< Width of rectangle. */
  double height; /**< Height of rectangle. */
  double angle;  /**< Clockwise rotation in degrees. */
  double radius; /**< Corner radius. Must be <= min(width, height)/2. */

  Rect()
    : cx(0.0), cy(0.0), width(0.0), height(0.0),
      angle(0.0), radius(0.0)
  {}


  Rect(double center_x, double center_y, double w, double h)
    : cx(center_x), cy(center_y), width(w), height(h),
      angle(0.0), radius(0.0)
  {}


  Rect(double center_x, double center_y, double w, double h,
       double rot)
    : cx(center_x), cy(center_y), width(w), height(h),
      angle(rot), radius(0.0)
  {}


  Rect(double center_x, double center_y, double w, double h,
       double rot, double corner_radius)
    : cx(center_x), cy(center_y), width(w), height(h),
      angle(rot), radius(corner_radius)
  {}

  /**
   * Construct from initializer list with 4 to 6 elements (refer
   * to any other c'tor for the order of parameters).
   */
  Rect(std::initializer_list<double> values);


  inline double half_width() const {
    return width / 2.0;
  }


  inline double half_height() const {
    return height / 2.0;
  }


  bool IsValid() const {
    return width > 0.0 && height > 0.0;
  }

  std::string ToString() const;

  friend std::ostream &operator<<(std::ostream &os, const Rect &r) {
    os << r.ToString();
    return os;
  }


  //TODO nice-to-have operator overloads
  // +(Vec2d) translates
  // *(double) scales the box
  // +(double) rotates
  // ---> add these only if needed, e.g. if we frequently
  //      need to shift/scale/rotate rects when drawing
  //      (which I highly doubt atm)
};


//-------------------------------------------------  Comparison operators
bool operator==(const Rect& lhs, const Rect& rhs);
bool operator!=(const Rect& lhs, const Rect& rhs);

} // namespace viren2d

#endif // __VIREN2D_PRIMITIVES_H__
