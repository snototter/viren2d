#include <utility>
#include <sstream>
#include <iomanip>
#include <iostream> // TODO remove after spdlog switch

#include <math.h>

#include <cairo/cairo.h>

#include <vivi/drawing.hpp>
#include <vivi/colors.hpp>
#include <vivi/math.hpp>


//TODO remove opencv dependencies!
#include <opencv2/opencv.hpp>
//#include <opencv2/core/eigen.hpp>
#include <opencv2/highgui.hpp>

namespace vivi
{
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

  return true;
}


bool operator!=(const LineStyle &lhs, const LineStyle &rhs)
{
  return !(lhs == rhs);
}


/** @brief Sets the source color. */
void ApplyColor(cairo_t *context, const Color &color)
{
  cairo_set_source_rgba(context, color.red, color.green,
                        color.blue, color.alpha);
}


/** @brief Changes the given Cairo context to use the given LineStyle definitions. */
void ApplyLineStyle(cairo_t *context, const LineStyle &line_style)
{
  cairo_set_line_width(context, line_style.line_width);
  ApplyColor(context, line_style.color);

  //TODO if we add line caps:
  // * adjust context here
  // * adjust pybind11 parsing (tuple to LineStyle) in CreateLineStyle

  if (!line_style.dash_pattern.empty())
  {
    // https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-dash
    const double *dash = &line_style.dash_pattern[0];
    cairo_set_dash(context, dash,
                   static_cast<int>(line_style.dash_pattern.size()),
                   0); // We don't need an offset into the dash pattern
  }
}


//TODO remove opencv dependencies and conversion
cairo_surface_t *Mat2Cairo(const cv::Mat &mat)
{
  //TODO check stb usage: https://stackoverflow.com/a/68015791/400948
  assert(mat.type() == CV_8UC3 || mat.type() == CV_8UC4);
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mat.cols, mat.rows);
  if (mat.channels() == 3)
  {
    cv::Mat m4;
    cv::cvtColor(mat, m4, CV_RGB2RGBA);
    memcpy(cairo_image_surface_get_data(surface),
           m4.data, 4*m4.cols*m4.rows); //TODO only valid if iscontiguous!
  }
  else
  {
    memcpy(cairo_image_surface_get_data(surface),
           mat.data, 4*mat.cols*mat.rows); //TODO only if iscontiguous!
  }
  // Mark the surface dirty after directly manipulating the memory!
  cairo_surface_mark_dirty(surface);
  return surface;
}

cv::Mat Cairo2Mat(cairo_surface_t *surface)
{
  cv::Mat from_cairo(cairo_image_surface_get_height(surface),
                     cairo_image_surface_get_width(surface),
                     CV_8UC4);
  memcpy(from_cairo.data, cairo_image_surface_get_data(surface), 4*cairo_image_surface_get_width(surface) * cairo_image_surface_get_height(surface));
  return from_cairo;
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

  ImagePainter(const ImagePainter& other) // copy constructor
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

  ImagePainter(ImagePainter&& other) noexcept // move constructor
    : surface_(std::exchange(other.surface_, nullptr)),
      context_(std::exchange(other.context_, nullptr))
  {
    std::cout << "Inside ImagePainter::MoveConstructor()" << std::endl; //TODO remove - was curious about pybind11 object creation/move/copy
//    surface_ = other.surface_;
//    context_ = other.context_;
//    other.surface_ = nullptr;
//    other.context_ = nullptr;
  }

  ImagePainter& operator=(const ImagePainter& other) // Copy assignment
  {
    std::cout << "Inside ImagePainter::Copy Assignment" << std::endl;//TODO remove
    return *this = ImagePainter(other);
  }

  ImagePainter& operator=(ImagePainter&& other) noexcept // Move assignment
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

  void SetCanvas(int width, int height, const Color& color) override;

  void SetCanvas(const cv::Mat &image) override //TODO replace by buffer (maybe stb?)
  {
    assert(image.type() == CV_8UC3 || image.type() == CV_8UC4);
    //TODO if surface_ is set, reuse memory
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
    surface_ = Mat2Cairo(image);
    context_ = cairo_create(surface_);
  }

  cv::Mat GetCanvas() override
  {
    if (surface_)
      return Cairo2Mat(surface_);
    else
      return cv::Mat();
  }

  void DummyShow() override
  {
    cv::Mat img = Cairo2Mat(surface_);
    cv::imshow("Canvas", img);
    cv::waitKey();
    cv::destroyWindow("Canvas");
  }

  void DrawLine(const Vec2d& from, const Vec2d& to,
                const LineStyle &line_style) override;

protected:
  void DrawCircleImpl(const Vec2d& center, double radius,
                      const LineStyle &line_style,
                      const Color &fill) override;

  void DrawRectImpl(const Rect &rect, const LineStyle &line_style,
                    const Color &fill) override;

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
  cairo_set_source_rgba(context_, color.red, color.green, color.blue, color.alpha);
  cairo_paint(context_);
  cairo_restore(context_);
}

