#ifndef __VIREN2D_IMAGEBUFFER_HELPERS_H__
#define __VIREN2D_IMAGEBUFFER_HELPERS_H__

#include <stdexcept>
#include <sstream>

#include <werkzeugkiste/geometry/utils.h>

#include <viren2d/colors.h>
#include <viren2d/styles.h>

#include <helpers/logging.h>
#include <helpers/color_conversion.h>

namespace wkg = werkzeugkiste::geometry;

namespace viren2d {
namespace helpers {

template<typename _Tp> inline
void SwapChannels(ImageBuffer &buffer, int ch1, int ch2) {
  int rows = buffer.Height();
  int values_per_row = buffer.Width() * buffer.Channels();

  // If the memory is contiguous, we can speed up the
  // following loop, similar to the efficient OpenCV matrix scan:
  // https://docs.opencv.org/2.4/doc/tutorials/core/how_to_scan_images/how_to_scan_images.html#the-efficient-way
  if (buffer.IsContiguous()) {
    values_per_row *= rows;
    rows = 1;
  }

  _Tp *ptr_row;
  for (int row = 0; row < rows; ++row) {
    ptr_row = buffer.MutablePtr<_Tp>(row, 0, 0);
    for (int col = 0; col < values_per_row; col += buffer.Channels()) {
      _Tp tmp = ptr_row[col + ch1];
      ptr_row[col + ch1] = ptr_row[col + ch2];
      ptr_row[col + ch2] = tmp;
    }
  }
}


template<typename _Tp>
ImageBuffer ExtractChannel(const ImageBuffer &src, int channel) {
  ImageBuffer dst(src.Height(), src.Width(), 1, src.BufferType());

  int rows = src.Height();
  int values_per_row = src.Width() * src.Channels();
  if (src.IsContiguous()) {
    values_per_row *= rows;
    rows = 1;
  }

  _Tp const *src_row;
  _Tp *dst_ptr = dst.MutablePtr<_Tp>(0, 0, 0);
  int dst_col = 0;
  for (int row = 0; row < rows; ++row) {
    src_row = src.ImmutablePtr<_Tp>(row, 0, 0);
    for (int col = channel; col < values_per_row; col += src.Channels()) {
      dst_ptr[dst_col] = src_row[col];
      ++dst_col;
    }
  }
  return dst;
}

//TODO/FIXME - implement blend

template<typename _Tp>
ImageBuffer ConversionHelperGray(
    const ImageBuffer &src, int channels_out) {
  SPDLOG_DEBUG(
        "ImageBuffer converting grayscale to {:d} channels.",
        channels_out);

  if (!src.IsValid()) {
    const std::string msg("Cannot convert an invalid ImageBuffer to RGB(A)!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  if (src.Channels() != 1) {
    const std::string msg("Input image must be grayscale!");
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }

  if (!src.ImmutableData()) {
    const std::string msg("Invalid input image (nullptr)!");
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }

  if (channels_out != 3 && channels_out != 4) {
    std::ostringstream msg;
    msg << "Number of output channels must be 3 or 4, but got "
        << channels_out << '!';
    SPDLOG_ERROR(msg.str());
    throw std::invalid_argument(msg.str());
  }

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.Height(), src.Width(), channels_out, src.BufferType());

  int rows = src.Height();
  int cols = src.Width(); // src channels is 1
  // dst was freshly allocated, so it's guaranteed to be contiguous
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      dst.AtUnchecked<_Tp>(row, col, 0) = src.AtUnchecked<_Tp>(row, col, 0);
      dst.AtUnchecked<_Tp>(row, col, 1) = src.AtUnchecked<_Tp>(row, col, 0);
      dst.AtUnchecked<_Tp>(row, col, 2) = src.AtUnchecked<_Tp>(row, col, 0);
      if (channels_out == 4) {
        dst.AtUnchecked<_Tp>(row, col, 3) = 255;
      }
    }
  }

  return dst;
}


