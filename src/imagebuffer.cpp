#include <sstream>
#include <iomanip>
#include <type_traits>
#include <stdexcept>
#include <cstdlib>
#include <cassert>
#include <cstring> // memcpy
#include <algorithm> // std::swap
#include <iostream> // TODO remove

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#ifdef __GNUC__
// GCC reports a lot of missing field initializers, which is known,
// not easy to fix & not a real problem: https://github.com/nothings/stb/issues/1099
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif  // __GNUC__
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

#include <werkzeugkiste/strings/strings.h>
#include <werkzeugkiste/geometry/utils.h>

#include <viren2d/imagebuffer.h>

#include <helpers/logging.h>


namespace wgu = werkzeugkiste::geometry;

namespace viren2d {
//---------------------------------------------------- Image buffer
std::string ImageBufferTypeToString(ImageBufferType t) {
  switch (t) {
    case ImageBufferType::UInt8:
      return "uint8";

    case ImageBufferType::Int32:
      return "int32";

    case ImageBufferType::Float:
      return "float";

    case ImageBufferType::Double:
      return "double";
  }

  std::ostringstream s;
  s << "ImageBufferType `" << static_cast<int>(t)
    << "` not handled in `ImageBufferTypeToString` switch!";
  throw std::logic_error(s.str());
}

int ItemSizeFromImageBufferType(ImageBufferType t) {
  switch (t) {
    case ImageBufferType::UInt8:
      return static_cast<int>(sizeof(uint8_t));

    case ImageBufferType::Int32:
      return static_cast<int>(sizeof(int32_t));

    case ImageBufferType::Float:
      return static_cast<int>(sizeof(float));

    case ImageBufferType::Double:
      return static_cast<int>(sizeof(double));
  }

  std::string s("ImageBufferType `");
  s += ImageBufferTypeToString(t);
  s += "` not handled in `FormatDescriptor` switch!";
  throw std::logic_error(s);
}


ImageBuffer::ImageBuffer()
  : data(nullptr), width(0), height(0), channels(0),
    item_size(0), stride(0), owns_data(false) {
  SPDLOG_DEBUG("ImageBuffer default constructor.");
}


ImageBuffer::ImageBuffer(int w, int h, int ch, ImageBufferType buf_type) {
  SPDLOG_DEBUG(
        "ImageBuffer constructor allocating memory for a "
        "{:d}x{:d}x{:d} image, type: {:s}.",
        w, h, ch, ImageBufferTypeToString(buf_type));
  width = w;
  height = h;
  channels = ch;
  item_size = ItemSizeFromImageBufferType(buf_type);
  const int num_bytes = width * height * channels * item_size;
  data = static_cast<unsigned char*>(std::malloc(num_bytes));
  stride = width * channels * item_size;
  owns_data = true;
}

ImageBuffer::~ImageBuffer() {
  SPDLOG_DEBUG("ImageBuffer destructor.");
  Cleanup();
}

ImageBuffer::ImageBuffer(const ImageBuffer &other) {
  SPDLOG_DEBUG("ImageBuffer copy constructor.");
  owns_data = other.owns_data;
  if (other.owns_data) {
    SPDLOG_TRACE("Copying other's image buffer memory.");
    const int num_bytes = other.height * other.stride;
    data = static_cast<unsigned char*>(std::malloc(num_bytes));
    if (!data) {
      std::ostringstream s;
      s << "Cannot allocate " << num_bytes << " bytes to copy ImageBuffer!";
      throw std::runtime_error(s.str());
    }
    std::memcpy(data, other.data, num_bytes);
  } else {
    data = other.data;
  }

  width = other.width;
  height = other.height;
  channels = other.channels;
  item_size = other.item_size;
  stride = other.stride;
}


ImageBuffer::ImageBuffer(ImageBuffer &&other) noexcept
  : data(other.data), width(other.width), height(other.height),
    channels(other.channels), item_size(other.item_size),
    stride(other.stride), owns_data(other.owns_data) {
  SPDLOG_DEBUG("ImageBuffer move constructor.");
  // Reset "other" but ensure that the memory won't be freed:
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
  std::swap(owns_data, other.owns_data);
  std::swap(width, other.width);
  std::swap(height, other.height);
  std::swap(channels, other.channels);
  std::swap(item_size, other.item_size);
  std::swap(stride, other.stride);
  return *this;
}


void ImageBuffer::CreateSharedBuffer(unsigned char *buffer, int width, int height,
    int channels, int stride, ImageBufferType buffer_type) {
  SPDLOG_DEBUG("ImageBuffer::CreateSharedBuffer(w={:d}, h={:d},"
               " ch={:d}, stride={:d}, {:s}).",
               width, height, channels, item_size, stride,
               ImageBufferTypeToString(buffer_type));
  // Clean up first (if this instance already holds image data)
  Cleanup();

  owns_data = false;
  data = buffer;
  this->width = width;
  this->height = height;
  this->channels = channels;
  this->stride = stride;
  this->buffer_type = buffer_type;
  this->item_size = ItemSizeFromImageBufferType(buffer_type);
}


void ImageBuffer::CreateCopy(unsigned char const *buffer, int width, int height,
    int channels, int stride, ImageBufferType buffer_type) {
  SPDLOG_DEBUG(
        "ImageBuffer::CreateCopy(w={:d}, h={:d},"
        " ch={:d}, stride={:d}, {:s}).",
        width, height, channels, item_size, stride,
        ImageBufferTypeToString(buffer_type));
  // Clean up first (if this instance already holds image data)
  Cleanup();

  const int num_bytes = height * stride;
  data = static_cast<unsigned char*>(std::malloc(num_bytes));
  if (!data) {
    std::ostringstream s;
    s << "Cannot allocate " << num_bytes << " bytes to copy ImageBuffer!";
    throw std::runtime_error(s.str());
  }
  owns_data = true;

  std::memcpy(data, buffer, num_bytes);
  this->width = width;
  this->height = height;
  this->channels = channels;
  this->stride = stride;
  this->buffer_type = buffer_type;
  this->item_size = ItemSizeFromImageBufferType(buffer_type);
}


ImageBuffer ImageBuffer::CreateCopy() const {
  ImageBuffer cp;
  cp.CreateCopy(data, width, height, channels, stride, buffer_type);
  return cp;
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

  if (!data || (ch1 == ch2)) {
    return;
  }

  // We iterate over the image buffer, similar to the
  // efficient OpenCV matrix scan:
  // https://docs.opencv.org/2.4/doc/tutorials/core/how_to_scan_images/how_to_scan_images.html#the-efficient-way
  int rows = height;
  int cols = width * channels;
  // If the memory is contiguous, we can speed up the
  // following loop.
  if (stride == cols) {
    cols *= rows;
    rows = 1;
  }

  unsigned char *ptr_row;
  for (int row = 0; row < rows; ++row) {
    ptr_row = data + row * stride;
    for (int col = 0; col < cols; col+=channels) {
      // Swap red (at col) and blue (at col+2)
      unsigned char tmp = ptr_row[col + ch1];
      ptr_row[col + ch1] = ptr_row[col + ch2];
      ptr_row[col + ch2] = tmp;
    }
  }
}


void ImageBuffer::TakeOwnership() {
  owns_data = true;
}


ImageBuffer ImageBuffer::ToChannels(int output_channels) const {
  SPDLOG_DEBUG(
        "ImageBuffer::ToChannels converting {:d} to {:d} channels.",
        channels, output_channels);

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
      return CreateCopy();
    } else if (output_channels == 3) {
      return Gray2RGB(*this);
    } else if (output_channels == 4) {
      return Gray2RGBA(*this);
    } else {
      std::ostringstream s;
      s << "Conversion from single-channel ImageBuffer to "
        << output_channels << " output channels is not supported!";
      throw std::invalid_argument(s.str());
    }
  } else if (channels == 3) {
    // RGB-to-something
    if (output_channels == 3) {
      return CreateCopy();
    } else if (output_channels == 4) {
      return RGB2RGBA(*this);
    } else {
      std::ostringstream s;
      s << "Conversion from 3-channel ImageBuffer to "
        << output_channels << " output channel(s) is not supported!";
      throw std::invalid_argument(s.str());
    }
  } else {
    // RGBA-to-something
    if (output_channels == 3) {
      return RGBA2RGB(*this);
    } else if (output_channels == 4) {
      return CreateCopy();
    } else {
      std::ostringstream s;
      s << "Conversion from 4-channel ImageBuffer to "
        << output_channels << " output channel(s) is not supported!";
      throw std::invalid_argument(s.str());
    }
  }
}


