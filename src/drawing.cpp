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

// public viren2d headers
#include <viren2d/drawing.h>
#include <viren2d/colors.h>

// private viren2d headers
#include <helpers/drawing_helpers.h>
#include <helpers/logging.h>

//FIXME logging painter --> public interface (or first Impl()): debug
// all others: trace



//-------------------------------------------------
// Preprocessor macros to initialize the
// library upon loading. This is needed to
// set up logging.
// Macros taken from:
// https://stackoverflow.com/a/2390626/400948

// TODO: We place the initializer in drawing.cpp
// because this will always be linked into the
// library user's application.

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
//namespace {
///** Predefined default line style - can be replaced separately within each @see Painter. */
//const static LineStyle kdefault_line_style = LineStyle(2, Color(NamedColor::Azure));


///** Predefined default arrow style - can be replaced separately within each @see Painter. */
//const static ArrowStyle kdefault_arrow_style = ArrowStyle(4, Color(NamedColor::ForestGreen), 0.1, 20);

///** Predefined default text style - can be replaced separately within each @see Painter. */
//const static TextStyle kdefault_text_style = TextStyle(16, "monospace", Color::Black);

//const static BoundingBox2DStyle kdefault_bbox2d_style =
//    BoundingBox2DStyle(LineStyle(4, Color(NamedColor::Azure)),
//                       TextStyle(10, "monospace", Color::Black),
//                       0.2, 0.7, HorizontalAlignment::Left,
//                       BoundingBoxLabelPosition::Top, 5, true);
//}  // anonymous namespace


void shutdown() {
  SPDLOG_DEBUG("Shutting down the viren2d++ library.");
  spdlog::shutdown();
}


INITIALIZER(initialize) {
//  spdlog::set_level(spdlog::level::trace);
//  spdlog::set_pattern("[%l][%@, %!] %v");
  spdlog::set_level(viren2d_ACTIVE_SPDLOG_LEVEL); //TODO change to default info/warn before release
  SPDLOG_DEBUG("Initializing the viren2d++ library.");

  std::atexit(shutdown);
}

/** Implements the Painter interface using a Cairo image surface. */
class ImagePainter : public Painter {
public:
  ImagePainter();

  virtual ~ImagePainter();

  ImagePainter(const ImagePainter &other); // copy constructor

  ImagePainter(ImagePainter &&other) noexcept; // move constructor

  ImagePainter& operator=(const ImagePainter &other); // Copy assignment

  ImagePainter& operator=(ImagePainter &&other) noexcept; // Move assignment

  bool IsValid() const override;

  void SetCanvas(int width, int height, const Color &color) override;

  void SetCanvas(const std::string &image_filename) override;

  void SetCanvas(const ImageBuffer &image_buffer) override;

  Vec2i GetCanvasSize() const override;

  ImageBuffer GetCanvas(bool copy) const override;

//  void SetDefaultLineStyle(const LineStyle &line_style) override;
//  LineStyle GetDefaultLineStyle() const override;

//  void SetDefaultArrowStyle(const ArrowStyle &arrow_style) override;
//  ArrowStyle GetDefaultArrowStyle() const override;

//  void SetDefaultTextStyle(const TextStyle &text_style) override;
//  TextStyle GetDefaultTextStyle() const override;


protected:
  void DrawArcImpl(const Vec2d &center, double radius,
                   double angle1, double angle2,
                   const LineStyle &line_style,
                   bool include_center,
                   const Color &fill_color) override {
    SPDLOG_DEBUG("ImagePainter::DrawArc: c={:s}, r={:.1f}, a1={:.1f},"
                 " a2={:.1f}, style={:s}, inc_center={:s},"
                 " fill={:s}.", center, radius, angle1, angle2,
                 line_style, include_center, fill_color);

    helpers::DrawArc(surface_, context_, center, radius,
                     angle1, angle2, line_style,
                     include_center, fill_color);
  }

  void DrawArrowImpl(const Vec2d &from, const Vec2d &to,
                     const ArrowStyle &arrow_style) override {
    SPDLOG_DEBUG("ImagePainter::DrawArrow: p1={:s}, p2={:s}, style={:s}.",
                 from, to, arrow_style);

    helpers::DrawArrow(surface_, context_, from, to, arrow_style);
  }


