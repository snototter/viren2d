#ifndef __VIREN2D_DRAWING_HELPERS_H__
#define __VIREN2D_DRAWING_HELPERS_H__

#include <stdexcept>
#include <sstream>
#include <vector>
#include <functional>

#include <math.h>
#include <cairo/cairo.h>

#include <viren2d/colors.h>
#include <viren2d/styles.h>
#include <viren2d/drawing.h>

#include <helpers/logging.h>


namespace viren2d {
namespace helpers {
//---------------------------------------------------- Sanity checks
// To be used by all drawing helpers.

/// Ensures that the canvas is set up correctly. Should be
/// called within each drawing helper function.
inline void CheckCanvas(cairo_surface_t *surface, cairo_t *context) {
  if (!surface) {
    throw std::logic_error(
          "Invalid cairo surface (nullptr). "
          "Did you forget to set up the canvas first?");
  }

  cairo_status_t surf_stat = cairo_surface_status(surface);
  if (surf_stat != CAIRO_STATUS_SUCCESS) {
    std::ostringstream s;
    s << "Invalid Cairo surface status (" << surf_stat
      << "), check "
         "https://www.cairographics.org/manual/cairo-Error-handling.html#cairo-status-t "
         "for details.";
    throw std::logic_error(s.str());
  }

  if (!context) {
    throw std::logic_error(
          "Invalid Cairo context (nullptr) - "
          "cannot continue drawing.");
  }
}


// TODO should replace the throwing CheckCanvas calls everywhere.
// Once this is done, rename it back to CheckCanvas.
/// Ensures that the canvas is set up correctly. Should be
/// called within each drawing helper function.
inline bool CheckCanvasNoExcept(cairo_surface_t *surface, cairo_t *context) {
  if (!surface) {
    SPDLOG_WARN(
          "Invalid cairo surface (nullptr). "
          "Did you forget to set up the canvas first?");
    return false;
  }

  cairo_status_t surf_stat = cairo_surface_status(surface);
  if (surf_stat != CAIRO_STATUS_SUCCESS) {
    std::ostringstream s;
    s << "Invalid Cairo surface status (" << surf_stat
      << "), check "
         "https://www.cairographics.org/manual/cairo-Error-handling.html#cairo-status-t "
         "for details.";
    SPDLOG_WARN(s.str());
    return false;
  }

  if (!context) {
    SPDLOG_WARN(
          "Invalid Cairo context (nullptr) - "
          "cannot continue drawing.");
    return false;
  }

  return true;
}


/// Checks if the line style is valid.
inline void CheckLineStyle(const LineStyle &style) {
  if (!style.IsValid()) {
    std::string s("Cannot draw with invalid line style ");
    s += style.ToDetailedString();
    s += '!';
    throw std::invalid_argument(s);
  }
}


// TODO should replace the throwing CheckLineStyle calls everywhere.
// Once this is done, rename it back to CheckLineStyle.
/// Checks if the line style is valid.
inline void CheckLineStyleNoExcept(const LineStyle &style) {
  if (!style.IsValid()) {
    std::string s("Cannot draw with invalid line style ");
    s += style.ToDetailedString();
    s += '!';
    SPDLOG_WARN(s);
    return false;
  } else {
    return true;
  }
}


/// Checks if line style *or* fill color are valid.
/// To be used in functions which allow only filling or
/// only drawing a shape's contour.
inline void CheckLineStyleAndFill(
    const LineStyle &style, Color &fill_color) {
  if (fill_color.IsSpecialSame()) {
    fill_color = style.color.WithAlpha(fill_color.alpha);
  }

  if (!style.IsValid() && !fill_color.IsValid()) {
    std::string s(
          "Cannot draw with both invalid line "
          "style and invalid fill color: ");
    s += style.ToDetailedString();
    s += " and ";
    s += fill_color.ToString();
    s += '!';
    throw std::invalid_argument(s);
  }
}


// TODO should replace the throwing CheckLineStyleAndFill calls everywhere.
// Once this is done, rename it back to CheckLineStyleAndFill.
/// Checks if line style *or* fill color are valid.
/// To be used in functions which allow only filling or
/// only drawing a shape's contour.
inline void CheckLineStyleAndFillNoExcept(
    const LineStyle &style, Color &fill_color) {
  if (fill_color.IsSpecialSame()) {
    fill_color = style.color.WithAlpha(fill_color.alpha);
  }

  if (!style.IsValid() && !fill_color.IsValid()) {
    std::string s(
          "Cannot draw with both invalid line "
          "style and invalid fill color: ");
    s += style.ToDetailedString();
    s += " and ";
    s += fill_color.ToString();
    s += '!';
    SPDLOG_WARN(s);
    return false;
  } else {
    return true;
  }
}


//---------------------------------------------------- ApplyXXX
// To be used by all drawing helpers.

/// Sets the source color. **Should be used by all
/// drawing methods**, unless you know what you are doing.
///
/// In a nutshell: Cairo's `ARGB` format uses the same
/// memory layout as OpenCV's `BGRA` format. We, however,
/// want to work with `RGB(A)` images. Thus, we simply flip
/// `red` and `blue` when setting the color.
inline void ApplyColor(cairo_t *context, const Color &color) {
  if (context && color.IsValid()) {
    SPDLOG_TRACE("helpers::ApplyColor: {:s}.", color);
    cairo_set_source_rgba(
          context, color.blue, color.green,
          color.red, color.alpha);
  }
}


inline cairo_line_cap_t LineCap2Cairo(LineCap cap) {
  switch (cap) {
    case LineCap::Butt:
      return CAIRO_LINE_CAP_BUTT;

    case LineCap::Round:
      return CAIRO_LINE_CAP_ROUND;

    case LineCap::Square:
      return CAIRO_LINE_CAP_SQUARE;
  }

  // This exception can remain as it would be caused by an implementation
  // error (and Cairo will very likely not introduce a new cap style).
  std::string s("Line cap style \"");
  s += LineCapToString(cap);
  s += "\" is not yet mapped to corresponding Cairo type!";
  throw std::logic_error(s);
}


inline cairo_line_join_t LineJoin2Cairo(LineJoin join) {
  switch (join) {
    case LineJoin::Miter:
      return CAIRO_LINE_JOIN_MITER;

    case LineJoin::Bevel:
      return CAIRO_LINE_JOIN_BEVEL;

    case LineJoin::Round:
      return CAIRO_LINE_JOIN_ROUND;
  }

  // This exception can remain as it would be caused by an implementation
  // error (and Cairo will very likely not introduce a new cap style).
  std::string s("Line join style \"");
  s += LineJoinToString(join);
  s += "\" is not yet mapped to corresponding Cairo type!";
  throw std::logic_error(s);
}


/// Changes the given Cairo context to use the
/// given MarkerStyle definition.
inline void ApplyMarkerStyle(
    cairo_t *context,
    const MarkerStyle &style) {
  SPDLOG_TRACE(
        "helpers::ApplyMarkerStyle: style={:s}.", style);

  if (!context) {
    return;
  }

  cairo_set_line_width(context, style.thickness);
  cairo_set_line_cap(context, LineCap2Cairo(style.cap));
  cairo_set_line_join(context, LineJoin2Cairo(style.join));
  ApplyColor(context, style.color);
}


/// Changes the given Cairo context to use the
/// given LineStyle definitions.
inline void ApplyLineStyle(
    cairo_t *context,
    const LineStyle &style,
    bool ignore_dash = false) {
  SPDLOG_TRACE(
        "helpers::ApplyLineStyle: style={:s}, ignore_dash={:s}.",
        style, ignore_dash);

  if (!context) {
    return;
  }

  cairo_set_line_width(context, style.width);
  cairo_set_line_cap(context, LineCap2Cairo(style.cap));
  cairo_set_line_join(context, LineJoin2Cairo(style.join));
  ApplyColor(context, style.color);

  if (!style.dash_pattern.empty() && !ignore_dash) {
    // https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-dash
    const double *dash = &style.dash_pattern[0];
    cairo_set_dash(
          context, dash,
          static_cast<int>(style.dash_pattern.size()),
          style.dash_offset);
  }
}


/// Changes the given Cairo context to use the
/// given TextStyle definitions.
inline void ApplyTextStyle(
    cairo_t *context,
    const TextStyle &text_style,
    bool apply_color) {
  SPDLOG_TRACE(
        "helpers::ApplyTextStyle: {:s}.", text_style);

  if (!context) {
    return;
  }

  cairo_select_font_face(
      context, text_style.family.c_str(),
      (text_style.italic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL),
      (text_style.bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL));
  // TODO: dev2user distance changes when the surface
  // is rotated! - document this behavior and don't
  // adjust the font size!
//  // For image painting, the device to user
//  // scaling factor should always be 1.
//  // Nevertheless, check if we need to adjust
//  // the given font size (in case we ever change
//  // to an SVG painter).
//  double ux = 1.0;
//  double uy = 1.0;
//  cairo_device_to_user_distance(context, &ux, &uy);
//  double px = (ux > uy) ? ux : uy;
//  cairo_set_font_size(context, static_cast<double>(text_style.font_size) * px);
  cairo_set_font_size(
      context, static_cast<double>(text_style.size));

  if (apply_color) {
    ApplyColor(context, text_style.color);
  }
}


//---------------------------------------------------- Text metrics
/// Encapsulates a single text line to be drawn onto
/// the canvas.
///
/// Workflow:
///   1) Initialize - the caller must ensure that the
///      text pointed to by `line` stays in memory while
///      using this TextLine object.
///   2) Call `Align` to compute the reference position.
///   3) Draw onto canvas via `PlaceText`
class SingleLineText {
public:
  SingleLineText(
      const char *line, cairo_t *context,
      cairo_font_extents_t *font_metrics);