/// Selects the corresponding templated ConversionHelper
inline ImageBuffer Gray2RGBx(
    const ImageBuffer &img, int num_channels_out) {
  switch(img.BufferType()) {
    case ImageBufferType::UInt8:
      return ConversionHelperGray<uint8_t>(img, num_channels_out);

    case ImageBufferType::Int16:
      return ConversionHelperGray<int16_t>(img, num_channels_out);

    case ImageBufferType::UInt16:
      return ConversionHelperGray<uint16_t>(img, num_channels_out);

    case ImageBufferType::Int32:
      return ConversionHelperGray<int32_t>(img, num_channels_out);

    case ImageBufferType::UInt32:
      return ConversionHelperGray<uint32_t>(img, num_channels_out);

    case ImageBufferType::Int64:
      return ConversionHelperGray<int64_t>(img, num_channels_out);

    case ImageBufferType::UInt64:
      return ConversionHelperGray<uint64_t>(img, num_channels_out);

    case ImageBufferType::Float:
      return ConversionHelperGray<float>(img, num_channels_out);

    case ImageBufferType::Double:
      return ConversionHelperGray<double>(img, num_channels_out);
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg(
        "Grayscale to RGB(A) conversion is not supported for type `");
  msg += ImageBufferTypeToString(img.BufferType());
  msg += "`!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


template <typename _Tp>
ImageBuffer ConversionHelperRGB(
    const ImageBuffer &src, int channels_out) {
  SPDLOG_DEBUG(
        "ImageBuffer converting RGB(A) to {:d} channels.",
        channels_out);

  if (!src.IsValid()) {
    const std::string msg("Cannot convert an invalid ImageBuffer to RGB(A)!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  if (src.Channels() != 3 && src.Channels() != 4) {
    const std::string msg("Input ImageBuffer must be RGB or RGBA!");
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }

  if (channels_out != 3 && channels_out != 4) {
    std::ostringstream msg;
    msg << "Number of output channels must be 3 or 4, but got "
        << channels_out << '!';
    SPDLOG_ERROR(msg.str());
    throw std::invalid_argument(msg.str());
  }

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.Height(), src.Width(), channels_out, src.BufferType());

  int rows = src.Height();
  int cols = src.Width();
  // dst was freshly allocated, so it's guaranteed to be contiguous
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  const bool add_alpha = (channels_out == 4);
  for (int row = 0; row < rows; ++row) {
//    _Tp *dst_ptr = dst.MutablePtr<_Tp>(row, 0);
//    const _Tp *src_ptr = src.ImmutablePtr<_Tp>(row, 0);
    for (int col = 0; col < cols; ++col) {
      // FIXME: pointer access is much faster, but we need to take care
      //   of strided memory (e.g. numpy slices/views). Could implement a
      //   generic "pixelwise iterator"
//      *dst_ptr++ = *src_ptr++;
//      *dst_ptr++ = *src_ptr++;
//      *dst_ptr++ = *src_ptr++;
      dst.AtUnchecked<_Tp>(row, col, 0) = src.AtUnchecked<_Tp>(row, col, 0);
      dst.AtUnchecked<_Tp>(row, col, 1) = src.AtUnchecked<_Tp>(row, col, 1);
      dst.AtUnchecked<_Tp>(row, col, 2) = src.AtUnchecked<_Tp>(row, col, 2);
      // Two cases:
      // * RGBA --> RGB, we're already done
      // * RGB  --> RGBA, we must add the alpha channel
      if (add_alpha) {
        dst.AtUnchecked<_Tp>(0, col, 3) = 255;
//        *dst_ptr++ = 255;
      }
    }
  }

  return dst;
}


/// Selects the corresponding templated ConversionHelper
inline ImageBuffer RGBx2RGBx(
    const ImageBuffer &img, int num_channels_out) {
  switch(img.BufferType()) {
    case ImageBufferType::UInt8:
      return ConversionHelperRGB<uint8_t>(img, num_channels_out);

    case ImageBufferType::Int16:
      return ConversionHelperRGB<int16_t>(img, num_channels_out);

    case ImageBufferType::UInt16:
      return ConversionHelperRGB<uint16_t>(img, num_channels_out);

    case ImageBufferType::Int32:
      return ConversionHelperRGB<int32_t>(img, num_channels_out);

    case ImageBufferType::UInt32:
      return ConversionHelperRGB<uint32_t>(img, num_channels_out);

    case ImageBufferType::Int64:
      return ConversionHelperRGB<int64_t>(img, num_channels_out);

    case ImageBufferType::UInt64:
      return ConversionHelperRGB<uint64_t>(img, num_channels_out);

    case ImageBufferType::Float:
      return ConversionHelperRGB<float>(img, num_channels_out);

    case ImageBufferType::Double:
      return ConversionHelperRGB<double>(img, num_channels_out);
  }

  // Throw an exception as fallback, because due to the default
  // compiler settings, we would have ignored the warning about
  // missing case values.
  std::string msg(
        "Grayscale to RGB(A) conversion is not supported for type `");
  msg += ImageBufferTypeToString(img.BufferType());
  msg += "`!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


template <typename _Tp>
ImageBuffer RGBx2Gray(
    const ImageBuffer &src,
    int channels_out,
    bool is_bgr_format) {
  SPDLOG_DEBUG(
        "ImageBuffer converting {:s} to {:d}-channel grayscale.",
        (is_bgr_format ? "BGR(A)" : "RGB(A)"), channels_out);

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.Height(), src.Width(), channels_out, src.BufferType());

  int rows = src.Height();
  int cols = src.Width();
  // dst was freshly allocated, so it's guaranteed to be contiguous
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  const int ch_r = is_bgr_format ? 2 : 0;
  const int ch_b = is_bgr_format ? 0 : 2;
  _Tp luminance;

  for (int row = 0; row < rows; ++row) {
    _Tp *dst_ptr = dst.MutablePtr<_Tp>(row, 0, 0);
    for (int col = 0; col < cols; ++col) {
      luminance = CvtHelperRGB2Gray(
            src.AtUnchecked<_Tp>(row, col, ch_r),
            src.AtUnchecked<_Tp>(row, col, 1),
            src.AtUnchecked<_Tp>(row, col, ch_b));

      *dst_ptr++ = luminance;
      if (channels_out > 1) {
        *dst_ptr++ = luminance;
        if (channels_out > 2) {
          *dst_ptr++ = luminance;
          if (channels_out == 4) {
            if (src.Channels() == 4) {
              *dst_ptr++ = src.AtUnchecked<_Tp>(row, col, 3);
            } else {
              *dst_ptr++ = static_cast<_Tp>(255);
            }
          }
        }
      }
    }
  }

  return dst;
}


template <typename _Tp, int C>
void PixelateImpl(ImageBuffer &roi, int block_width, int block_height) {
  if ((block_width <= 0) || (block_height <= 0)) {
    const std::string msg("Block width & height must be > 0 in `Pixelate`!");
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }

  // Increase the block size at the edges of the ROI if needed:
  const int num_blocks_horz = roi.Width() / block_width;
  const int missed_horz = roi.Width() - (num_blocks_horz * block_width);
  int extend_left = missed_horz / 2;
  int extend_right = missed_horz - extend_left;
  if (num_blocks_horz == 1) {
    extend_left += extend_right;
    extend_right = 0;
  }

  const int num_blocks_vert = roi.Height() / block_height;
  const int missed_vert = roi.Height() - (num_blocks_vert * block_height);
  int extend_top = missed_vert / 2;
  int extend_bottom = missed_vert - extend_top;
  if (num_blocks_vert == 1) {
    extend_top += extend_bottom;
    extend_bottom = 0;
  }

  int top = 0;
  for (int brow = 0; brow < num_blocks_vert; ++brow) {
    // At the top & bottom edge, the blocks might be up to
    // block_height/2 larger, to ensure proper anonymization
    // there, too.
    const int bheight = (brow == 0)
        ? (block_height + extend_top)
        : ((brow == num_blocks_vert - 1)
           ? (block_height + extend_bottom)
           : block_height);
    // All pixels within a block are assigned the value of the center pixel:
    const int cy = top + bheight / 2;
    int left = 0;
    for (int bcol = 0; bcol < num_blocks_horz; ++bcol) {
      const int bwidth = (bcol == 0)
          ? (block_width + extend_left)
          : ((bcol == num_blocks_horz - 1)
             ? (block_width + extend_right)
             : block_width);
      const int cx = left + bwidth / 2;
      viren2d::ImageBuffer block = roi.ROI(left, top, bwidth, bheight);
      if (C == 1) {
        block.SetToScalar<_Tp>(roi.AtUnchecked<_Tp>(cy, cx, 0));
      } else if (C == 2) {
        block.SetToPixel<_Tp>(
              roi.AtUnchecked<_Tp>(cy, cx, 0),
              roi.AtUnchecked<_Tp>(cy, cx, 1));
      } else if (C == 3) {
        block.SetToPixel<_Tp>(
              roi.AtUnchecked<_Tp>(cy, cx, 0),
              roi.AtUnchecked<_Tp>(cy, cx, 1),
              roi.AtUnchecked<_Tp>(cy, cx, 2));
      } else {
        block.SetToPixel<_Tp>(
              roi.AtUnchecked<_Tp>(cy, cx, 0),
              roi.AtUnchecked<_Tp>(cy, cx, 1),
              roi.AtUnchecked<_Tp>(cy, cx, 2),
              roi.AtUnchecked<_Tp>(cy, cx, 3));
      }

      left += bwidth;
    }

    top += bheight;
  }
}


template <typename _Tp>
void Pixelate(ImageBuffer &roi, int block_width, int block_height) {
  SPDLOG_DEBUG(
        "Pixelate {:s} with block_width={:d}, block_height={:d}",
        roi.ToString(), block_width, block_height);

  switch (roi.Channels()) {
    case 1:
      PixelateImpl<_Tp, 1>(roi, block_width, block_height);
      break;

    case 2:
      PixelateImpl<_Tp, 2>(roi, block_width, block_height);
      break;

    case 3:
      PixelateImpl<_Tp, 3>(roi, block_width, block_height);
      break;

    case 4:
      PixelateImpl<_Tp, 4>(roi, block_width, block_height);
      break;

    default: {
        const std::string msg(
              "Pixelation helper only supports up to 4 channels!");
        SPDLOG_ERROR(msg);
        throw std::logic_error(msg);
      }
  }
}


template <typename _Tp>
void MinMaxLocation(
    const ImageBuffer &buf, int channel,
    double *min_val, double *max_val,
    Vec2i *min_loc, Vec2i *max_loc) {
  if ((channel < 0) && buf.Channels() == 1) {
    channel = 0;
  }

  if ((channel < 0) || (channel >= buf.Channels())) {
    std::ostringstream msg;
    msg << "Cannot perform `MinMaxLocation` on channel " << channel
        << " with a buffer that has " << buf.Channels() << " channels!";
    SPDLOG_ERROR(msg.str());
    throw std::out_of_range(msg.str());
  }

  int rows = buf.Height();
  int cols = buf.Width();
  if (buf.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  _Tp _minval = buf.AtUnchecked<_Tp>(0, 0, channel);
  _Tp _maxval = buf.AtUnchecked<_Tp>(0, 0, channel);
  Vec2i _minloc{0, 0};
  Vec2i _maxloc{0, 0};
  // To report the image location, we need to keep track
  // of it (in case we're working with a contiguous buffer...)
  Vec2i curr_loc{0, -1};
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      curr_loc[0] = col % buf.Width();
      if (curr_loc[0] == 0) {
        ++curr_loc[1];
      }
      const _Tp val = buf.AtUnchecked<_Tp>(row, col, channel);
      if (val < _minval) {
        _minval = val;
        _minloc = curr_loc;
      }
      if (val > _maxval) {
        _maxval = val;
        _maxloc = curr_loc;
      }
    }
  }

  if (min_val) {
    *min_val = static_cast<double>(_minval);
  }
  if (max_val) {
    *max_val = static_cast<double>(_maxval);
  }
  if (min_loc) {
    *min_loc = _minloc;
  }
  if (max_loc) {
    *max_loc = _maxloc;
  }
}


template <typename _Tp>
ImageBuffer BlendConstant(
    const ImageBuffer &src1,
    const ImageBuffer &src2,
    double alpha2) {
  SPDLOG_DEBUG(
        "Blending {:s} and {:s} with alpha2={:f}.",
        src1.ToString(), src2.ToString(), alpha2);

  if ((src1.Width() != src2.Width())
      || (src1.Height() != src2.Height())
      || (src1.BufferType() != src2.BufferType())) {
    std::string msg(
          "Blending is only supported for ImageBuffers with same size and "
          "type, but got: ");
    msg += src1.ToString();
    msg += " vs. ";
    msg += src2.ToString();
    msg += '!';
    throw std::logic_error(msg);
  }

  const int channels_out = std::max(src1.Channels(), src2.Channels());
  const int channels_to_blend = std::min(src1.Channels(), src2.Channels());
  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src1.Height(), src1.Width(), channels_out, src1.BufferType());

  // If the number of input channels are not the same, we fill the result
  // with values from the buffer that has more channels.
  const ImageBuffer &rem_channels = (src1.Channels() > src2.Channels()) ? src1 : src2;

  int rows = src1.Height();
  int cols = src1.Width();
  if (src1.IsContiguous() && src2.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      for (int ch = 0; ch < channels_out; ++ch) {
        if (ch < channels_to_blend) {
          dst.AtUnchecked<_Tp>(row, col, ch) = static_cast<_Tp>(
                ((1.0 - alpha2) * src1.AtUnchecked<_Tp>(row, col, ch))
                + (alpha2 * src2.AtUnchecked<_Tp>(row, col, ch)));
        } else {
          dst.AtUnchecked<_Tp>(row, col, ch) =
              rem_channels.AtUnchecked<_Tp>(row, col, ch);
        }
      }
    }
  }

  return dst;
}

