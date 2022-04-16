#ifndef __VIVI_MATH_H__
#define __VIVI_MATH_H__

#include <limits>
#include <type_traits>

#include <math.h>

namespace vivi
{

inline double deg2rad(double deg)
{
  return deg * M_PI / 180.0;
}

//TODO check
// Informative blog post about the caveats:
// https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
template<typename T>
inline bool eps_zero(T x)
{
  if (std::is_floating_point<T>::value)
  {
    return std::fabs(x) < 2*std::numeric_limits<T>::epsilon();
  }
  else
  {
    return x == static_cast<T>(0);
  }
}


/** @brief Uses the machine epsilon to check for equality based on the desired
 *  precision in ULPs (units in the last place).
 *  Watch out: never use to check against 0! For example, 0.0 is NOT eps_equal to 1.1e-16!
 */
template<typename T>
inline bool eps_equal(T x, T y, int ulp=2)
{
  if (std::is_floating_point<T>::value)
  {
    // Adapted (using fabs) from the STL reference:
    // https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
    // The machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::fabs(x-y) <= std::numeric_limits<T>::epsilon() * std::fabs(x+y) * ulp
    // unless the result is subnormal
           || std::fabs(x-y) < std::numeric_limits<T>::min();
  }
  else
      return x == y;
}

//template<class T>
//typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
//    eps_equal(T x, T y, int ulp=2)
//{
//  // Adapted (using fabs) from the STL reference:
//  // https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
//  // The machine epsilon has to be scaled to the magnitude of the values used
//  // and multiplied by the desired precision in ULPs (units in the last place)
//  return std::fabs(x-y) <= std::numeric_limits<T>::epsilon() * std::fabs(x+y) * ulp
//  // unless the result is subnormal
//         || std::fabs(x-y) < std::numeric_limits<T>::min();
//}

} // namespace vivi

#endif // __VIVI_MATH_H__

