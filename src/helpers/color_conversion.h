#ifndef __VIREN2D_COLOR_CONVERSION_HELPERS_H__
#define __VIREN2D_COLOR_CONVERSION_HELPERS_H__

#include <utility>
#include <tuple>


#include <werkzeugkiste/geometry/utils.h>
namespace wkg = werkzeugkiste::geometry;


namespace viren2d {
namespace helpers {

/// Returns the value and index of the maximum parameter.
template <typename _Tp, typename... _Ts>
inline std::pair<_Tp, int> MaxValueIndex(_Tp val0, _Ts... rest) {
  const int num_el = 1 + sizeof...(rest);
  const _Tp lst[num_el] = {val0, static_cast<_Tp>(rest)...};

  int max_idx = 0;
  for (int idx = 1; idx < num_el; ++idx) {
    if (lst[idx] > lst[max_idx]) {
      max_idx = idx;
    }
  }
  return std::make_pair(lst[max_idx], max_idx);
}


/// Converts one r,g,b (in [0, 1]) value to HSV, with
/// hue in [0, 360], saturation & value in [0, 1].
inline std::tuple<float, float, float> CvtHelperRGB2HSV(
    float r, float g, float b) {
  const std::pair<float, int> max_val_idx = MaxValueIndex(r, g, b);
  const float min_val = std::min(r, std::min(g, b));
  const float delta = max_val_idx.first - min_val;

  float hue, sat;
  if (wkg::IsEpsZero(max_val_idx.first)
      || wkg::IsEpsZero(delta)) {
    sat = 0.0f;
    hue = 0.0f;
  } else {
    sat = delta / max_val_idx.first;
    if (max_val_idx.second == 0) {  // Max is at red
      hue = 60.0f * ((g - b) / delta);
    } else if (max_val_idx.second == 1) {  // Max is at green
      hue = 60.0f * ((b - r) / delta) + 120.0f;
    } else {  // Max is at blue
      hue = 60.0f * ((r - g) / delta) + 240.0f;
    }
  }

  if (hue < 0.0f) {
    hue += 360.0f;
  }

  return std::make_tuple(hue, sat, max_val_idx.first);
}


/// Converts one HSV value (hue in [0, 360], saturation & value in [0, 1]) to
/// r,g,b (in [0, 1]).
inline std::tuple<float, float, float> CvtHelperHSV2RGB(
    float hue, float sat, float val) {
  const int hue_bin = static_cast<int>(hue / 60.0f) % 6;
  const float rem = (hue / 60.0f) - hue_bin;
  const float p  = val * (1.0f - sat);
  const float q  = val * (1.0f - sat * rem);
  const float t  = val * (1.0f - sat * (1.0f - rem));

  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;

  switch(hue_bin) {
    case 0:
      r = val, g = t, b = p;
      break;

    case 1:
      r = q, g = val, b = p;
      break;

    case 2:
      r = p, g = val, b = t;
    break;

    case 3:
      r = p, g = q, b = val;
      break;

    case 4:
      r = t, g = p, b = val;
      break;

    case 5:
      r = val, g = p, b = q;
      break;
  }

  return std::make_tuple(r, g, b);
}


template <typename _Tp> inline
_Tp CvtHelperRGB2Gray(_Tp red, _Tp green, _Tp blue) {
  return static_cast<_Tp>(
      (0.2989 * red) + (0.5870 * green) + (0.1141 * blue));
}

} // namespace helpers
} // namespace viren2d

#endif // __VIREN2D_COLOR_CONVERSION_HELPERS_H__