bool ImageBuffer::IsValid() const {
  return data != nullptr;
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
          width, height, channels, width * height * channels, item_size);
    std::free(data);
  }
  data = nullptr;
  owns_data = false;
  width = 0;
  height = 0;
  channels = 0;
  item_size = 0;
  stride = 0;
}


ImageBuffer LoadImage(
    const std::string &image_filename,
    int force_num_channels) {
  SPDLOG_DEBUG(
        "ImageBuffer::LoadImage(\"{:s}\", force_num_channels={:d}).",
        image_filename, force_num_channels);

  int width, height, bytes_per_pixel;
  unsigned char *data = stbi_load(image_filename.c_str(),
                                  &width, &height,
                                  &bytes_per_pixel,
                                  force_num_channels);
  if (!data) {
    std::ostringstream s;
    s << "Could not load image from '" << image_filename << "'!";
    throw std::runtime_error(s.str());
  }
  const int num_channels = (force_num_channels != STBI_default)
      ? force_num_channels : bytes_per_pixel;


  // First, let ImageBuffer reuse the buffer (no separate memory allocation)
  ImageBuffer buffer;
  buffer.CreateSharedBuffer(
        data, width, height, num_channels,
        width * num_channels, ImageBufferType::UInt8);
  // Then, transfer ownership
  buffer.TakeOwnership();
  // Alternatively, we could:
  //     buffer.CreateCopy(data, width, height, num_channels,
  //                       width * num_channels);
  //     stbi_image_free(data);
  // But according to valgrind - the reuse option works as intended (i.e.
  // not leaking memory)
  return buffer;
}


