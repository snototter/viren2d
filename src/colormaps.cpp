#include <algorithm>
#include <exception>
#include <sstream>
#include <string>
#include <cmath>

#include <werkzeugkiste/strings/strings.h>

#include <viren2d/colormaps.h>

#include <helpers/colormaps_helpers.h>
#include <helpers/enum.h>
#include <helpers/logging.h>

namespace wks = werkzeugkiste::strings;

namespace viren2d {
std::string ColorMapToString(ColorMap cm) {
  switch (cm) {
    case ColorMap::BlackBody:
      return "black-body";
    case ColorMap::Cold:
      return "cold";
    case ColorMap::ColorBlind:
      return "color-blind";
    case ColorMap::Disparity:
      return "disparity";
    case ColorMap::Earth:
      return "earth";
    case ColorMap::GlasbeyDark:
      return "glasbey-dark";
    case ColorMap::GlasbeyLight:
      return "glasbey-light";
    case ColorMap::Gouldian:
      return "gouldian";
    case ColorMap::Gray:
      return "gray";
    case ColorMap::Hell:
      return "hell";
    case ColorMap::Hot:
      return "hot";
    case ColorMap::HSV:
      return "hsv";
    case ColorMap::Inferno:
      return "inferno";
    case ColorMap::Ocean:
      return "ocean";
    case ColorMap::Orientation:
      return "orientation";
    case ColorMap::OrientationColorBlind:
      return "orientation-color-blind";
    case ColorMap::Rainbow:
      return "rainbow";
    case ColorMap::Relief:
      return "relief";
    case ColorMap::ReliefLowContrast:
      return "relief-low-contrast";
    case ColorMap::Temperature:
      return "temperature";
    case ColorMap::TemperatureDark:
      return "temperature-dark";
    case ColorMap::Terrain:
      return "terrain";
    case ColorMap::Thermal:
      return "thermal";
    case ColorMap::Turbo:
      return "turbo";
    case ColorMap::Viridis:
      return "viridis";
    case ColorMap::Water:
      return "water";
    case ColorMap::Yarg:
      return "yarg";
  }

  //TODO(dev) For each new color map, also extend ColorMapFromString!

  std::ostringstream s;
  s << "ColorMap (" << static_cast<int>(cm)
    << ") is not yet supported in `ColorMapToString`!";
  throw std::logic_error(s.str());
}


ColorMap ColorMapFromString(const std::string &cm) {
  std::string lower = wks::Lower(cm);
  lower.erase(std::remove_if(lower.begin(), lower.end(), [](char ch) -> bool {
      return ::isspace(ch) || (ch == '-') || (ch == '_');
    }), lower.end());
  if (lower.compare("blackbody") == 0) {
    return ColorMap::BlackBody;
  } else if (lower.compare("cold") == 0) {
    return ColorMap::Cold;
  } else if (lower.compare("colorblind") == 0) {
    return ColorMap::ColorBlind;
  } else if (lower.compare("disparity") == 0) {
    return ColorMap::Disparity;
  } else if (lower.compare("earth") == 0) {
    return ColorMap::Earth;
  } else if (lower.compare("glasbeydark") == 0) {
    return ColorMap::GlasbeyDark;
  } else if (lower.compare("glasbeylight") == 0) {
    return ColorMap::GlasbeyLight;
  } else if (lower.compare("gouldian") == 0) {
    return ColorMap::Gouldian;
  } else if ((lower.compare("gray") == 0)
             || (lower.compare("grey") == 0)) {
    return ColorMap::Gray;
  } else if (lower.compare("hell") == 0) {
    return ColorMap::Hell;
  } else if (lower.compare("hot") == 0) {
    return ColorMap::Hot;
  } else if (lower.compare("hsv") == 0) {
    return ColorMap::HSV;
  } else if (lower.compare("inferno") == 0) {
    return ColorMap::Inferno;
  } else if (lower.compare("ocean") == 0) {
    return ColorMap::Ocean;
  } else if (lower.compare("orientation") == 0) {
    return ColorMap::Orientation;
  } else if (lower.compare("orientationcolorblind") == 0) {
    return ColorMap::OrientationColorBlind;
  } else if (lower.compare("rainbow") == 0) {
    return ColorMap::Rainbow;
  } else if (lower.compare("relief") == 0) {
    return ColorMap::Relief;
  } else if ((lower.compare("reliefisoluminant") == 0)
             || (lower.compare("relieflowcontrast") == 0)) {
    return ColorMap::ReliefLowContrast;
  } else if (lower.compare("temperature") == 0) {
    return ColorMap::Temperature;
  } else if (lower.compare("temperaturedark") == 0) {
    return ColorMap::TemperatureDark;
  } else if (lower.compare("terrain") == 0) {
    return ColorMap::Terrain;
  } else if (lower.compare("thermal") == 0) {
    return ColorMap::Thermal;
  } else if (lower.compare("turbo") == 0) {
    return ColorMap::Turbo;
  } else if (lower.compare("viridis") == 0) {
    return ColorMap::Viridis;
  } else if (lower.compare("water") == 0) {
    return ColorMap::Water;
  } else if ((lower.compare("yarg") == 0)
             || (lower.compare("yerg") == 0)
             || (lower.compare("-gray") == 0)
             || (lower.compare("!gray") == 0)
             || (lower.compare("-grey") == 0)
             || (lower.compare("!grey") == 0)) {
    return ColorMap::Yarg;
  }

  std::string s("Could not deduce `ColorMap` from string representation \"");
  s += cm;
  s += "\"!";
  throw std::logic_error(s);
}


std::ostream &operator<<(std::ostream &os, ColorMap cm) {
  os << ColorMapToString(cm);
  return os;
}


std::vector<ColorMap> ListColorMaps() {
  std::vector<ColorMap> lst;
  typedef ContinuousEnumIterator<ColorMap,
    ColorMap::BlackBody, ColorMap::Yarg> ColormapIterator;

  for (ColorMap cm: ColormapIterator()) {
    lst.push_back(cm);
  }
  return lst;
}


template <typename _Tp>
ImageBuffer ColorLookup(
    const ImageBuffer &data, const helpers::RGBColor *map,
    double limit_low, double limit_high, int output_channels, int bins) {
  // Sanity checks have been performed in `Colorize`
  ImageBuffer dst(
        data.Height(), data.Width(), output_channels, ImageBufferType::UInt8);

  --bins;
  const double map_idx_factor = 255.0 / bins;
  const double interval = (limit_high - limit_low) / bins;
  const double divider = (std::fabs(interval) > 0.0) ? interval : 1.0;

  int rows = data.Height();
  int cols = data.Width();

  if (data.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  for (int row = 0; row < rows; ++row) {
    const _Tp *data_ptr = data.ImmutablePtr<_Tp>(row, 0, 0);
    unsigned char *dst_ptr = dst.MutablePtr<unsigned char>(row, 0, 0);
    for (int col = 0; col < cols; ++col) {
      double value = std::max(
            limit_low,
            std::min(
              limit_high,
              static_cast<double>(*data_ptr)));
      int bin = std::max(
            0, std::min(
              255, static_cast<int>(
                map_idx_factor * std::floor((value - limit_low) / divider))));

      *dst_ptr++ = map[bin].red;
      *dst_ptr++ = map[bin].green;
      *dst_ptr++ = map[bin].blue;
      if (output_channels == 4) {
        *dst_ptr++ = 255;
      }
      ++data_ptr;
    }
  }

  return dst;
}



Colorizer::Colorizer(ColorMap cmap,
    LimitsMode mode, int num_bins, int channels_out,
    double low, double high)
  : colormap(cmap), limits_mode(mode), bins(num_bins),
    output_channels(channels_out), limit_low(low), limit_high(high) {
  ValidateConfiguration();
}


void Colorizer::SetLimitLow(double low) {
  // If the limits are set, we switch the mode - TODO add to interface doc!
  limits_mode = LimitsMode::Fixed;
  limit_low = low;
  ValidateConfiguration();
}


void Colorizer::SetLimitHigh(double high) {
  limits_mode = LimitsMode::Fixed;
  limit_high = high;
  ValidateConfiguration();
}


void Colorizer::SetColorMap(ColorMap cmap) {
  colormap = cmap;
}


void Colorizer::SetBins(int num_bins) {
  bins = num_bins;
  ValidateConfiguration();
}


void Colorizer::SetOutputChannels(int channels_out) {
  output_channels = channels_out;
  ValidateConfiguration();
}


ImageBuffer Colorizer::operator()(const ImageBuffer &data) {
  if ((limits_mode == LimitsMode::FromDataContinuously)
      || (limits_mode == LimitsMode::FromDataOnce)) {
    // Sanity checks are performed by a) MinMaxLocation (if buffer was invalid)
    // and b) by Colorize (if buffer had more than a single channel), so we
    // can simply try to update the upper/lower limits:
    data.MinMaxLocation(&limit_low, &limit_high, nullptr, nullptr, 0);

    // Ensure that we only compute the limits once if requested to do so:
    if (limits_mode == LimitsMode::FromDataOnce) {
      limits_mode = LimitsMode::Fixed;
    }
  }
  return Colorize(data, colormap, limit_low, limit_high, output_channels, bins);
}


void Colorizer::ValidateConfiguration() const {
  if ((bins < 2) || (bins > 256)) {
    std::ostringstream s;
    s << "Number of bins in `Colorizer` must be >= 2 and <= 256, but got: "
      << bins << '!';
    throw std::invalid_argument(s.str());
  }

  if ((limits_mode == LimitsMode::Fixed)
      && (std::isinf(limit_low)
          || std::isinf(limit_high)
          || (limit_high <= limit_low))) {
    std::ostringstream s;
    s << "Invalid colorization limits [" << std::fixed
      << std::setprecision(2) << limit_low
      << ", " << limit_high << "]!";
    throw std::invalid_argument(s.str());
  }

  if ((output_channels < 3)
      || (output_channels > 4)) {
    std::ostringstream s;
    s << "Parameter `output_channels` in `Colorizer` must be "
         "either 3 or 4, but got: " << output_channels << '!';
    throw std::invalid_argument(s.str());
  }
}


std::string LimitsModeToString(Colorizer::LimitsMode lm) {
  switch (lm) {
    case Colorizer::LimitsMode::Fixed:
      return "fixed";

    case Colorizer::LimitsMode::FromDataContinuously:
      return "continuous";

    case Colorizer::LimitsMode::FromDataOnce:
      return "once";
  }

  std::ostringstream s;
  s << "LimitsMode (" << static_cast<int>(lm)
    << ") is not yet supported in `LimitsModeToString`!";
  throw std::logic_error(s.str());
}


Colorizer::LimitsMode LimitsModeFromString(const std::string &lm) {
  const std::string lower = wks::Trim(wks::Lower(lm));

  if (wks::StartsWith(lower, "fix")) {
    return Colorizer::LimitsMode::Fixed;
  } else if (wks::StartsWith(lower, "cont")) {
    return Colorizer::LimitsMode::FromDataContinuously;
  } else if (lower.compare("once") == 0) {
    return Colorizer::LimitsMode::FromDataOnce;
  }

  std::string s("Could not deduce `LimitsMode` from string representation \"");
  s += lm;
  s += "\"!";
  throw std::logic_error(s);
}


ImageBuffer Colorize(
    const ImageBuffer &data, ColorMap colormap,
    double limit_low, double limit_high, int output_channels, int bins) {
  SPDLOG_DEBUG(
        "Colorize {:s} with {:s}, limits [{:f}, {:f}].",
        data.ToString(), ColorMapToString(colormap),
        limit_low, limit_high);

  if (data.Channels() != 1) {
    std::string s("`Colorize` requires a single-channel data buffer, not ");
    s += data.ToString();
    throw std::invalid_argument(s);
  }

  if ((bins < 2) || (bins > 256)) {
    std::ostringstream s;
    s << "Number of bins in `Colorize` must be >= 2 and <= 256, but got: "
      << bins << '!';
    throw std::invalid_argument(s.str());
  }

  if (std::isinf(limit_low) || std::isinf(limit_high)
      || std::isnan(limit_low) ||std::isnan(limit_high)) {
    data.MinMaxLocation(&limit_low, &limit_high);
  }

  if (limit_high <= limit_low) {
    std::ostringstream s;
    s << "Invalid colorization limits [" << std::fixed
      << std::setprecision(2) << limit_low
      << ", " << limit_high << "]!";
    throw std::invalid_argument(s.str());
  }

  if ((output_channels < 3)
      || (output_channels > 4)) {
    std::ostringstream s;
    s << "Parameter `output_channels` in `Colorize` must be "
         "either 3 or 4, but got: " << output_channels << '!';
    throw std::invalid_argument(s.str());
  }

  const helpers::RGBColor *map = helpers::GetColorMap(colormap);

  switch (data.BufferType()) {
    case ImageBufferType::UInt8:
      return ColorLookup<uint8_t>(
            data, map, limit_low, limit_high, output_channels, bins);

    case ImageBufferType::Int16:
      return ColorLookup<int16_t>(
            data, map, limit_low, limit_high, output_channels, bins);

    case ImageBufferType::Int32:
      return ColorLookup<int32_t>(
            data, map, limit_low, limit_high, output_channels, bins);

    case ImageBufferType::Float:
      return ColorLookup<float>(
            data, map, limit_low, limit_high, output_channels, bins);

    case ImageBufferType::Double:
      return ColorLookup<double>(
            data, map, limit_low, limit_high, output_channels, bins);
  }

  std::string s("Type `");
  s += ImageBufferTypeToString(data.BufferType());
  s += "` not handled in `Colorize` switch!";
  throw std::logic_error(s);
}


ImageBuffer ReliefShading(const ImageBuffer &relief, const ImageBuffer &colorized) {
  if (relief.Channels() != 1) {
    std::string s("Input `data` to `ReliefShading` must be single-channel, but got: ");
    s += relief.ToString();
    s += '!';
    throw std::invalid_argument(s);
  }

  if (colorized.BufferType() != ImageBufferType::UInt8) {
    std::string s("Input `colorization` to `ReliefShading` must be `uint8`, but got: ");
    s += colorized.ToString();
    s += '!';
    throw std::invalid_argument(s);
  }

  if ((colorized.Width() != relief.Width())
      || (colorized.Height() != relief.Height())) {
    std::string s("Input resolution to `ReliefShading` does not match, got: ");
    s += relief.ToString();
    s += " vs. ";
    s += colorized.ToString();
    s += '!';
    throw std::invalid_argument(s);
  }

  const ImageBuffer &data_float = (relief.BufferType() == ImageBufferType::Float)
      ? relief : relief.ToFloat();

  ImageBuffer dst = colorized.DeepCopy();

  int rows = data_float.Height();
  int cols = data_float.Width();
  // The deeply copied dst will always be contiguous.
  if (data_float.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  unsigned char *prow_dst;
  const float *prow_data;
  for (int row = 0; row < rows; ++row) {
    prow_dst = dst.MutablePtr<unsigned char>(row, 0, 0);
    prow_data = data_float.ImmutablePtr<float>(row, 0, 0);

    for (int data_col = 0, color_idx = 0; data_col < cols; ++data_col) {
      for (int ch = 0; ch < colorized.Channels(); ++ch) {
        prow_dst[color_idx] = static_cast<unsigned char>(
              prow_data[data_col] * prow_dst[color_idx]);
        ++color_idx;
      }
    }
  }

  return dst;
}


ImageBuffer Peaks(int height, int width) {
  // Numpy code:
  // coords = np.arange(-3, 3, 0.015)
  // x, y = np.meshgrid(coords, coords)
  // peaks = 3 * np.power((1 - x), 2) * np.exp(-np.power(x, 2) - np.power((y + 1), 2)) - 10 * (x / 5 - np.power(x, 3) - np.power(y, 5)) * np.exp(-np.power(x, 2) - np.power(y, 2)) - 1/3 * np.exp(-np.power((x + 1), 2) - np.power(y, 2))
  ImageBuffer dst(height, width, 1, ImageBufferType::Double);

  const double step_x = 6.0 / width;
  const double step_y = 6.0 / height;

  double y = -3.0;
  for (int row = 0; row < height; ++row) {
    double x = -3.0;
    for (int col = 0; col < width; ++col) {
      dst.AtUnchecked<double>(row, col, 0) =
          (3.0 * ((1.0 - x) * (1.0 - x)) * std::exp(-(x * x) - ((y + 1.0) * (y + 1.0))))
          - (10.0 * (x / 5.0 - (x * x * x) - (y * y * y * y * y)) * std::exp(-(x * x) - (y * y)))
          - (1.0 / 3.0 * std::exp(-((x + 1.0) * (x + 1.0)) - (y * y)));
      x += step_x;
    }
    y += step_y;
  }

  return dst;
}

} // namespace viren2d
