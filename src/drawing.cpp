#include <utility>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cassert>
#include <cstring> // memcpy
#include <cstdlib> // atexit

#include <math.h>

#include <spdlog/spdlog.h>

#include <cairo/cairo.h>
#include <werkzeugkiste/strings/strings.h>
#include <werkzeugkiste/container/math.h>

// public viren2d headers
#include <viren2d/drawing.h>
#include <viren2d/colors.h>

// private viren2d headers
#include <helpers/drawing_helpers.h>
#include <helpers/logging.h>


//-------------------------------------------------
// Preprocessor macros to initialize the library
// upon loading. This is needed to automatically
// set up logging.
// Included for convenience & because I wanted to
// check on library initialization/unloading hooks.
// The initialization code belongs in drawing.cpp,
// because this will always be linked into the
// library user's application.
//
// The macros are taken from:
// https://stackoverflow.com/a/2390626/400948

#ifdef __cplusplus
    #define INITIALIZER(f) \
        static void f(void); \
        struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
        static void f(void)
#elif defined(_MSC_VER)
    #pragma section(".CRT$XCU",read)
    #define INITIALIZER2_(f,p) \
        static void f(void); \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
        __pragma(comment(linker,"/include:" p #f "_")) \
        static void f(void)
    #ifdef _WIN64
        #define INITIALIZER(f) INITIALIZER2_(f,"")
    #else
        #define INITIALIZER(f) INITIALIZER2_(f,"_")
    #endif
#else
    #define INITIALIZER(f) \
        static void f(void) __attribute__((constructor)); \
        static void f(void)
#endif


//-------------------------------------------------

namespace viren2d {

void shutdown() {
  SPDLOG_DEBUG("Shutting down the viren2d++ library.");
  spdlog::shutdown();
}


INITIALIZER(initialize) {
//  spdlog::set_level(spdlog::level::trace);
//  spdlog::set_pattern("[%l][%@, %!] %v");
  spdlog::set_level(viren2d_ACTIVE_SPDLOG_LEVEL); //TODO change (cmake variable) to default info/warn before release
  SPDLOG_DEBUG("Initializing the viren2d++ library.");

  std::atexit(shutdown);
}


/** Implements the Painter interface using a Cairo image surface. */
class ImagePainter : public Painter {
public:
  ImagePainter();

  virtual ~ImagePainter();

  ImagePainter(const ImagePainter &other);

  ImagePainter(ImagePainter &&other) noexcept;

  ImagePainter& operator=(const ImagePainter &other);

  ImagePainter& operator=(ImagePainter &&other) noexcept;

  bool IsValid() const override;

  void SetCanvas(int width, int height, const Color &color) override;

  void SetCanvas(const std::string &image_filename) override;

  void SetCanvas(const ImageBuffer &image_buffer) override;

  Vec2i GetCanvasSize() const override;

  ImageBuffer GetCanvas(bool copy) const override;


protected:
  void DrawArcImpl(
      const Vec2d &center, double radius,
      double angle1, double angle2, const LineStyle &line_style,
      bool include_center, const Color &fill_color) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawArc: c={:s}, r={:.1f}, a1={:.1f},"
          " a2={:.1f}, style={:s}, inc_center={:s},"
          " fill={:s}.", center, radius, angle1, angle2,
          line_style, include_center, fill_color);

    helpers::DrawArc(
          surface_, context_, center, radius,
          angle1, angle2, line_style,
          include_center, fill_color);
  }


  void DrawArrowImpl(
      const Vec2d &from, const Vec2d &to,
      const ArrowStyle &arrow_style) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawArrow: p1={:s}, p2={:s}, style={:s}.",
          from, to, arrow_style);

    helpers::DrawArrow(surface_, context_, from, to, arrow_style);
  }


  void DrawBoundingBox2DImpl(
      const Rect &rect, const std::vector<const char*> &label,
      const BoundingBox2DStyle &style) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawBoundingBox2D: {:s},"
          " label=\"{:s}\", style={:s}.", rect, label, style);//TODO werkzeugkiste: string shortening!

    helpers::DrawBoundingBox2D(
          surface_, context_, rect, label, style);
  }


  void DrawCircleImpl(
      const Vec2d &center, double radius,
      const LineStyle &line_style,
      const Color &fill_color) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawCircle: c={:s}, r={:.1f},"
          " style={:s}, fill={:s}.", center, radius,
          line_style, fill_color);

    helpers::DrawCircle(
          surface_, context_, center, radius,
          line_style, fill_color);
  }


  void DrawEllipseImpl(
      const Ellipse &ellipse, const LineStyle &line_style,
      const Color &fill_color) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawEllipse: {:s}, style={:s}, fill={:s}.",
          ellipse, line_style, fill_color);

    helpers::DrawEllipse(
          surface_, context_, ellipse, line_style, fill_color);
  }


  void DrawGridImpl(
      const Vec2d &top_left, const Vec2d &bottom_right,
      double spacing_x, double spacing_y,
      const LineStyle &line_style) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawGrid: cells={:.1f}x{:.1f}, "
          "tl={:s}, br={:s}, style={:s}.",
          spacing_x, spacing_y, top_left, bottom_right, line_style);

    helpers::DrawGrid(
          surface_, context_, top_left, bottom_right,
          spacing_x, spacing_y, line_style);
  }


  void DrawLineImpl(
      const Vec2d &from, const Vec2d &to,
      const LineStyle &line_style) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawLine: p1={:s}, p2={:s}, style={:s}.",
          from, to, line_style);

    helpers::DrawLine(
          surface_, context_, from, to, line_style);
  }


  void DrawMarkerImpl(
      const Vec2d &pos, const MarkerStyle &style) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawMarker: pos={:s}, style={:s}.",
          pos, style);

    helpers::DrawMarker(surface_, context_, pos, style);
  }


  void DrawMarkersImpl(
      const std::vector<std::pair<Vec2d, Color>> &markers,
      const MarkerStyle &style) override {
    SPDLOG_DEBUG("ImagePainter::DrawMarkers: {:d} markers, style={:s}.",
                 markers.size(), style);

    // Currently, we simply forward each point to the
    // "single marker" Cairo helper. While there is undoubtedly
    // potential for performance improvement, this is not
    // my current focus, because:
    // a) viren2d is neither a high-speed library nor
    //    is it noticably slow, and
    // b) unless we're drawing hundreds+ points at once,
    //    I doubt that the speedup would be noticable
    MarkerStyle s(style);
    for (const auto &p : markers) {
      if (p.second.IsValid()) {
        s.color = p.second;
      } else {
        s.color = style.color;
      }
      helpers::DrawMarker(surface_, context_, p.first, s);
    }
  }


  void DrawPolygonImpl(
      const std::vector<Vec2d> &points,
      const LineStyle &line_style,
      const Color &fill_color) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawPolygon: {:d} points, "
          "style={:s}, fill={:s}.",
          points.size(), line_style, fill_color);

    helpers::DrawPolygon(
          surface_, context_, points, line_style, fill_color);
  }


  void DrawRectImpl(
      const Rect &rect, const LineStyle &line_style,
      const Color &fill_color) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawRect: {:s}, "
          "style={:s}, fill={:s}.",
          rect, line_style, fill_color);

    helpers::DrawRect(
          surface_, context_, rect, line_style, fill_color);
  }


  Rect DrawTextImpl(
      const std::vector<const char*> &text,
      const Vec2d &anchor_position, Anchor anchor,
      const TextStyle &text_style, const Vec2d &padding,
      double rotation) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawText: {:d} lines at {:s}, {:s} "
          "style={:s}, padding={:s}, rotation={:.1f}°.",
          text.size(), anchor_position, anchor,
          text_style, padding, rotation);

    return helpers::DrawText(
          surface_, context_, text,
          anchor_position, anchor,
          text_style, padding, rotation,
          LineStyle::Invalid, Color::Invalid,
          0.0, {-1.0, -1.0});
  }


  Rect DrawTextBoxImpl(
      const std::vector<const char*> &text,
      const Vec2d &anchor_position, Anchor anchor,
      const TextStyle &text_style, const Vec2d &padding,
      double rotation, const LineStyle &box_line_style,
      const Color &box_fill_color, double box_corner_radius,
      const Vec2d &fixed_box_size) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawTextBox: {:d} lines at {:s}, {:s} "
          "style={:s}, padding={:s}, rotation={:.1f}°, box-style={:s}, "
          "box-fill={:s}, box-radius={:.1f}, box-fixed-size={:d}x{:d}.",
          text.size(), anchor_position, anchor, text_style,
          padding, rotation, box_line_style, box_fill_color,
          box_corner_radius, (int)fixed_box_size.width(),
          (int)fixed_box_size.height());

    return helpers::DrawText(
          surface_, context_, text, anchor_position, anchor,
          text_style, padding, rotation, box_line_style,
          box_fill_color, box_corner_radius, fixed_box_size);
  }


  void DrawTrajectoryImpl(
      const std::vector<Vec2d> &points,
      const LineStyle &style,
      const Color &color_fade_out,
      bool oldest_position_first,
      int smoothing_window,
      const std::function<double(double)> &mix_factor) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawTrajectory: {:d} points, style={:s}, "
          "fade_out={:s}, oldest_first={:s}, smooth={:d}.",
          points.size(), style, color_fade_out,
          oldest_position_first, smoothing_window);

    const std::vector<Vec2d> &smoothed =
        (smoothing_window > 0)
        ? werkzeugkiste::container::SmoothMovingAverage(
            points, smoothing_window)
        : points;

    helpers::DrawTrajectory(
          surface_, context_, smoothed, style, color_fade_out,
          oldest_position_first, mix_factor);
  }


  void DrawTrajectoriesImpl(
      const std::vector<std::pair<std::vector<Vec2d>, Color>> &trajectories,
      const LineStyle &style, const Color &color_fade_out,
      bool oldest_position_first, int smoothing_window,
      const std::function<double(double)> &mix_factor) override {
    SPDLOG_DEBUG(
          "ImagePainter::DrawTrajectories: {:d} trajectories, style={:s}, "
          "fade_out={:s}, oldest_first={:s}, smooth={:d}.",
          trajectories.size(), style, color_fade_out,
          oldest_position_first, smoothing_window);

    LineStyle s(style);
    for (const auto &p : trajectories) {
      const std::vector<Vec2d> &smoothed =
          (smoothing_window > 0)
          ? werkzeugkiste::container::SmoothMovingAverage(
              p.first, smoothing_window)
          : p.first;

      if (p.second.IsValid()) {
        s.color = p.second;
      } else if (p.second.IsSpecialSame()) {
        s.color = style.color.WithAlpha(p.second.alpha);
      } else {
        s.color = p.second;
      }

      helpers::DrawTrajectory(
            surface_, context_, smoothed, s, color_fade_out,
            oldest_position_first, mix_factor);
    }
  }


