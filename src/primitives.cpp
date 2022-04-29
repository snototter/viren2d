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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <viren2d/primitives.h>
#include <viren2d/math.h>
#include <viren2d/string_utils.h>


namespace viren2d {
//---------------------------------------------------- Image buffer

ImageBuffer::ImageBuffer(int w, int h, int ch) {
  const int num_bytes = w * h * ch;
  data = static_cast<unsigned char*>(std::malloc(num_bytes));
  width = w;
  height = h;
  channels = ch;
  stride = w * channels;
  owns_data_ = true;
}

ImageBuffer::~ImageBuffer() {
  Cleanup();
}

ImageBuffer::ImageBuffer(const ImageBuffer &other) {
  std::cout << "Inside ImageBuffer Copy Constructor" << std::endl; // TODO remove
  owns_data_ = other.owns_data_;
  if (other.owns_data_) {
    const int num_bytes = other.height * other.stride;
    data = static_cast<unsigned char*>(std::malloc(num_bytes));
    if (!data) {
      std::stringstream s;
      s << "Cannot allocate " << num_bytes << " bytes to copy ImageBuffer!";
      throw std::runtime_error(s.str());
    }
    std::memcpy(data, other.data, num_bytes);
    std::cout << "ImageBuffer allocated " << num_bytes << " bytes" << std::endl; // TODO remove
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
  std::cout << "Inside ImageBuffer Move Constructor" << std::endl; // TODO remove
  // Reset "other" but ensure that the memory won't be freed:
  other.owns_data_ = false;
  other.Cleanup();
}


ImageBuffer &ImageBuffer::operator=(const ImageBuffer &other) {
  std::cout << "Inside ImageBuffer Copy Assignment" << std::endl; // TODO remove
  return *this = ImageBuffer(other);
}


ImageBuffer &ImageBuffer::operator=(ImageBuffer &&other) noexcept {
  std::cout << "Inside ImageBuffer Move Assignment" << std::endl; // TODO remove
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
  // Clean up first (if this instance already holds image data)
  Cleanup();

  const int num_bytes = height * stride;
  data = static_cast<unsigned char*>(std::malloc(num_bytes));
  if (!data) {
    std::stringstream s;
    s << "Cannot allocate " << num_bytes << " bytes to copy ImageBuffer!";
    throw std::runtime_error(s.str());
  }
  owns_data_ = true;

  std::memcpy(data, buffer, num_bytes);
  this->width = width;
  this->height = height;
  this->channels = channels;
  this->stride = stride;

  std::cout << "ImageBuffer copied " << num_bytes << " bytes (CreateCopy)" << std::endl; // TODO remove
}

void ImageBuffer::RGB2BGR() {
  if (!data)
    return;

  if (channels != 4 && channels != 3) {
    std::stringstream s;
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

ImageBuffer ImageBuffer::ToRGBA() const
{
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


std::string ImageBuffer::ToString() const {
  std::stringstream s;
  s << "ImageBuffer(" << width << "x" << height << "x" << channels;
  if (owns_data_)
    s << ", copied memory)";
  else
    s << ", shared memory)";
  return s.str();
}


void ImageBuffer::Cleanup() {
  if (data && owns_data_) {
    std::cout << "ImageBuffer freeing " << width*stride << " bytes" << std::endl; //TODO remove
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
  int width, height, bytes_per_pixel;
  unsigned char *data = stbi_load(image_filename.c_str(),
                                  &width, &height,
                                  &bytes_per_pixel,
                                  force_num_channels);
  if (!data) {
    std::stringstream s;
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
  int stb_result = 0; // stb return code 0 indicates failure

  const std::string fn_lower = strings::Lower(image_filename);
  if (strings::EndsWith(fn_lower, ".jpg")
      || strings::EndsWith(fn_lower, ".jpeg")) {
    // stbi_write_jpg requires contiguous memory
    if (image.stride != image.channels * image.width) {
      std::stringstream s;
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
    if (strings::EndsWith(fn_lower, ".png")) {
      stb_result = stbi_write_png(image_filename.c_str(),
                                  image.width, image.height,
                                  image.channels, image.data,
                                  image.stride);
    } else {
      throw std::invalid_argument("ImageBuffer can only be saved as JPEG or PNG. File extension must be '.jpg', '.jpeg' or '.png'.");
    }
  }

  if (stb_result == 0) {
    std::stringstream s;
    s << "Could not save ImageBuffer to '" << image_filename << "' - unknown error!";
    throw std::runtime_error(s.str());
  }
}


ImageBuffer ConversionHelperGray(const ImageBuffer &src,
                                 int channels_out) {
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


//---------------------------------------------------- Templated vector class
template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec() {
  for (int i = 0; i < dim; ++i)
    val[i] = static_cast<_Tp>(0);
}

template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec(_Tp x, _Tp y) {
  if (dim != 2) {
    std::stringstream s;
    s << "You cannot initialize " << TypeName() << " with 2 values.";
    throw std::invalid_argument(s.str());
  }
  val[0] = x;
  val[1] = y;
}

template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec(_Tp x, _Tp y, _Tp z) {
  if (dim != 3) {
    std::stringstream s;
    s << "You cannot initialize " << TypeName() << " with 3 values.";
    throw std::invalid_argument(s.str());
  }
  val[0] = x;
  val[1] = y;
  val[2] = z;
}


template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec(std::initializer_list<_Tp> values) {
  if (static_cast<size_t>(dim) != values.size()) {
    std::stringstream s;
    s << "You cannot initialize " << TypeName()
      << " with " << values.size() << " values";
    throw std::invalid_argument(s.str());
  }

  for (size_t i = 0; i < values.size(); ++i)
    val[i] = values.begin()[i];
}


template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec(_Tp x, _Tp y, _Tp z, _Tp w) {
  if (dim != 4) {
    std::stringstream s;
    s << "You cannot initialize " << TypeName() << " with 4 values.";
    throw std::invalid_argument(s.str());
  }
  val[0] = x;
  val[1] = y;
  val[2] = z;
  val[3] = w;
}


template<typename _Tp, int dim>
Vec<_Tp, dim>::Vec(const Vec<_Tp, dim>& other) {
  for (int i = 0; i < dim; ++i)
    val[i] = other.val[i];
}

template<typename _Tp, int dim>
Vec<_Tp, dim>::operator Vec<double, dim>() const {
  Vec<double, dim> conv;
  for (int i = 0; i < dim; ++i)
    conv.val[i] = static_cast<double>(val[i]);
  return conv;
}


template<typename _Tp, int dim>
const _Tp& Vec<_Tp, dim>::operator[](int i) const {
  if (i >= dim) {
    std::stringstream s;
    s << "Index-out-of-bounds: cannot access ["
      << i << "] for " << TypeName() << ".";
    throw std::out_of_range(s.str());
  }
  return val[i];
}


template<typename _Tp, int dim>
_Tp& Vec<_Tp, dim>::operator[](int i) {
  if (i >= dim) {
    std::stringstream s;
    s << "Index-out-of-bounds: cannot access ["
      << i << "] for " << TypeName() << ".";
    throw std::out_of_range(s.str());
  }
  return val[i];
}


template<typename _Tp, int dim>
const _Tp& Vec<_Tp, dim>::x() const {
  return (*this)[0];
}


template<typename _Tp, int dim>
const _Tp& Vec<_Tp, dim>::y() const {
  return (*this)[1];
}


template<typename _Tp, int dim>
const _Tp& Vec<_Tp, dim>::z() const {
  return (*this)[2];
}


template<typename _Tp, int dim>
const _Tp& Vec<_Tp, dim>::w() const {
  return (*this)[3];
}


template<typename _Tp, int dim>
_Tp& Vec<_Tp, dim>::x() {
  return (*this)[0];
}


template<typename _Tp, int dim>
_Tp& Vec<_Tp, dim>::y() {
  return (*this)[1];
}


template<typename _Tp, int dim>
_Tp& Vec<_Tp, dim>::z() {
  return (*this)[2];
}


template<typename _Tp, int dim>
_Tp& Vec<_Tp, dim>::w() {
  return (*this)[3];
}


template<typename _Tp, int dim>
void Vec<_Tp, dim>::SetX(_Tp x) {
  (*this)[0] = x;
}


template<typename _Tp, int dim>
void Vec<_Tp, dim>::SetY(_Tp y) {
  (*this)[1] = y;
}


template<typename _Tp, int dim>
void Vec<_Tp, dim>::SetZ(_Tp z) {
  (*this)[2] = z;
}


template<typename _Tp, int dim>
void Vec<_Tp, dim>::SetW(_Tp w) {
  (*this)[3] = w;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> &Vec<_Tp, dim>::operator+=(const Vec<_Tp, dim>& rhs) {
  for (int i = 0; i < dim; ++i)
    val[i] += rhs[i];
  return *this;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> &Vec<_Tp, dim>::operator-=(const Vec<_Tp, dim>& rhs) {
  for (int i = 0; i < dim; ++i)
    val[i] -= rhs[i];
  return *this;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> &Vec<_Tp, dim>::operator*=(double scale) {
  for (int i = 0; i < dim; ++i)
    val[i] *= scale;
  return *this;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> &Vec<_Tp, dim>::operator/=(double scale) {
  for (int i = 0; i < dim; ++i)
    val[i] /= scale;
  return *this;
}


template<typename _Tp, int dim>
_Tp Vec<_Tp, dim>::Dot(const Vec<_Tp, dim>& other) {
  _Tp s = static_cast<_Tp>(0);
  for (int i = 0; i < dim; ++i)
    s += val[i] * other.val[i];
  return s;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> Vec<_Tp, dim>::Cross(const Vec<_Tp, dim>& other) {
  if (dim != 3)
    throw std::logic_error("Cross product is only defined for 3-dim vectors!");
  // There's actually an analog for 2d space, but I didn't need
  // it yet: https://mathworld.wolfram.com/CrossProduct.html

  return Vec<_Tp, dim>(val[1] * other.val[2] - val[2] * other.val[1],
                       val[2] * other.val[0] - val[0] * other.val[2],
                       val[0] * other.val[1] - val[1] * other.val[0]);
}


template<typename _Tp, int dim>
double Vec<_Tp, dim>::Length() const {
  double squared = 0.0;
  for (int i = 0; i < dim; ++i)
    squared += static_cast<double>(val[i] * val[i]);
  return std::sqrt(squared);
}


template<typename _Tp, int dim>
double Vec<_Tp, dim>::Distance(const Vec<_Tp, dim>& other) {
  auto diff = *this - other;
  return diff.Length();
}


// Typename to char lookup:
template<typename _Tp> char VecType();
template<> char VecType<unsigned char>() { return 'b'; }
template<> char VecType<short>()         { return 's'; }
template<> char VecType<int>()           { return 'i'; }
template<> char VecType<double>()        { return 'd'; }


template<typename _Tp, int dim>
std::string Vec<_Tp, dim>::TypeName() {
  std::stringstream s;
  s << "Vec" << dim << VecType<_Tp>();
  return s.str();
}


template<typename _Tp, int dim>
std::string Vec<_Tp, dim>::ToString() const {
  std::stringstream s;
  s << Vec<_Tp, dim>::TypeName() << "("
    << std::fixed << std::setprecision(2);

  for (int i = 0; i < dim; ++i) {
    s << val[i];
    if (i < dim -1)
      s << ", ";
  }

  s << ")";
  return s.str();
}

//---------------------------------------------------- Vector operators
template<typename _Tp, int dim>
bool operator==(const Vec<_Tp, dim>& lhs, const Vec<_Tp, dim>& rhs) {
  for (int i = 0; i < dim; ++i) {
    if (!eps_equal(lhs.val[i], rhs.val[i])) {
      return false;
    }
  }
  return true;
}


template<typename _Tp, int dim>
bool operator!=(const Vec<_Tp, dim>& lhs, const Vec<_Tp, dim>& rhs) {
  return !(lhs == rhs);
}


template<typename _Tp, int dim>
Vec<_Tp, dim> operator+(Vec<_Tp, dim> lhs, const Vec<_Tp, dim>& rhs) {
  lhs += rhs;
  return lhs;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> operator-(Vec<_Tp, dim> lhs, const Vec<_Tp, dim>& rhs) {
  lhs -= rhs;
  return lhs;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> operator*(Vec<_Tp, dim> lhs, double scale) {
  lhs *= scale;
  return lhs;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> operator*(double scale, Vec<_Tp, dim> rhs) {
  rhs *= scale;
  return rhs;
}


template<typename _Tp, int dim>
Vec<_Tp, dim> operator/(Vec<_Tp, dim> lhs, double scale) {
  lhs /= scale;
  return lhs;
}


//---------------------------------------------------- Explicit vector instantiation
// Explicit instantiation:
template class Vec<double, 2>;
template class Vec<double, 3>;
template class Vec<double, 4>;

template class Vec<int, 2>;
template class Vec<int, 3>;

// Comparison Vec2d
template bool operator==(const Vec2d& lhs, const Vec2d& rhs);
template bool operator!=(const Vec2d& lhs, const Vec2d& rhs);
// Arithmetic Vec2d
template Vec2d operator+(Vec2d lhs, const Vec2d& rhs);
template Vec2d operator-(Vec2d lhs, const Vec2d& rhs);
template Vec2d operator*(Vec2d lhs, double scale);
template Vec2d operator*(double scale, Vec2d rhs);
template Vec2d operator/(Vec2d lhs, double scale);


// Comparison Vec3d
template bool operator==(const Vec3d& lhs, const Vec3d& rhs);
template bool operator!=(const Vec3d& lhs, const Vec3d& rhs);
// Arithmetic Vec3d
template Vec3d operator+(Vec3d lhs, const Vec3d& rhs);
template Vec3d operator-(Vec3d lhs, const Vec3d& rhs);
template Vec3d operator*(Vec3d lhs, double scale);
template Vec3d operator*(double scale, Vec3d rhs);
template Vec3d operator/(Vec3d lhs, double scale);


// Comparison Vec4d
template bool operator==(const Vec4d& lhs, const Vec4d& rhs);
template bool operator!=(const Vec4d& lhs, const Vec4d& rhs);
// Arithmetic Vec4d
template Vec4d operator+(Vec4d lhs, const Vec4d& rhs);
template Vec4d operator-(Vec4d lhs, const Vec4d& rhs);
template Vec4d operator*(Vec4d lhs, double scale);
template Vec4d operator*(double scale, Vec4d rhs);
template Vec4d operator/(Vec4d lhs, double scale);


// Comparison Vec2i
template bool operator==(const Vec2i& lhs, const Vec2i& rhs);
template bool operator!=(const Vec2i& lhs, const Vec2i& rhs);
// Arithmetic Vec2i
template Vec2i operator+(Vec2i lhs, const Vec2i& rhs);
template Vec2i operator-(Vec2i lhs, const Vec2i& rhs);
template Vec2i operator*(Vec2i lhs, double scale);
template Vec2i operator*(double scale, Vec2i rhs);
template Vec2i operator/(Vec2i lhs, double scale);


// Comparison Vec3i
template bool operator==(const Vec3i& lhs, const Vec3i& rhs);
template bool operator!=(const Vec3i& lhs, const Vec3i& rhs);
// Arithmetic Vec3i
template Vec3i operator+(Vec3i lhs, const Vec3i& rhs);
template Vec3i operator-(Vec3i lhs, const Vec3i& rhs);
template Vec3i operator*(Vec3i lhs, double scale);
template Vec3i operator*(double scale, Vec3i rhs);
template Vec3i operator/(Vec3i lhs, double scale);

//---------------------------------------------------- Rectangle

Rect::Rect(std::initializer_list<double> values) {
  if (values.size() < 4 || values.size() > 6) {
    std::stringstream s;
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
    angle = val[4];

  if (values.size() > 5)
    radius = val[5];
}


std::string Rect::ToString() const {
  std::stringstream s;
  s << "Rect(" << std::fixed << std::setprecision(1)
    << cx << ", " << cy << ", " << width << ", " << height
    << "; rot=" << angle << "°, radius=" << radius << ")";
  return s.str();
}

bool operator==(const Rect& lhs, const Rect& rhs) {
  return eps_equal(lhs.cx, rhs.cx) && eps_equal(lhs.cy, rhs.cy)
      && eps_equal(lhs.width, rhs.width) && eps_equal(lhs.height, rhs.height)
      && eps_equal(lhs.angle, rhs.angle) && eps_equal(lhs.radius, rhs.radius);
}

bool operator!=(const Rect& lhs, const Rect& rhs) {
  return !(lhs == rhs);
}

} // namespace viren2d
