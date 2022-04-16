#include <sstream>
#include <iomanip>
#include <type_traits>
#include <stdexcept>

#include <vivi/primitives.hpp>
#include <vivi/math.hpp>

namespace vivi
{
//---------------------------------------------------- Templated vector class
template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec()
{
  for (int i = 0; i < dim; ++i)
    val[i] = static_cast<_Tp>(0);
}

template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec(_Tp x, _Tp y)
{
  if (dim != 2)
  {
    std::stringstream s;
    s << "You cannot initialize " << TypeName() << " with 2 values.";
    throw std::invalid_argument(s.str());
  }
  val[0] = x;
  val[1] = y;
}

template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec(_Tp x, _Tp y, _Tp z)
{
  if (dim != 3)
  {
    std::stringstream s;
    s << "You cannot initialize " << TypeName() << " with 3 values.";
    throw std::invalid_argument(s.str());
  }
  val[0] = x;
  val[1] = y;
  val[2] = z;
}


template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec(_Tp x, _Tp y, _Tp z, _Tp w)
{
  if (dim != 4)
  {
    std::stringstream s;
    s << "You cannot initialize " << TypeName() << " with 4 values.";
    throw std::invalid_argument(s.str());
  }
  val[0] = x;
  val[1] = y;
  val[2] = z;
  val[3] = w;
}


template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec(const Vec<_Tp, dim>& other)
{
  for (int i = 0; i < dim; ++i)
    val[i] = other.val[i];
}

template<typename _Tp, int dim>
Vec<_Tp, dim>::operator Vec<double, dim>() const
{
  Vec<double, dim> conv;
  for (int i = 0; i < dim; ++i)
    conv.val[i] = static_cast<double>(val[i]);
  return conv;
}


template<typename _Tp, int dim>
const _Tp& Vec<_Tp, dim>::operator[](int i) const
{
  if (i >= dim)
  {
    std::stringstream s;
    s << "Index-out-of-bounds: cannot access ["
      << i << "] for " << TypeName() << ".";
    throw std::out_of_range(s.str());
  }
  return val[i];
}


template<typename _Tp, int dim>
_Tp& Vec<_Tp, dim>::operator[](int i)
{
  if (i >= dim)
  {
    std::stringstream s;
    s << "Index-out-of-bounds: cannot access ["
      << i << "] for " << TypeName() << ".";
    throw std::out_of_range(s.str());
  }
  return val[i];
}


template<typename _Tp, int dim>
const _Tp& Vec<_Tp, dim>::x() const
{
  return (*this)[0];
}


template<typename _Tp, int dim>
const _Tp& Vec<_Tp, dim>::y() const
{
  return (*this)[1];
}


template<typename _Tp, int dim>
const _Tp& Vec<_Tp, dim>::z() const
{
  return (*this)[2];
}


template<typename _Tp, int dim>
const _Tp& Vec<_Tp, dim>::w() const
{
  return (*this)[3];
}


template<typename _Tp, int dim>
_Tp& Vec<_Tp, dim>::x()
{
  return (*this)[0];
}


template<typename _Tp, int dim>
_Tp& Vec<_Tp, dim>::y()
{
  return (*this)[1];
}


template<typename _Tp, int dim>
_Tp& Vec<_Tp, dim>::z()
{
  return (*this)[2];
}


template<typename _Tp, int dim>
_Tp& Vec<_Tp, dim>::w()
{
  return (*this)[3];
}


template<typename _Tp, int dim>
void Vec<_Tp, dim>::SetX(_Tp x)
{
  (*this)[0] = x;
}


template<typename _Tp, int dim>
void Vec<_Tp, dim>::SetY(_Tp y)
{
  (*this)[1] = y;
}


template<typename _Tp, int dim>
void Vec<_Tp, dim>::SetZ(_Tp z)
{
  (*this)[2] = z;
}


template<typename _Tp, int dim>
void Vec<_Tp, dim>::SetW(_Tp w)
{
  (*this)[3] = w;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> &Vec<_Tp, dim>::operator+=(const Vec<_Tp, dim>& rhs)
{
  for (int i = 0; i < dim; ++i)
    val[i] += rhs[i];
  return *this;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> &Vec<_Tp, dim>::operator-=(const Vec<_Tp, dim>& rhs)
{
  for (int i = 0; i < dim; ++i)
    val[i] -= rhs[i];
  return *this;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> &Vec<_Tp, dim>::operator*=(double scale)
{
  for (int i = 0; i < dim; ++i)
    val[i] *= scale;
  return *this;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> &Vec<_Tp, dim>::operator/=(double scale)
{
  for (int i = 0; i < dim; ++i)
    val[i] /= scale;
  return *this;
}


template<typename _Tp, int dim>
_Tp Vec<_Tp, dim>::Dot(const Vec<_Tp, dim>& other)
{
  _Tp s = static_cast<_Tp>(0);
  for (int i = 0; i < dim; ++i)
    s += val[i] * other.val[i];
  return s;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> Vec<_Tp, dim>::Cross(const Vec<_Tp, dim>& other)
{
  if (dim != 3)
    throw std::logic_error("Cross product is only defined for 3-dim vectors!");
  // There's actually an analog for 2d space, but I didn't need
  // it yet: https://mathworld.wolfram.com/CrossProduct.html

  return Vec<_Tp, dim>(val[1] * other.val[2] - val[2] * other.val[1],
                       val[2] * other.val[0] - val[0] * other.val[2],
                       val[0] * other.val[1] - val[1] * other.val[0]);
}


template<typename _Tp, int dim>
double Vec<_Tp, dim>::Length() const
{
  double squared = 0.0;
  for (int i = 0; i < dim; ++i)
    squared += static_cast<double>(val[i] * val[i]);
  return std::sqrt(squared);
}


template<typename _Tp, int dim>
double Vec<_Tp, dim>::Distance(const Vec<_Tp, dim>& other)
{
  auto diff = *this - other;
  return diff.Length();
}


// Typename to char lookup:
template<typename _Tp> char VecType();
template<> char VecType<unsigned char>() { return 'b'; }
template<> char VecType<short>()         { return 's'; }
template<> char VecType<int>()           { return 'i'; }
template<> char VecType<double>()        { return 'd'; }


template<typename _Tp, int dim>
std::string Vec<_Tp, dim>::TypeName()
{
  std::stringstream s;
  s << "Vec" << dim << VecType<_Tp>();
  return s.str();
}


template<typename _Tp, int dim>
std::string Vec<_Tp, dim>::ToString() const
{
  std::stringstream s;
  s << Vec<_Tp, dim>::TypeName() << "("
    << std::fixed << std::setprecision(2);
  for (int i = 0; i < dim; ++i)
  {
    s << val[i];
    if (i < dim -1)
      s << ", ";
  }
  s << ")";
  return s.str();
}

//---------------------------------------------------- Vector operators
template<typename _Tp, int dim>
bool operator==(const Vec<_Tp, dim>& lhs, const Vec<_Tp, dim>& rhs)
{
  for (int i = 0; i < dim; ++i)
  {
    if (!eps_equal(lhs.val[i], rhs.val[i]))
    {
      return false;
    }
  }
  return true;
}


template<typename _Tp, int dim>
bool operator!=(const Vec<_Tp, dim>& lhs, const Vec<_Tp, dim>& rhs)
{
  return !(lhs == rhs);
}


template<typename _Tp, int dim>
Vec<_Tp, dim> operator+(Vec<_Tp, dim> lhs, const Vec<_Tp, dim>& rhs)
{
  lhs += rhs;
  return lhs;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> operator-(Vec<_Tp, dim> lhs, const Vec<_Tp, dim>& rhs)
{
  lhs -= rhs;
  return lhs;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> operator*(Vec<_Tp, dim> lhs, double scale)
{
  lhs *= scale;
  return lhs;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> operator*(double scale, Vec<_Tp, dim> rhs)
{
  rhs *= scale;
  return rhs;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> operator/(Vec<_Tp, dim> lhs, double scale)
{
  lhs /= scale;
  return lhs;
}


//---------------------------------------------------- Explicit vector instantiation
// Explicit instantiation:
template class Vec<double, 2>;
template class Vec<double, 3>;
template class Vec<double, 4>;

template class Vec<int, 2>;
template class Vec<int, 3>;

// Comparison Vec2d
template bool operator==(const Vec2d& lhs, const Vec2d& rhs);
template bool operator!=(const Vec2d& lhs, const Vec2d& rhs);
// Arithmetic Vec2d
template Vec2d operator+(Vec2d lhs, const Vec2d& rhs);
template Vec2d operator-(Vec2d lhs, const Vec2d& rhs);
template Vec2d operator*(Vec2d lhs, double scale);
template Vec2d operator*(double scale, Vec2d rhs);
template Vec2d operator/(Vec2d lhs, double scale);


// Comparison Vec3d
template bool operator==(const Vec3d& lhs, const Vec3d& rhs);
template bool operator!=(const Vec3d& lhs, const Vec3d& rhs);
// Arithmetic Vec3d
template Vec3d operator+(Vec3d lhs, const Vec3d& rhs);
template Vec3d operator-(Vec3d lhs, const Vec3d& rhs);
template Vec3d operator*(Vec3d lhs, double scale);
template Vec3d operator*(double scale, Vec3d rhs);
template Vec3d operator/(Vec3d lhs, double scale);


// Comparison Vec4d
template bool operator==(const Vec4d& lhs, const Vec4d& rhs);
template bool operator!=(const Vec4d& lhs, const Vec4d& rhs);
// Arithmetic Vec4d
template Vec4d operator+(Vec4d lhs, const Vec4d& rhs);
template Vec4d operator-(Vec4d lhs, const Vec4d& rhs);
template Vec4d operator*(Vec4d lhs, double scale);
template Vec4d operator*(double scale, Vec4d rhs);
template Vec4d operator/(Vec4d lhs, double scale);


// Comparison Vec2i
template bool operator==(const Vec2i& lhs, const Vec2i& rhs);
template bool operator!=(const Vec2i& lhs, const Vec2i& rhs);
// Arithmetic Vec2i
template Vec2i operator+(Vec2i lhs, const Vec2i& rhs);
template Vec2i operator-(Vec2i lhs, const Vec2i& rhs);
template Vec2i operator*(Vec2i lhs, double scale);
template Vec2i operator*(double scale, Vec2i rhs);
template Vec2i operator/(Vec2i lhs, double scale);


// Comparison Vec3i
template bool operator==(const Vec3i& lhs, const Vec3i& rhs);
template bool operator!=(const Vec3i& lhs, const Vec3i& rhs);
// Arithmetic Vec3i
template Vec3i operator+(Vec3i lhs, const Vec3i& rhs);
template Vec3i operator-(Vec3i lhs, const Vec3i& rhs);
template Vec3i operator*(Vec3i lhs, double scale);
template Vec3i operator*(double scale, Vec3i rhs);
template Vec3i operator/(Vec3i lhs, double scale);

//---------------------------------------------------- Rectangle
std::string Rect::ToString() const
{
  std::stringstream s;
  s << "Rect(" << std::fixed << std::setprecision(1)
    << cx << ", " << cy << ", " << width << ", " << height
    << "; rot=" << angle << "°, radius=" << radius << ")";
  return s.str();
}

bool operator==(const Rect& lhs, const Rect& rhs)
{
  return eps_equal(lhs.cx, rhs.cx) && eps_equal(lhs.cy, rhs.cy)
      && eps_equal(lhs.width, rhs.width) && eps_equal(lhs.height, rhs.height)
      && eps_equal(lhs.angle, rhs.angle) && eps_equal(lhs.radius, rhs.radius);
}

bool operator!=(const Rect& lhs, const Rect& rhs)
{
  return !(lhs == rhs);
}

} // namespace vivi