  void DrawBoundingBox2DImpl(const Rect &rect,
                             const std::string &label,
                             const BoundingBox2DStyle &style) override {
    SPDLOG_DEBUG("ImagePainter::DrawBoundingBox2D: {:s},"
                 " label=\"{:s}\", style={:s}.", rect, label, style);//TODO werkzeugkiste: string shortening!
    helpers::DrawBoundingBox2D(surface_, context_, rect,
                               label, style);
  }


  void DrawCircleImpl(const Vec2d &center, double radius,
                      const LineStyle &line_style,
                      const Color &fill_color) override {
    SPDLOG_DEBUG("ImagePainter::DrawCircle: c={:s}, r={:.1f},"
                 " style={:s}, fill={:s}.", center, radius,
                 line_style, fill_color);

    helpers::DrawCircle(surface_, context_, center, radius,
                        line_style, fill_color);
  }


  void DrawEllipseImpl(const Ellipse &ellipse, const LineStyle &line_style,
                       const Color &fill_color) override {
    SPDLOG_DEBUG("ImagePainter::DrawEllipse: {:s},"
                 " style={:s}, fill={:s}.", ellipse,
                 line_style, fill_color);

    helpers::DrawEllipse(surface_, context_, ellipse,
                         line_style, fill_color);
  }


  void DrawGridImpl(const Vec2d &top_left, const Vec2d &bottom_right,
                    double spacing_x, double spacing_y,
                    const LineStyle &line_style) override {
    SPDLOG_DEBUG("ImagePainter::DrawGrid: cells={:.1f}x{:.1f}, tl={:s}, br={:s},"
                 " style={:s}.", spacing_x, spacing_y,
                 top_left, bottom_right, line_style);

    helpers::DrawGrid(surface_, context_, top_left, bottom_right,
                      spacing_x, spacing_y, line_style);
  }


  void DrawLineImpl(const Vec2d &from, const Vec2d &to,
                    const LineStyle &line_style) override {
    SPDLOG_DEBUG("ImagePainter::DrawLine: p1={:s}, p2={:s}, style={:s}.",
                 from, to, line_style);

    helpers::DrawLine(surface_, context_, from, to,
                      line_style);
  }


  void DrawRectImpl(const Rect &rect, const LineStyle &line_style,
                    const Color &fill_color) override {
    SPDLOG_DEBUG("ImagePainter::DrawRect: {:s},"
                 " style={:s}, fill={:s}.", rect,
                 line_style, fill_color);

    helpers::DrawRect(surface_, context_, rect,
                      line_style, fill_color);
  }


  //TODO debug log here and trace in helpers!
  void DrawTextImpl(const std::vector<const char*> &text, const Vec2d &position,
                    TextAnchor text_anchor, const TextStyle &text_style,
                    const Vec2d &padding, double rotation) override {
    SPDLOG_DEBUG("ImagePainter::DrawText: {:d} chars at {:s}, {:s} "
                 "style={:s}, padding={:s}, rotation={:.1f}°.", text.size(),
                 position, text_anchor, text_style, padding, rotation);

    helpers::DrawText(surface_, context_, text, position, text_anchor,
                      text_style, padding, rotation,
                      LineStyle::Invalid, Color::Invalid, 0.0);
  }


  void DrawTextBoxImpl(const std::vector<const char*> &text, const Vec2d &position,
                       TextAnchor text_anchor, const TextStyle &text_style,
                       const Vec2d &padding, double rotation,
                       const LineStyle &box_line_style, const Color &box_fill_color,
                       double box_corner_radius) override {
    SPDLOG_DEBUG("ImagePainter::DrawTextBox: {:d} chars at {:s}, {:s} "
                 "style={:s}, padding={:s}, rotation={:.1f}°, box-style={:s}, "
                 "box-fill={:s}, box-radius={:.1f}.", text.size(),
                 position, text_anchor, text_style, padding, rotation,
                 box_line_style, box_fill_color, box_corner_radius);

    helpers::DrawText(surface_, context_, text, position, text_anchor,
                      text_style, padding, rotation, box_line_style,
                      box_fill_color, box_corner_radius);
  }


private:
  //TODO document
  cairo_surface_t *surface_;
  cairo_t *context_;

//  LineStyle default_line_style_;
//  ArrowStyle default_arrow_style_;
//  TextStyle default_text_style_;
//  BoundingBox2DStyle default_bbox2d_style_;

//  void ApplyDefaultStyles();

//  const LineStyle &CheckInputStyle(const LineStyle &user_input) const;
//  const ArrowStyle &CheckInputStyle(const ArrowStyle &user_input) const;
//  const TextStyle &CheckInputStyle(const TextStyle &user_input) const;
//  BoundingBox2DStyle CheckInputStyle(const BoundingBox2DStyle &user_input) const;
};