template <typename _TImage, typename _TWeights>
ImageBuffer BlendWeightsImpl(
    const ImageBuffer &src1,
    const ImageBuffer &src2,
    const ImageBuffer &alpha2) {
  const int channels_out = std::max(src1.Channels(), src2.Channels());
  const int channels_to_blend = std::min(src1.Channels(), src2.Channels());
  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(
        src1.Height(), src1.Width(), channels_out, src1.BufferType());

  // If the number of input channels are not the same, we fill the result
  // with values from the buffer that has more channels.
  const ImageBuffer &rem_channels =
      (src1.Channels() > src2.Channels()) ? src1 : src2;

  int rows = src1.Height();
  int cols = src1.Width();
  if (src1.IsContiguous() && src2.IsContiguous() && alpha2.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      for (int ch = 0; ch < channels_out; ++ch) {
        if (ch < channels_to_blend) {
          const _TWeights a2 = alpha2.AtUnchecked<_TWeights>(
                row, col, (ch < alpha2.Channels()) ? ch : 0);
          dst.AtUnchecked<_TImage>(row, col, ch) = static_cast<_TImage>(
                ((1.0 - a2) * src1.AtUnchecked<_TImage>(row, col, ch))
                + (a2 * src2.AtUnchecked<_TImage>(row, col, ch)));
        } else {
          dst.AtUnchecked<_TImage>(row, col, ch) =
              rem_channels.AtUnchecked<_TImage>(row, col, ch);
        }
      }
    }
  }

  return dst;
}


