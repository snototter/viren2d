#ifndef __VIVI_PRIMITIVES_H__
#define __VIVI_PRIMITIVES_H__

#include <stdexcept>
#include <string>
#include <ostream>

#include <vivi/math.hpp>


namespace vivi
{
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
