#ifndef __VIVI_PRIMITIVES_H__
#define __VIVI_PRIMITIVES_H__

#include <stdexcept>
#include <string>
#include <ostream>

#include <vivi/math.hpp>


namespace vivi
{
//---------------------------------------------------- Image buffer
//TODO support only RGBA
// Idea: getcanvas returns imagebuffer via move
struct ImageBuffer
{
  unsigned char *data;
  int width;
  int height;
  int channels;
  int stride;
  bool owns_data_; // TODO doc: we can use a shared memory buffer (won't be cleaned up) or allocate our own (will be cleaned up)


  ImageBuffer()
    : data(nullptr), width(0), height(0), channels(0), stride(0),
      owns_data_(false)
  {}

  ~ImageBuffer();
  ImageBuffer(const ImageBuffer &other); // copy c'tor
  ImageBuffer(ImageBuffer &&other) noexcept; // move c'tor
  ImageBuffer &operator=(const ImageBuffer &other); // copy assignment
  ImageBuffer &operator=(ImageBuffer &&other) noexcept; // move assignment

  //will NOT take ownership - you are responsible for cleaning up the buffer
  void CreateSharedBuffer(unsigned char *buffer, int width, int height, int channels, int stride);

  // makes a 1:1 memory copy
  void CreateCopy(unsigned char *buffer, int width, int height, int channels, int stride);

  // Flips red and green in-place
  // Useful, if you're working with OpenCV's BGR format (e.g. for displaying)
  // Watch out if you're using a sharedbuffer!
  void RGB2BGR();

private:
  void Cleanup();
};

//------------------------------------------------- Vectors/Coordinates
//TODO doc
template<typename _Tp, int dim>
class Vec
{
public:
  using VT = Vec<_Tp, dim>;

  Vec();

  Vec(_Tp x, _Tp y);
  Vec(_Tp x, _Tp y, _Tp z);
  Vec(_Tp x, _Tp y, _Tp z, _Tp w);
  Vec(const Vec<_Tp, dim>& other);

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
  Vec<_Tp, dim> &operator-=(const Vec<_Tp, dim>& rhs);
  Vec<_Tp, dim> &operator*=(double scale);
  Vec<_Tp, dim> &operator/=(double scale);

  _Tp Dot(const Vec<_Tp, dim>& other);
  Vec<_Tp, dim> Cross(const Vec<_Tp, dim>& other);
  double Length() const;
  double Distance(const Vec<_Tp, dim>& other);

  /** @brief Returns the class type name, e.g. "Vec2d" */
  static std::string TypeName();

  std::string ToString() const;

  friend std::ostream &operator<<(std::ostream &os, const Vec<_Tp, dim> &vec)
  {
    os << vec.ToString();
    return os;
  }
};


//-------------------------------------------------  Comparison operators
template<typename _Tp, int dim>
bool operator==(const Vec<_Tp, dim>& lhs, const Vec<_Tp, dim>& rhs);
template<typename _Tp, int dim>
bool operator!=(const Vec<_Tp, dim>& lhs, const Vec<_Tp, dim>& rhs);


//-------------------------------------------------  Arithmetic operators
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
// divide
template<typename _Tp, int dim>
Vec<_Tp, dim> operator/(Vec<_Tp, dim> lhs, double rhs);


//-------------------------------------------------  Available specializations:
typedef Vec<double, 2> Vec2d;
typedef Vec<double, 3> Vec3d;
typedef Vec<double, 4> Vec4d;

typedef Vec<int, 2> Vec2i;
typedef Vec<int, 3> Vec3i;


//-------------------------------------------------  Rectangle
//TODO doc
struct Rect
{
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


  inline double half_width() const
  { return width / 2.0; }


  inline double half_height() const
  { return height / 2.0; }


  bool Empty() const
  { return eps_zero(width) || eps_zero(height); }

  std::string ToString() const;

  friend std::ostream &operator<<(std::ostream &os, const Rect &r)
  {
    os << r.ToString();
    return os;
  }
};


//-------------------------------------------------  Comparison operators
bool operator==(const Rect& lhs, const Rect& rhs);
bool operator!=(const Rect& lhs, const Rect& rhs);

} // namespace vivi

#endif // __VIVI_PRIMITIVES_H__