template <typename _Tp>
ImageBuffer BlendWeights(
    const ImageBuffer &src1,
    const ImageBuffer &src2,
    const ImageBuffer &alpha2) {
  SPDLOG_DEBUG(
        "Blending {:s} and {:s} with alpha2={:s}.",
        src1.ToString(), src2.ToString(), alpha2);

  if ((src1.Width() != src2.Width())
      || (src1.Height() != src2.Height())
      || (src1.BufferType() != src2.BufferType())) {
    std::string msg(
          "Blending is only supported for ImageBuffers with same size and "
          "type, but got: ");
    msg += src1.ToString();
    msg += " vs. ";
    msg += src2.ToString();
    msg += '!';
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  if ((src1.Width() != alpha2.Width())
      || (src1.Height() != alpha2.Height())) {
    std::string msg(
          "Blending weights must have the same size as the inputs, "
          "but `weights` is: ");
    msg += alpha2.ToString();
    msg += " vs. inputs: ";
    msg += src1.ToString();
    msg += " and ";
    msg += src2.ToString();
    msg += '!';
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  switch (alpha2.BufferType()) {
    case ImageBufferType::Double:
      return BlendWeightsImpl<_Tp, double>(src1, src2, alpha2);

    case ImageBufferType::Float:
      return BlendWeightsImpl<_Tp, float>(src1, src2, alpha2);

    default: {
        std::string msg(
              "Blending weights must be single or double precision "
              "floating points, but got: ");
        msg += ImageBufferTypeToString(alpha2.BufferType());
        SPDLOG_ERROR(msg);
        throw std::logic_error(msg);
      }
  }
}


template <typename _T>
ImageBuffer DimImpl(
    const ImageBuffer &src,
    double &alpha) {
  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.Height(), src.Width(), src.Channels(), src.BufferType());

  int rows = src.Height();
  int cols = src.Width();
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  for (int row = 0; row < rows; ++row) {
    const _T *src_ptr = src.ImmutablePtr<_T>(row, 0);
    _T *dst_ptr = dst.MutablePtr<_T>(row, 0);
    for (int col = 0; col < cols; ++col) {
      for (int ch = 0; ch < src.Channels(); ++ch) {
        *dst_ptr = static_cast<_T>(alpha * (*src_ptr));
        ++dst_ptr;
        ++src_ptr;
      }
    }
  }

  return dst;
}


template <typename _Tp>
ImageBuffer ToUInt8(const ImageBuffer &src, int channels_out, uint8_t scale) {
  SPDLOG_DEBUG(
        "Converting {:s} to {:d}-channel `uint8`, scale={}.",
        src.ToString(), channels_out, (int)scale);

  if ((channels_out < 1)
      || (channels_out == 2)
      || (channels_out > 4)
      || (channels_out < src.Channels())) {
    std::ostringstream msg;
    msg << "Number of output channels must be 1, 3, or 4 and >= buffer "
           "channels (i.e. " << src.Channels() << "), but requested: "
        << channels_out << '!';
    SPDLOG_ERROR(msg.str());
    throw std::invalid_argument(msg.str());
  }

  if (src.BufferType() == ImageBufferType::UInt8) {
    return src.ToChannels(channels_out);
  }

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.Height(), src.Width(), channels_out, ImageBufferType::UInt8);

  int rows = src.Height();
  int cols = src.Width();
  // dst was freshly allocated, so it's guaranteed to be contiguous
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      for (int ch = 0; ch < channels_out; ++ch) {
        if (ch < src.Channels()) {
          dst.AtUnchecked<uint8_t>(row, col, ch) = static_cast<uint8_t>(
                scale * src.AtUnchecked<_Tp>(row, col, ch));
        } else {
          if (ch == 3) {
            dst.AtUnchecked<uint8_t>(row, col, ch) = 255;
          } else {
            dst.AtUnchecked<uint8_t>(row, col, ch) = *(dst.ImmutablePtr<uint8_t>(row, col, 0));
          }
        }
      }
    }
  }

  return dst;
}


