#include <utility>
#include <sstream>
#include <iomanip>
#include <iostream> // TODO remove after switching to spdlog
#include <stdexcept>
#include <cassert>
#include <cstring> // memcpy

#include <math.h>

#include <cairo/cairo.h>

// public viren2d headers
#include <viren2d/drawing.hpp>
#include <viren2d/colors.hpp>
#include <viren2d/math.hpp>
#include <viren2d/string_utils.hpp>

// private viren2d headers
#include "drawing_helpers.hpp"

////TODO remove opencv dependencies!
//#include <opencv2/opencv.hpp>
////#include <opencv2/core/eigen.hpp>
//#include <opencv2/highgui.hpp>

namespace viren2d
{
////TODO remove opencv dependencies and conversion
//@deprecated
//cairo_surface_t *Mat2Cairo(const cv::Mat &mat)
//{
//  //TODO check stb usage: https://stackoverflow.com/a/68015791/400948
//  assert(mat.type() == CV_8UC3 || mat.type() == CV_8UC4);
//  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mat.cols, mat.rows);
//  if (mat.channels() == 3)
//  {
//    cv::Mat m4;
//#if (CV_VERSION_MAJOR >= 4)
//    cv::cvtColor(mat, m4, cv::COLOR_RGB2RGBA);
//#else
//    cv::cvtColor(mat, m4, CV_RGB2RGBA);
//#endif
//    std::memcpy(cairo_image_surface_get_data(surface),
//                m4.data, 4*m4.cols*m4.rows); //TODO only valid if iscontiguous!
//  }
//  else
//  {
//    std::memcpy(cairo_image_surface_get_data(surface),
//                mat.data, 4*mat.cols*mat.rows); //TODO only if iscontiguous!
//  }
//  // Mark the surface dirty after directly manipulating the memory!
//  cairo_surface_mark_dirty(surface);
//  return surface;
//}

//@deprecated
//cv::Mat Cairo2Mat(cairo_surface_t *surface)
//{
//  cv::Mat from_cairo(cairo_image_surface_get_height(surface),
//                     cairo_image_surface_get_width(surface),
//                     CV_8UC4);
//  std::memcpy(from_cairo.data, cairo_image_surface_get_data(surface), 4*cairo_image_surface_get_width(surface) * cairo_image_surface_get_height(surface));
//  return from_cairo;
//}


std::string LineStyle::ToString() const
{
  std::stringstream s;
  s << "LineStyle(w=" << std::fixed << std::setprecision(1)
    << line_width << ", " << color.ToString() << ", "
    << (dash_pattern.empty() ? "solid" : "dashed") << ")";
  return s.str();
}


bool operator==(const LineStyle &lhs, const LineStyle &rhs)
{
  if (!eps_equal(lhs.line_width, rhs.line_width))
    return false;

  if (lhs.color != rhs.color)
    return false;

  if (lhs.dash_pattern.size() != rhs.dash_pattern.size())
    return false;

  for (size_t i = 0; i < lhs.dash_pattern.size(); ++i)
    if (!eps_equal(lhs.dash_pattern[i], rhs.dash_pattern[i]))
      return false;

  if (lhs.line_cap != rhs.line_cap)
    return false;

  if (lhs.line_join != rhs.line_join)
    return false;

  return true;
}


bool operator!=(const LineStyle &lhs, const LineStyle &rhs)
{
  return !(lhs == rhs);
}



class ImagePainter : public Painter
{
public:
  ImagePainter() : surface_(nullptr), context_(nullptr)
  {
    std::cout << "Inside ImagePainter::Constructor()" << std::endl; //TODO remove
  }

  virtual ~ImagePainter()
  {
    std::cout << "Inside ImagePainter::Destructor()" << std::endl; //TODO remove
    if (context_)
      cairo_destroy(context_);
    if (surface_)
      cairo_surface_destroy(surface_);
  }

  ImagePainter(const ImagePainter &other) // copy constructor
    : surface_(nullptr), context_(nullptr)
  {
    std::cout << "Inside ImagePainter::CopyConstructor()" << std::endl;//TODO remove
    if (other.surface_)
    {
      cairo_format_t format = cairo_image_surface_get_format(other.surface_);
      assert(format == CAIRO_FORMAT_ARGB32 || format == CAIRO_FORMAT_RGB24);
      const int width = cairo_image_surface_get_width(other.surface_);
      const int height = cairo_image_surface_get_height(other.surface_);

      surface_ = cairo_image_surface_create(cairo_image_surface_get_format(other.surface_),
                                            width, height);
      memcpy(cairo_image_surface_get_data(surface_),
             cairo_image_surface_get_data(other.surface_),
             width*height*4);

      context_ = cairo_create(surface_); // TODO currently, we create a new context - the state is lost!
      //TODO no big deal - copy c'tor is never called when used via the factory (as intended)
    }
  }

  ImagePainter(ImagePainter &&other) noexcept // move constructor
    : surface_(std::exchange(other.surface_, nullptr)),
      context_(std::exchange(other.context_, nullptr))
  {
    std::cout << "Inside ImagePainter::MoveConstructor()" << std::endl; //TODO remove - was curious about pybind11 object creation/move/copy
//    surface_ = other.surface_;
//    context_ = other.context_;
//    other.surface_ = nullptr;
//    other.context_ = nullptr;
  }

  ImagePainter& operator=(const ImagePainter &other) // Copy assignment
  {
    std::cout << "Inside ImagePainter::Copy Assignment" << std::endl;//TODO remove
    return *this = ImagePainter(other);
  }

  ImagePainter& operator=(ImagePainter &&other) noexcept // Move assignment
  {
    std::cout << "Inside ImagePainter::Move Assignment" << std::endl;//TODO remove
    std::swap(surface_, other.surface_);
    std::swap(context_, other.context_);
    return *this;
  }

