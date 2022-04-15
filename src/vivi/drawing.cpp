#include <utility>
#include <iostream> // TODO remove after spdlog switch

#include <cairo/cairo.h>

#include <vivi/drawing.hpp>
#include <vivi/colors.hpp>


//TODO remove opencv dependencies!
#include <opencv2/opencv.hpp>
//#include <opencv2/core/eigen.hpp>
#include <opencv2/highgui.hpp>

namespace vivi
{
//TODO remove opencv dependencies and conversion
cairo_surface_t *Mat2Cairo(const cv::Mat &mat)
{
  assert(mat.type() == CV_8UC3 || mat.type() == CV_8UC4);
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, mat.cols, mat.rows);
  if (mat.channels() == 3)
  {
    cv::Mat m4;
    cv::cvtColor(mat, m4, CV_RGB2RGBA);
//    std::cout << "Is cont? " << m4.isContinuous() << " step: " << m4.step << std::endl;
    memcpy(cairo_image_surface_get_data(surface),
           m4.data, 4*m4.cols*m4.rows); //TODO only valid if iscontiguous!
  }
  else
  {
//    std::cout << "Is cont? " << mat.isContinuous() << " step: " << mat.step << std::endl;
    memcpy(cairo_image_surface_get_data(surface),
           mat.data, 4*mat.cols*mat.rows); //TODO only if iscontiguous!
  }
  return surface;
//  cairo_image_surface_create_for_data()
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
  }

protected:
  //TODO make linestyle class - encapsulate width, color, dash, (can be extended with cap, etc)
  void DrawLineImpl(const Vec2d& from, const Vec2d& to,
                    double line_width, const Color& color,
                    const std::vector<double> &dash_style) override;

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

void ImagePainter::DrawLineImpl(const Vec2d &from, const Vec2d &to,
                                double line_width, const Color &color,
                                const std::vector<double> &dash_style)
{
  if (!surface_)
    throw std::runtime_error("Invalid cairo surface - did you forget to SetCanvas() first?");

  cairo_save(context_);
  // Set line style
  cairo_set_line_width(context_, line_width);
  cairo_set_source_rgba(context_, color.red, color.green, color.blue, color.alpha);
  if (!dash_style.empty())
  {
    const double *dash = &dash_style[0];
    cairo_set_dash(context_, dash,
                   static_cast<int>(dash_style.size()), 0);
  }
  // Draw line
  cairo_move_to(context_, from[0], from[1]);
  cairo_line_to(context_, to[0], to[1]);
  cairo_stroke(context_);
  // Restore context
  cairo_restore(context_);
}


std::unique_ptr<Painter> CreateImagePainter()
{
  return std::unique_ptr<Painter>(new ImagePainter());
}

} // namespace vivi