template <typename _Tp>
ImageBuffer ToFloat(const ImageBuffer &src, float scale) {
  SPDLOG_DEBUG("Converting {:s} to `float`, scale={}.", src.ToString(), scale);

  if (src.BufferType() == ImageBufferType::Float) {
    return src.DeepCopy();
  }

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.Height(), src.Width(), src.Channels(), ImageBufferType::Float);

  int rows = src.Height();
  int cols = src.Width();
  // dst was freshly allocated, so it's guaranteed to be contiguous
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      for (int ch = 0; ch < src.Channels(); ++ch) {
        dst.AtUnchecked<float>(row, col, ch) = static_cast<float>(
                scale * src.AtUnchecked<_Tp>(row, col, ch));
      }
    }
  }

  return dst;
}



template <typename _Tp_src, ImageBufferType _BTp_dst>
ImageBuffer ConvertTypeImpl(
    const ImageBuffer &src, double scale) {
  ImageBuffer dst(src.Height(), src.Width(), src.Channels(), _BTp_dst);
  int rows = src.Height();
  int cols = src.Width();
  // dst was freshly allocated, so it's guaranteed to be contiguous
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  using _Tp_dst = image_buffer_t<_BTp_dst>;
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      for (int ch = 0; ch < src.Channels(); ++ch) {
        dst.AtUnchecked<_Tp_dst>(row, col, ch) = static_cast<_Tp_dst>(
                scale * src.AtUnchecked<_Tp_src>(row, col, ch));
      }
    }
  }

  return dst;
}


