#include <utility>
#include <sstream>
#include <iomanip>
#include <iostream> // TODO remove after switching to spdlog
#include <stdexcept>
#include <cassert>
#include <cstring> // memcpy

// TODO add cmake options to set log levels & disable debug:
// https://github.com/gabime/spdlog/wiki/0.-FAQ#how-to-remove-all-debug-statements-at-compile-time-
// different log levels (nice concise summary) - https://stackoverflow.com/questions/2031163/when-to-use-the-different-log-levels
//TODO Decide whether to trace each method invocation - or debug-log the "most" important (preference: trace each call, so we can actually trace what's going on if it should ever break)
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <math.h>

#include <cairo/cairo.h>

// public viren2d headers
#include <viren2d/drawing.h>
#include <viren2d/colors.h>
#include <viren2d/math.h>
#include <viren2d/string_utils.h>

// private viren2d headers
#include <helpers/drawing_helpers.h>



//-------------------------------------------------
// Preprocessor macros to initialize the
// library upon loading. This is needed to
// set up logging.
// Macros taken from:
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
namespace {
/** Predefined default line style - can be replaced separately within each @see Painter. */
const static LineStyle kdefault_line_style = LineStyle(2, Color(NamedColor::Azure));


/** Predefined default arrow style - can be replaced separately within each @see Painter. */
const static ArrowStyle kdefault_arrow_style = ArrowStyle(4, Color(NamedColor::ForestGreen), 0.1, 20);

/** Predefined default text style - can be replaced separately within each @see Painter. */
const static TextStyle kdefault_text_style = TextStyle(16, "monospace", Color::Black);
}  // anonymous namespace


INITIALIZER(initialize)
{
  spdlog::set_level(spdlog::level::debug);
//  spdlog::set_pattern("[%l][%@, %!] %v");
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

  void SetDefaultLineStyle(const LineStyle &line_style) override;
  LineStyle GetDefaultLineStyle() const override;

  void SetDefaultArrowStyle(const ArrowStyle &arrow_style) override;
  ArrowStyle GetDefaultArrowStyle() const override;

  void SetDefaultTextStyle(const TextStyle &text_style) override;
  TextStyle GetDefaultTextStyle() const override;


  void DrawGrid(const Vec2d &top_left, const Vec2d &bottom_right,
                          double spacing_x, double spacing_y,
                const LineStyle &line_style) override {
    helpers::DrawGrid(surface_, context_, top_left, bottom_right,
                      spacing_x, spacing_y,
                      CheckInputStyle(line_style));
  }


protected:
  void DrawArcImpl(const Vec2d &center, double radius,
                   double angle1, double angle2,
                   const LineStyle &line_style,
                   bool include_center,
                   const Color &fill_color) override {
    helpers::DrawArc(surface_, context_, center, radius,
                     angle1, angle2,
                     CheckInputStyle(line_style),
                     include_center, fill_color);
  }


  void DrawArrowImpl(const Vec2d &from, const Vec2d &to,
                     const ArrowStyle &arrow_style) override {
    helpers::DrawArrow(surface_, context_, from, to,
                       CheckInputStyle(arrow_style));
  }


  void DrawCircleImpl(const Vec2d &center, double radius,
                      const LineStyle &line_style,
                      const Color &fill_color) override {
    helpers::DrawCircle(surface_, context_, center, radius,
                        CheckInputStyle(line_style), fill_color);
  }


  void DrawEllipseImpl(const Ellipse &ellipse, const LineStyle &line_style,
                       const Color &fill_color) override {
    helpers::DrawEllipse(surface_, context_, ellipse,
                         CheckInputStyle(line_style), fill_color);
  }


  void DrawLineImpl(const Vec2d &from, const Vec2d &to,
                    const LineStyle &line_style) override {
    helpers::DrawLine(surface_, context_, from, to,
                      CheckInputStyle(line_style));
  }


  void DrawRectImpl(const Rect &rect, const LineStyle &line_style,
                    const Color &fill_color) override {
    helpers::DrawRect(surface_, context_, rect,
                      CheckInputStyle(line_style), fill_color);
  }


  void DrawTextImpl(const std::string &text, const Vec2d &position,
                    TextAnchor text_anchor, const TextStyle &text_style,
                    const Vec2d &padding, double rotation) override {
    helpers::DrawText(surface_, context_, text, position, text_anchor,
                      CheckInputStyle(text_style), default_text_style_,
                      padding, rotation,
                      LineStyle::Invalid, Color::Invalid, 0.0);
  }


  void DrawTextBoxImpl(const std::string &text, const Vec2d &position,
                       TextAnchor text_anchor, const TextStyle &text_style,
                       const Vec2d &padding, double rotation,
                       const LineStyle &box_line_style, const Color &box_fill_color,
                       double box_corner_radius) override {
    helpers::DrawText(surface_, context_, text, position, text_anchor,
                      CheckInputStyle(text_style), default_text_style_,
                      padding, rotation, CheckInputStyle(box_line_style),
                      box_fill_color, box_corner_radius);
  }


private:
  cairo_surface_t *surface_;
  cairo_t *context_;

  LineStyle default_line_style_;
  ArrowStyle default_arrow_style_;
  TextStyle default_text_style_;

  void ApplyDefaultStyles();

  const LineStyle &CheckInputStyle(const LineStyle &user_input) const;
  const ArrowStyle &CheckInputStyle(const ArrowStyle &user_input) const;
  const TextStyle &CheckInputStyle(const TextStyle &user_input) const;
};