  void Align(Vec2d anchor_position, Anchor anchor);

  void PlaceText(cairo_t *context) const;

  double Width() const { return width; }

  double Height() const { return height; }

private:
  /// Pointer to the C-string text used to
  /// initialize this `TextLine` instance.
  const char *text;


  /// Reference point for `cairo_show_text`, which
  /// will be set **after** `Align` has been called.
  Vec2d reference_point;


  /// Exact width of the bounding box.
  double width;


  /// Height of the bounding box (either exact or
  /// specified by the font metrics).
  double height;


  /// Horizontal distance from the origin to the
  /// leftmost part of the glyphs.
  double bearing_x;


  /// Vertical distance from the origin to the
  /// top edge of the bounding box defined by
  /// this TextExtent.
  double bearing_y;

  void Init(
      cairo_t *context,
      cairo_font_extents_t *font_metrics);
};


/// Encapsulates multiple lines of text.
/// Computes the text extents, takes care of
/// alignment, and finally allows to place
/// the lines onto the canvas.
///
/// The text height will *always* depend on the
/// chosen font - this is to allow consistent
/// alignment in multi-line settings.
///
/// Workflow:
///   1) Initialize - the caller must ensure that the
///      text stays in memory while using this
///      MultilineText object.
///   2) Call `Align` to compute the reference position.
///   3) Draw onto canvas via `PlaceText`
/// Users must ensure that the text lines
/// stay in memory as long as this object
/// is in use!
class MultiLineText {
public:
  MultiLineText(
      const std::vector<std::string> &text,
      const TextStyle &text_style,
      cairo_t *context);