ImagePainter::ImagePainter() : Painter(),
  surface_(nullptr), context_(nullptr) {
//  default_line_style_(kdefault_line_style),
//  default_arrow_style_(kdefault_arrow_style),
//  default_text_style_(kdefault_text_style),
//  default_bbox2d_style_(kdefault_bbox2d_style) {
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
//    default_line_style_(other.default_line_style_),
//    default_arrow_style_(other.default_arrow_style_),
//    default_text_style_(other.default_text_style_) {
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

//    // We need to re-apply the default styles after every
//    // context switch:
//    ApplyDefaultStyles();
  }
}


ImagePainter::ImagePainter(ImagePainter &&other) noexcept
  : Painter(),
    surface_(std::exchange(other.surface_, nullptr)),
    context_(std::exchange(other.context_, nullptr)) {
//    default_line_style_(other.default_line_style_),
//    default_arrow_style_(other.default_arrow_style_),
//    default_text_style_(other.default_text_style_) {
  SPDLOG_TRACE("ImagePainter move constructor.");
//  TODO REMOVE// No need to ApplyDefaultStyles() - the "other"
  // should've already set these up (and we're
  // simply stealing their context)
}


ImagePainter& ImagePainter::operator=(const ImagePainter &other) { // Copy assignment
  SPDLOG_TRACE("ImagePainter copy assignment operator.");
  return *this = ImagePainter(other);
}


