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

#include <viren2d/primitives.h>

#include <helpers/logging.h>


namespace wgu = werkzeugkiste::geometry;

namespace viren2d {
//---------------------------------------------------- Image buffer
ImageBuffer::ImageBuffer()
  : data(nullptr), width(0), height(0), channels(0), stride(0),
  owns_data_(false) {
  SPDLOG_DEBUG("ImageBuffer default constructor.");
}


ImageBuffer::ImageBuffer(int w, int h, int ch) {
  SPDLOG_DEBUG("ImageBuffer constructor allocating"
               " memory for {:d}x{:d}x{:d} image.",
               w, h, ch);
  const int num_bytes = w * h * ch;
  data = static_cast<unsigned char*>(std::malloc(num_bytes));
  width = w;
  height = h;
  channels = ch;
  stride = w * channels;
  owns_data_ = true;
}

ImageBuffer::~ImageBuffer() {
  SPDLOG_DEBUG("ImageBuffer destructor.");
  Cleanup();
}

ImageBuffer::ImageBuffer(const ImageBuffer &other) {
  SPDLOG_DEBUG("ImageBuffer copy constructor.");
  owns_data_ = other.owns_data_;
  if (other.owns_data_) {
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
  stride = other.stride;
}


ImageBuffer::ImageBuffer(ImageBuffer &&other) noexcept
  : data(other.data), width(other.width), height(other.height),
    channels(other.channels), stride(other.stride),
    owns_data_(other.owns_data_) {
  SPDLOG_DEBUG("ImageBuffer move constructor.");
  // Reset "other" but ensure that the memory won't be freed:
  other.owns_data_ = false;
  other.Cleanup();
}


ImageBuffer &ImageBuffer::operator=(const ImageBuffer &other) {
  SPDLOG_DEBUG("ImageBuffer copy assignment operator.");
  return *this = ImageBuffer(other);
}


ImageBuffer &ImageBuffer::operator=(ImageBuffer &&other) noexcept {
  SPDLOG_DEBUG("ImageBuffer move assignment operator.");
  std::swap(data, other.data);
  std::swap(owns_data_, other.owns_data_);
  std::swap(width, other.width);
  std::swap(height, other.height);
  std::swap(channels, other.channels);
  std::swap(stride, other.stride);
  return *this;
}


void ImageBuffer::CreateSharedBuffer(unsigned char *buffer, int width, int height,
                                     int channels, int stride) {
  SPDLOG_DEBUG("ImageBuffer::CreateSharedBuffer(w={:d}, h={:d},"
               " ch={:d}, stride={:d}).", width, height, channels, stride);
  // Clean up first (if this instance already holds image data)
  Cleanup();

  owns_data_ = false;
  data = buffer;
  this->width = width;
  this->height = height;
  this->channels = channels;
  this->stride = stride;
}


void ImageBuffer::CreateCopy(unsigned char const *buffer, int width, int height,
                             int channels, int stride) {
  SPDLOG_DEBUG("ImageBuffer::CreateCopy(w={:d}, h={:d},"
               " ch={:d}, stride={:d}).", width, height, channels, stride);
  // Clean up first (if this instance already holds image data)
  Cleanup();

  const int num_bytes = height * stride;
  data = static_cast<unsigned char*>(std::malloc(num_bytes));
  if (!data) {
    std::ostringstream s;
    s << "Cannot allocate " << num_bytes << " bytes to copy ImageBuffer!";
    throw std::runtime_error(s.str());
  }
  owns_data_ = true;

  std::memcpy(data, buffer, num_bytes);
  this->width = width;
  this->height = height;
  this->channels = channels;
  this->stride = stride;
}

void ImageBuffer::RGB2BGR() {
  SPDLOG_DEBUG("ImageBuffer::RGB2BGR changing"
               " layer order in-place.");

  if (!data)
    return;

  if (channels != 4 && channels != 3) {
    std::ostringstream s;
    s << "Cannot flip red & blue channel of an image with " << channels << " channels";
    throw std::logic_error(s.str());
  }
  // We iterate over the image buffer similar to the
  // efficient OpenCV matrix scan:
  // https://docs.opencv.org/2.4/doc/tutorials/core/how_to_scan_images/how_to_scan_images.html#the-efficient-way
  int rows = height;
  int cols = width * channels;
  if (stride == cols) { // Is memory contiguous?
    cols *= rows;
    rows = 1;
  }

  unsigned char *ptr_row;
  for (int row = 0; row < rows; ++row) {
    ptr_row = data + row * stride;
    for (int col = 0; col < cols; col+=channels) {
      // Swap red (at col) and blue (at col+2)
      unsigned char tmp = ptr_row[col];
      ptr_row[col]      = ptr_row[col+2];
      ptr_row[col + 2]  = tmp;
    }
  }
}

ImageBuffer ImageBuffer::ToRGB() const {
  SPDLOG_DEBUG("ImageBuffer::ToRGB().");

  if (channels != 1 && channels != 3 && channels != 4)
    throw std::logic_error("ImageBuffer must have 1, 3, or 4 channels to be convertible to RGB!");

  if (channels == 1) {
    return Gray2RGB(*this);
  } else {
    if (channels == 3) {
      return ImageBuffer(*this);
    } else {
      return RGBA2RGB(*this);
    }
  }
}

ImageBuffer ImageBuffer::ToRGBA() const {
  SPDLOG_DEBUG("ImageBuffer::ToRGBA().");

  if (channels != 1 && channels != 3 && channels != 4)
    throw std::logic_error("ImageBuffer must have 1, 3, or 4 channels to be convertible to RGBA!");

  if (channels == 1) {
    return Gray2RGBA(*this);
  } else {
    if (channels == 3) {
      return RGB2RGBA(*this);
    } else {
      return ImageBuffer(*this);
    }
  }
}


bool ImageBuffer::IsValid() const {
  return data != nullptr;
}


std::string ImageBuffer::ToString() const {
  if (!IsValid()) {
    return "ImageBuffer::Invalid";
  }

  std::ostringstream s;
  s << "ImageBuffer(" << width << "x" << height
    << "x" << channels;

  if (owns_data_)
    s << ", copied memory";
  else
    s << ", shared memory";

  s << ")";

  return s.str();
}


void ImageBuffer::Cleanup() {
  SPDLOG_TRACE("ImageBuffer::Cleanup().");
  if (data && owns_data_) {
    SPDLOG_TRACE("ImageBuffer freeing {:d}x{:d}x{:d}={:d} bytes.",
                 width, height, channels, width * height * channels);
    std::free(data);
  }
  data = nullptr;
  owns_data_ = false;
  width = 0;
  height = 0;
  channels = 0;
  stride = 0;
}


ImageBuffer LoadImage(const std::string &image_filename,
                      int force_num_channels) {
  SPDLOG_DEBUG("ImageBuffer::LoadImage(\"{:s}\", force_num_channels={:d}).",
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
  buffer.CreateSharedBuffer(data, width, height, num_channels,
                            width * num_channels);
  // Then, transfer ownership
  buffer.owns_data_ = true;
  // Alternatively, we could:
  //     buffer.CreateCopy(data, width, height, num_channels,
  //                       width * num_channels);
  //     stbi_image_free(data);
  // But according to valgrind - the reuse option works as intended (i.e.
  // not leaking memory)
  return buffer;
}


void SaveImage(const std::string &image_filename,
               const ImageBuffer &image) {
  SPDLOG_DEBUG("ImageBuffer::SaveImage(\"{:s}\", {:s}).",
               image_filename, image);

  int stb_result = 0; // stb return code 0 indicates failure

  const std::string fn_lower = werkzeugkiste::strings::Lower(image_filename);
  if (werkzeugkiste::strings::EndsWith(fn_lower, ".jpg")
      || werkzeugkiste::strings::EndsWith(fn_lower, ".jpeg")) {
    // stbi_write_jpg requires contiguous memory
    if (image.stride != image.channels * image.width) {
      std::ostringstream s;
      s << "Cannot save JPEG because image memory is not contiguous. Expected "
        << image.channels * image.width << " bytes per row, but image buffer has "
        << image.stride << "!";
      throw std::runtime_error(s.str());
    }
    // Default JPEG quality setting: 90%
    stb_result = stbi_write_jpg(image_filename.c_str(),
                                image.width, image.height,
                                image.channels, image.data,
                                90);
  } else {
    if (werkzeugkiste::strings::EndsWith(fn_lower, ".png")) {
      stb_result = stbi_write_png(image_filename.c_str(),
                                  image.width, image.height,
                                  image.channels, image.data,
                                  image.stride);
    } else {
      throw std::invalid_argument("ImageBuffer can only be saved"
                  " as JPEG or PNG. File extension must be '.jpg',"
                  " '.jpeg' or '.png'.");
    }
  }

  if (stb_result == 0) {
    std::ostringstream s;
    s << "Could not save ImageBuffer to '" << image_filename
      << "' - unknown error!";
    throw std::runtime_error(s.str());
  }
}


ImageBuffer ConversionHelperGray(const ImageBuffer &src,
                                 int channels_out) {
  SPDLOG_DEBUG("ImageBuffer converting grayscale to {:d} channels.",
               channels_out);

  if (src.channels != 1)
    throw std::invalid_argument("Input image must be grayscale!");

  if (!src.data)
    throw std::invalid_argument("Invalid input image (nullptr)!");

  if (channels_out != 3 && channels_out != 4)
    throw std::invalid_argument("Number of output channels must be 3 or 4!");

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.width, src.height, channels_out);

  int rows = src.height;
  int cols = src.width; // src channels is 1
  if (src.stride == cols) { // Is memory contiguous?
    cols *= rows;
    rows = 1;
  }

  unsigned char *src_row;
  unsigned char *dst_row;
  for (int row = 0; row < rows; ++row) {
    src_row = src.data + row * src.stride;
    dst_row = dst.data + row * dst.stride;
    for (int src_col = 0, dst_col = 0;
         src_col < cols;
         src_col+=src.channels, dst_col+=dst.channels) {
      dst_row[dst_col] = src_row[src_col];
      dst_row[dst_col + 1] = src_row[src_col];
      dst_row[dst_col + 2] = src_row[src_col];
      if (channels_out == 4)
        dst_row[dst_col + 3] = 255;
    }
  }

  return dst;
}

ImageBuffer Gray2RGB(const ImageBuffer &img) {
  return ConversionHelperGray(img, 3);
}

ImageBuffer Gray2RGBA(const ImageBuffer &img) {
  return ConversionHelperGray(img, 4);
}



ImageBuffer ConversionHelperRGB(const ImageBuffer &src,
                                int channels_out) {
  SPDLOG_DEBUG("ImageBuffer converting RGB(A) to {:d} channels.",
               channels_out);

  if (src.channels != 3 && src.channels != 4)
    throw std::invalid_argument("Input image must be RGB or RGBA!");

  if (!src.data)
    throw std::invalid_argument("Invalid input image (nullptr)!");

  if (channels_out != 3 && channels_out != 4)
    throw std::invalid_argument("Number of output channels must be 3 or 4!");

  // Create destination buffer (will have contiguous memory)
  ImageBuffer dst(src.width, src.height, channels_out);

  int rows = src.height;
  int cols = src.width * src.channels;
  if (src.stride == cols) { // Is memory contiguous?
    cols *= rows;
    rows = 1;
  }

  unsigned char *src_row;
  unsigned char *dst_row;
  for (int row = 0; row < rows; ++row) {
    src_row = src.data + row * src.stride;
    dst_row = dst.data + row * dst.stride;
    for (int src_col = 0, dst_col = 0;
         src_col < cols;
         src_col+=src.channels, dst_col+=dst.channels) {
      // Copy RGB
      dst_row[dst_col]     = src_row[src_col];
      dst_row[dst_col + 1] = src_row[src_col + 1];
      dst_row[dst_col + 2] = src_row[src_col + 2];
      // Two cases:
      // * RGBA --> RGB, we're already done
      // * RGB  --> RGBA, we must add the alpha channel
      if (channels_out == 4)
        dst_row[dst_col + 3] = 255;
    }
  }

  return dst;
}

ImageBuffer RGBA2RGB(const ImageBuffer &img) {
  return ConversionHelperRGB(img, 3);
}

ImageBuffer RGB2RGBA(const ImageBuffer &img) {
  return ConversionHelperRGB(img, 4);
}

//---------------------------------------------------- Ellipse

Ellipse::Ellipse(std::initializer_list<double> values) {
  if (values.size() < 4 || values.size() > 7) {
    std::ostringstream s;
    s << "Ellipse c'tor requires 4 to 7 entries in initializer_list, "
      << "but got " << values.size() << ".";
    throw std::invalid_argument(s.str());
  }

  const auto val = values.begin();
  cx = val[0];
  cy = val[1];
  major_axis = val[2];
  minor_axis = val[3];

  if (values.size() > 4)
    rotation = val[4];

  if (values.size() > 5)
    angle_from = val[5];

  if (values.size() > 6)
    angle_to = val[6];

  include_center = true;
}


Ellipse &Ellipse::operator+=(double offset) {
  cx += offset;
  cy += offset;
  return *this;
}


Ellipse &Ellipse::operator-=(double offset) {
  cx -= offset;
  cy -= offset;
  return *this;
}


Ellipse &Ellipse::operator+=(const Vec2d &offset) {
  cx += offset.x();
  cy += offset.y();
  return *this;
}


Ellipse &Ellipse::operator-=(const Vec2d &offset) {
  cx -= offset.x();
  cy -= offset.y();
  return *this;
}


bool Ellipse::IsValid() const {
  return (major_axis > 0.0) && (minor_axis > 0.0)
      && (major_axis >= minor_axis)
      && !wgu::eps_equal(angle_from, angle_to);
}


std::string Ellipse::ToString() const {
  std::ostringstream s;
  s << "Ellipse(cx=" << std::fixed << std::setprecision(1)
    << cx << ", cy=" << cy << ", mj=" << major_axis
    << ", mn=" << minor_axis << "; rot=" << rotation
    << ", a1=" << angle_from << "°, a2=" << angle_to << "°";

  if (!wgu::eps_zero(angle_from) || !wgu::eps_equal(angle_to, 360.0)) {
    s << ", " << (include_center ? "w/" : "w/o") << " center";
  }

  if (!IsValid())
    s << ", invalid";

  s << ")";
  return s.str();
}


bool operator==(const Ellipse& lhs, const Ellipse& rhs) {
  return wgu::eps_equal(lhs.cx, rhs.cx)
      && wgu::eps_equal(lhs.cy, rhs.cy)
      && wgu::eps_equal(lhs.major_axis, rhs.major_axis)
      && wgu::eps_equal(lhs.minor_axis, rhs.minor_axis)
      && wgu::eps_equal(lhs.rotation, rhs.rotation)
      && wgu::eps_equal(lhs.angle_from, rhs.angle_from)
      && wgu::eps_equal(lhs.angle_to, rhs.angle_to)
      && (lhs.include_center == rhs.include_center);
}


bool operator!=(const Ellipse& lhs, const Ellipse& rhs) {
  return !(lhs == rhs);
}


//---------------------------------------------------- Rectangle

Rect::Rect(std::initializer_list<double> values) {
  if (values.size() < 4 || values.size() > 6) {
    std::ostringstream s;
    s << "Rect c'tor requires 4 to 6 entries in initializer_list, "
      << "but got " << values.size() << ".";
    throw std::invalid_argument(s.str());
  }

  const auto val = values.begin();
  cx = val[0];
  cy = val[1];
  width = val[2];
  height = val[3];

  if (values.size() > 4)
    rotation = val[4];

  if (values.size() > 5)
    radius = val[5];
}


Rect &Rect::operator+=(double offset) {
  cx += offset;
  cy += offset;
  return *this;
}


Rect &Rect::operator-=(double offset) {
  cx -= offset;
  cy -= offset;
  return *this;
}


Rect &Rect::operator+=(const Vec2d &offset) {
  cx += offset.x();
  cy += offset.y();
  return *this;
}


Rect &Rect::operator-=(const Vec2d &offset) {
  cx -= offset.x();
  cy -= offset.y();
  return *this;
}

double Rect::left() const {
  return cx - half_width();
}


double Rect::right() const {
  return cx + half_width();
}


double Rect::top() const {
  return cy - half_height();
}


double Rect::bottom() const {
  return cy + half_height();
}


//Vec2d Rect::TopLeft() const {
//  return Vec2d(left(), top());
//}


//Vec2d Rect::TopRight() const {
//  return Vec2d(right(), top());
//}


//Vec2d Rect::BottomLeft() const {
//  return Vec2d(left(), bottom());
//}


//Vec2d Rect::BottomRight() const {
//  return Vec2d(right(), bottom());
//}


bool Rect::IsValid() const {
  if ((radius > 0.5) && (radius < 1.0)) {
    return false;
  }
  return (width > 0.0) && (height > 0.0)
      && (radius <= std::min(half_height(), half_width()))
      && (radius >= 0.0);
}


std::string Rect::ToString() const {
  std::ostringstream s;
  s << "Rect(cx=" << std::fixed << std::setprecision(1)
    << cx << ", cy=" << cy << ", w=" << width << ", h=" << height
    << "; rot=" << rotation << "°, radius=" << radius;

  if (!IsValid())
    s << ", invalid";

  s << ")";
  return s.str();
}


Rect Rect::FromLTWH(double left, double top, double width, double height,
                    double rot, double corner_radius) {
  return Rect(left + width / 2.0, top + height / 2.0,
              width, height, rot, corner_radius);
}


bool operator==(const Rect& lhs, const Rect& rhs) {
  return wgu::eps_equal(lhs.cx, rhs.cx)
      && wgu::eps_equal(lhs.cy, rhs.cy)
      && wgu::eps_equal(lhs.width, rhs.width)
      && wgu::eps_equal(lhs.height, rhs.height)
      && wgu::eps_equal(lhs.rotation, rhs.rotation)
      && wgu::eps_equal(lhs.radius, rhs.radius);
}

bool operator!=(const Rect& lhs, const Rect& rhs) {
  return !(lhs == rhs);
}

} // namespace viren2d