void SaveImage(
    const std::string &image_filename,
    const ImageBuffer &image) {
  //TODO check with float/double dtype!
  SPDLOG_DEBUG(
        "ImageBuffer::SaveImage(\"{:s}\", {:s}).",
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
            image.Channels(), image.ImmutableData(), image.Stride());
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


template<typename _Tp>
ImageBuffer ConversionHelperGray(
    const ImageBuffer &src, int channels_out) {
  SPDLOG_DEBUG(
        "ImageBuffer converting grayscale to {:d} channels.",
        channels_out);

  if (src.Channels() != 1) {
    throw std::invalid_argument("Input image must be grayscale!");
  }

  if (!src.ImmutableData()) {
    throw std::invalid_argument("Invalid input image (nullptr)!");
  }

  if (channels_out != 3 && channels_out != 4) {
    throw std::invalid_argument("Number of output channels must be 3 or 4!");
  }

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.Width(), src.Height(), channels_out, src.BufferType());

  int rows = src.Height();
  int cols = src.Width(); // src channels is 1
  // dst was freshly allocated, so it's guaranteed to be contiguous
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  _Tp const *src_row;
  _Tp *dst_row;
  for (int row = 0; row < rows; ++row) {
    src_row = src.ImmutableRowPtr<_Tp>(row);
    dst_row = dst.MutableRowPtr<_Tp>(row);
    for (int src_col = 0, dst_col = 0;
         src_col < cols;
         src_col+=src.Channels(), dst_col+=dst.Channels()) {
      dst_row[dst_col] = src_row[src_col];
      dst_row[dst_col + 1] = src_row[src_col];
      dst_row[dst_col + 2] = src_row[src_col];
      if (channels_out == 4) {
        dst_row[dst_col + 3] = 255;
      }
    }
  }
//  unsigned char *src_row;
//  unsigned char *dst_row;
//  for (int row = 0; row < rows; ++row) {
//    src_row = src.data + row * src.stride;
//    dst_row = dst.data + row * dst.stride;
//    for (int src_col = 0, dst_col = 0;
//         src_col < cols;
//         src_col+=src.channels, dst_col+=dst.channels) {
//      dst_row[dst_col] = src_row[src_col];
//      dst_row[dst_col + 1] = src_row[src_col];
//      dst_row[dst_col + 2] = src_row[src_col];
//      if (channels_out == 4)
//        dst_row[dst_col + 3] = 255;
//    }
//  }

  return dst;
}


/// Selects the corresponding templated ConversionHelper
inline ImageBuffer Gray2RGBx(const ImageBuffer &img, int num_channels_out) {
  switch(img.BufferType()) {
    case ImageBufferType::UInt8:
      return ConversionHelperGray<uint8_t>(img, num_channels_out);

    case ImageBufferType::Int32:
      return ConversionHelperGray<int32_t>(img, num_channels_out);

    case ImageBufferType::Float:
      return ConversionHelperGray<float>(img, num_channels_out);

    case ImageBufferType::Double:
      return ConversionHelperGray<double>(img, num_channels_out);
  }
  // Throw an exception as fallback, because due to the default
  // compiler settings, we would have ignored the warning about
  // missing case values.
  std::string s("Grayscale to RGB(A) conversion is not supported for type `");
  s += ImageBufferTypeToString(img.BufferType());
  s += "`!";
  throw std::logic_error(s);
}


