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
#include <viren2d/drawing.h>
#include <viren2d/colors.h>
#include <viren2d/math.h>
#include <viren2d/string_utils.h>

// private viren2d headers
#include <helpers/drawing_helpers.h>

namespace viren2d {
/** Implements the Painter interface using a Cairo image surface. */
class ImagePainter : public Painter {
public:
  ImagePainter() : Painter(),
    surface_(nullptr), context_(nullptr) {
    std::cout << "Inside ImagePainter::Constructor()" << std::endl; //TODO remove
  }

  virtual ~ImagePainter() {
    std::cout << "Inside ImagePainter::Destructor()" << std::endl; //TODO remove
    if (context_)
      cairo_destroy(context_);
    if (surface_)
      cairo_surface_destroy(surface_);
  }

  ImagePainter(const ImagePainter &other) // copy constructor
    : Painter(), surface_(nullptr), context_(nullptr) {
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
    : Painter(),
      surface_(std::exchange(other.surface_, nullptr)),
      context_(std::exchange(other.context_, nullptr)) {
    std::cout << "Inside ImagePainter::MoveConstructor()" << std::endl; //TODO remove - was curious about pybind11 object creation/move/copy
//    surface_ = other.surface_;
//    context_ = other.context_;
//    other.surface_ = nullptr;
//    other.context_ = nullptr;
  }

  ImagePainter& operator=(const ImagePainter &other) { // Copy assignment
    std::cout << "Inside ImagePainter::Copy Assignment" << std::endl;//TODO remove
    return *this = ImagePainter(other);
  }

  ImagePainter& operator=(ImagePainter &&other) noexcept { // Move assignment
    std::cout << "Inside ImagePainter::Move Assignment" << std::endl;//TODO remove
    std::swap(surface_, other.surface_);
    std::swap(context_, other.context_);
    return *this;
  }

  bool IsValid() const override {
    return (surface_ != nullptr) && (context_ != nullptr);
  }

  void SetCanvas(int width, int height, const Color &color) override;

  void SetCanvas(const std::string &image_filename) override;

  void SetCanvas(const ImageBuffer &image_buffer) override;

  Vec2i GetCanvasSize() const override;

  ImageBuffer GetCanvas(bool copy) const override;


  void DrawArrow(const Vec2d &from, const Vec2d &to,
                 const ArrowStyle &arrow_style) override {
    helpers::DrawArrow(surface_, context_, from, to, arrow_style);
  }

  void DrawGrid(const Vec2d &top_left, const Vec2d &bottom_right,
                          double spacing_x, double spacing_y,
                const LineStyle &line_style) override {
    helpers::DrawGrid(surface_, context_, top_left, bottom_right,
                      spacing_x, spacing_y, line_style);
  }


  void DrawLine(const Vec2d &from, const Vec2d &to,
                const LineStyle &line_style) override {
    helpers::DrawLine(surface_, context_, from, to, line_style);
  }



protected:
  void DrawArcImpl(const Vec2d &center, double radius,
                             double angle1, double angle2,
                             const LineStyle &line_style,
                             const Color &fill) override {
    helpers::DrawArc(surface_, context_, center, radius,
                     angle1, angle2, line_style, fill);
  }


  void DrawCircleImpl(const Vec2d &center, double radius,
                      const LineStyle &line_style,
                      const Color &fill) override {
    helpers::DrawCircle(surface_, context_, center, radius, line_style, fill);
  }


  void DrawRectImpl(const Rect &rect, const LineStyle &line_style,
                    const Color &fill) override {
    helpers::DrawRect(surface_, context_, rect, line_style, fill);
  }


private:
  cairo_surface_t *surface_;
  cairo_t *context_;
};


void ImagePainter::SetCanvas(int width, int height,
                             const Color &color) {
  // Check if we can reuse the current image surface to
  // save ourselves the memory allocation:
  if (surface_) {
    int prev_width = cairo_image_surface_get_width(surface_);
    int prev_height = cairo_image_surface_get_height(surface_);
    if (prev_width != width || prev_height != height) {
      if (context_) {
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
    surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                          width, height);
  if (!context_)
    context_ = cairo_create(surface_);

  // Now simply fill the canvas with the given color:
  cairo_save(context_);
  helpers::ApplyColor(context_, color);
  cairo_paint(context_);
  cairo_restore(context_);
}


void ImagePainter::SetCanvas(const std::string &image_filename) {
  // Force to load 4 bytes per pixel (STBI_rgb_alpha), so we
  // can easily plug/copy it into the Cairo surface
  ImageBuffer buffer = LoadImage(image_filename, 4);
  SetCanvas(buffer);
}

void ImagePainter::SetCanvas(const ImageBuffer &image_buffer) {
  if (image_buffer.channels != 4) {
    SetCanvas(image_buffer.ToRGBA());
  } else {
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
    if (context_) {
      cairo_destroy(context_);
      context_ = nullptr;
    }

    if (surface_) {
      cairo_surface_destroy(surface_);
      surface_ = nullptr;
    }

    surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                          image_buffer.width, image_buffer.height);
    std::memcpy(cairo_image_surface_get_data(surface_), image_buffer.data,
                4 * image_buffer.width * image_buffer.height);
    context_ = cairo_create(surface_);
    cairo_surface_mark_dirty(surface_);

    /// FIXME how to dim? image + transparent color; image + grayscale?
    //cairo_save(context_);
    //helpers::ApplyColor(context_, "white!50");
    ////cairo_paint_with_alpha(context_, 0.5);
    //cairo_paint(context_);
    //cairo_restore(context_);
  }
}


Vec2i ImagePainter::GetCanvasSize() const {
  if (!IsValid())
    throw std::logic_error("Invalid canvas - did you forget SetCanvas()?");

  return Vec2i(cairo_image_surface_get_width(surface_),
               cairo_image_surface_get_height(surface_));
}

ImageBuffer ImagePainter::GetCanvas(bool copy) const {
  if (!IsValid())
    throw std::logic_error("Invalid canvas - did you forget SetCanvas()?");

  assert(cairo_image_surface_get_format(surface_) == CAIRO_FORMAT_ARGB32);
  const int channels = 4;

  unsigned char *data = cairo_image_surface_get_data(surface_);
  const int width = cairo_image_surface_get_width(surface_);
  const int height = cairo_image_surface_get_height(surface_);
  const int stride = cairo_image_surface_get_stride(surface_);

  ImageBuffer buffer;
  if (copy) {
    buffer.CreateCopy(data, width, height, channels, stride);
  } else {
    buffer.CreateSharedBuffer(data, width, height, channels, stride);
  }
  return buffer;
}


std::unique_ptr<Painter> CreateImagePainter() {
  return std::unique_ptr<Painter>(new ImagePainter());
}

} // namespace viren2d
