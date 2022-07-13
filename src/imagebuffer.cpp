#include <sstream>
#include <iomanip>
#include <type_traits>
#include <stdexcept>
#include <cstdlib>
#include <cassert>
#include <cstring> // memcpy
#include <algorithm> // std::swap
#include <utility> // pair


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


namespace viren2d {
namespace helpers {
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


inline void CvtColorRGB2HSV(
    unsigned char *dst, unsigned char red, unsigned char green,
    unsigned char blue) {
  const float r = red / 255.0f;
  const float g = green / 255.0f;
  const float b = blue / 255.0f;

  const std::pair<float, int> max_val_idx = MaxValueIndex(r, g, b);
  const float min_val = std::min(r, std::min(g, b));
  const float delta = max_val_idx.first - min_val;

  float hue, sat;

  if (wkg::eps_zero(max_val_idx.first)
      || wkg::eps_zero(delta)) {
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

  // Sacrifice angular resolution to fit into uint8:
  dst[0] = static_cast<unsigned char>(hue / 2.0f);
  dst[1] = static_cast<unsigned char>(255.0f * sat);
  dst[2] = static_cast<unsigned char>(255.0f * max_val_idx.first);
}


ImageBuffer RGBx2HSV(
    const ImageBuffer &src,
    bool is_bgr_format) {
  SPDLOG_DEBUG(
        "ImageBuffer converting {:s} to HSV.",
        (is_bgr_format ? "BGR(A)" : "RGB(A)"));

  if (!src.IsValid()) {
    throw std::logic_error("Input ImageBuffer is invalid in `RGBx2HSV`!");
  }

  if ((src.BufferType() != ImageBufferType::UInt8)
      || (src.Channels() < 2)
      || (src.Channels() > 4)) {
    std::string s("Input to `RGB2HSV` must be 3- or 4-channel buffer of type `uint8`, but got ");
    s += src.ToString();
    s += '!';
    throw std::invalid_argument(s);
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

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      CvtColorRGB2HSV(
            dst.MutablePtr<unsigned char>(row, col, 0),
            src.AtUnchecked<unsigned char>(row, col, ch_r),
            src.AtUnchecked<unsigned char>(row, col, 1),
            src.AtUnchecked<unsigned char>(row, col, ch_b));
    }
  }

  return dst;
}


inline void CvtColorHSV2RGB(
    unsigned char *red, unsigned char *green, unsigned char *blue,
    unsigned char h, unsigned char s, unsigned char v) {

  const float hue = h * 2.0f;  // Input range [0, 180]
  const float sat = s / 255.0f;  // Input range [0, 255]
  const float val = v / 255.0f;  // Input range [0, 255]

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

  *red = static_cast<unsigned char>(255.0f * r);
  *green = static_cast<unsigned char>(255.0f * g);
  *blue = static_cast<unsigned char>(255.0f * b);
}


ImageBuffer HSV2RGBx(
    const ImageBuffer &src,
    int output_channels,
    bool request_bgr_format) {
  SPDLOG_DEBUG(
        "ImageBuffer converting HSV to {:s}.",
        (request_bgr_format ? "BGR(A)" : "RGB(A)"));

  if (!src.IsValid()) {
    throw std::logic_error("Input ImageBuffer is invalid in `HSV2RGBx`!");
  }

  if ((src.BufferType() != ImageBufferType::UInt8)
      || (src.Channels() != 3)) {
    std::string s("Input to `HSV2RGB` must be 3-channel buffer of type `uint8`, but got ");
    s += src.ToString();
    s += '!';
    throw std::invalid_argument(s);
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

  for (int row = 0; row < rows; ++row) {
    unsigned char *dst_ptr = dst.MutablePtr<unsigned char>(row, 0, 0);
    for (int col = 0; col < cols; ++col) {
      CvtColorHSV2RGB(
            dst_ptr + ch_r, dst_ptr + 1, dst_ptr + ch_b,
            src.AtUnchecked<unsigned char>(row, col, 0),
            src.AtUnchecked<unsigned char>(row, col, 1),
            src.AtUnchecked<unsigned char>(row, col, 2));

      if (output_channels == 4) {
        dst_ptr[3] = 255;
      }
      dst_ptr += output_channels;
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

    case ImageBufferType::Int32:
      return typeid(image_buffer_t<ImageBufferType::Int32>);

    case ImageBufferType::Float:
      return typeid(image_buffer_t<ImageBufferType::Float>);

    case ImageBufferType::Double:
      return typeid(image_buffer_t<ImageBufferType::Double>);
  }

  std::string s("Type `");
  s += ImageBufferTypeToString(t);
  s += "` not handled in `ImageBufferTypeInfo` switch!";
  throw std::logic_error(s);
}


std::string ImageBufferTypeToString(ImageBufferType t) {
  switch (t) {
    case ImageBufferType::UInt8:
      return "uint8";

    case ImageBufferType::Int16:
      return "int16";

    case ImageBufferType::Int32:
      return "int32";

    case ImageBufferType::Float:
      return "float";

    case ImageBufferType::Double:
      return "double";
  }
  //TODO(dev) Include newly added string representation also in
  //  `ImageBufferFromString`!

  std::ostringstream s;
  s << "Type `" << static_cast<int>(t)
    << "` not handled in `ImageBufferTypeToString` switch!";
  throw std::logic_error(s.str());
}


ImageBufferType ImageBufferTypeFromString(const std::string &s) {
  std::string srep = werkzeugkiste::strings::Lower(s);
  if (srep.compare("uint8") == 0) {
    return ImageBufferType::UInt8;
  } else if ((srep.compare("int16") == 0)
             || (srep.compare("short") == 0)) {
    return ImageBufferType::Int16;
  } else if ((srep.compare("int") == 0)
             || (srep.compare("int32") == 0)) {
    return ImageBufferType::Int32;
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
    throw std::invalid_argument(msg);
  }
}


int ElementSizeFromImageBufferType(ImageBufferType t) {
  switch (t) {
    case ImageBufferType::UInt8:
      return static_cast<int>(sizeof(uint8_t));

    case ImageBufferType::Int16:
      return static_cast<int>(sizeof(int16_t));

    case ImageBufferType::Int32:
      return static_cast<int>(sizeof(int32_t));

    case ImageBufferType::Float:
      return static_cast<int>(sizeof(float));

    case ImageBufferType::Double:
      return static_cast<int>(sizeof(double));
  }

  std::string s("Type `");
  s += ImageBufferTypeToString(t);
  s += "` not handled in `FormatDescriptor` switch!";
  throw std::logic_error(s);
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


void ImageBuffer::CreateCopiedBuffer(unsigned char const *buffer, int height, int width, int channels,
    int row_stride, int pixel_stride, ImageBufferType buffer_type) {
  SPDLOG_DEBUG(
        "ImageBuffer::CreateSharedBuffer: h={:d}, w={:d},"
        " ch={:d}, {:s}, row_stride={:d}, col_stride={:d}.",
        height, width, channels, ImageBufferTypeToString(buffer_type),
        row_stride, pixel_stride);
  // Clean up first (if this instance already holds image data)
  Cleanup();

  this->element_size = ElementSizeFromImageBufferType(buffer_type);
  const int num_bytes = height * width * channels * element_size;
  data = static_cast<unsigned char*>(std::malloc(num_bytes));
  if (!data) {
    std::ostringstream s;
    s << "Cannot allocate " << num_bytes << " bytes to copy ImageBuffer!";
    throw std::runtime_error(s.str());
  }
  owns_data = true;
  this->width = width;
  this->height = height;
  this->channels = channels;
  this->row_stride = width * channels * element_size;
  this->buffer_type = buffer_type;
  this->pixel_stride = channels * this->element_size;

  if (row_stride == (width * channels * element_size)) {
    // Buffer is contiguous, only need a single memcpy:
    std::memcpy(data, buffer, num_bytes);
  } else {
    // Is a single row contiguous?
    if (pixel_stride == (channels * element_size)) {
      for (int row = 0; row < height; ++row) {
        std::memcpy(
              data + (row * this->row_stride),
              buffer + (row * row_stride),
              width * pixel_stride);
      }
    } else {
      // Copy each pixel
      for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
          for (int ch = 0; ch < channels; ++ch) {
            const int dst_idx = (row * this->row_stride) + (col * this->pixel_stride) + (ch * this->element_size);
            const int src_idx = (row * row_stride) + (col * pixel_stride) + (ch * this->element_size);
            data[dst_idx] = buffer[src_idx];
          }
        }
      }
    }
  }

  //TODO add tests for numpy views with unusual column strides
  // maybe we'll want to extend ImageBuffer to handle these, too.
}


ImageBuffer ImageBuffer::DeepCopy() const {
  ImageBuffer cp;
  cp.CreateCopiedBuffer(
        data, height, width, channels,
        row_stride, pixel_stride, buffer_type);
  return cp;
}


ImageBuffer ImageBuffer::ROI(int left, int top, int roi_width, int roi_height) {
  if ((roi_width <= 0) || (roi_height <= 0)) {
    std::ostringstream s;
    s << "Invalid ROI(l=" << left << ", t=" << top << ", w=" << roi_width
      << ", h=" << roi_height << "), height & width must be > 0!";
    throw std::invalid_argument(s.str());
  }

  if ((left < 0) || ((left + roi_width) > width)
      || (top < 0) || ((top + roi_height) > height)) {
    std::ostringstream s;
    s << "Invalid ROI(l=" << left << ", t=" << top << ", w=" << roi_width
      << ", h=" << roi_height << ") for ImageBuffer of size w=" << width
      << ", h=" << height << '!';
    throw std::out_of_range(s.str());
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
    std::ostringstream s;
    s << "Cannot swap channels " << ch1
      << " and " << ch2 << " of a "
      << channels
      << "-channel ImageBuffer: Invalid inputs!";
    throw std::invalid_argument(s.str());
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

    case ImageBufferType::Int32:
      helpers::SwapChannels<int32_t>(*this, ch1, ch2);
      return;

    case ImageBufferType::Float:
      helpers::SwapChannels<float>(*this, ch1, ch2);
      return;

    case ImageBufferType::Double:
      helpers::SwapChannels<double>(*this, ch1, ch2);
      return;
  }

  std::string s("Type `");
  s += ImageBufferTypeToString(buffer_type);
  s += "` not handled in `SwapChannels` switch!";
  throw std::logic_error(s);
}


void ImageBuffer::TakeOwnership() {
  owns_data = true;
}


ImageBuffer ImageBuffer::ToChannels(int output_channels) const {
  SPDLOG_DEBUG(
        "ImageBuffer::ToChannels converting {:d} to {:d} channels.",
        channels, output_channels);
  if (!IsValid()) {
    throw std::logic_error(
          "Cannot convert an invalid ImageBuffer in `ToChannels`!");
  }

  if ((channels != 1) && (channels != 3) && (channels != 4)) {
    std::ostringstream s;
    s << "Channel conversion is only supported for ImageBuffer with "
         "1, 3, or 4 channels, but this buffer has "
      << channels << '!';

    throw std::invalid_argument(s.str());
  }

  if (channels == 1) {
    // Grayscale-to-something
    if (output_channels == 1) {
      return DeepCopy();
    } else if ((output_channels == 3)
               || (output_channels == 4)){
      return helpers::Gray2RGBx(*this, output_channels);
    } else {
      std::ostringstream s;
      s << "Conversion from single-channel ImageBuffer to "
        << output_channels << " output channels is not supported!";
      throw std::invalid_argument(s.str());
    }
  } else if (channels == 3) {
    // RGB-to-something
    if (output_channels == 3) {
      return DeepCopy();
    } else if (output_channels == 4) {
      return helpers::RGBx2RGBx(*this, 4);
    } else {
      std::ostringstream s;
      s << "Conversion from 3-channel ImageBuffer to "
        << output_channels << " output channel(s) is not supported!";
      throw std::invalid_argument(s.str());
    }
  } else {
    // RGBA-to-something
    if (output_channels == 3) {
      return helpers::RGBx2RGBx(*this, 3);
    } else if (output_channels == 4) {
      return DeepCopy();
    } else {
      std::ostringstream s;
      s << "Conversion from 4-channel ImageBuffer to "
        << output_channels << " output channel(s) is not supported!";
      throw std::invalid_argument(s.str());
    }
  }
}


ImageBuffer ImageBuffer::ToUInt8(int output_channels) const {
  if (!IsValid()) {
    throw std::logic_error(
          "Cannot convert an invalid ImageBuffer to `uint8`!");
  }

  switch (buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::ToUInt8<uint8_t>(*this, output_channels, 1);

    case ImageBufferType::Int16:
      return helpers::ToUInt8<int16_t>(*this, output_channels, 1);

    case ImageBufferType::Int32:
      return helpers::ToUInt8<int32_t>(*this, output_channels, 1);

    case ImageBufferType::Float:
      return helpers::ToUInt8<float>(*this, output_channels, 255);

    case ImageBufferType::Double:
      return helpers::ToUInt8<double>(*this, output_channels, 255);
  }

  std::string s("Type `");
  s += ImageBufferTypeToString(buffer_type);
  s += "` not handled in `ToUInt8` switch!";
  throw std::logic_error(s);
}


ImageBuffer ImageBuffer::ToFloat() const {
  if (!IsValid()) {
    throw std::logic_error(
          "Cannot convert an invalid ImageBuffer to `float`!");
  }

  switch (buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::ToFloat<uint8_t>(*this, 1.0f/255.0f);

    case ImageBufferType::Int16:
      return helpers::ToFloat<int16_t>(*this, 1.0f/255.0f);

    case ImageBufferType::Int32:
      return helpers::ToFloat<int32_t>(*this, 1.0f/255.0f);

    case ImageBufferType::Float:
      return helpers::ToFloat<float>(*this, 1.0f);

    case ImageBufferType::Double:
      return helpers::ToFloat<double>(*this, 1.0f);
  }

  std::string s("Type `");
  s += ImageBufferTypeToString(buffer_type);
  s += "` not handled in `ToFloat` switch!";
  throw std::logic_error(s);
}


ImageBuffer ImageBuffer::ToGrayscale(
    int output_channels, bool is_bgr_format) const {
  if (!IsValid()) {
    throw std::logic_error(
          "Cannot convert an invalid ImageBuffer to grayscale!");
  }

  if (channels == 1) {
    return ToChannels(output_channels);
  } else if ((channels == 3)
             || (channels == 4)) {
    switch (buffer_type) {
      case ImageBufferType::UInt8:
        return helpers::RGBx2Gray<uint8_t>(
              *this, output_channels, is_bgr_format);

      case ImageBufferType::Int16:
        return helpers::RGBx2Gray<int16_t>(
              *this, output_channels, is_bgr_format);

      case ImageBufferType::Int32:
        return helpers::RGBx2Gray<int32_t>(
              *this, output_channels, is_bgr_format);

      case ImageBufferType::Float:
        return helpers::RGBx2Gray<float>(
              *this, output_channels, is_bgr_format);

      case ImageBufferType::Double:
        return helpers::RGBx2Gray<double>(
              *this, output_channels, is_bgr_format);
    }

    std::string s("Type `");
    s += ImageBufferTypeToString(buffer_type);
    s += "` not handled in `ToGrayscale` switch!";
    throw std::logic_error(s);
  } else {
    std::ostringstream s;
    s << "`ToGrayscale` conversion is only supported for "
         "1-, 3-, and 4-channel buffers. This one has "
      << channels << '!';
    throw std::logic_error(s.str());
  }
}


ImageBuffer ImageBuffer::Magnitude() const {
  if (!IsValid()) {
    throw std::logic_error(
          "Cannot compute `Magnitude` of an invalid ImageBuffer!");
  }

  switch (buffer_type) {
    case ImageBufferType::Float:
      return helpers::Magnitude<float>(*this);

    case ImageBufferType::Double:
      return helpers::Magnitude<double>(*this);

    default: {
        std::ostringstream s;
        s << "`Magnitude` can only be computed for buffers of type `float` or "
             "`double`, but got "
          << ToString() << '!';
        throw std::logic_error(s.str());
      }
  }
}


ImageBuffer ImageBuffer::Orientation(float invalid) const {
  if (!IsValid()) {
    throw std::logic_error(
          "Cannot compute `Orientation` of an invalid ImageBuffer!");
  }

  switch (buffer_type) {
    case ImageBufferType::Float:
      return helpers::Orientation<float>(*this, invalid);

    case ImageBufferType::Double:
      return helpers::Orientation<double>(*this, invalid);

    default: {
        std::ostringstream s;
        s << "`Orientation` can only be computed for buffers of type `float` or "
             "`double`, but got "
          << ToString() << '!';
        throw std::logic_error(s.str());
      }
  }
}


void ImageBuffer::Pixelate(
    int block_width, int block_height,
    int roi_left, int roi_top, int roi_width, int roi_height) {
  if (!IsValid()) {
    throw std::logic_error(
          "Cannot pixelate an invalid ImageBuffer!");
  }

  // ROI returns a shared image buffer and performs out-of-range checks:
  const bool full_image = (roi_left == -1) && (roi_top == -1)
      && (roi_width == -1) && (roi_height == -1);
  viren2d::ImageBuffer roi = full_image
      ? ROI(0, 0, width, height)
      : ROI(roi_left, roi_top, roi_width, roi_height);

  // Definitely not the most elegant way of calling
  // the corresponding helper, but given the current
  // variadic `SetToPixel` template (and that I can't
  // think of a use case where we have more than 4
  // channels to pixelate), this is good enough for me:
  switch (buffer_type) {
    case ImageBufferType::UInt8: {
        if (channels == 1) {
          helpers::Pixelate<uint8_t, 1>(roi, block_width, block_height);
        } else if (channels == 2) {
          helpers::Pixelate<uint8_t, 2>(roi, block_width, block_height);
        } else if (channels == 3) {
          helpers::Pixelate<uint8_t, 3>(roi, block_width, block_height);
        } else if (channels == 4) {
          helpers::Pixelate<uint8_t, 4>(roi, block_width, block_height);
        } else {
          std::ostringstream s;
          s << "Pixelation is only supported for up to 4-channel "
               "images, but this `uint8_t` ImageBuffer has "
            << channels << '!';
          throw std::logic_error(s.str());
        }
        return;
      }

    case ImageBufferType::Int16: {
        if (channels == 1) {
          helpers::Pixelate<int16_t, 1>(roi, block_width, block_height);
        } else if (channels == 2) {
          helpers::Pixelate<int16_t, 2>(roi, block_width, block_height);
        } else if (channels == 3) {
          helpers::Pixelate<int16_t, 3>(roi, block_width, block_height);
        } else if (channels == 4) {
          helpers::Pixelate<int16_t, 4>(roi, block_width, block_height);
        } else {
          std::ostringstream s;
          s << "Pixelation is only supported for up to 4-channel "
               "images, but this `int16_t` ImageBuffer has "
            << channels << '!';
          throw std::logic_error(s.str());
        }
        return;
      }

    case ImageBufferType::Int32: {
        if (channels == 1) {
          helpers::Pixelate<int32_t, 1>(roi, block_width, block_height);
        } else if (channels == 2) {
          helpers::Pixelate<int32_t, 2>(roi, block_width, block_height);
        } else if (channels == 3) {
          helpers::Pixelate<int32_t, 3>(roi, block_width, block_height);
        } else if (channels == 4) {
          helpers::Pixelate<int32_t, 4>(roi, block_width, block_height);
        } else {
          std::ostringstream s;
          s << "Pixelation is only supported for up to 4-channel "
               "images, but this `int32_t` ImageBuffer has "
            << channels << '!';
          throw std::logic_error(s.str());
        }
        return;
      }

    case ImageBufferType::Float: {
        if (channels == 1) {
          helpers::Pixelate<float, 1>(roi, block_width, block_height);
        } else if (channels == 2) {
          helpers::Pixelate<float, 2>(roi, block_width, block_height);
        } else if (channels == 3) {
          helpers::Pixelate<float, 3>(roi, block_width, block_height);
        } else if (channels == 4) {
          helpers::Pixelate<float, 4>(roi, block_width, block_height);
        } else {
          std::ostringstream s;
          s << "Pixelation is only supported for up to 4-channel "
               "images, but this `float` ImageBuffer has "
            << channels << '!';
          throw std::logic_error(s.str());
        }
        return;
      }

    case ImageBufferType::Double: {
        if (channels == 1) {
          helpers::Pixelate<double, 1>(roi, block_width, block_height);
        } else if (channels == 2) {
          helpers::Pixelate<double, 2>(roi, block_width, block_height);
        } else if (channels == 3) {
          helpers::Pixelate<double, 3>(roi, block_width, block_height);
        } else if (channels == 4) {
          helpers::Pixelate<double, 4>(roi, block_width, block_height);
        } else {
          std::ostringstream s;
          s << "Pixelation is only supported for up to 4-channel "
               "images, but this `float` ImageBuffer has "
            << channels << '!';
          throw std::logic_error(s.str());
        }
        return;
      }
  }

  std::string s("Type `");
  s += ImageBufferTypeToString(buffer_type);
  s += "` was not handled in `Pixelate` switch!";
  throw std::logic_error(s);
}


ImageBuffer ImageBuffer::Blend(const ImageBuffer &other, double alpha_other) const {
  if (!IsValid() || !other.IsValid()) {
    throw std::logic_error(
          "Cannot blend invalid ImageBuffers!");
  }

  switch(buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::Blend<uint8_t>(*this, other, alpha_other);

    case ImageBufferType::Int16:
      return helpers::Blend<int16_t>(*this, other, alpha_other);

    case ImageBufferType::Int32:
      return helpers::Blend<int32_t>(*this, other, alpha_other);

    case ImageBufferType::Float:
      return helpers::Blend<float>(*this, other, alpha_other);

    case ImageBufferType::Double:
      return helpers::Blend<double>(*this, other, alpha_other);
  }

  // Throw an exception as fallback, because due to the default
  // compiler settings, we would have ignored the warning about
  // missing case values.
  std::string s("Type `");
  s += ImageBufferTypeToString(buffer_type);
  s += "` was not handled in `Blend` switch!";
  throw std::logic_error(s);
}


ImageBuffer ImageBuffer::Channel(int channel) const {
  if ((channel < 0) || (channel >= channels)) {
    std::ostringstream s;
    s << "Cannot extract channel #" << channel
      << " from ImageBuffer with " << channels
      << " channels!";
    throw std::invalid_argument(s.str());
  }

  switch (buffer_type) {
    case ImageBufferType::UInt8:
      return helpers::ExtractChannel<uint8_t>(*this, channel);

    case ImageBufferType::Int16:
      return helpers::ExtractChannel<int16_t>(*this, channel);

    case ImageBufferType::Int32:
      return helpers::ExtractChannel<int32_t>(*this, channel);

    case ImageBufferType::Float:
      return helpers::ExtractChannel<float>(*this, channel);

    case ImageBufferType::Double:
      return helpers::ExtractChannel<double>(*this, channel);
  }

  std::string s("Type `");
  s += ImageBufferTypeToString(buffer_type);
  s += "` not handled in `Channel` switch!";
  throw std::logic_error(s);
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

    case ImageBufferType::Int32:
      helpers::MinMaxLocation<int32_t>(
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

  std::string s("Type `");
  s += ImageBufferTypeToString(buffer_type);
  s += "` was not handled in `MinMaxLocation` switch!";
  throw std::logic_error(s);
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
    std::string s("Could not load image from '");
    s += image_filename;
    s += "'!";
    throw std::runtime_error(s);
  }

  const int num_channels = (force_num_channels != STBI_default)
      ? force_num_channels : bytes_per_pixel;

  // First, let ImageBuffer reuse the buffer (no separate memory allocation)
  // Note regarding the pixel_stride: stbi_load will *always* set bytes_per_pixel
  // to the number it would have been if `force_num_channels` == 0.
  ImageBuffer buffer;
  buffer.CreateSharedBuffer(
        data, height, width, num_channels,
        width * num_channels, num_channels, ImageBufferType::UInt8);
  // Then, transfer ownership
  buffer.TakeOwnership();
  // Alternatively, we could:
  //     buffer.CreateCopy(data, width, height, num_channels,
  //                       width * num_channels);
  //     stbi_image_free(data);
  // But according to valgrind, the reuse option works as intended (i.e.
  // not leaking memory)
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
    std::string s("Saving ImageBuffer expected `uint8` buffer type, but got `");
    s += ImageBufferTypeToString(image.BufferType());
    s += "`!";
    throw std::logic_error(s);
  }

  const std::string fn_lower = werkzeugkiste::strings::Lower(image_filename);
  if (werkzeugkiste::strings::EndsWith(fn_lower, ".jpg")
      || werkzeugkiste::strings::EndsWith(fn_lower, ".jpeg")) {
    // stbi_write_jpg requires contiguous memory
    if (!image.IsContiguous()) {
      throw std::logic_error(
            "Cannot save JPEG because image memory is not contiguous!");
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
      throw std::invalid_argument("ImageBuffer can only be saved"
                  " as JPEG or PNG. File extension must be '.jpg',"
                  " '.jpeg' or '.png'.");
    }
  }

  if (stb_result == 0) {
    std::ostringstream s;
    s << "Could not save ImageBuffer to '" << image_filename
      << "' - failed with `stb` error code " << stb_result << '!';
    throw std::runtime_error(s.str());
  }
}

} // namespace viren2d