private:
  //TODO document
  cairo_surface_t *surface_;
  cairo_t *context_;
};


ImagePainter::ImagePainter() : Painter(),
  surface_(nullptr), context_(nullptr) {
  SPDLOG_DEBUG("ImagePainter default constructor.");
}


ImagePainter::~ImagePainter() {
  SPDLOG_TRACE("ImagePainter destructor.");
  if (context_)
    cairo_destroy(context_);
  if (surface_)
    cairo_surface_destroy(surface_);
}


ImagePainter::ImagePainter(const ImagePainter &other) // copy constructor
  : Painter(),
    surface_(nullptr), context_(nullptr) {
  SPDLOG_TRACE("ImagePainter copy constructor.");
  if (other.surface_)
  {
    SPDLOG_TRACE("Copying other ImagePainter's surface.");
    cairo_format_t format = cairo_image_surface_get_format(other.surface_);
    assert(format == CAIRO_FORMAT_ARGB32 || format == CAIRO_FORMAT_RGB24);
    const int width = cairo_image_surface_get_width(other.surface_);
    const int height = cairo_image_surface_get_height(other.surface_);

    surface_ = cairo_image_surface_create(cairo_image_surface_get_format(other.surface_),
                                          width, height);
    memcpy(cairo_image_surface_get_data(surface_),
           cairo_image_surface_get_data(other.surface_),
           width*height*4);

    // We don't reuse the context on purpose. If someone
    // really wants a copy of an ImagePainter, they can
    // afford the extra allocation
    context_ = cairo_create(surface_);
  }
}