  bool Empty() const override
  {
    return !surface_;
  }

  void SetCanvas(int width, int height, const Color &color) override;

  void SetCanvas(const std::string &image_filename) override;

  void SetCanvas(const ImageBuffer &image_buffer) override;

//  void SetCanvas(const cv::Mat &image) override //TODO replace by buffer (maybe stb?)
//  {
//    assert(image.type() == CV_8UC3 || image.type() == CV_8UC4);
//    //TODO if surface_ is set, reuse memory
//    if (context_)
//    {
//      cairo_destroy(context_);
//      context_ = nullptr;
//    }
//    if (surface_)
//    {
//      cairo_surface_destroy(surface_);
//      surface_ = nullptr;
//    }
//    surface_ = Mat2Cairo(image);
//    context_ = cairo_create(surface_);
//  }

  ImageBuffer GetCanvas(bool copy) override;

  void DrawLine(const Vec2d &from, const Vec2d &to,
                const LineStyle &line_style) override
  {
    helpers::DrawLine(surface_, context_, from, to, line_style);
  }



protected:
  void DrawArcImpl(const Vec2d &center, double radius,
                             double angle1, double angle2,
                             const LineStyle &line_style,
                             const Color &fill) override
  {
    helpers::DrawArc(surface_, context_, center, radius,
                     angle1, angle2, line_style, fill);
  }


  void DrawCircleImpl(const Vec2d &center, double radius,
                      const LineStyle &line_style,
                      const Color &fill) override
  {
    helpers::DrawCircle(surface_, context_, center, radius, line_style, fill);
  }


  void DrawRectImpl(const Rect &rect, const LineStyle &line_style,
                    const Color &fill) override
  {
    helpers::DrawRect(surface_, context_, rect, line_style, fill);
  }


private:
  cairo_surface_t *surface_;
  cairo_t *context_;
};


void ImagePainter::SetCanvas(int width, int height, const Color &color)
{
  // Check if we can reuse the current image surface to
  // save ourselves the memory allocation:
  if (surface_)
  {
    int prev_width = cairo_image_surface_get_width(surface_);
    int prev_height = cairo_image_surface_get_height(surface_);
    if (prev_width != width || prev_height != height)
    {
      if (context_)
      {
        cairo_destroy(context_);
        context_ = nullptr;
      }
      cairo_surface_destroy(surface_);
      surface_ = nullptr;
    }
  }

  // If we couldn't reuse the surface (or we didn't have one
  // to start with), we have to create the canvas:
  if (!surface_)
    surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  if (!context_)
    context_ = cairo_create(surface_);

  // Now simply fill the canvas with the given color:
  cairo_save(context_);
  helpers::ApplyColor(context_, color);
  cairo_paint(context_);
  cairo_restore(context_);
}


void ImagePainter::SetCanvas(const std::string &image_filename)
{
  // Force to load 4 bytes per pixel (STBI_rgb_alpha), so we
  // can easily plug/copy it into the Cairo surface
  ImageBuffer buffer = LoadImage(image_filename, 4);
  SetCanvas(buffer);
}

void ImagePainter::SetCanvas(const ImageBuffer &image_buffer)
{
  if (image_buffer.channels != 4)
  {
    SetCanvas(image_buffer.ToRGBA());
  }
  else
  {
    // TODO Avoid premature optimization:
    // Currently, we clean up previously created contexts/surfaces to
    // avoid unnecessarily cluttering the implementation.
    // Then, we copy the given ImageBuffer.
    //
    // If this becomes a bottleneck, we need to provide a "bool copy" flag and
    // distinguish 4 scenarios:
    // * copy-flag true, existing data --> check if it surface can be reused (memcpy)
    // * copy-flag true, no surface --> malloc(surface create) + memcpy
    // * copy-flag false, existing data --> clean up data, reuse surface
    // * copy-flag false, no surface --> surface create_for_data
    if (context_)
    {
      cairo_destroy(context_);
      context_ = nullptr;
    }
    if (surface_)
    {
      cairo_surface_destroy(surface_);
      surface_ = nullptr;
    }

    surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                          image_buffer.width, image_buffer.height);
    std::memcpy(cairo_image_surface_get_data(surface_), image_buffer.data,
                4 * image_buffer.width * image_buffer.height);
  //  {
  //    surface_ = cairo_image_surface_create_for_data(image_buffer.data,
  //                                                   CAIRO_FORMAT_ARGB32,
  //                                                   image_buffer.width, image_buffer.height,
  //                                                   image_buffer.stride);
  //  }
    context_ = cairo_create(surface_);
    cairo_surface_mark_dirty(surface_);
  }
}


ImageBuffer ImagePainter::GetCanvas(bool copy)
{
  if (!surface_)
    throw std::logic_error("Invalid cairo surface - did you forget to SetCanvas() first?");

  assert(cairo_image_surface_get_format(surface_) == CAIRO_FORMAT_ARGB32);
  const int channels = 4;

  unsigned char *data = cairo_image_surface_get_data(surface_);
  const int width = cairo_image_surface_get_width(surface_);
  const int height = cairo_image_surface_get_height(surface_);
  const int stride = cairo_image_surface_get_stride(surface_);

  ImageBuffer buffer;
  if (copy)
    buffer.CreateCopy(data, width, height, channels, stride);
  else
    buffer.CreateSharedBuffer(data, width, height, channels, stride);
  return buffer;
}



std::unique_ptr<Painter> CreateImagePainter()
{
  return std::unique_ptr<Painter>(new ImagePainter());
}

} // namespace viren2d