  /// Computes reference points for each line.
  /// Must be called **before** `PlaceText`
  void Align(
      Vec2d anchor_point, Anchor anchor,
      Vec2d padding, Vec2d fixed_size);


  /// Returns the axis-aligned bounding box.
  /// Valid results are only available
  /// **after** `Align` was called.
  Rect BoundingBox(
      double corner_radius = 0.0) const;


  /// Draws the text lines onto the given context.
  void PlaceText(cairo_t *context) const;


  /// Returns the width of the text box. This might differ from
  /// the actual width (maximum line width and padding) if  a fixed size box
  /// is requested.
  double Width() const;


  /// Returns the height of the text box. This might differ from
  /// the actual height (induced by lines and padding) if  a fixed size box
  /// is requested.
  double Height() const;

  /// Returns the height (font extent + line spacing) for the given line
  /// number - because we don't include line spacing for the first line.
  double LineHeight(std::size_t idx) const;


private:
  /// Top left corner of the bounding box which
  /// contains all (properly spaced) text lines.
  /// Will be set after `Align` has been called.
  Vec2d top_left;


  /// Padding between reference position and
  /// start of the glyphs.
  Vec2d padding;


  /// If set, `Align` will use this as a size
  /// hint instead of the actual text extent.
  Vec2d fixed_size;