ImagePainter::ImagePainter(ImagePainter &&other) noexcept
  : Painter(),
    surface_(std::exchange(other.surface_, nullptr)),
    context_(std::exchange(other.context_, nullptr)) {
  SPDLOG_TRACE("ImagePainter move constructor.");
}


ImagePainter& ImagePainter::operator=(const ImagePainter &other) { // Copy assignment
  SPDLOG_TRACE("ImagePainter copy assignment operator.");
  return *this = ImagePainter(other);
}


ImagePainter& ImagePainter::operator=(ImagePainter &&other) noexcept { // Move assignment
  SPDLOG_TRACE("ImagePainter move assignment operator.");
  std::swap(surface_, other.surface_);
  std::swap(context_, other.context_);
  return *this;
}


bool ImagePainter::IsValid() const {
  SPDLOG_TRACE("ImagePainter::IsValid().");
  return (surface_ != nullptr) && (context_ != nullptr);
}


void ImagePainter::SetCanvas(int width, int height,
                             const Color &color) {
  SPDLOG_DEBUG("ImagePainter::SetCanvas(width={:d},"
               " height={:d}, color={:s}).",
               width, height, color);
  // Check if we can reuse the current image surface to
  // save ourselves the memory allocation:
  if (surface_) {
    int prev_width = cairo_image_surface_get_width(surface_);
    int prev_height = cairo_image_surface_get_height(surface_);

    if (prev_width != width || prev_height != height) {
      SPDLOG_TRACE("Cannot reuse previous canvas, size "
                   "mismatch: prev={:d}x{:d}, now={:d}x{:d}.",
                   prev_width, prev_height, width, height);
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
  if (!surface_) {
    SPDLOG_TRACE("ImagePainter::SetCanvas: Creating Cairo image surface for {:d}x{:d} canvas.",
                 width, height);
    surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                          width, height);
  }

  if (!context_) {
    SPDLOG_TRACE("ImagePainter::SetCanvas: Creating Cairo context.");
    context_ = cairo_create(surface_);
  }

  SPDLOG_TRACE("ImagePainter::SetCanvas: Painting the canvas using {:s}.", color);
  // Now simply fill the canvas with the given color:
  cairo_save(context_);
  helpers::ApplyColor(context_, color);
  cairo_paint(context_);
  cairo_restore(context_);
}


void ImagePainter::SetCanvas(const std::string &image_filename) {
  SPDLOG_DEBUG("ImagePainter::SetCanvas(filename={:s}).",
               image_filename);
  // Force to load 4 bytes per pixel (STBI_rgb_alpha), so we
  // can easily plug/copy it into the Cairo surface
  ImageBuffer buffer = LoadImage(image_filename, 4);
  SetCanvas(buffer);
}

void ImagePainter::SetCanvas(const ImageBuffer &image_buffer) {
  SPDLOG_DEBUG("ImagePainter::SetCanvas(image_buffer{:s}).",
               image_buffer);
  if (image_buffer.channels != 4) {
    SetCanvas(image_buffer.ToChannels(4));
  } else {
    // Currently, we clean up previously created contexts/surfaces to
    // avoid unnecessarily cluttering the implementation. Then, we
    // copy the given ImageBuffer.
    // If this becomes a bottleneck, we need to provide a boolean "copy"
    // flag and distinguish 4 scenarios:
    // * copy-flag true, existing data --> check if it surface can
    //   be reused (memcpy)
    // * copy-flag true, no surface --> malloc(surface create) + memcpy
    // * copy-flag false, existing data --> clean up data, reuse surface
    // * copy-flag false, no surface --> surface create_for_data
    if (context_) {
      SPDLOG_TRACE("ImagePainter::SetCanvas: Releasing previous Cairo context.");
      cairo_destroy(context_);
      context_ = nullptr;
    }

    if (surface_) {
      SPDLOG_TRACE("ImagePainter::SetCanvas: Releasing previous Cairo surface.");
      cairo_surface_destroy(surface_);
      surface_ = nullptr;
    }

    SPDLOG_TRACE("ImagePainter::SetCanvas: Creating Cairo surface and context from image buffer.");
    surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                          image_buffer.width, image_buffer.height);
    std::memcpy(cairo_image_surface_get_data(surface_), image_buffer.data,
                4 * image_buffer.width * image_buffer.height);
    context_ = cairo_create(surface_);

    // Needed to ensure that the underlying image surface
    // will be rendered:
    cairo_surface_mark_dirty(surface_);

    /// FIXME how to dim? image + transparent color; image + grayscale?
    /// caveat: cairo uses premultiplied alpha
    //cairo_save(context_);
    //helpers::ApplyColor(context_, "white!50");
    ////cairo_paint_with_alpha(context_, 0.5);
    //cairo_paint(context_);
    //cairo_restore(context_);
  }
}


Vec2i ImagePainter::GetCanvasSize() const {
  SPDLOG_DEBUG("ImagePainter::GetCanvasSize().");
  if (IsValid()) {
    return Vec2i(
          cairo_image_surface_get_width(surface_),
          cairo_image_surface_get_height(surface_));
  } else {
    return Vec2i(0, 0);
  }
}

ImageBuffer ImagePainter::GetCanvas(bool copy) const {
  SPDLOG_DEBUG("ImagePainter::GetCanvas(copy={}).", copy);
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

std::unique_ptr<Painter> CreatePainter() {
  SPDLOG_DEBUG("Creating image painter.");
  return std::unique_ptr<Painter>(new ImagePainter());
}

} // namespace viren2d
