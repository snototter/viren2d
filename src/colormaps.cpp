#include <algorithm>
#include <exception>
#include <sstream>
#include <string>
#include <cmath>
#include <cstddef>
#include <cstdint>

#include <werkzeugkiste/strings/strings.h>

#include <viren2d/colormaps.h>

#include <helpers/colormaps_helpers.h>
#include <helpers/enum.h>
#include <helpers/logging.h>

namespace wks = werkzeugkiste::strings;

namespace viren2d {
namespace helpers {
template <typename _Tp>
ImageBuffer ColorLookupScaled(
    const ImageBuffer &data, const helpers::RGBColor *map, int num_colors,
    double limit_low, double limit_high, int output_channels, int bins) {
  // Sanity checks have been performed in `ColorizeScaled`
  ImageBuffer dst(
        data.Height(), data.Width(), output_channels, ImageBufferType::UInt8);

  const int map_bins = num_colors - 1;
  const double map_idx_factor = static_cast<double>(map_bins) / (bins - 1);
  const double interval = (limit_high - limit_low) / bins;

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
      const double value = std::max(
            limit_low,
            std::min(
              limit_high,
              static_cast<double>(*data_ptr)));
      const int bin = std::max(
            0, std::min(
              map_bins, static_cast<int>(
                map_idx_factor * std::floor((value - limit_low) / interval))));

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


template <typename _Tp>
ImageBuffer Label2Image(
    const ImageBuffer &data, const helpers::RGBColor *map,
    std::size_t num_colors, int output_channels) {
  if (!std::numeric_limits<_Tp>::is_integer) {
    throw std::logic_error(
          "`Label2Image` is only supported for labels of integral type!");
  }

  ImageBuffer dst(
        data.Height(), data.Width(), output_channels, ImageBufferType::UInt8);

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
      const std::size_t bin = static_cast<std::size_t>(*data_ptr) % num_colors;
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

} // namespace helpers


std::string ColorMapToString(ColorMap cm) {
  switch (cm) {
    case ColorMap::Autumn:
      return "autumn";
    case ColorMap::BlackBody:
      return "black-body";
    case ColorMap::Categories10:
      return "categories-10";
    case ColorMap::Categories12:
      return "categories-12";
    case ColorMap::Categories20:
      return "categories-20";
    case ColorMap::Cividis:
      return "cividis";
    case ColorMap::Cold:
      return "cold";
    case ColorMap::ColorBlindDiverging:
      return "cvd-diverging";
    case ColorMap::ColorBlindOrientation:
      return "cvd-orientation";
    case ColorMap::ColorBlindSequential:
      return "cvd-sequential";
    case ColorMap::ColorBlindSequentialVivid:
      return "cvd-sequential-vivid";
    case ColorMap::Copper:
      return "copper";
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
    case ColorMap::Jet:
      return "jet";
    case ColorMap::Ocean:
      return "ocean";
    case ColorMap::OpticalFlow:
      return "optical-flow";
    case ColorMap::Orientation4:
      return "orientation4";
    case ColorMap::Orientation6:
      return "orientation6";
    case ColorMap::Rainbow:
      return "rainbow";
    case ColorMap::Relief:
      return "relief";
    case ColorMap::ReliefLowContrast:
      return "relief-low-contrast";
    case ColorMap::Seismic:
      return "seismic";
    case ColorMap::Spectral:
      return "spectral";
    case ColorMap::SpectralDiverging:
      return "spectral-diverging";
    case ColorMap::Spring:
      return "spring";
    case ColorMap::Summer:
      return "summer";
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
    case ColorMap::Twilight:
      return "twilight";
    case ColorMap::TwilightShifted:
      return "twilight-shifted";
    case ColorMap::Viridis:
      return "viridis";
    case ColorMap::Water:
      return "water";
    case ColorMap::Winter:
      return "winter";
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
  if (lower.compare("autumn") == 0) {
    return ColorMap::Autumn;
  } else if (lower.compare("blackbody") == 0) {
    return ColorMap::BlackBody;
  } else if ((lower.compare("categories10") == 0)
             || (lower.compare("cat10") == 0)) {
    return ColorMap::Categories10;
  } else if ((lower.compare("categories12") == 0)
             || (lower.compare("cat12") == 0)) {
    return ColorMap::Categories12;
  } else if ((lower.compare("categories20") == 0)
             || (lower.compare("cat20") == 0)) {
    return ColorMap::Categories20;
  } else if (lower.compare("cividis") == 0) {
    return ColorMap::Cividis;
  } else if (lower.compare("cold") == 0) {
    return ColorMap::Cold;
  } else if ((lower.compare("cvddiverging") == 0)
             || (lower.compare("colorblinddiverging") == 0)) {
    return ColorMap::ColorBlindDiverging;
  } else if ((lower.compare("cvdorientation") == 0)
             || (lower.compare("colorblindorientation") == 0)) {
    return ColorMap::ColorBlindOrientation;
  } else if ((lower.compare("cvdsequential") == 0)
             || (lower.compare("colorblindsequential") == 0)) {
    return ColorMap::ColorBlindSequential;
  } else if ((lower.compare("cvdsequentialvivid") == 0)
             || (lower.compare("colorblindsequentialvivid") == 0)) {
    return ColorMap::ColorBlindSequentialVivid;
  } else if (lower.compare("copper") == 0) {
    return ColorMap::Copper;
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
  } else if (lower.compare("jet") == 0) {
    return ColorMap::Jet;
  } else if (lower.compare("ocean") == 0) {
    return ColorMap::Ocean;
  } else if (lower.compare("opticalflow") == 0) {
    return ColorMap::OpticalFlow;
  } else if ((lower.compare("orientation4") == 0)
             || (lower.compare("orientationfour") == 0)) {
    return ColorMap::Orientation4;
  } else if ((lower.compare("orientation6") == 0)
             || (lower.compare("orientationsix") == 0)) {
    return ColorMap::Orientation6;
  } else if (lower.compare("rainbow") == 0) {
    return ColorMap::Rainbow;
  } else if (lower.compare("relief") == 0) {
    return ColorMap::Relief;
  } else if ((lower.compare("reliefisoluminant") == 0)
             || (lower.compare("relieflowcontrast") == 0)) {
    return ColorMap::ReliefLowContrast;
  } else if (lower.compare("seismic") == 0) {
    return ColorMap::Seismic;
  } else if (lower.compare("spectral") == 0) {
    return ColorMap::Spectral;
  } else if (lower.compare("spectraldiverging") == 0) {
    return ColorMap::SpectralDiverging;
  } else if (lower.compare("spring") == 0) {
    return ColorMap::Spring;
  } else if (lower.compare("summer") == 0) {
    return ColorMap::Summer;
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
  } else if (lower.compare("twilight") == 0) {
    return ColorMap::Twilight;
  } else if (lower.compare("twilightshifted") == 0) {
    return ColorMap::TwilightShifted;
  } else if (lower.compare("viridis") == 0) {
    return ColorMap::Viridis;
  } else if (lower.compare("water") == 0) {
    return ColorMap::Water;
  } else if (lower.compare("winter") == 0) {
    return ColorMap::Winter;
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
    ColorMap::Autumn, ColorMap::Yarg> ColormapIterator;

  for (ColorMap cm: ColormapIterator()) {
    lst.push_back(cm);
  }
  return lst;
}


StreamColorizer::StreamColorizer(ColorMap cmap,
    LimitsMode mode, int num_bins, int channels_out,
    double low, double high)
  : colormap(cmap), limits_mode(mode), bins(num_bins),
    output_channels(channels_out), limit_low(low), limit_high(high) {
  ValidateConfiguration();
}


void StreamColorizer::SetLimitLow(double low) {
  limits_mode = LimitsMode::Fixed;
  limit_low = low;
  ValidateConfiguration();
}


void StreamColorizer::SetLimitHigh(double high) {
  limits_mode = LimitsMode::Fixed;
  limit_high = high;
  ValidateConfiguration();
}


void StreamColorizer::SetLimitsMode(LimitsMode m) {
  limits_mode = m;
  ValidateConfiguration();
}


void StreamColorizer::SetColorMap(ColorMap cmap) {
  colormap = cmap;
}


void StreamColorizer::SetBins(int num_bins) {
  bins = num_bins;
  ValidateConfiguration();
}


void StreamColorizer::SetOutputChannels(int channels_out) {
  output_channels = channels_out;
  ValidateConfiguration();
}


ImageBuffer StreamColorizer::operator()(const ImageBuffer &data) {
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
  return ColorizeScaled(
        data, colormap, limit_low, limit_high, output_channels, bins);
}


void StreamColorizer::ValidateConfiguration() const {
  if ((bins < 2) || (bins > 256)) {
    std::ostringstream s;
    s << "Number of bins in `Colorizer` must be >= 2 and <= 256, but got: "
      << bins << '!';
    throw std::invalid_argument(s.str());
  }

  if ((limits_mode == LimitsMode::Fixed)
      && (std::isinf(limit_low)
          || std::isinf(limit_high)
          || std::isnan(limit_low)
          || std::isnan(limit_high)
          || (limit_high <= limit_low))) {
    std::ostringstream s;
    s << "Colorization mode is set to `fixed`, but the limits are invalid: ["
      << std::fixed << std::setprecision(2) << limit_low
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


std::string LimitsModeToString(StreamColorizer::LimitsMode lm) {
  switch (lm) {
    case StreamColorizer::LimitsMode::Fixed:
      return "fixed";

    case StreamColorizer::LimitsMode::FromDataContinuously:
      return "continuous";

    case StreamColorizer::LimitsMode::FromDataOnce:
      return "once";
  }

  std::ostringstream s;
  s << "LimitsMode (" << static_cast<int>(lm)
    << ") is not yet supported in `LimitsModeToString`!";
  throw std::logic_error(s.str());
}


StreamColorizer::LimitsMode LimitsModeFromString(const std::string &lm) {
  const std::string lower = wks::Trim(wks::Lower(lm));

  if (wks::StartsWith(lower, "fix")) {
    return StreamColorizer::LimitsMode::Fixed;
  } else if (wks::StartsWith(lower, "cont")) {
    return StreamColorizer::LimitsMode::FromDataContinuously;
  } else if (lower.compare("once") == 0) {
    return StreamColorizer::LimitsMode::FromDataOnce;
  }

  std::string s("Could not deduce `LimitsMode` from string representation \"");
  s += lm;
  s += "\"!";
  throw std::logic_error(s);
}


ImageBuffer ColorizeScaled(
    const ImageBuffer &data, ColorMap colormap,
    double limit_low, double limit_high, int output_channels, int bins) {
  SPDLOG_DEBUG(
        "Colorize {:s} with {:s}, limits [{:f}, {:f}].",
        data.ToString(), ColorMapToString(colormap),
        limit_low, limit_high);

  if (data.Channels() != 1) {
    std::string s(
          "`ColorizeScaled` requires a single-channel data buffer, not ");
    s += data.ToString();
    throw std::invalid_argument(s);
  }

  if (bins < 2) {
    std::ostringstream s;
    s << "Number of bins for `ColorizeScaled` must be > 1, but got: "
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
    s << "Parameter `output_channels` in `ColorizeScaled` must be "
         "either 3 or 4, but got: " << output_channels << '!';
    throw std::invalid_argument(s.str());
  }

  std::pair<const helpers::RGBColor *, std::size_t> map = helpers::GetColorMap(colormap);
  bins = std::min(bins, static_cast<int>(map.second));

  switch (data.BufferType()) {
    case ImageBufferType::UInt8:
      return helpers::ColorLookupScaled<uint8_t>(
            data, map.first, map.second, limit_low, limit_high, output_channels, bins);

    case ImageBufferType::Int16:
      return helpers::ColorLookupScaled<int16_t>(
            data, map.first, map.second, limit_low, limit_high, output_channels, bins);

    case ImageBufferType::Int32:
      return helpers::ColorLookupScaled<int32_t>(
            data, map.first, map.second, limit_low, limit_high, output_channels, bins);

    case ImageBufferType::Float:
      return helpers::ColorLookupScaled<float>(
            data, map.first, map.second, limit_low, limit_high, output_channels, bins);

    case ImageBufferType::Double:
      return helpers::ColorLookupScaled<double>(
            data, map.first, map.second, limit_low, limit_high, output_channels, bins);
  }

  std::string s("Type `");
  s += ImageBufferTypeToString(data.BufferType());
  s += "` not handled in `ColorizeScaled` switch!";
  throw std::logic_error(s);
}


ImageBuffer ColorizeLabels(
    const ImageBuffer &labels, ColorMap colormap, int output_channels) {
  //FIXME extend image buffer with int64
  SPDLOG_DEBUG(
        "Colorize labels: {:s} with {:s}.",
        labels.ToString(), ColorMapToString(colormap));

  if (labels.Channels() != 1) {
    std::string s("`ColorizeLabels` requires a single-channel data buffer, not ");
    s += labels.ToString();
    throw std::invalid_argument(s);
  }

  if ((output_channels < 3)
      || (output_channels > 4)) {
    std::ostringstream s;
    s << "Parameter `output_channels` in `ColorizeLabels` must be "
         "either 3 or 4, but got: " << output_channels << '!';
    throw std::invalid_argument(s.str());
  }

  std::pair<const helpers::RGBColor *, std::size_t> map = helpers::GetColorMap(colormap);

  switch (labels.BufferType()) {
    case ImageBufferType::UInt8:
      return helpers::Label2Image<uint8_t>(
            labels, map.first, map.second, output_channels);

    case ImageBufferType::Int16:
      return helpers::Label2Image<int16_t>(
            labels, map.first, map.second, output_channels);

    case ImageBufferType::Int32:
      return helpers::Label2Image<int32_t>(
            labels, map.first, map.second, output_channels);

    case ImageBufferType::Float:
    case ImageBufferType::Double:
        throw std::invalid_argument(
              "Labels must be of integral type, not float/double!");
  }

  std::string s("Type `");
  s += ImageBufferTypeToString(labels.BufferType());
  s += "` not handled in `ColorizeScaled` switch!";
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
