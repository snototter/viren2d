#include <utility>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cassert>
#include <cstring> // memcpy
#include <cstdlib> // atexit
#include <cmath>

#include <cairo/cairo.h>
#include <werkzeugkiste/strings/strings.h>
#include <werkzeugkiste/container/math.h>

// public viren2d headers
#include <viren2d/drawing.h>
#include <viren2d/colors.h>

// private viren2d headers
#include <helpers/drawing_helpers.h>
#include <helpers/logging.h>


namespace viren2d {

//TODO refactor -> PainterImpl, outsource surface handling (SVG vs Image)
/// Implements the Painter interface
class PainterImpl : public Painter {
public:
  PainterImpl();

  virtual ~PainterImpl();

  PainterImpl(const PainterImpl &other);

  PainterImpl(PainterImpl &&other) noexcept;

  PainterImpl& operator=(const PainterImpl &other);

  PainterImpl& operator=(PainterImpl &&other) noexcept;

  bool IsValid() const override;

  void SetCanvas(int height, int width, const Color &color) override;

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
          "DrawArc: c={:s}, r={:.1f}, a1={:.1f}, a2={:.1f}, style={:s}, "
          "inc_center={:s}, fill={:s}.",
          center, radius, angle1, angle2, line_style,
          include_center, fill_color);