template <typename _Tsrc>
ImageBuffer ConvertType(
    const ImageBuffer &src, ImageBufferType dst_type, double scale) {
  SPDLOG_DEBUG(
        "Converting {:s} to `{:s}`, scale={:.2f}.",
        src.ToString(), dst_type, scale);

  switch (dst_type) {
    case ImageBufferType::UInt8:
      return ConvertTypeImpl<_Tsrc, ImageBufferType::UInt8>(src, scale);

    case ImageBufferType::Int16:
      return ConvertTypeImpl<_Tsrc, ImageBufferType::Int16>(src, scale);

    case ImageBufferType::UInt16:
      return ConvertTypeImpl<_Tsrc, ImageBufferType::UInt16>(src, scale);

    case ImageBufferType::Int32:
      return ConvertTypeImpl<_Tsrc, ImageBufferType::Int32>(src, scale);

    case ImageBufferType::UInt32:
      return ConvertTypeImpl<_Tsrc, ImageBufferType::UInt32>(src, scale);

    case ImageBufferType::Int64:
      return ConvertTypeImpl<_Tsrc, ImageBufferType::Int64>(src, scale);

    case ImageBufferType::UInt64:
      return ConvertTypeImpl<_Tsrc, ImageBufferType::UInt64>(src, scale);

    case ImageBufferType::Float:
      return ConvertTypeImpl<_Tsrc, ImageBufferType::Float>(src, scale);

    case ImageBufferType::Double:
      return ConvertTypeImpl<_Tsrc, ImageBufferType::Double>(src, scale);
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(dst_type);
  msg += "` not handled in `ConvertType` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


template <typename _Tp>
ImageBuffer Magnitude(const ImageBuffer &src) {
  SPDLOG_DEBUG("Computing magnitude of {:s}.", src.ToString());

  ImageBuffer dst(src.Height(), src.Width(), 1, src.BufferType());

  int rows = src.Height();
  int cols = src.Width();
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  for (int row = 0; row < rows; ++row) {
    _Tp *dst_ptr = dst.MutablePtr<_Tp>(row, 0, 0);
    const _Tp *src_ptr = src.ImmutablePtr<_Tp>(row, 0, 0);

    for (int col = 0; col < cols; ++col) {
      _Tp sqr_sum = 0.0f;
      for (int ch = 0; ch < src.Channels(); ++ch) {
        const _Tp val = *src_ptr++;
        sqr_sum += (val * val);
      }
      *dst_ptr++ = std::sqrt(sqr_sum);
    }
  }

  return dst;
}


template <typename _Tp>
ImageBuffer Orientation(const ImageBuffer &src, float invalid) {
  SPDLOG_DEBUG("Computing orientation of {:s}.", src.ToString());

  if (src.Channels() != 2) {
    std::string msg(
          "Input to `Orientation` must be a dual-channel image, but got ");
    msg += src.ToString();
    msg += '!';
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }

  ImageBuffer dst(src.Height(), src.Width(), 1, src.BufferType());

  int rows = src.Height();
  int cols = src.Width();
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  for (int row = 0; row < rows; ++row) {
    _Tp *dst_ptr = dst.MutablePtr<_Tp>(row, 0, 0);
    const _Tp *src_ptr = src.ImmutablePtr<_Tp>(row, 0, 0);

    for (int col = 0; col < cols; ++col) {
      _Tp u = *src_ptr++;
      _Tp v = *src_ptr++;
      if (wkg::IsEpsZero(u) && wkg::IsEpsZero(v)) {
        *dst_ptr++ = static_cast<_Tp>(invalid);
      } else {
        *dst_ptr++ = std::atan2(v, u);
      }
    }
  }

  return dst;
}




} // namespace helpers
} // namespace viren2d

#endif // __VIREN2D_IMAGEBUFFER_HELPERS_H__