  /// Actual text width (maximum line length).
  double width;


  /// Actual height of all text lines (computed from
  /// font extent and line spacing)
  double height;


  /// The customized style to be used for drawing these text lines.
  TextStyle style;


  /// The text lines which should be drawn.
  std::vector<SingleLineText> lines;
};


//---------------------------------------------------- Available drawing helpers
// These declarations should stay alphabetically sorted:

bool DrawArc(
    cairo_surface_t *surface, cairo_t *context,
    Vec2d center, double radius, double angle1, double angle2,
    const LineStyle &line_style, bool include_center,
    Color fill_color);


bool DrawArrow(
    cairo_surface_t *surface, cairo_t *context,
    Vec2d from, Vec2d to, const ArrowStyle &arrow_style);


bool DrawBoundingBox2D(cairo_surface_t *surface, cairo_t *context,
    Rect rect, const std::vector<std::string> &label,
    const BoundingBox2DStyle &style);


inline bool DrawCircle(
    cairo_surface_t *surface, cairo_t *context,
    Vec2d center, double radius, const LineStyle &line_style,
    const Color &fill_color) {
  return DrawArc(surface, context, center, radius, 0, 360, line_style,
          false, fill_color);
}


void DrawEllipse(
    cairo_surface_t *surface, cairo_t *context,
    Ellipse ellipse, const LineStyle &line_style,
    Color fill_color);


void DrawGrid(
    cairo_surface_t *surface, cairo_t *context,
    Vec2d top_left, Vec2d bottom_right,
    double spacing_x, double spacing_y,
    const LineStyle &line_style);


Line2d DrawHorizonLineImpl(
    cairo_surface_t *surface, cairo_t *context,
    const Matrix3x3d &K, const Matrix3x3d &R, const Vec3d &t,
    const LineStyle &line_style, const Vec2i &img_size);


bool DrawImage(cairo_surface_t *surface, cairo_t *context,
    const ImageBuffer &image, const Vec2d &position, Anchor anchor,
    double alpha, double scale_x, double scale_y,
    double rotation, double clip_factor, LineStyle line_style);


void DrawLine(
    cairo_surface_t *surface, cairo_t *context,
    Vec2d from, Vec2d to, const LineStyle &line_style);


void DrawMarker(
    cairo_surface_t *surface, cairo_t *context,
    Vec2d pos, const MarkerStyle &style);


void DrawPolygon(
    cairo_surface_t *surface, cairo_t *context,
    const std::vector<Vec2d> &points,
    const LineStyle &line_style, Color fill_color);


void DrawRect(
    cairo_surface_t *surface, cairo_t *context,
    Rect rect, const LineStyle &line_style,
    Color fill_color);


Rect DrawText(cairo_surface_t *surface, cairo_t *context,
    const std::vector<std::string> &text,
    const Vec2d &anchor_position, Anchor anchor,
    const TextStyle &text_style, const Vec2d &padding,
    double rotation, const LineStyle &box_line_style,
    const Color &box_fill_color, double box_corner_radius,
    const Vec2d &fixed_box_size);


bool DrawTrajectory(
    cairo_surface_t *surface, cairo_t *context,
    const std::vector<Vec2d> &points, const LineStyle &style,
    Color color_fade_out, bool oldest_position_first,
    const std::function<double(double)> &mix_factor);


bool DrawXYZAxes(
    cairo_surface_t *surface, cairo_t *context,
    const Matrix3x3d &K, const Matrix3x3d &R, const Vec3d &t,
    const Vec3d &origin, const Vec3d &lengths, const ArrowStyle &style,
    const Color &color_x, const Color &color_y, const Color &color_z,
    const Vec2i &img_size);


/// Creates a path for a rectangle with rounded corners.
/// Assumes that the viewport is already translated (and optionally
/// rotated)!
void PathHelperRoundedRect(cairo_t *context, Rect rect);

} // namespace helpers
} // namespace viren2d

#endif // __VIREN2D_DRAWING_HELPERS_H__