ImagePainter::ImagePainter() : Painter(),
  surface_(nullptr), context_(nullptr),
  default_line_style_(kdefault_line_style),
  default_arrow_style_(kdefault_arrow_style),
  default_text_style_(kdefault_text_style) {
  SPDLOG_TRACE("ImagePainter::Default constructor.");
}


ImagePainter::~ImagePainter() {
  SPDLOG_TRACE("ImagePainter::Destructor.");
  if (context_)
    cairo_destroy(context_);
  if (surface_)
    cairo_surface_destroy(surface_);
}


ImagePainter::ImagePainter(const ImagePainter &other) // copy constructor
  : Painter(),
    surface_(nullptr), context_(nullptr),
    default_line_style_(other.default_line_style_),
    default_arrow_style_(other.default_arrow_style_),
    default_text_style_(other.default_text_style_) {
  SPDLOG_DEBUG("ImagePainter::Copy constructor.");
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

    ApplyDefaultStyles();  // Needed after each context change
  }
}


// Move c'tor rips off the other's memory but simply
// copies the default style settings
ImagePainter::ImagePainter(ImagePainter &&other) noexcept
  : Painter(),
    surface_(std::exchange(other.surface_, nullptr)),
    context_(std::exchange(other.context_, nullptr)),
    default_line_style_(other.default_line_style_),
    default_arrow_style_(other.default_arrow_style_),
    default_text_style_(other.default_text_style_) {
  SPDLOG_TRACE("ImagePainter::Move constructor.");
  // No need to ApplyDefaultStyles() - the "other"
  // should've already set these up (and we're
  // simply stealing their context)
}


ImagePainter& ImagePainter::operator=(const ImagePainter &other) { // Copy assignment
  SPDLOG_TRACE("ImagePainter::Copy assignment operator.");
  return *this = ImagePainter(other);
}


ImagePainter& ImagePainter::operator=(ImagePainter &&other) noexcept { // Move assignment
  SPDLOG_TRACE("ImagePainter::Move assignment operator.");
  std::swap(surface_, other.surface_);
  std::swap(context_, other.context_);
  std::swap(default_line_style_, other.default_line_style_);
  std::swap(default_arrow_style_, other.default_arrow_style_);
  std::swap(default_text_style_, other.default_text_style_);
  return *this;
}


bool ImagePainter::IsValid() const {
  SPDLOG_DEBUG("ImagePainter::IsValid().");
  return (surface_ != nullptr) && (context_ != nullptr);
}


void ImagePainter::SetCanvas(int width, int height,
                             const Color &color) {
  SPDLOG_DEBUG("ImagePainter::SetCanvas(width={:d}, height={:d}, color={:s}).",
               width, height, color);
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
  if (!context_) {
    context_ = cairo_create(surface_);
    ApplyDefaultStyles();  // Needed after each context change
  }

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

    ApplyDefaultStyles();  // Needed after each context change

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


void ImagePainter::SetDefaultLineStyle(const LineStyle &line_style) {
  if (!line_style.IsValid()) {
    std::stringstream s;
    s << "Cannot change the default line style to an invalid configuration: "
      << line_style;
    throw std::invalid_argument(s.str());
  }
  default_line_style_ = line_style;
  ApplyDefaultStyles();
}


LineStyle ImagePainter::GetDefaultLineStyle() const {
  return default_line_style_;
}


void ImagePainter::SetDefaultArrowStyle(const ArrowStyle &arrow_style) {
  if (!arrow_style.IsValid()) {
    std::stringstream s;
    s << "Cannot change the default arrow style to an invalid configuration: "
      << arrow_style;
    throw std::invalid_argument(s.str());
  }
  default_arrow_style_ = arrow_style;
  ApplyDefaultStyles();
}


ArrowStyle ImagePainter::GetDefaultArrowStyle() const {
  return default_arrow_style_;
}


void ImagePainter::SetDefaultTextStyle(const TextStyle &text_style) {
  if (!text_style.IsValid()) {
    std::stringstream s;
    s << "Cannot change the default text style to an invalid configuration: "
      << text_style;
    throw std::invalid_argument(s.str());
  }
  default_text_style_ = text_style;
  ApplyDefaultStyles();
}


TextStyle ImagePainter::GetDefaultTextStyle() const {
  return default_text_style_;
}


void ImagePainter::ApplyDefaultStyles() {
  if (context_) {
    helpers::ApplyLineStyle(context_, default_line_style_, false);

    // The arrow style will not be preset in the context (as
    // an arrow is basically just a line and would thus overwrite
    // the default line style)

    helpers::ApplyTextStyle(context_, default_text_style_);
  }
}


const LineStyle &
ImagePainter::CheckInputStyle(const LineStyle &user_input) const {
  if (user_input.IsSpecialDefault()) {
    return default_line_style_;
  } else {
    return user_input;
  }
}


const ArrowStyle &
ImagePainter::CheckInputStyle(const ArrowStyle &user_input) const {
  if (user_input.IsSpecialInvalid()) {
    throw std::invalid_argument("ArrowStyle::Invalid is not supported as input.");
  }

  if (user_input.IsSpecialDefault()) {
    return default_arrow_style_;
  } else {
    return user_input;
  }
}


const TextStyle &
ImagePainter::CheckInputStyle(const TextStyle &user_input) const {
  if (user_input.IsValid()) {
    return user_input;
  } else {
    return default_text_style_;
  }
}

std::unique_ptr<Painter> CreatePainter() {
  return std::unique_ptr<Painter>(new ImagePainter());
}

} // namespace viren2d
