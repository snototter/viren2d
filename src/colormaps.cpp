#include <exception>
#include <sstream>
#include <string>
#include <cmath>

#include <werkzeugkiste/strings/strings.h>

#include <viren2d/colormaps.h>

#include <helpers/colormaps_helpers.h>
#include <helpers/logging.h>

namespace wks = werkzeugkiste::strings;

namespace viren2d {

/// Returns the string representation.
std::string ColorMapToString(ColorMap cm) {
  switch (cm) {
    case ColorMap::Autumn:
      return "autumn";
    case ColorMap::Bone:
      return "bone";
    case ColorMap::Cold:
      return "cold";
    case ColorMap::Disparity:
      return "disparity";
    case ColorMap::Earth:
      return "earth";
    case ColorMap::Grayscale:
      return "grayscale";
    case ColorMap::Hot:
      return "hot";
    case ColorMap::HSV:
      return "hsv";
    case ColorMap::Inferno:
      return "inferno";
    case ColorMap::Jet:
      return "jet";
    case ColorMap::Magma:
      return "magma";
    case ColorMap::Pastel:
      return "pastel";
    case ColorMap::Plasma:
      return "plasma";
    case ColorMap::Sepia:
      return "sepia";
    case ColorMap::Temperature:
      return "temperature";
    case ColorMap::Thermal:
      return "thermal";
    case ColorMap::Turbo:
      return "turbo";
    case ColorMap::Viridis:
      return "viridis";
  }
  //TODO(dev) For each new colormap, also extend ColorMapFromString!

  std::ostringstream s;
  s << "ColorMap (" << static_cast<int>(cm)
    << ") is not yet supported in `ColorMapToString`!";
  throw std::logic_error(s.str());
}


ColorMap ColorMapFromString(const std::string &cm) {
  const auto lower = wks::Trim(wks::Lower(cm));
  if (lower.compare("autumn") == 0) {
    return ColorMap::Autumn;
  } else if (lower.compare("bone") == 0) {
    return ColorMap::Bone;
  } else if (lower.compare("cold") == 0) {
    return ColorMap::Cold;
  } else if (lower.compare("disparity") == 0) {
    return ColorMap::Disparity;
  } else if (lower.compare("earth") == 0) {
    return ColorMap::Earth;
  } else if ((lower.compare("grayscale") == 0)
             || (lower.compare("greyscale") == 0)) {
    return ColorMap::Grayscale;
  } else if (lower.compare("hot") == 0) {
    return ColorMap::Hot;
  } else if (lower.compare("hsv") == 0) {
    return ColorMap::HSV;
  } else if (lower.compare("inferno") == 0) {
    return ColorMap::Inferno;
  } else if (lower.compare("jet") == 0) {
    return ColorMap::Jet;
  } else if (lower.compare("magma") == 0) {
    return ColorMap::Magma;
  } else if (lower.compare("pastel") == 0) {
    return ColorMap::Pastel;
  } else if (lower.compare("plasma") == 0) {
    return ColorMap::Plasma;
  } else if (lower.compare("sepia") == 0) {
    return ColorMap::Sepia;
  } else if (lower.compare("temperature") == 0) {
    return ColorMap::Temperature;
  } else if (lower.compare("thermal") == 0) {
    return ColorMap::Thermal;
  } else if (lower.compare("turbo") == 0) {
    return ColorMap::Turbo;
  } else if (lower.compare("viridis") == 0) {
    return ColorMap::Viridis;
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


ImageBuffer Colorize(
    const ImageBuffer &data, ColorMap colormap,
    double limit_low, double limit_high, int output_channels, int bins) {
  SPDLOG_DEBUG(
        "Colorize {:s} with {:s}, limits [{:f}, {:f}].",
        data.ToString(), ColorMapToString(colormap),
        limit_low, limit_high);

  if ((bins < 2) || (bins > 256)) {
    std::ostringstream s;
    s << "Number of bins in `Colorize` must be >= 2 and <= 256, but got: "
      << bins << '!';
    throw std::invalid_argument(s.str());
  }

  if (data.Channels() != 1) {
    std::string s("`Colorize` requires a single-channel data buffer, not ");
    s += data.ToString();
    throw std::invalid_argument(s);
  }

  if (limit_high <= limit_low) {
    std::ostringstream s;
    s << "Invalid limits [" << std::fixed
      << std::setprecision(2) << limit_low
      << ", " << limit_high << "] in `Colorize` for "
      << data.ToString() << '!';
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
