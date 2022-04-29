#ifndef __VIREN2D_HELPERS_ENUM_H__
#define __VIREN2D_HELPERS_ENUM_H__

#include <type_traits>

namespace viren2d {
/**
 * @brief Iterator listing all enum values.
 *
 * Only works for continuous enum definitions, i.e.
 * there cannot be "gaps" between enum values.
 *
 * Taken from https://stackoverflow.com/a/31836401
 */
template <typename C, C beginval, C endval>
class ContinuousEnumIterator {
 private:
  typedef typename std::underlying_type<C>::type val_t;
  int val;

 public:
  ContinuousEnumIterator(const C & f)
    : val(static_cast<val_t>(f))
  {}

  ContinuousEnumIterator()
    : val(static_cast<val_t>(beginval))
  {}

  ContinuousEnumIterator operator++() {
    ++val;
    return *this;
  }

  C operator*() {
    return static_cast<C>(val);
  }

  ContinuousEnumIterator begin() const {
    return *this;  // default ctor is good
  }

  ContinuousEnumIterator end() const {
      static const ContinuousEnumIterator enditer = ++ContinuousEnumIterator(endval);  // cache it
      return enditer;
  }

  bool operator!=(const ContinuousEnumIterator& i) {
    return val != i.val;
  }
};

}  // namespace viren2d

#endif  // __VIREN2D_HELPERS_ENUM_H__