void ImagePainter::DrawLine(const Vec2d &from, const Vec2d &to,
                            const LineStyle &line_style)
{
  if (!surface_)
    throw std::runtime_error("Invalid cairo surface - did you forget to SetCanvas() first?");

  cairo_save(context_);
  ApplyLineStyle(context_, line_style);
  // Draw line
  cairo_move_to(context_, from.x(), from.y());
  cairo_line_to(context_, to.x(), to.y());
  cairo_stroke(context_);
  // Restore context
  cairo_restore(context_);
}

void ImagePainter::DrawCircleImpl(const Vec2d &center, double radius,
                                  const LineStyle &line_style, const Color &fill)
{
  if (!surface_)
    throw std::runtime_error("Invalid cairo surface - did you forget to SetCanvas() first?");

  cairo_save(context_);
  cairo_arc(context_, center.x(), center.y(), radius, 0, 2 * M_PI);
  if (fill.alpha > 0.0)
  {
    ApplyColor(context_, fill);
    cairo_fill_preserve(context_);
  }

  ApplyLineStyle(context_, line_style);
  cairo_stroke(context_);
  // Restore context
  cairo_restore(context_);
}



/** Creates a path for a rectangle with rounded corners.
 *  Assumes that the viewport is already translated (and optionally
 *  rotated).
 */
void PathHelperRoundedRect(cairo_t *context, const Rect &rect)
{
  if (rect.radius > std::min(rect.half_height(), rect.half_width()))
  {
    std::stringstream s;
    s << "Invalid rounded rect: radius " << std::fixed << std::setprecision(2)
      << " must be less than half the smaller dimension (i.e. "
      << std::min(rect.half_height(), rect.half_width()) << ")!";
    throw std::runtime_error(s.str());
  }

  const double half_width = rect.half_width() - rect.radius;
  const double half_height = rect.half_height() - rect.radius;
  cairo_move_to(context, -rect.half_width(), -half_height);
  cairo_arc(context, -half_width, -half_height, rect.radius, deg2rad(180), deg2rad(270));
  cairo_arc(context,  half_width, -half_height, rect.radius, deg2rad(-90), 0);
  cairo_arc(context,  half_width,  half_height, rect.radius, 0, deg2rad(90));
  cairo_arc(context, -half_width,  half_height, rect.radius, deg2rad(90), deg2rad(180));
  cairo_close_path(context);
}

void ImagePainter::DrawRectImpl(const Rect &rect, const LineStyle &line_style,
                                const Color &fill)
{
  if (!surface_)
    throw std::runtime_error("Invalid cairo surface - did you forget to SetCanvas() first?");

  cairo_save(context_);
  cairo_translate(context_, rect.cx, rect.cy);
  cairo_rotate(context_, deg2rad(rect.angle));

  // Draw a standard (box) rect or rounded rectangle:
  if (rect.radius > 0.0)
    PathHelperRoundedRect(context_, rect);
  else
    cairo_rectangle(context_, -rect.half_width(), -rect.half_height(),
                    rect.width, rect.height);
//    PathHelperStandardRect(context_, rect);

  if (fill.alpha > 0.0)
  {
    ApplyColor(context_, fill);
    cairo_fill_preserve(context_);
  }

  ApplyLineStyle(context_, line_style);
  cairo_stroke(context_);
  // Restore context
  cairo_restore(context_);
}


std::unique_ptr<Painter> CreateImagePainter()
{
  return std::unique_ptr<Painter>(new ImagePainter());
}

} // namespace vivi