    helpers::DrawArc(
          surface_, context_, center, radius,
          angle1, angle2, line_style,
          include_center, fill_color);
  }


  void DrawArrowImpl(
      const Vec2d &from, const Vec2d &to,
      const ArrowStyle &arrow_style) override {
    SPDLOG_DEBUG(
          "DrawArrow: p1={:s} --> p2={:s}, style={:s}.",
          from, to, arrow_style);

    helpers::DrawArrow(surface_, context_, from, to, arrow_style);
  }


  void DrawBoundingBox2DImpl(
      const Rect &rect, const std::vector<std::string> &label,
      const BoundingBox2DStyle &style) override {
    SPDLOG_DEBUG(
          "DrawBoundingBox2D: {:s}, {:d} label lines, style={:s}.",
          rect, label.size(), style);

    helpers::DrawBoundingBox2D(
          surface_, context_, rect, label, style);
  }


  void DrawCircleImpl(
      const Vec2d &center, double radius,
      const LineStyle &line_style,
      const Color &fill_color) override {
    SPDLOG_DEBUG(
          "DrawCircle: c={:s}, r={:.1f}, style={:s}, fill={:s}.",
          center, radius, line_style, fill_color);

    helpers::DrawCircle(
          surface_, context_, center, radius,
          line_style, fill_color);
  }


  void DrawEllipseImpl(
      const Ellipse &ellipse, const LineStyle &line_style,
      const Color &fill_color) override {
    SPDLOG_DEBUG(
          "DrawEllipse: {:s}, style={:s}, fill={:s}.",
          ellipse, line_style, fill_color);

    helpers::DrawEllipse(
          surface_, context_, ellipse, line_style, fill_color);
  }


  void DrawGridImpl(
      const Vec2d &top_left, const Vec2d &bottom_right,
      double spacing_x, double spacing_y,
      const LineStyle &line_style) override {
    SPDLOG_DEBUG(
          "DrawGrid: cells={:.1f}x{:.1f}, tl={:s}, br={:s}, style={:s}.",
          spacing_x, spacing_y, top_left, bottom_right, line_style);

    helpers::DrawGrid(
          surface_, context_, top_left, bottom_right,
          spacing_x, spacing_y, line_style);
  }


  void DrawImageImpl(
      const ImageBuffer &image,
      const Vec2d &anchor_position, Anchor anchor,
      double alpha, double scale_x, double scale_y,
      double rotation, double clip_factor) override {
    SPDLOG_DEBUG(
          "DrawImage: {:s}, {:s} at {:s}, alpha={:.2f}, scale_x={:.2f}, "
          "scale_y={:.2f}, rotation={:.2f}°, clip_factor={:.2f}.",
          image.ToString(), AnchorToString(anchor), anchor_position.ToString(),
          alpha, scale_x, scale_y, rotation, clip_factor);

    helpers::DrawImage(
          surface_, context_, image, anchor_position, anchor, alpha,
          scale_x, scale_y, rotation, clip_factor);
  }


  void DrawLineImpl(
      const Vec2d &from, const Vec2d &to,
      const LineStyle &line_style) override {
    SPDLOG_DEBUG(
          "DrawLine: p1={:s}, p2={:s}, style={:s}.", from, to, line_style);

    helpers::DrawLine(
          surface_, context_, from, to, line_style);
  }


  void DrawMarkerImpl(
      const Vec2d &pos, const MarkerStyle &style) override {
    SPDLOG_DEBUG("DrawMarker: pos={:s}, style={:s}.", pos, style);

    helpers::DrawMarker(surface_, context_, pos, style);
  }


  void DrawMarkersImpl(
      const std::vector<std::pair<Vec2d, Color>> &markers,
      const MarkerStyle &style) override {
    SPDLOG_DEBUG(
          "DrawMarkers: {:d} markers, style={:s}.", markers.size(), style);

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
          "DrawPolygon: {:d} points, style={:s}, fill={:s}.",
          points.size(), line_style, fill_color);

    helpers::DrawPolygon(
          surface_, context_, points, line_style, fill_color);
  }


  void DrawRectImpl(
      const Rect &rect, const LineStyle &line_style,
      const Color &fill_color) override {
    SPDLOG_DEBUG(
          "DrawRect: {:s}, style={:s}, fill={:s}.",
          rect, line_style, fill_color);

    helpers::DrawRect(
          surface_, context_, rect, line_style, fill_color);
  }


  Rect DrawTextImpl(
      const std::vector<std::string> &text,
      const Vec2d &anchor_position, Anchor anchor,
      const TextStyle &text_style, const Vec2d &padding,
      double rotation) override {
    SPDLOG_DEBUG(
          "DrawText: {:d} lines at {:s} {:s}, style={:s}, padding={:s}, "
          "rotation={:.1f}°.",
          text.size(), anchor_position, anchor, text_style, padding, rotation);

    return helpers::DrawText(
          surface_, context_, text, anchor_position, anchor, text_style,
          padding, rotation, LineStyle::Invalid, Color::Invalid,
          0.0, {-1.0, -1.0});
  }


  Rect DrawTextBoxImpl(
      const std::vector<std::string> &text,
      const Vec2d &anchor_position, Anchor anchor,
      const TextStyle &text_style, const Vec2d &padding,
      double rotation, const LineStyle &box_line_style,
      const Color &box_fill_color, double box_corner_radius,
      const Vec2d &fixed_box_size) override {
    SPDLOG_DEBUG(
          "DrawTextBox: {:d} lines at {:s} {:s}, style={:s}, padding={:s}, "
          "rotation={:.1f}°, box-style={:s},  box-fill={:s}, box-radius={:.1f}, "
          "box-fixed-size={:d}x{:d}.",
          text.size(), anchor_position, anchor, text_style, padding, rotation,
          box_line_style, box_fill_color, box_corner_radius,
          (int)fixed_box_size.width(), (int)fixed_box_size.height());

    return helpers::DrawText(
          surface_, context_, text, anchor_position, anchor, text_style,
          padding, rotation, box_line_style, box_fill_color,
          box_corner_radius, fixed_box_size);
  }


  void DrawTrajectoryImpl(
      const std::vector<Vec2d> &points,
      const LineStyle &style,
      const Color &color_fade_out,
      bool oldest_position_first,
      int smoothing_window,
      const std::function<double(double)> &mix_factor) override {
    SPDLOG_DEBUG(
          "DrawTrajectory: {:d} points, style={:s}, "
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
          "DrawTrajectories: {:d} trajectories, style={:s}, "
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
  cairo_surface_t *surface_;
  cairo_t *context_;
};


PainterImpl::PainterImpl() : Painter(),
  surface_(nullptr), context_(nullptr) {
  SPDLOG_DEBUG("PainterImpl default constructor.");
}


PainterImpl::~PainterImpl() {
  SPDLOG_DEBUG("PainterImpl destructor.");
  if (context_)
    cairo_destroy(context_);
  if (surface_)
    cairo_surface_destroy(surface_);
}


PainterImpl::PainterImpl(const PainterImpl &other) // copy constructor
  : Painter(),
    surface_(nullptr), context_(nullptr) {
  SPDLOG_DEBUG("PainterImpl copy constructor.");
  if (other.surface_)
  {
    SPDLOG_TRACE("Copying other PainterImpl's surface.");
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


PainterImpl::PainterImpl(PainterImpl &&other) noexcept
  : Painter(),
    surface_(std::exchange(other.surface_, nullptr)),
    context_(std::exchange(other.context_, nullptr)) {
  SPDLOG_DEBUG("PainterImpl move constructor.");
}


PainterImpl& PainterImpl::operator=(const PainterImpl &other) { // Copy assignment
  SPDLOG_DEBUG("PainterImpl copy assignment operator.");
  return *this = PainterImpl(other);
}


PainterImpl& PainterImpl::operator=(PainterImpl &&other) noexcept { // Move assignment
  SPDLOG_DEBUG("PainterImpl move assignment operator.");
  std::swap(surface_, other.surface_);
  std::swap(context_, other.context_);
  return *this;
}


bool PainterImpl::IsValid() const {
  return (surface_ != nullptr) && (context_ != nullptr);
}

//FIXME PainterImpl
//FIXME log --> set level via CMakeLists.txt
void PainterImpl::SetCanvas(int height, int width, const Color &color) {
  SPDLOG_DEBUG(
        "SetCanvas: width={:d}, height={:d}, color={:s}).",
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
    SPDLOG_TRACE(
          "SetCanvas: Creating Cairo image surface for w={:d}, h={:d} canvas.",
          width, height);
    surface_ = cairo_image_surface_create(
          CAIRO_FORMAT_ARGB32, width, height);
  }

  if (!context_) {
    context_ = cairo_create(surface_);
  }

  // Now simply fill the canvas with the given color:
  cairo_save(context_);
  helpers::ApplyColor(context_, color);
  cairo_paint(context_);
  cairo_restore(context_);
}


void PainterImpl::SetCanvas(const std::string &image_filename) {
  SPDLOG_DEBUG("SetCanvas: filename={:s}.", image_filename);
  // Force to load 4 bytes per pixel (STBI_rgb_alpha), so we
  // can easily plug/copy it into the Cairo surface
  ImageBuffer buffer = LoadImage(image_filename, 4);
  SetCanvas(buffer);
}


void PainterImpl::SetCanvas(const ImageBuffer &image_buffer) {
  SPDLOG_DEBUG("SetCanvas: {:s}).", image_buffer.ToString());
  if (image_buffer.Channels() != 4) {
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
      SPDLOG_TRACE("SetCanvas: Releasing previous Cairo context.");
      cairo_destroy(context_);
      context_ = nullptr;
    }

    if (surface_) {
      SPDLOG_TRACE("SetCanvas: Releasing previous Cairo surface.");
      cairo_surface_destroy(surface_);
      surface_ = nullptr;
    }

    SPDLOG_TRACE(
          "SetCanvas: Creating Cairo surface and context from image buffer.");
    surface_ = cairo_image_surface_create(
          CAIRO_FORMAT_ARGB32, image_buffer.Width(), image_buffer.Height());
    std::memcpy(
          cairo_image_surface_get_data(surface_),
          image_buffer.ImmutableData(),
          4 * image_buffer.Width() * image_buffer.Height());
    context_ = cairo_create(surface_);

    // Ensure that the underlying image surface will be rendered immediately:
    cairo_surface_mark_dirty(surface_);
  }
}


Vec2i PainterImpl::GetCanvasSize() const {
  if (IsValid()) {
    return Vec2i(
          cairo_image_surface_get_width(surface_),
          cairo_image_surface_get_height(surface_));
  } else {
    return Vec2i(0, 0);
  }
}

ImageBuffer PainterImpl::GetCanvas(bool copy) const {
  SPDLOG_DEBUG("GetCanvas: copy={}.", copy);

  if (!IsValid()) {
    throw std::logic_error("Invalid canvas - did you forget `SetCanvas()`?");
  }

  assert(cairo_image_surface_get_format(surface_) == CAIRO_FORMAT_ARGB32);
  const int channels = 4;
  // For CAIRO_FORMAT_ARGB32, the pixel stride is 4 bytes, i.e. the
  // number of channels

  unsigned char *data = cairo_image_surface_get_data(surface_);
  const int width = cairo_image_surface_get_width(surface_);
  const int height = cairo_image_surface_get_height(surface_);
  const int row_stride = cairo_image_surface_get_stride(surface_);

  ImageBuffer buffer;
  if (copy) {
    buffer.CreateCopiedBuffer(
          data, height, width, channels,
          row_stride, channels, ImageBufferType::UInt8);
  } else {
    buffer.CreateSharedBuffer(
          data, height, width, channels,
          row_stride, channels, ImageBufferType::UInt8);
  }
  return buffer;
}


std::unique_ptr<Painter> CreatePainter() {
  return std::unique_ptr<Painter>(new PainterImpl());
}

} // namespace viren2d