ImagePainter& ImagePainter::operator=(ImagePainter &&other) noexcept { // Move assignment
  SPDLOG_TRACE("ImagePainter move assignment operator.");
  std::swap(surface_, other.surface_);
  std::swap(context_, other.context_);
//  std::swap(default_line_style_, other.default_line_style_);
//  std::swap(default_arrow_style_, other.default_arrow_style_);
//  std::swap(default_text_style_, other.default_text_style_);
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
//    ApplyDefaultStyles();  // Needed after each context change
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

//    ApplyDefaultStyles();  // Needed after each context change

    // Needed to ensure that the underlying image surface
    // will be rendered:
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
  SPDLOG_DEBUG("ImagePainter::GetCanvasSize().");
  if (!IsValid())
    throw std::logic_error("Invalid canvas - did you forget SetCanvas()?");

  return Vec2i(cairo_image_surface_get_width(surface_),
               cairo_image_surface_get_height(surface_));
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


//void ImagePainter::SetDefaultLineStyle(const LineStyle &line_style) {
//  SPDLOG_DEBUG("ImagePainter::SetDefaultLineStyle({:s}).",
//               line_style);

//  if (!line_style.IsValid()) {
//    std::ostringstream s;
//    s << "Cannot change the default line style to an invalid configuration: "
//      << line_style;
//    throw std::invalid_argument(s.str());
//  }

//  default_line_style_ = line_style;
//  ApplyDefaultStyles();
//}


//LineStyle ImagePainter::GetDefaultLineStyle() const {
//  SPDLOG_TRACE("ImagePainter::GetDefaultLineStyle().");
//  return default_line_style_;
//}


//void ImagePainter::SetDefaultArrowStyle(const ArrowStyle &arrow_style) {
//  SPDLOG_DEBUG("ImagePainter::SetDefaultArrowStyle({:s}).",
//               arrow_style);

//  if (!arrow_style.IsValid()) {
//    std::ostringstream s;
//    s << "Cannot change the default arrow style to an invalid configuration: "
//      << arrow_style;
//    throw std::invalid_argument(s.str());
//  }

//  default_arrow_style_ = arrow_style;
//  ApplyDefaultStyles();
//}


//ArrowStyle ImagePainter::GetDefaultArrowStyle() const {
//  SPDLOG_TRACE("ImagePainter::GetDefaultArrowStyle().");
//  return default_arrow_style_;
//}


//void ImagePainter::SetDefaultTextStyle(const TextStyle &text_style) {
//  SPDLOG_DEBUG("ImagePainter::SetDefaultTextStyle({:s}).",
//               text_style);

//  if (!text_style.IsValid()) {
//    std::ostringstream s;
//    s << "Cannot change the default text style to an invalid configuration: "
//      << text_style;
//    throw std::invalid_argument(s.str());
//  }

//  default_text_style_ = text_style;
//  ApplyDefaultStyles();
//}


//TextStyle ImagePainter::GetDefaultTextStyle() const {
//  SPDLOG_TRACE("ImagePainter::GetDefaultTextStyle().");
//  return default_text_style_;
//}


//void ImagePainter::ApplyDefaultStyles() {
//  SPDLOG_TRACE("ImagePainter::ApplyDefaultStyles: line={:s},"
//               " arrow={:s}, text={:s}.",
//               default_line_style_, default_arrow_style_,
//               default_text_style_);
//  if (context_) {
//    helpers::ApplyLineStyle(context_, default_line_style_, false);

//    // The arrow style will not be preset in the context (as
//    // an arrow is basically just a line and would thus overwrite
//    // the default line style)

//    helpers::ApplyTextStyle(context_, default_text_style_);
//  }
//}


//const LineStyle &
//ImagePainter::CheckInputStyle(const LineStyle &user_input) const {
//  if (user_input.IsSpecialDefault()) {
//    SPDLOG_TRACE("ImagePainter::CheckInputStyle: User line style is "
//                 "'Default', switching to painter's default={:s}.",
//                 default_line_style_);
//    return default_line_style_;
//  } else {
//    SPDLOG_TRACE("ImagePainter::CheckInputStyle: Using user input {:s}",
//                 user_input);
//    return user_input;
//  }
//}


//const ArrowStyle &
//ImagePainter::CheckInputStyle(const ArrowStyle &user_input) const {
//  if (user_input.IsSpecialInvalid()) {
//    throw std::invalid_argument("ArrowStyle::Invalid is not supported as input.");
//  }

//  if (user_input.IsSpecialDefault()) {
//    SPDLOG_TRACE("ImagePainter::CheckInputStyle: User arrow style is "
//                 "'Default', switching to painter's default={:s}.",
//                 default_arrow_style_);
//    return default_arrow_style_;
//  } else {
//    SPDLOG_TRACE("ImagePainter::CheckInputStyle: Using user input {:s}",
//                 user_input);
//    return user_input;
//  }
//}


//const TextStyle &
//ImagePainter::CheckInputStyle(const TextStyle &user_input) const {
//  if (user_input.IsValid()) {
//    SPDLOG_TRACE("ImagePainter::CheckInputStyle: Using user input {:s}",
//                 user_input);
//    return user_input;
//  } else {
//    SPDLOG_TRACE("ImagePainter::CheckInputStyle: User text style is "
//                 "invalid, switching to painter's default={:s}.",
//                 default_text_style_);
//    return default_text_style_;
//  }
//}

//BoundingBox2DStyle
//ImagePainter::CheckInputStyle(const BoundingBox2DStyle &user_input) const {
//  //FIXME refactor the default style stuff - it's already too complex (don't need to handle each and every possibility/use case)
//  auto style(user_input);
//  // Do we need to replace the line style?
//  if (user_input.line_style.IsSpecialDefault()) {
//    SPDLOG_TRACE("ImagePainter::CheckInputStyle: User's bounding box line style is "
//                 "'Default', switching to painter's default={:s}.",
//                 default_bbox2d_style_.line_style);
//    style.line_style = default_bbox2d_style_.line_style;
//  } else {
//    SPDLOG_TRACE("ImagePainter::CheckInputStyle: Using user's bounding box line style {:s}",
//                 user_input.line_style);
//  }

//  // Do we need to replace the text style?
//  if (user_input.text_style.IsValid()) {
//    SPDLOG_TRACE("ImagePainter::CheckInputStyle: Using user's bounding box text style {:s}",
//                 user_input.text_style);
//  } else {
//    SPDLOG_TRACE("ImagePainter::CheckInputStyle: User's bounding box text style is "
//                 "invalid, switching to painter's default={:s}.",
//                 default_bbox2d_style_.text_style);
//    style.text_style = default_bbox2d_style_.text_style;
//  }

//  return style;
//}

std::unique_ptr<Painter> CreatePainter() {
  SPDLOG_DEBUG("Creating image painter.");
  return std::unique_ptr<Painter>(new ImagePainter());
}

} // namespace viren2d
