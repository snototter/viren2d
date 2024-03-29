#include <stdexcept>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <utility>
#include <cstdlib>

// public viren2d headers
#include <viren2d/opticalflow.h>
#include <viren2d/drawing.h>

// private viren2d headers
#include <helpers/logging.h>
#include <helpers/colormaps_helpers.h>

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif


namespace viren2d {
namespace helpers {
/// First 4 bytes of a .flo file should be this magic number,
/// which corresponds to the 4 bytes 'P',  'I', 'E', 'H'.
/// This number is used to assert we're on little endian systems
constexpr float kFloMagicFloat = 202021.25f;

typedef union {
  char buffer[4];
  std::int32_t ival;
  float fval;
} FloTag;

constexpr FloTag kFloMagicTag = {{'P', 'I', 'E', 'H' }};

template <typename _Tp>
std::pair<_Tp, int> ComputeFlowMagnitudeAndBin(
    const _Tp u, const _Tp v, int wheel_size) {
  const _Tp radius = std::sqrt(u * u + v * v);
  const _Tp angle = std::atan2(-v, -u) / static_cast<_Tp>(M_PI);

  const _Tp fk = (angle + 1) / static_cast<_Tp>(2) * (wheel_size - 1);
  return std::make_pair(
        radius, static_cast<int>(fk) % wheel_size);
}

template <typename _Tp>
void ColorizePixelFromFlow(
    const _Tp u, const _Tp v, const _Tp max_radius,
    unsigned char *dst_ptr, int output_channels,
    const std::pair<const helpers::RGBColor *, std::size_t> map) {
  const _Tp radius = std::sqrt(u * u + v * v) / max_radius;
  const _Tp angle = std::atan2(-v, -u) / static_cast<_Tp>(M_PI);

  const _Tp fk = (angle + 1) / static_cast<_Tp>(2) * (map.second - 1);
  const int k0 = static_cast<int>(fk);
  const int k1 = (k0 + 1) % static_cast<int>(map.second);
  const _Tp f = fk - k0;

  for (int ch = 0; ch < 3; ++ch) {
    const _Tp color0 = static_cast<_Tp>(map.first[k0][ch]);
    const _Tp color1 = static_cast<_Tp>(map.first[k1][ch]);
    _Tp color = (1 - f) * color0 + f * color1;

    if (radius <= 1.0) {
      // Increase saturation with radius:
      color = 255.0 - radius * (255.0 - color);
    } else {
      // Dim non-normalized flow vectors:
      color *= static_cast<_Tp>(0.75);
    }

    *dst_ptr++ = static_cast<unsigned char>(color);
  }

  // Add optional alpha channel
  if (output_channels == 4) {
    *dst_ptr = 255;
  }
}
} // namespace helpers


ImageBuffer LoadOpticalFlow(const std::string &filename) {
  // Format of `.flo` files:
  // Bytes 0-3: "PIEH" in ASCII, which is 202021.25f in little endian.
  //   Used as a sanity check that floats are represented correctly.
  // Bytes 4-7: Width as integer.
  // Bytes 8-11: Height as integer.
  // Bytes 12-end: Data in row-major order, `width * height * 2 * 4` bytes
  std::ifstream file(filename, std::ios::in | std::ios::binary);

  if (!file.is_open()) {
    std::string msg("Could not open optical flow file: \"");
    msg += filename;
    msg += "\"!";
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  helpers::FloTag tag, width, height;

  file.read(tag.buffer, 4);
  if (tag.fval != helpers::kFloMagicFloat) {
    std::ostringstream msg;
    msg << "Invalid magic number " << std::fixed << std::setprecision(2)
        << tag.fval << "/\"" << tag.buffer[0] << tag.buffer[1] << tag.buffer[2]
        << tag.buffer[3] << "\" instead of " << std::fixed << std::setprecision(2)
        << helpers::kFloMagicFloat << "/\"" << helpers::kFloMagicTag.buffer[0]
        << helpers::kFloMagicTag.buffer[1] << helpers::kFloMagicTag.buffer[2]
        << helpers::kFloMagicTag.buffer[3] << "\" in file \""
        << filename << "\"";
    SPDLOG_ERROR(msg.str());
    throw std::logic_error(msg.str());
  }

  file.read(width.buffer, 4);
  file.read(height.buffer, 4);

  if (!file.good()) {
    std::string msg("Optical flow file is corrupt, could not read dimensions from \"");
    msg += filename;
    msg += "\"!";
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  ImageBuffer flow(height.ival, width.ival, 2, ImageBufferType::Float);
  // Freshly allocated image buffer is always contiguous, so
  // we can read the whole file in a single sweep:
  file.read(
        reinterpret_cast<char *>(flow.MutableData()),
        width.ival * height.ival * 2 * sizeof(float));

  file.close();
  return flow;
}


void SaveOpticalFlow(const std::string &filename, const ImageBuffer &flow) {
  if (filename.empty()) {
    const std::string msg("File name must not be empty in `SaveOpticalFlow`!");
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }

  const ImageBuffer &float_flow = (flow.BufferType() == ImageBufferType::Float)
      ? flow : flow.ToFloat();

  std::ofstream file(filename, std::ios::out | std::ios::binary);
  file.write(helpers::kFloMagicTag.buffer, 4);

  helpers::FloTag width, height;
  width.ival = flow.Width();
  height.ival = flow.Height();
  file.write(width.buffer, sizeof(std::int32_t));
  file.write(height.buffer, sizeof(std::int32_t));

  if (float_flow.IsContiguous()) {
    file.write(
          reinterpret_cast<const char *>(float_flow.ImmutableData()),
          float_flow.NumBytes());
  } else {
    // Is a single row contiguous?
    if (float_flow.PixelStride() == (float_flow.Channels() * float_flow.ElementSize())) {
      for (int row = 0; row < float_flow.Height(); ++row) {
        file.write(
              reinterpret_cast<const char *>(float_flow.ImmutablePtr<float>(row, 0, 0)),
              float_flow.PixelStride() * float_flow.Width());
      }
    } else {
      // Write each pixel
      for (int row = 0; row < float_flow.Height(); ++row) {
        for (int col = 0; col < float_flow.Width(); ++col) {
          file.write(
                reinterpret_cast<const char *>(float_flow.ImmutablePtr<float>(row, col, 0)),
                float_flow.ElementSize());
          file.write(
                reinterpret_cast<const char *>(float_flow.ImmutablePtr<float>(row, col, 1)),
                float_flow.ElementSize());
        }
      }
    }
  }

  file.close();
}


template <typename _Tp>
ImageBuffer ColorizeFlowHelper(
    const ImageBuffer &flow, ColorMap colormap, _Tp max_motion, int output_channels) {
  const std::pair<const helpers::RGBColor *, std::size_t> map = helpers::GetColorMap(colormap);

  ImageBuffer dst(flow.Height(), flow.Width(), output_channels, ImageBufferType::UInt8);
  int rows = flow.Height();
  int values_per_row = flow.Width() * flow.Channels();
  if (flow.IsContiguous()) {
    values_per_row *= rows;
    rows = 1;
  }

  for (int row = 0; row < rows; ++row) {
    const _Tp *flow_ptr = flow.ImmutablePtr<_Tp>(row, 0, 0);
    unsigned char *dst_ptr = dst.MutablePtr<unsigned char>(row, 0, 0);
    int dst_col = 0;

    for (int src_element = 0; src_element < values_per_row; src_element += 2) {
      helpers::ColorizePixelFromFlow(
            flow_ptr[src_element], flow_ptr[src_element + 1], max_motion,
            &dst_ptr[dst_col], output_channels, map);

      dst_col += output_channels;
    }
  }
  return dst;
}

//FIXME change stringstream to ostringstream!
ImageBuffer ColorizeOpticalFlow(
    const ImageBuffer &flow, ColorMap colormap,
    double motion_normalizer, int output_channels) {
  if ((output_channels < 3) || (output_channels > 4)) {
    std::stringstream msg;
    msg << "Invalid input to `ColorizeOpticalFlow`: Number of output channels "
           "must be 3 or 4, but got "
        << output_channels << '!';
    SPDLOG_ERROR(msg.str());
    throw std::invalid_argument(msg.str());
  }

  switch (flow.BufferType()) {
    case ImageBufferType::Float:
      return ColorizeFlowHelper<float>(
            flow, colormap, static_cast<float>(motion_normalizer),
            output_channels);

    case ImageBufferType::Double:
      return ColorizeFlowHelper<double>(
            flow, colormap, motion_normalizer, output_channels);

    default: {
        std::string msg(
              "Invalid input to `ColorizeOpticalFlow`: Flow values must be of "
              "type `float` or `double`, but got ");
        msg += flow.ToString();
        msg += '!';
        SPDLOG_ERROR(msg);
        throw std::invalid_argument(msg);
      }
  }
}


ImageBuffer OpticalFlowLegend(
    int size, ColorMap colormap, LineStyle line_style, bool draw_circle,
    bool clip_circle, int output_channels) {

  if ((output_channels < 3) || (output_channels > 4)) {
    std::ostringstream msg;
    msg << "Invalid input to `OpticalFlowColorWheel`: Number of output channels "
           "must be 3 or 4, but got "
        << output_channels << '!';
    SPDLOG_ERROR(msg.str());
    throw std::invalid_argument(msg.str());
  }

  if (size < 2) {
    std::stringstream msg;
    msg << "Invalid input to `OpticalFlowColorWheel`: Size must be > 2, but got "
        << size << '!';
    SPDLOG_ERROR(msg.str());
    throw std::invalid_argument(msg.str());
  }

  const std::pair<const helpers::RGBColor *, std::size_t> map = helpers::GetColorMap(colormap);

  const float interval = 2.0f / (size - 1);

  ImageBuffer dst(size, size, output_channels, ImageBufferType::UInt8);
  float v = -1.0f;
  for (int row = 0; row < size; ++row) {
    float u = -1.0f;
    unsigned char *dst_ptr = dst.MutablePtr<unsigned char>(row, 0, 0);

    for (int col = 0; col < size; ++col) {
      helpers::ColorizePixelFromFlow(
            u, v, 1.0f, &dst_ptr[col * output_channels], output_channels, map);
      u += interval;
    }

    v += interval;
  }

  // Overlay grid if needed:
  const double sz_half = size / 2.0;
  std::unique_ptr<Painter> painter = nullptr;
  if (line_style.IsValid()) {
    painter = CreatePainter();
    painter->SetCanvas(dst);

    const double lw_half = line_style.width / 2.0;
    if (draw_circle) {
      painter->DrawCircle(
            {sz_half, sz_half}, sz_half - lw_half, line_style);
    }
    const double offset = draw_circle ? line_style.width : 0.0;
    painter->DrawLine(
          {sz_half, offset}, {sz_half, size - offset}, line_style);
    painter->DrawLine(
          {offset, sz_half}, {size - offset, sz_half}, line_style);

    dst = painter->GetCanvas(true);
  }

  // Clip if needed (and feasible, i.e. this only works on
  // images with an alpha channel):
  if (clip_circle && (output_channels == 4)) {
    if (!painter) {
      painter = CreatePainter();
    }
    painter->SetCanvas(size, size, Color::White.WithAlpha(0.0));
    painter->DrawImage(
          dst, {sz_half, sz_half}, Anchor::Center, 1.0, 1.0, 1.0, 0.0, 1.0);
    return painter->GetCanvas(true);
  } else {
    if (dst.Channels() != output_channels) {
      return dst.ToChannels(output_channels);
    } else {
      return dst;
    }
  }
}

} // namespace viren2d