ImageBuffer Gray2RGB(const ImageBuffer &img) {
  return Gray2RGBx(img, 3);
}

ImageBuffer Gray2RGBA(const ImageBuffer &img) {
  return Gray2RGBx(img, 4);
}


template <typename _Tp>
ImageBuffer ConversionHelperRGB(
    const ImageBuffer &src, int channels_out) {
  SPDLOG_DEBUG(
        "ImageBuffer converting RGB(A) to {:d} channels.",
        channels_out);

  if (src.Channels() != 3 && src.Channels() != 4) {
    throw std::invalid_argument("Input image must be RGB or RGBA!");
  }

  if (!src.ImmutableData()) {
    throw std::invalid_argument("Invalid input image (nullptr)!");
  }

  if (channels_out != 3 && channels_out != 4) {
    throw std::invalid_argument("Number of output channels must be 3 or 4!");
  }

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.Width(), src.Height(), channels_out, src.BufferType());

//  //FIXME itemsize! (make templated version)
//  int rows = src.height;
//  int cols = src.width * src.channels;
//  if (src.stride == cols) { // Is memory contiguous?
//    cols *= rows;
//    rows = 1;
//  }

//  unsigned char *src_row;
//  unsigned char *dst_row;
//  for (int row = 0; row < rows; ++row) {
//    src_row = src.data + row * src.stride;
//    dst_row = dst.data + row * dst.stride;
//    for (int src_col = 0, dst_col = 0;
//         src_col < cols;
//         src_col+=src.channels, dst_col+=dst.channels) {
//      // Copy RGB
//      dst_row[dst_col]     = src_row[src_col];
//      dst_row[dst_col + 1] = src_row[src_col + 1];
//      dst_row[dst_col + 2] = src_row[src_col + 2];
//      // Two cases:
//      // * RGBA --> RGB, we're already done
//      // * RGB  --> RGBA, we must add the alpha channel
//      if (channels_out == 4)
//        dst_row[dst_col + 3] = 255;
//    }
//  }
  int rows = src.Height();
  int cols = src.Width(); // src channels is 1
  // dst was freshly allocated, so it's guaranteed to be contiguous
  if (src.IsContiguous()) {
    cols *= rows;
    rows = 1;
  }

  _Tp const *src_row;
  _Tp *dst_row;
  for (int row = 0; row < rows; ++row) {
    src_row = src.ImmutableRowPtr<_Tp>(row);
    dst_row = dst.MutableRowPtr<_Tp>(row);
    for (int src_col = 0, dst_col = 0;
         src_col < cols;
         src_col+=src.Channels(), dst_col+=dst.Channels()) {
        // Copy RGB
        dst_row[dst_col]     = src_row[src_col];
        dst_row[dst_col + 1] = src_row[src_col + 1];
        dst_row[dst_col + 2] = src_row[src_col + 2];
        // Two cases:
        // * RGBA --> RGB, we're already done
        // * RGB  --> RGBA, we must add the alpha channel
        if (channels_out == 4) {
          dst_row[dst_col + 3] = 255;
        }
    }
  }

  return dst;
}


/// Selects the corresponding templated ConversionHelper
inline ImageBuffer RGBx2RGBx(const ImageBuffer &img, int num_channels_out) {
  switch(img.BufferType()) {
    case ImageBufferType::UInt8:
      return ConversionHelperRGB<uint8_t>(img, num_channels_out);

    case ImageBufferType::Int32:
      return ConversionHelperRGB<int32_t>(img, num_channels_out);

    case ImageBufferType::Float:
      return ConversionHelperRGB<float>(img, num_channels_out);

    case ImageBufferType::Double:
      return ConversionHelperRGB<double>(img, num_channels_out);
  }
  // Throw an exception as fallback, because due to the default
  // compiler settings, we would have ignored the warning about
  // missing case values.
  std::string s("Grayscale to RGB(A) conversion is not supported for type `");
  s += ImageBufferTypeToString(img.BufferType());
  s += "`!";
  throw std::logic_error(s);
}


ImageBuffer RGBA2RGB(const ImageBuffer &img) {
  return RGBx2RGBx(img, 3);
}


ImageBuffer RGB2RGBA(const ImageBuffer &img) {
  return RGBx2RGBx(img, 4);
}
} // namespace viren2d
