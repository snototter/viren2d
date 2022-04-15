#ifndef __VIVI_PRIMITIVES_H__
#define __VIVI_PRIMITIVES_H__

#include <stdexcept>
#include <string>
#include <ostream>


namespace vivi
{
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


template<typename _Tp, int dim>
bool operator==(const Vec<_Tp, dim>& lhs, const Vec<_Tp, dim>& rhs);
template<typename _Tp, int dim>
bool operator!=(const Vec<_Tp, dim>& lhs, const Vec<_Tp, dim>& rhs);
//TODO <, >, ...

// Arithmetic
template<typename _Tp, int dim>
Vec<_Tp, dim> operator+(Vec<_Tp, dim> lhs, const Vec<_Tp, dim>& rhs);
template<typename _Tp, int dim>
Vec<_Tp, dim> operator-(Vec<_Tp, dim> lhs, const Vec<_Tp, dim>& rhs);

template<typename _Tp, int dim>
Vec<_Tp, dim> operator*(Vec<_Tp, dim> lhs, double rhs);
template<typename _Tp, int dim>
Vec<_Tp, dim> operator*(double lhs, Vec<_Tp, dim> rhs);

template<typename _Tp, int dim>
Vec<_Tp, dim> operator/(Vec<_Tp, dim> lhs, double rhs);


// Available specializations:
typedef Vec<double, 2> Vec2d;
typedef Vec<double, 3> Vec3d;
typedef Vec<double, 4> Vec4d;

typedef Vec<int, 2> Vec2i;
typedef Vec<int, 3> Vec3i;

} // namespace vivi

#endif // __VIVI_PRIMITIVES_H__
