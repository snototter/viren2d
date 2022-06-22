#ifndef __VIREN2D_HELPERS_ENUM_H__
#define __VIREN2D_HELPERS_ENUM_H__

#include <type_traits>

// Provide bitwise operators for type-safe enums (must be in global scope)

/// Bitwise OR
template<typename Enum>
Enum operator |(Enum lhs, Enum rhs) {
  static_assert(std::is_enum<Enum>::value, "Template parameter is not an enum type");
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
    static_cast<underlying>(lhs) | static_cast<underlying>(rhs)
  );
}


/// Bitwise OR assignment
template<typename Enum>
Enum& operator |=(Enum& lhs, Enum rhs) {
  static_assert(std::is_enum<Enum>::value, "Template parameter is not an enum type");
  using underlying = typename std::underlying_type<Enum>::type;
  lhs = static_cast<Enum> (
    static_cast<underlying>(lhs) | static_cast<underlying>(rhs)
  );
  return lhs;
}


/// Bitwise AND
template<typename Enum>
Enum operator &(Enum lhs, Enum rhs) {
  static_assert(std::is_enum<Enum>::value, "Template parameter is not an enum type");
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
    static_cast<underlying>(lhs) & static_cast<underlying>(rhs)
  );
}


/// Bitwise AND assignment
template<typename Enum>
Enum& operator &=(Enum& lhs, Enum rhs) {
  static_assert(std::is_enum<Enum>::value, "Template parameter is not an enum type");
  using underlying = typename std::underlying_type<Enum>::type;
  lhs = static_cast<Enum> (
    static_cast<underlying>(lhs) & static_cast<underlying>(rhs)
  );
  return lhs;
}


/// Bitwise XOR
template<typename Enum>
Enum operator ^(Enum lhs, Enum rhs) {
  static_assert(std::is_enum<Enum>::value, "Template parameter is not an enum type");
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (
    static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs)
  );
}


/// Bitwise XOR assignment
template<typename Enum>
Enum& operator ^=(Enum& lhs, Enum rhs) {
  static_assert(std::is_enum<Enum>::value, "Template parameter is not an enum type");
  using underlying = typename std::underlying_type<Enum>::type;
  lhs = static_cast<Enum> (
    static_cast<underlying>(lhs) ^
    static_cast<underlying>(rhs)
  );
  return lhs;
}


/// Bitwise NOT
template<typename Enum>
Enum operator ~(Enum rhs) {
  static_assert(std::is_enum<Enum>::value, "Template parameter is not an enum type");
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<Enum> (~static_cast<underlying>(rhs));
}



namespace viren2d {
/// Check if the given flag is set.
template<typename Enum>
bool IsFlagSet(Enum value, Enum flag) {
  static_assert(std::is_enum<Enum>::value, "Template parameter is not an enum type");
  using underlying = typename std::underlying_type<Enum>::type;
  return static_cast<underlying>(value & flag) != 0;
}


template<typename EnumSpec, typename EnumBase>
bool IsFlagSet(EnumSpec value, EnumBase flag) {
  static_assert(std::is_enum<EnumSpec>::value, "Template parameter 'value' is not an enum type");
  static_assert(std::is_enum<EnumBase>::value, "Template parameter 'flag' is not an enum type");
  static_assert(std::is_same<typename std::underlying_type<EnumSpec>::type,
                             typename std::underlying_type<EnumBase>::type>::value,
                "Underlying type of both enums must be the same");
  using underlying = typename std::underlying_type<EnumSpec>::type;
  return (static_cast<underlying>(value) & static_cast<underlying>(flag)) != 0;
}


/// Iterator listing all enum values.
///
/// Only works for continuous enum definitions, i.e.
/// there cannot be "gaps" between enum values.
///
/// Taken from https://stackoverflow.com/a/31836401
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

