#include <ostream>
#include <iomanip>
#include <type_traits>
#include <stdexcept>
#include <cstdlib>
#include <cassert>
#include <cstring> // memcpy
#include <algorithm> // std::swap
#include <utility> // pair
#include <tuple>
#include <functional> // std::function


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#ifdef __GNUC__
// GCC reports a lot of missing field initializers, which is known,
// not easy to fix & not a real problem, see
// https://github.com/nothings/stb/issues/1099
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif  // __GNUC__
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__


#include <werkzeugkiste/strings/strings.h>

#include <viren2d/imagebuffer.h>
#include <helpers/imagebuffer_helpers.impl.h>


#include <helpers/logging.h>
#include <helpers/color_conversion.h>


namespace viren2d {
namespace helpers {
ImageBuffer RGBx2HSV(
    const ImageBuffer &src,
    bool is_bgr_format) {
  SPDLOG_DEBUG(
        "ImageBuffer converting {:s} to HSV.",
        (is_bgr_format ? "BGR(A)" : "RGB(A)"));

  if (!src.IsValid()) {
    const std::string msg("Input ImageBuffer is invalid in `RGBx2HSV`!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  if ((src.BufferType() != ImageBufferType::UInt8)
      || (src.Channels() < 2)
      || (src.Channels() > 4)) {
    std::string msg(
          "Input to `RGB2HSV` must be 3- or 4-channel buffer of type `uint8`, "
          "but got ");
    msg += src.ToString();
    msg += '!';
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.Height(), src.Width(), 3, ImageBufferType::UInt8);

  int rows = src.Height();
  int cols = src.Width();
  // dst was freshly allocated, so it's guaranteed to be contiguous
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  const int ch_r = is_bgr_format ? 2 : 0;
  const int ch_b = is_bgr_format ? 0 : 2;

  float hue, sat, val;
  for (int row = 0; row < rows; ++row) {
    unsigned char *dst_ptr = dst.MutablePtr<unsigned char>(row, 0, 0);
    for (int col = 0; col < cols; ++col) {
      std::tie(hue, sat, val) = CvtHelperRGB2HSV(
            src.AtUnchecked<unsigned char>(row, col, ch_r) / 255.0f,
            src.AtUnchecked<unsigned char>(row, col, 1) / 255.0f,
            src.AtUnchecked<unsigned char>(row, col, ch_b) / 255.0f);

      // Sacrifice angular resolution to fit into uint8:
      *dst_ptr++ = static_cast<unsigned char>(hue / 2.0f);
      *dst_ptr++ = static_cast<unsigned char>(255.0f * sat);
      *dst_ptr++ = static_cast<unsigned char>(255.0f * val);
    }
  }

  return dst;
}


ImageBuffer HSV2RGBx(
    const ImageBuffer &src,
    int output_channels,
    bool request_bgr_format) {
  SPDLOG_DEBUG(
        "ImageBuffer converting HSV to {:s}.",
        (request_bgr_format ? "BGR(A)" : "RGB(A)"));

  if (!src.IsValid()) {
    const std::string msg("Input ImageBuffer is invalid in `HSV2RGBx`!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  if ((src.BufferType() != ImageBufferType::UInt8)
      || (src.Channels() != 3)) {
    std::string msg(
          "Input to `HSV2RGB` must be 3-channel buffer of type `uint8`, "
          "but got ");
    msg += src.ToString();
    msg += '!';
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(
        src.Height(), src.Width(), output_channels, ImageBufferType::UInt8);

  int rows = src.Height();
  int cols = src.Width();
  // dst was freshly allocated, so it's guaranteed to be contiguous
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  const int ch_r = request_bgr_format ? 2 : 0;
  const int ch_b = request_bgr_format ? 0 : 2;

  float r, g, b;
  for (int row = 0; row < rows; ++row) {
    unsigned char *dst_ptr = dst.MutablePtr<unsigned char>(row, 0, 0);
    const unsigned char *src_ptr = dst.ImmutablePtr<unsigned char>(row, 0, 0);
    for (int col = 0; col < cols; ++col) {
      std::tie(r, g, b) = CvtHelperHSV2RGB(
          src_ptr[0] * 2.0f,  // Hue input in [0, 180]
          src_ptr[1] / 255.0f,  // Saturation input in [0, 255]
          src_ptr[2] / 255.0f); // Value input in [0, 255]

      dst_ptr[ch_r] = static_cast<unsigned char>(255.0f * r);
      dst_ptr[1] = static_cast<unsigned char>(255.0f * g);
      dst_ptr[ch_b] = static_cast<unsigned char>(255.0f * b);
      if (output_channels == 4) {
        dst_ptr[3] = 255;
      }
      dst_ptr += output_channels;
      src_ptr += 3;
    }
  }

  return dst;
}
}  // namespace helpers

//---------------------------------------------------- ImageBufferType
const std::type_info &ImageBufferTypeInfo(ImageBufferType t) {
  switch (t) {
    case ImageBufferType::UInt8:
      return typeid(image_buffer_t<ImageBufferType::UInt8>);

    case ImageBufferType::Int16:
      return typeid(image_buffer_t<ImageBufferType::Int16>);

    case ImageBufferType::UInt16:
      return typeid(image_buffer_t<ImageBufferType::UInt16>);

    case ImageBufferType::Int32:
      return typeid(image_buffer_t<ImageBufferType::Int32>);

    case ImageBufferType::UInt32:
      return typeid(image_buffer_t<ImageBufferType::UInt32>);

    case ImageBufferType::Int64:
      return typeid(image_buffer_t<ImageBufferType::Int64>);

    case ImageBufferType::UInt64:
      return typeid(image_buffer_t<ImageBufferType::UInt64>);

    case ImageBufferType::Float:
      return typeid(image_buffer_t<ImageBufferType::Float>);

    case ImageBufferType::Double:
      return typeid(image_buffer_t<ImageBufferType::Double>);
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(t);
  msg += "` not handled in `ImageBufferTypeInfo` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


std::string ImageBufferTypeToString(ImageBufferType t) {
  switch (t) {
    case ImageBufferType::UInt8:
      return "uint8";

    case ImageBufferType::Int16:
      return "int16";

    case ImageBufferType::UInt16:
      return "uint16";

    case ImageBufferType::Int32:
      return "int32";

    case ImageBufferType::UInt32:
      return "uint32";

    case ImageBufferType::Int64:
      return "int64";

    case ImageBufferType::UInt64:
      return "uint64";

    case ImageBufferType::Float:
      return "float";

    case ImageBufferType::Double:
      return "double";
  }
  //TODO(dev) Include newly added string representation also in
  //  `ImageBufferFromString`!

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::ostringstream msg;
  msg << "Type `" << static_cast<int>(t)
      << "` not handled in `ImageBufferTypeToString` switch!";
  SPDLOG_ERROR(msg.str());
  throw std::logic_error(msg.str());
}


ImageBufferType ImageBufferTypeFromString(const std::string &s) {
  std::string srep = werkzeugkiste::strings::Lower(s);
  if (srep.compare("uint8") == 0) {
    return ImageBufferType::UInt8;
  } else if ((srep.compare("int16") == 0)
             || (srep.compare("short") == 0)) {
    return ImageBufferType::Int16;
  } else if (srep.compare("uint16") == 0) {
    return ImageBufferType::UInt16;
  } else if ((srep.compare("int") == 0)
             || (srep.compare("int32") == 0)) {
    return ImageBufferType::Int32;
  } else if (srep.compare("uint32") == 0) {
    return ImageBufferType::UInt32;
  } else if (srep.compare("int64") == 0) {
    return ImageBufferType::Int64;
  } else if (srep.compare("uint64") == 0) {
    return ImageBufferType::UInt64;
  } else if ((srep.compare("float") == 0)
             || (srep.compare("float32") == 0)) {
    return ImageBufferType::Float;
  } else if ((srep.compare("double") == 0)
             || (srep.compare("float64") == 0)) {
    return ImageBufferType::Double;
  } else {
    std::string msg("Could not look up `ImageBufferType` corresponding to \"");
    msg += s;
    msg += "\"!";
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }
}


int ElementSizeFromImageBufferType(ImageBufferType t) {
  switch (t) {
    case ImageBufferType::UInt8:
      return static_cast<int>(sizeof(uint8_t));

    case ImageBufferType::Int16:
      return static_cast<int>(sizeof(int16_t));

    case ImageBufferType::UInt16:
      return static_cast<int>(sizeof(uint16_t));

    case ImageBufferType::Int32:
      return static_cast<int>(sizeof(int32_t));

    case ImageBufferType::UInt32:
      return static_cast<int>(sizeof(uint32_t));

    case ImageBufferType::Int64:
      return static_cast<int>(sizeof(int64_t));

    case ImageBufferType::UInt64:
      return static_cast<int>(sizeof(uint64_t));

    case ImageBufferType::Float:
      return static_cast<int>(sizeof(float));

    case ImageBufferType::Double:
      return static_cast<int>(sizeof(double));
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(t);
  msg += "` not handled in `FormatDescriptor` switch!";
  throw std::logic_error(msg);
}


std::ostream &operator<<(std::ostream &os, ImageBufferType t) {
  os << ImageBufferTypeToString(t);
  return os;
}


//---------------------------------------------------- ImageBuffer
ImageBuffer::ImageBuffer()
  : data(nullptr),
    height(0),
    width(0),
    channels(0),
    element_size(0),
    row_stride(0),
    pixel_stride(0),
    buffer_type(ImageBufferType::UInt8),
    owns_data(false) {
  SPDLOG_DEBUG("ImageBuffer default constructor.");
}


ImageBuffer::ImageBuffer(
    int h, int w, int ch, ImageBufferType buf_type) {
  SPDLOG_DEBUG(
        "ImageBuffer constructor allocating memory for a "
        "{:d}x{:d}x{:d} {:s} image.",
        h, w, ch, ImageBufferTypeToString(buf_type));
  height = h;
  width = w;
  channels = ch;
  buffer_type = buf_type;
  element_size = ElementSizeFromImageBufferType(buf_type);
  pixel_stride = channels * element_size;
  row_stride = width * pixel_stride;
  const int num_bytes = height * row_stride;
  owns_data = true;
  data = static_cast<unsigned char*>(std::malloc(num_bytes));
  if (!data) {
    SPDLOG_CRITICAL(
          "Cannot allocate {:d} bytes to construct a {:d}x{:d}x{:d} {:s} ImageBuffer!",
          num_bytes, w, h, ch, ImageBufferTypeToString(buf_type));
    // Reset this ImageBuffer:
    owns_data = false;
    Cleanup();
  }
}


ImageBuffer::~ImageBuffer() {
  SPDLOG_DEBUG("ImageBuffer destructor.");
  Cleanup();
}


ImageBuffer::ImageBuffer(const ImageBuffer &other) noexcept {
  SPDLOG_DEBUG(
        "ImageBuffer copy constructor, with other: {:s}.", other.ToString());
  height = other.height;
  width = other.width;
  channels = other.channels;
  element_size = other.element_size;
  buffer_type = other.buffer_type;
  row_stride = other.row_stride;
  pixel_stride = other.pixel_stride;
  owns_data = other.owns_data;

  if (other.owns_data) {
    const int num_bytes = other.NumBytes();
    data = static_cast<unsigned char*>(std::malloc(num_bytes));
    if (data) {
      std::memcpy(data, other.data, num_bytes);
    } else {
      SPDLOG_CRITICAL(
            "Cannot allocate {:d} bytes to copy construct an ImageBuffer "
            "from {:s}!", num_bytes, other.ToString());
      // Reset this ImageBuffer:
      owns_data = false;
      Cleanup();
    }
  } else {
    data = other.data;
  }
}


ImageBuffer::ImageBuffer(ImageBuffer &&other) noexcept
  : data(other.data),
    height(other.height),
    width(other.width),
    channels(other.channels),
    element_size(other.element_size),
    row_stride(other.row_stride),
    pixel_stride(other.pixel_stride),
    buffer_type(other.buffer_type),
    owns_data(other.owns_data) {
  SPDLOG_DEBUG("ImageBuffer move constructor.");
  // Reset "other", but ensure that the memory won't be freed:
  other.owns_data = false;
  other.Cleanup();
}


ImageBuffer &ImageBuffer::operator=(const ImageBuffer &other) {
  SPDLOG_DEBUG("ImageBuffer copy assignment operator.");
  return *this = ImageBuffer(other);
}


ImageBuffer &ImageBuffer::operator=(ImageBuffer &&other) noexcept {
  SPDLOG_DEBUG("ImageBuffer move assignment operator.");
  std::swap(data, other.data);
  std::swap(width, other.width);
  std::swap(height, other.height);
  std::swap(channels, other.channels);
  std::swap(element_size, other.element_size);
  std::swap(row_stride, other.row_stride);
  std::swap(pixel_stride, other.pixel_stride);
  std::swap(buffer_type, other.buffer_type);
  std::swap(owns_data, other.owns_data);
  return *this;
}


void ImageBuffer::CreateSharedBuffer(unsigned char *buffer, int height, int width, int channels,
    int row_stride, int pixel_stride, ImageBufferType buffer_type) {
  SPDLOG_DEBUG(
        "ImageBuffer::CreateSharedBuffer: h={:d}, w={:d},"
        " ch={:d}, {:s}, row_stride={:d}, col_stride={:d}.",
        height, width, channels, ImageBufferTypeToString(buffer_type),
        row_stride, pixel_stride);
  // Clean up first (if this instance already holds image data)
  Cleanup();

  this->owns_data = false;
  this->data = buffer;
  this->width = width;
  this->height = height;
  this->channels = channels;
  this->row_stride = row_stride;
  this->buffer_type = buffer_type;
  this->element_size = ElementSizeFromImageBufferType(buffer_type);
  this->pixel_stride = pixel_stride;
}


void ImageBuffer::CreateCopiedBuffer(
    unsigned char const *buffer, int height, int width, int channels,
    int row_stride, int column_stride, int channel_stride,
    ImageBufferType buffer_type) {
  SPDLOG_DEBUG(
        "ImageBuffer::CreateCopiedBuffer: h={:d}, w={:d},"
        " ch={:d}, {:s}, row_stride={:d}, col_stride={:d},"
        " ch_stride={:d}.",
        height, width, channels, ImageBufferTypeToString(buffer_type),
        row_stride, column_stride, channel_stride);
  // Clean up first (if this instance already holds image data)
  Cleanup();

  this->element_size = ElementSizeFromImageBufferType(buffer_type);
  const int num_bytes = height * width * channels * element_size;
  data = static_cast<unsigned char*>(std::malloc(num_bytes));
  if (!data) {
    std::ostringstream msg;
    msg << "Cannot allocate " << num_bytes << " bytes to copy ImageBuffer!";
    SPDLOG_ERROR(msg.str());
    throw std::runtime_error(msg.str());
  }
  owns_data = true;
  this->width = width;
  this->height = height;
  this->channels = channels;
  this->row_stride = width * channels * element_size;
  this->buffer_type = buffer_type;
  this->pixel_stride = channels * element_size;

  if ((row_stride == (width * channels * element_size))
      && (column_stride > 0) && (channel_stride > 0)) {
    // Buffer is contiguous, only need a single memcpy:
    std::memcpy(data, buffer, num_bytes);
  } else {
    // Is a single row contiguous?
    if ((column_stride == (channels * element_size))
        && (channel_stride > 0)) {
      for (int row = 0; row < height; ++row) {
        std::memcpy(
              data + (row * this->row_stride),
              buffer + (row * row_stride),
              width * column_stride);
      }
    } else {
      // Copy pixel by pixel because the input buffer might be a sliced,
      // transposed, or any other view (e.g. with negative strides)
      for (int row = 0, src_row_offset = 0;
           row < height;
           ++row, src_row_offset += row_stride) {
        // The destination buffer is freshly allocated, thus the memory
        // is nicely aligned.
        unsigned char *dst_ptr = MutablePtr<unsigned char>(row, 0, 0);

        for (int col = 0, src_col_offset = 0;
             col < width;
             ++col, src_col_offset += column_stride) {

          for (int ch = 0, src_channel_offset = 0;
               ch < channels;
               ++ch, src_channel_offset += channel_stride) {
            // We can only copy one element after the other
            std::memcpy(
                  dst_ptr, buffer + src_row_offset + src_col_offset + src_channel_offset, element_size);
            dst_ptr += element_size;
          }
        }
      }
//      for (int row = 0; row < height; ++row) {
//        for (int col = 0; col < width; ++col) {
//          for (int ch = 0; ch < channels; ++ch) {
//            const int dst_idx = (row * this->row_stride) + (col * this->pixel_stride) + (ch * this->element_size);
//            const int src_idx = (row * row_stride) + (col * column_stride) + (ch * channel_stride);
//            std::memcpy(data + dst_idx, buffer + src_idx, element_size);
//          }
//        }
//      }
    }
  }
}


ImageBuffer ImageBuffer::DeepCopy() const {
  ImageBuffer cp;
  cp.CreateCopiedBuffer(
        data, height, width, channels,
        row_stride, pixel_stride, element_size, buffer_type);
  return cp;
}


ImageBuffer ImageBuffer::ROI(int left, int top, int roi_width, int roi_height) {
  if ((roi_width <= 0) || (roi_height <= 0)) {
    std::ostringstream msg;
    msg << "Invalid ROI(l=" << left << ", t=" << top << ", w=" << roi_width
        << ", h=" << roi_height << "), height & width must be > 0!";
    SPDLOG_ERROR(msg.str());
    throw std::invalid_argument(msg.str());
  }

  if ((left < 0) || ((left + roi_width) > width)
      || (top < 0) || ((top + roi_height) > height)) {
    std::ostringstream msg;
    msg << "Invalid ROI(l=" << left << ", t=" << top << ", w=" << roi_width
        << ", h=" << roi_height << ") for ImageBuffer of size w=" << width
        << ", h=" << height << '!';
    SPDLOG_ERROR(msg.str());
    throw std::out_of_range(msg.str());
  }

  ImageBuffer roi;
  unsigned char *roi_data = data + ByteOffset(top, left, 0);
  roi.CreateSharedBuffer(
        roi_data, roi_height, roi_width, channels,
        row_stride, pixel_stride, buffer_type);
  return roi;
}


void ImageBuffer::SwapChannels(int ch1, int ch2) {
  SPDLOG_DEBUG("ImageBuffer::SwapChannels {:d} & {:d}.", ch1, ch2);

  if ((ch1 < 0) || (ch1 >= channels)
      || (ch2 < 0) || (ch2 >= channels)) {
    std::ostringstream msg;
    msg << "Cannot swap channels " << ch1
        << " and " << ch2 << " of a "
        << channels << "-channel ImageBuffer: Invalid inputs!";
    SPDLOG_ERROR(msg.str());
    throw std::invalid_argument(msg.str());
  }

  if ((!data) || (ch1 == ch2)) {
    return;
  }

  switch (buffer_type) {
    case ImageBufferType::UInt8:
      helpers::SwapChannels<uint8_t>(*this, ch1, ch2);
      return;

    case ImageBufferType::Int16:
      helpers::SwapChannels<int16_t>(*this, ch1, ch2);
      return;

    case ImageBufferType::UInt16:
      helpers::SwapChannels<uint16_t>(*this, ch1, ch2);
      return;

    case ImageBufferType::Int32:
      helpers::SwapChannels<int32_t>(*this, ch1, ch2);
      return;

    case ImageBufferType::UInt32:
      helpers::SwapChannels<uint32_t>(*this, ch1, ch2);
      return;

    case ImageBufferType::Int64:
      helpers::SwapChannels<int64_t>(*this, ch1, ch2);
      return;

    case ImageBufferType::UInt64:
      helpers::SwapChannels<uint64_t>(*this, ch1, ch2);
      return;

    case ImageBufferType::Float:
      helpers::SwapChannels<float>(*this, ch1, ch2);
      return;

    case ImageBufferType::Double:
      helpers::SwapChannels<double>(*this, ch1, ch2);
      return;
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(buffer_type);
  msg += "` not handled in `SwapChannels` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


void ImageBuffer::TakeOwnership() {
  owns_data = true;
}


ImageBuffer ImageBuffer::ToChannels(int output_channels) const {
  SPDLOG_DEBUG(
        "ImageBuffer::ToChannels converting {:d} to {:d} channels.",
        channels, output_channels);
  if (!IsValid()) {
    const std::string msg(
          "Cannot convert an invalid ImageBuffer in `ToChannels`!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  if ((channels != 1) && (channels != 3) && (channels != 4)) {
    std::ostringstream msg;
    msg << "Channel conversion is only supported for ImageBuffer with "
           "1, 3, or 4 channels, but this buffer has "
        << channels << '!';
    SPDLOG_ERROR(msg.str());
    throw std::invalid_argument(msg.str());
  }

  if (channels == 1) {
    // Grayscale-to-something
    if (output_channels == 1) {
      return DeepCopy();
    } else if ((output_channels == 3)
               || (output_channels == 4)){
      return helpers::Gray2RGBx(*this, output_channels);
    } else {
      std::ostringstream msg;
      msg << "Conversion from single-channel ImageBuffer to "
          << output_channels << " output channels is not supported!";
      SPDLOG_ERROR(msg.str());
      throw std::invalid_argument(msg.str());
    }
  } else if (channels == 3) {
    // RGB-to-something
    if (output_channels == 3) {
      return DeepCopy();
    } else if (output_channels == 4) {
      return helpers::RGBx2RGBx(*this, 4);
    } else {
      std::ostringstream msg;
      msg << "Conversion from 3-channel ImageBuffer to "
          << output_channels << " output channel(s) is not supported!";
      SPDLOG_ERROR(msg.str());
      throw std::invalid_argument(msg.str());
    }
  } else {
    // RGBA-to-something
    if (output_channels == 3) {
      return helpers::RGBx2RGBx(*this, 3);
    } else if (output_channels == 4) {
      return DeepCopy();
    } else {
      std::ostringstream msg;
      msg << "Conversion from 4-channel ImageBuffer to "
          << output_channels << " output channel(s) is not supported!";
      SPDLOG_ERROR(msg.str());
      throw std::invalid_argument(msg.str());
    }
  }
}


ImageBuffer ImageBuffer::ToUInt8(int output_channels) const {
  if (!IsValid()) {
    const std::string msg("Cannot convert an invalid ImageBuffer to `uint8`!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  switch (buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::ToUInt8<uint8_t>(*this, output_channels, 1);

    case ImageBufferType::Int16:
      return helpers::ToUInt8<int16_t>(*this, output_channels, 1);

    case ImageBufferType::UInt16:
      return helpers::ToUInt8<uint16_t>(*this, output_channels, 1);

    case ImageBufferType::Int32:
      return helpers::ToUInt8<int32_t>(*this, output_channels, 1);

    case ImageBufferType::UInt32:
      return helpers::ToUInt8<uint32_t>(*this, output_channels, 1);

    case ImageBufferType::Int64:
      return helpers::ToUInt8<int64_t>(*this, output_channels, 1);

    case ImageBufferType::UInt64:
      return helpers::ToUInt8<uint64_t>(*this, output_channels, 1);

    case ImageBufferType::Float:
      return helpers::ToUInt8<float>(*this, output_channels, 255);

    case ImageBufferType::Double:
      return helpers::ToUInt8<double>(*this, output_channels, 255);
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(buffer_type);
  msg += "` not handled in `ToUInt8` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


ImageBuffer ImageBuffer::ToFloat() const {
  if (!IsValid()) {
    const std::string msg("Cannot convert an invalid ImageBuffer to `float`!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  switch (buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::ToFloat<uint8_t>(*this, 1.0f/255.0f);

    case ImageBufferType::Int16:
      return helpers::ToFloat<int16_t>(*this, 1.0f/255.0f);

    case ImageBufferType::UInt16:
      return helpers::ToFloat<uint16_t>(*this, 1.0f/255.0f);

    case ImageBufferType::Int32:
      return helpers::ToFloat<int32_t>(*this, 1.0f/255.0f);

    case ImageBufferType::UInt32:
      return helpers::ToFloat<uint32_t>(*this, 1.0f/255.0f);

    case ImageBufferType::Int64:
      return helpers::ToFloat<int64_t>(*this, 1.0f/255.0f);

    case ImageBufferType::UInt64:
      return helpers::ToFloat<uint64_t>(*this, 1.0f/255.0f);

    case ImageBufferType::Float:
      return helpers::ToFloat<float>(*this, 1.0f);

    case ImageBufferType::Double:
      return helpers::ToFloat<double>(*this, 1.0f);
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(buffer_type);
  msg += "` not handled in `ToFloat` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


ImageBuffer ImageBuffer::AsType(
    ImageBufferType type, double scaling_factor) const {
  if (!IsValid()) {
    const std::string msg("Cannot type-convert an invalid ImageBuffer!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  switch (buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::ConvertType<uint8_t>(*this, type, scaling_factor);

    case ImageBufferType::Int16:
      return helpers::ConvertType<int16_t>(*this, type, scaling_factor);

    case ImageBufferType::UInt16:
      return helpers::ConvertType<uint16_t>(*this, type, scaling_factor);

    case ImageBufferType::Int32:
      return helpers::ConvertType<int32_t>(*this, type, scaling_factor);

    case ImageBufferType::UInt32:
      return helpers::ConvertType<uint32_t>(*this, type, scaling_factor);

    case ImageBufferType::Int64:
      return helpers::ConvertType<int64_t>(*this, type, scaling_factor);

    case ImageBufferType::UInt64:
      return helpers::ConvertType<uint64_t>(*this, type, scaling_factor);

    case ImageBufferType::Float:
      return helpers::ConvertType<float>(*this, type, scaling_factor);

    case ImageBufferType::Double:
      return helpers::ConvertType<double>(*this, type, scaling_factor);
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(buffer_type);
  msg += "` not handled in `AsType` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}



ImageBuffer ImageBuffer::Magnitude() const {
  if (!IsValid()) {
    const std::string msg(
          "Cannot compute `Magnitude` of an invalid ImageBuffer!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  switch (buffer_type) {
    case ImageBufferType::Float:
      return helpers::Magnitude<float>(*this);

    case ImageBufferType::Double:
      return helpers::Magnitude<double>(*this);

    default: {
        std::ostringstream msg;
        msg << "`Magnitude` can only be computed for buffers of type `float` "
               "or `double`, but got " << ToString() << '!';
        SPDLOG_ERROR(msg.str());
        throw std::logic_error(msg.str());
      }
  }
}


ImageBuffer ImageBuffer::Orientation(float invalid) const {
  if (!IsValid()) {
    const std::string msg(
          "Cannot compute `Orientation` of an invalid ImageBuffer!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  switch (buffer_type) {
    case ImageBufferType::Float:
      return helpers::Orientation<float>(*this, invalid);

    case ImageBufferType::Double:
      return helpers::Orientation<double>(*this, invalid);

    default: {
        std::ostringstream msg;
        msg << "`Orientation` can only be computed for buffers of type "
               "`float` or `double`, but got " << ToString() << '!';
        SPDLOG_ERROR(msg.str());
        throw std::logic_error(msg.str());
      }
  }
}


void ImageBuffer::Pixelate(
    int block_width, int block_height,
    int roi_left, int roi_top, int roi_width, int roi_height) {
  if (!IsValid()) {
    const std::string msg("Cannot pixelate an invalid ImageBuffer!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  if (channels > 4) {
    std::ostringstream msg;
    msg << "Pixelation is only supported for up to 4-channel "
           "images, but " << ToString() << " has "
        << channels << '!';
    SPDLOG_ERROR(msg.str());
    throw std::logic_error(msg.str());
  }

  // ROI returns a shared image buffer and performs out-of-range checks:
  const bool full_image = (roi_left == -1) && (roi_top == -1)
      && (roi_width == -1) && (roi_height == -1);
  viren2d::ImageBuffer roi = full_image
      ? ROI(0, 0, width, height)
      : ROI(roi_left, roi_top, roi_width, roi_height);

  switch (buffer_type) {
    case ImageBufferType::UInt8:
      helpers::Pixelate<uint8_t>(roi, block_width, block_height);
      return;

    case ImageBufferType::Int16:
      helpers::Pixelate<int16_t>(roi, block_width, block_height);
      return;

    case ImageBufferType::UInt16:
      helpers::Pixelate<uint16_t>(roi, block_width, block_height);
      return;

    case ImageBufferType::Int32:
      helpers::Pixelate<int32_t>(roi, block_width, block_height);
      return;

    case ImageBufferType::UInt32:
      helpers::Pixelate<uint32_t>(roi, block_width, block_height);
      return;

    case ImageBufferType::Int64:
      helpers::Pixelate<int64_t>(roi, block_width, block_height);
      return;

    case ImageBufferType::UInt64:
      helpers::Pixelate<uint64_t>(roi, block_width, block_height);
      return;

    case ImageBufferType::Float:
      helpers::Pixelate<float>(roi, block_width, block_height);
      return;

    case ImageBufferType::Double:
      helpers::Pixelate<double>(roi, block_width, block_height);
      return;
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(buffer_type);
  msg += "` was not handled in `Pixelate` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


ImageBuffer ImageBuffer::Blend(
    const ImageBuffer &other, double alpha_other) const {
  if (!IsValid() || !other.IsValid()) {
    const std::string msg("Cannot blend invalid ImageBuffers!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  switch(buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::BlendConstant<uint8_t>(*this, other, alpha_other);

    case ImageBufferType::Int16:
      return helpers::BlendConstant<int16_t>(*this, other, alpha_other);

    case ImageBufferType::UInt16:
      return helpers::BlendConstant<uint16_t>(*this, other, alpha_other);

    case ImageBufferType::Int32:
      return helpers::BlendConstant<int32_t>(*this, other, alpha_other);

    case ImageBufferType::UInt32:
      return helpers::BlendConstant<uint32_t>(*this, other, alpha_other);

    case ImageBufferType::Int64:
      return helpers::BlendConstant<int64_t>(*this, other, alpha_other);

    case ImageBufferType::UInt64:
      return helpers::BlendConstant<uint64_t>(*this, other, alpha_other);

    case ImageBufferType::Float:
      return helpers::BlendConstant<float>(*this, other, alpha_other);

    case ImageBufferType::Double:
      return helpers::BlendConstant<double>(*this, other, alpha_other);
  }

  // Throw an exception as fallback, because due to the default
  // compiler settings, we would have ignored the warning about
  // missing case values.
  std::string msg("Type `");
  msg += ImageBufferTypeToString(buffer_type);
  msg += "` was not handled in `Blend` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


ImageBuffer ImageBuffer::Blend(
    const ImageBuffer &other, const ImageBuffer &weights) const {
  if (!IsValid() || !other.IsValid() || !weights.IsValid()) {
    const std::string msg("Cannot blend invalid ImageBuffers!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  switch(buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::BlendWeights<uint8_t>(*this, other, weights);

    case ImageBufferType::Int16:
      return helpers::BlendWeights<int16_t>(*this, other, weights);

    case ImageBufferType::UInt16:
      return helpers::BlendWeights<uint16_t>(*this, other, weights);

    case ImageBufferType::Int32:
      return helpers::BlendWeights<int32_t>(*this, other, weights);

    case ImageBufferType::UInt32:
      return helpers::BlendWeights<uint32_t>(*this, other, weights);

    case ImageBufferType::Int64:
      return helpers::BlendWeights<int64_t>(*this, other, weights);

    case ImageBufferType::UInt64:
      return helpers::BlendWeights<uint64_t>(*this, other, weights);

    case ImageBufferType::Float:
      return helpers::BlendWeights<float>(*this, other, weights);

    case ImageBufferType::Double:
      return helpers::BlendWeights<double>(*this, other, weights);
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(buffer_type);
  msg += "` was not handled in `Blend` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


ImageBuffer ImageBuffer::Channel(int channel) const {
  if ((channel < 0) || (channel >= channels)) {
    std::ostringstream msg;
    msg << "Cannot extract channel #" << channel
        << " from ImageBuffer with " << channels
        << " channels!";
    SPDLOG_ERROR(msg.str());
    throw std::invalid_argument(msg.str());
  }

  switch (buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::ExtractChannel<uint8_t>(*this, channel);

    case ImageBufferType::Int16:
      return helpers::ExtractChannel<int16_t>(*this, channel);

    case ImageBufferType::UInt16:
      return helpers::ExtractChannel<uint16_t>(*this, channel);

    case ImageBufferType::Int32:
      return helpers::ExtractChannel<int32_t>(*this, channel);

    case ImageBufferType::UInt32:
      return helpers::ExtractChannel<uint32_t>(*this, channel);

    case ImageBufferType::Int64:
      return helpers::ExtractChannel<int64_t>(*this, channel);

    case ImageBufferType::UInt64:
      return helpers::ExtractChannel<uint64_t>(*this, channel);

    case ImageBufferType::Float:
      return helpers::ExtractChannel<float>(*this, channel);

    case ImageBufferType::Double:
      return helpers::ExtractChannel<double>(*this, channel);
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(buffer_type);
  msg += "` not handled in `Channel` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


ImageBuffer ImageBuffer::Dim(double alpha) const {
  if (!IsValid()) {
    const std::string msg("Cannot dim an invalid ImageBuffer!");
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  switch(buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::DimImpl<uint8_t>(*this, alpha);

    case ImageBufferType::Int16:
      return helpers::DimImpl<int16_t>(*this, alpha);

    case ImageBufferType::UInt16:
      return helpers::DimImpl<uint16_t>(*this, alpha);

    case ImageBufferType::Int32:
      return helpers::DimImpl<int32_t>(*this, alpha);

    case ImageBufferType::UInt32:
      return helpers::DimImpl<uint32_t>(*this, alpha);

    case ImageBufferType::Int64:
      return helpers::DimImpl<int64_t>(*this, alpha);

    case ImageBufferType::UInt64:
      return helpers::DimImpl<uint64_t>(*this, alpha);

    case ImageBufferType::Float:
      return helpers::DimImpl<float>(*this, alpha);

    case ImageBufferType::Double:
      return helpers::DimImpl<double>(*this, alpha);
  }

  // Throw an exception as fallback, because due to the default
  // compiler settings, we would have ignored the warning about
  // missing case values.
  std::string msg("Type `");
  msg += ImageBufferTypeToString(buffer_type);
  msg += "` was not handled in `Dim` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


bool ImageBuffer::IsValid() const {
  return (data != nullptr);
}


void ImageBuffer::MinMaxLocation(
    double *min_val, double *max_val,
    Vec2i *min_loc, Vec2i *max_loc,
    int channel) const {
  switch (buffer_type) {
    case ImageBufferType::UInt8:
      helpers::MinMaxLocation<uint8_t>(
            *this, channel, min_val, max_val, min_loc, max_loc);
      return;

    case ImageBufferType::Int16:
      helpers::MinMaxLocation<int16_t>(
            *this, channel, min_val, max_val, min_loc, max_loc);
      return;

    case ImageBufferType::UInt16:
      helpers::MinMaxLocation<uint16_t>(
            *this, channel, min_val, max_val, min_loc, max_loc);
      return;

    case ImageBufferType::Int32:
      helpers::MinMaxLocation<int32_t>(
            *this, channel, min_val, max_val, min_loc, max_loc);
      return;

    case ImageBufferType::UInt32:
      helpers::MinMaxLocation<uint32_t>(
            *this, channel, min_val, max_val, min_loc, max_loc);
      return;

    case ImageBufferType::Int64:
      helpers::MinMaxLocation<int64_t>(
            *this, channel, min_val, max_val, min_loc, max_loc);
      return;

    case ImageBufferType::UInt64:
      helpers::MinMaxLocation<uint64_t>(
            *this, channel, min_val, max_val, min_loc, max_loc);
      return;

    case ImageBufferType::Float:
      helpers::MinMaxLocation<float>(
            *this, channel, min_val, max_val, min_loc, max_loc);
      return;

    case ImageBufferType::Double:
      helpers::MinMaxLocation<double>(
            *this, channel, min_val, max_val, min_loc, max_loc);
      return;
  }

  // Throw an exception as fallback, because ending up here would be an
  // implementation error (i.e. we ignored the warning about missing value
  // in the switch/case above).
  std::string msg("Type `");
  msg += ImageBufferTypeToString(buffer_type);
  msg += "` was not handled in `MinMaxLocation` switch!";
  SPDLOG_ERROR(msg);
  throw std::logic_error(msg);
}


std::string ImageBuffer::ToString() const {
  if (!IsValid()) {
    return "ImageBuffer(invalid)";
  }

  std::ostringstream s;
  s << "ImageBuffer(" << width << "x" << height
    << "x" << channels
    << ", " << ImageBufferTypeToString(buffer_type);

  if (owns_data)
    s << ", copied memory";
  else
    s << ", shared memory";

  s << ")";

  return s.str();
}


void ImageBuffer::Cleanup() {
  SPDLOG_TRACE("ImageBuffer::Cleanup().");
  if (data && owns_data) {
    SPDLOG_TRACE(
          "ImageBuffer freeing {:d}x{:d}x{:d}={:d} entries a {:d} byte(s).",
          width, height, channels, NumElements(), item_size);
    std::free(data);
  }
  data = nullptr;
  owns_data = false;
  width = 0;
  height = 0;
  channels = 0;
  element_size = 0;
  buffer_type = ImageBufferType::UInt8;
  row_stride = 0;
  pixel_stride = 0;
}


ImageBuffer ConvertRGB2HSV(const ImageBuffer &image_rgb, bool is_bgr_format) {
  return helpers::RGBx2HSV(image_rgb, is_bgr_format);
}


ImageBuffer MaskHSVRange(const ImageBuffer &hsv,
    const std::pair<float, float> &hue_range, const std::pair<float, float> &saturation_range,
    const std::pair<float, float> &value_range) {
  if ((hsv.Channels() != 3) || (hsv.BufferType() != ImageBufferType::UInt8)) {
    std::string s(
          "Invalid input to `MaskHSVRange`. Expected 3-channel HSV of type uint8, but got: ");
    s += hsv.ToString();
    s += '!';
    SPDLOG_ERROR(s);
    throw std::invalid_argument(s);
  }

  std::function<unsigned char(float)> cvt_hue = [](float h) {
    return static_cast<unsigned char>(h / 2.0f);
  };

  std::function<unsigned char(float)> cvt_sv = [](float v) {
    return static_cast<unsigned char>(255.0f * v);
  };

  return hsv.MaskRange(
        cvt_hue(hue_range.first), cvt_hue(hue_range.second),
        cvt_sv(saturation_range.first), cvt_sv(saturation_range.second),
        cvt_sv(value_range.first), cvt_sv(value_range.second));
}


ImageBuffer ColorPop(const ImageBuffer &image,
    const std::pair<float, float> &hue_range,
    const std::pair<float, float> &saturation_range,
    const std::pair<float, float> &value_range,
    bool is_bgr) {
  if (!image.IsValid()) {
    throw std::logic_error(
          "Cannot apply `ColorPop` on invalid ImageBuffer!");
  }

  if ((image.Channels() < 2) || (image.Channels() > 4)
      || (image.BufferType() != ImageBufferType::UInt8)) {
    std::ostringstream s;
    s << "`ColorPop` can only be applied on RGB(A)/BGR(A) inputs buffers of "
         "type `uint8`, but got: " << image.ToString() << '!';
    throw std::invalid_argument(s.str());
  }

  const ImageBuffer hsv = ConvertRGB2HSV(image, is_bgr);
  const ImageBuffer mask = MaskHSVRange(
        hsv, hue_range, saturation_range, value_range);
  const ImageBuffer gray = viren2d::ConvertRGB2Gray(image, 1);

  ImageBuffer pop = image.DeepCopy();
  for (int row = 0; row < image.Height(); ++row) {
    for (int col = 0; col < image.Width(); ++col) {
      if (mask.AtUnchecked<unsigned char>(row, col) == 0) {
        pop.AtUnchecked<unsigned char>(row, col, 0) = gray.AtUnchecked<unsigned char>(row, col);
        pop.AtUnchecked<unsigned char>(row, col, 1) = gray.AtUnchecked<unsigned char>(row, col);
        pop.AtUnchecked<unsigned char>(row, col, 2) = gray.AtUnchecked<unsigned char>(row, col);
      }
    }
  }
  return pop;
}


ImageBuffer ConvertRGB2Gray(
    const ImageBuffer &color, int output_channels, bool is_bgr_format) {
  if (!color.IsValid()) {
    throw std::logic_error(
          "Cannot convert an invalid ImageBuffer to grayscale!");
  }

  if (color.Channels() == 1) {
    return color.ToChannels(output_channels);
  } else if ((color.Channels() == 3)
             || (color.Channels() == 4)) {
    switch (color.BufferType()) {
      case ImageBufferType::UInt8:
        return helpers::RGBx2Gray<uint8_t>(
              color, output_channels, is_bgr_format);

      case ImageBufferType::Int16:
        return helpers::RGBx2Gray<int16_t>(
              color, output_channels, is_bgr_format);

      case ImageBufferType::UInt16:
        return helpers::RGBx2Gray<uint16_t>(
              color, output_channels, is_bgr_format);

      case ImageBufferType::Int32:
        return helpers::RGBx2Gray<int32_t>(
              color, output_channels, is_bgr_format);

      case ImageBufferType::UInt32:
        return helpers::RGBx2Gray<uint32_t>(
              color, output_channels, is_bgr_format);

      case ImageBufferType::Int64:
        return helpers::RGBx2Gray<int64_t>(
              color, output_channels, is_bgr_format);

      case ImageBufferType::UInt64:
        return helpers::RGBx2Gray<uint64_t>(
              color, output_channels, is_bgr_format);

      case ImageBufferType::Float:
        return helpers::RGBx2Gray<float>(
              color, output_channels, is_bgr_format);

      case ImageBufferType::Double:
        return helpers::RGBx2Gray<double>(
              color, output_channels, is_bgr_format);
    }

    // Throw an exception as fallback, because ending up here would be an
    // implementation error (i.e. we ignored the warning about missing value
    // in the switch/case above).
    std::string msg("Type `");
    msg += ImageBufferTypeToString(color.BufferType());
    msg += "` not handled in `ConvertRGB2Gray` switch!";
    throw std::logic_error(msg);
  } else {
    std::ostringstream msg;
    msg << "`ConvertRGB2Gray` conversion is only supported for "
           "1-, 3-, and 4-channel buffers. This one has "
        << color.Channels() << '!';
    SPDLOG_ERROR(msg.str());
    throw std::logic_error(msg.str());
  }
}


ImageBuffer ConvertHSV2RGB(
    const ImageBuffer &image_hsv, int output_channels,
    bool output_bgr_format) {
  return helpers::HSV2RGBx(image_hsv, output_channels, output_bgr_format);
}


ImageBuffer LoadImageUInt8(
    const std::string &image_filename,
    int force_num_channels) {
  SPDLOG_DEBUG(
        "LoadImageUInt8: \"{:s}\", force_num_channels={:d}.",
        image_filename, force_num_channels);

  int width, height, bytes_per_pixel;
  unsigned char *data = stbi_load(
        image_filename.c_str(), &width, &height, &bytes_per_pixel,
        force_num_channels);
  if (!data) {
    std::string msg("Could not load image from '");
    msg += image_filename;
    msg += "'!";
    SPDLOG_ERROR(msg);
    throw std::runtime_error(msg);
  }

  const int num_channels = (force_num_channels != STBI_default)
      ? force_num_channels : bytes_per_pixel;

  // First, let ImageBuffer reuse the buffer (no separate memory allocation)
  // Note regarding the pixel_stride: stbi_load will *always* set bytes_per_pixel
  // to the number it would have been if `force_num_channels` == 0.
  ImageBuffer buffer;
  buffer.CreateSharedBuffer(
        data, height, width, num_channels,
        width * num_channels, num_channels,
        ImageBufferType::UInt8);
  // Then, transfer ownership
  buffer.TakeOwnership();
  // Alternatively, we could create an explicit copy and free the stbi memory:
  //     buffer.CreateCopy(data, width, height, num_channels,
  //                       width * num_channels);
  //     stbi_image_free(data);
  // But according to valgrind, the reuse option works as intended, i.e. it
  // not leak memory (stbi only allocates the data buffer).
  return buffer;
}


void SaveImageUInt8(
    const std::string &image_filename,
    const ImageBuffer &image) {
  SPDLOG_DEBUG(
        "SaveImage: \"{:s}\", {:s}.",
        image_filename, image);

  int stb_result = 0; // stb return code 0 indicates failure

  if (image.BufferType() != ImageBufferType::UInt8) {
    std::string msg(
          "Saving ImageBuffer expected `uint8` buffer type, but got `");
    msg += ImageBufferTypeToString(image.BufferType());
    msg += "`!";
    SPDLOG_ERROR(msg);
    throw std::logic_error(msg);
  }

  const std::string fn_lower = werkzeugkiste::strings::Lower(image_filename);
  if (werkzeugkiste::strings::EndsWith(fn_lower, ".jpg")
      || werkzeugkiste::strings::EndsWith(fn_lower, ".jpeg")) {
    // stbi_write_jpg requires contiguous memory
    if (!image.IsContiguous()) {
      const std::string msg(
            "Cannot save JPEG because image memory is not contiguous!");
      SPDLOG_ERROR(msg);
      // TODO we could create a deep copy (which will be contiguous) and
      // then recurse (but ensure that this is only tried once!)
      throw std::logic_error(msg);
    }
    // Default JPEG quality setting: 90%
    stb_result = stbi_write_jpg(
          image_filename.c_str(), image.Width(), image.Height(),
          image.Channels(), image.ImmutableData(), 90);
  } else {
    if (werkzeugkiste::strings::EndsWith(fn_lower, ".png")) {
      stb_result = stbi_write_png(
            image_filename.c_str(), image.Width(), image.Height(),
            image.Channels(), image.ImmutableData(), image.RowStride());
    } else {
      const std::string msg(
            "ImageBuffer can only be saved as JPEG or PNG. File extension "
            "must be '.jpg', '.jpeg' or '.png'.");
      SPDLOG_ERROR(msg);
      throw std::invalid_argument(msg);
    }
  }

  if (stb_result == 0) {
    std::ostringstream msg;
    msg << "Could not save ImageBuffer to '" << image_filename
        << "' - failed with `stb` error code " << stb_result << '!';
    SPDLOG_ERROR(msg.str());
    throw std::runtime_error(msg.str());
  }
}

} // namespace viren2d
