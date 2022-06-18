#ifndef __VIREN2D_DRAWING_HELPERS_H__
#define __VIREN2D_DRAWING_HELPERS_H__

#include <stdexcept>
#include <sstream>
#include <vector>

#include <math.h>
#include <cairo/cairo.h>

#include <viren2d/colors.h>
#include <viren2d/styles.h>
#include <viren2d/drawing.h>

#include <helpers/logging.h>


namespace viren2d {
namespace helpers {
//---------------------------------------------------- Used by all drawing helpers

/**
 * @brief Sets the source color. Should be used by all
 * drawing methods (unless you know what you are doing).
 *
 * Issue in a nutshell: Cairo's `ARGB` format uses the same
 * memory layout as OpenCV's `BGRA` format. We, however,
 * want to work with `RGB(A)` images. Thus, we simply flip
 * red and blue when setting the color.
 *
 * This seemed to be the easiest/least confusing option.
 */
inline void ApplyColor(cairo_t *context, const Color &color) {
  if (color.IsValid() && context) {
    SPDLOG_TRACE("helpers::ApplyColor: {:s}.", color);
    cairo_set_source_rgba(context, color.blue, color.green,
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

  std::ostringstream s;
  s << "Line cap style \"" << cap
    << "\" is not yet mapped to Cairo type!";
  throw std::runtime_error(s.str());
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
  std::ostringstream s;
  s << "Line join style \"" << join
    << "\" is not yet mapped to Cairo type!";
  throw std::runtime_error(s.str());
}


/** @brief Changes the given Cairo context to use the given MarkerStyle definitions. */
inline void ApplyMarkerStyle(cairo_t *context, const MarkerStyle &style) {
  SPDLOG_TRACE("helpers::ApplyMarkerStyle: style={:s}.", style);

  if (!context) {
    return;
  }

  cairo_set_line_width(context, style.thickness);
  cairo_set_line_cap(context, LineCap2Cairo(style.line_cap));
  cairo_set_line_join(context, LineJoin2Cairo(style.line_join));
  ApplyColor(context, style.color);
}


/** @brief Changes the given Cairo context to use the given LineStyle definitions. */
inline void ApplyLineStyle(cairo_t *context, const LineStyle &style,
                           bool ignore_dash = false) {
  SPDLOG_TRACE("helpers::ApplyLineStyle: style={:s}, ignore_dash={:s}.",
               style, ignore_dash);

  if (!context) {
    return;
  }

  cairo_set_line_width(context, style.line_width);
  cairo_set_line_cap(context, LineCap2Cairo(style.line_cap));
  cairo_set_line_join(context, LineJoin2Cairo(style.line_join));
  ApplyColor(context, style.color);

  if (!style.dash_pattern.empty() && !ignore_dash) {
    // https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-dash
    const double *dash = &style.dash_pattern[0];
    cairo_set_dash(context, dash,
                   static_cast<int>(style.dash_pattern.size()),
                   0); // We don't need/support an offset into the dash pattern
  }
}


/** @brief Changes the given Cairo context to use the given TextStyle definitions. */
inline void ApplyTextStyle(cairo_t *context, const TextStyle &text_style, bool apply_color) {
  SPDLOG_TRACE("helpers::ApplyTextStyle: {:s}.", text_style);

  if (!context) {
    return;
  }
  cairo_select_font_face(context, text_style.font_family.c_str(),
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
  cairo_set_font_size(context, static_cast<double>(text_style.font_size));

  if (apply_color) {
    ApplyColor(context, text_style.color);
  }
}


/**
 * Ensure that the canvas is set up correctly. Should be
 * called within each drawing helper function.
 */
inline void CheckCanvas(cairo_surface_t *surface, cairo_t *context) {
  if (!surface) {
    throw std::logic_error("Invalid cairo surface (nullptr)."
                           " Did you forget to set up the canvas first?");
  }

  cairo_status_t surf_stat = cairo_surface_status(surface);
  if (surf_stat != CAIRO_STATUS_SUCCESS) {
    std::ostringstream s;
    s << "Invalid Cairo surface status (" << surf_stat
      << "), check "
         "https://www.cairographics.org/manual/cairo-Error-handling.html#cairo-status-t "
         "for details.";
    throw std::runtime_error(s.str());
  }

  if (!context) {
    throw std::runtime_error("Invalid Cairo context (nullptr), "
                             "cannot draw anymore.");
  }
}


//---------------------------------------------------- Text metrics
/**
 * @brief Encapsulates a single text line to be drawn onto the canvas.
 *
 * TODO doc height depends on @see TextStyle
 * asc+desc
 * https://www.cairographics.org/manual/cairo-cairo-scaled-font-t.html#cairo-font-extents-t
 * or exact height
 *
 */
class TextLine {
 public:
//TODO doc
  TextLine(const char *line, cairo_t *context,
           cairo_font_extents_t *font_metrics);

  void Align(Vec2d anchor_position, TextAnchor anchor);

//  Rect BoundingBox(Vec2d padding = {0, 0}, double corner_radius = 0.0) const;

  void PlaceText(cairo_t *context) const;

  double Width() const { return width; }

  double Height() const { return height; }


private:
  /**
   * @brief Pointer to the C-string text used to
   * initialize this @see TextLine instance.
   */
  const char *text;


  /**
   * @brief Reference point for cairo_show_text, which
   * will be set after @see Align().
   */
  Vec2d reference_point;


  /** @brief Exact width of the bounding box. */
  double width;


  /**
   * @brief Height of the bounding box (either exact or
   * specified by the font metrics).
   */
  double height;


  /**
   * @brief Horizontal distance from the origin to the
   * leftmost part of the glyphs
   */
  double bearing_x;


  /**
   * @brief Vertical distance from the origin to the
   * top edge of the bounding box defined by this TextExtent
   */
  double bearing_y;

  void Init(cairo_t *context,
            cairo_font_extents_t *font_metrics);
};

/**
 * @brief The MultilineText class
 *
 * TODO doc
 * TODO upon initialization, height/width are tight:
 * * font height + exact text width
 * after Align, ...
 */
class MultilineText {
 public:
  MultilineText(const std::vector<const char*> &text,
                const TextStyle &text_style, cairo_t *context);

  void Align(Vec2d anchor_point, TextAnchor anchor,
             Vec2d padding, Vec2d fixed_size);

  Rect BoundingBox(double corner_radius = 0.0) const; //TODO make padding a member; set in Align; reuse in BoundingBox()

  void PlaceText(cairo_t *context) const;

  double Width() const;
  double Height() const;

private:
  /**
   * @brief Top left corner of the bounding box which
   * contains all (properly spaced) text lines.
   * Will be set after @see Align().
   */
  Vec2d top_left;

  Vec2d padding;

  Vec2d fixed_size;

  double width;

  double height;

  TextStyle style;

  std::vector<TextLine> lines;
};


//---------------------------------------------------- Available drawing helpers
// These declarations should stay alphabetically sorted:

void DrawArc(cairo_surface_t *surface, cairo_t *context,
             Vec2d center, double radius, double angle1, double angle2,
             const LineStyle &line_style, bool include_center,
             Color fill_color);


void DrawArrow(cairo_surface_t *surface, cairo_t *context,
               Vec2d from, Vec2d to, const ArrowStyle &arrow_style);


void DrawBoundingBox2D(cairo_surface_t *surface, cairo_t *context,
                       Rect rect, const std::vector<const char *> &label,
                       const BoundingBox2DStyle &style);


inline void DrawCircle(cairo_surface_t *surface, cairo_t *context,
                       Vec2d center, double radius,
                       const LineStyle &line_style,
                       const Color &fill_color) {
  DrawArc(surface, context, center, radius, 0, 360, line_style,
          false, fill_color);
}


void DrawEllipse(cairo_surface_t *surface, cairo_t *context,
                 Ellipse ellipse, const LineStyle &line_style,
                 Color fill_color);


void DrawGrid(cairo_surface_t *surface, cairo_t *context,
              Vec2d top_left, Vec2d bottom_right,
              double spacing_x, double spacing_y,
              const LineStyle &line_style);


void DrawLine(cairo_surface_t *surface, cairo_t *context,
              Vec2d from, Vec2d to, const LineStyle &line_style);


void DrawMarker(cairo_surface_t *surface, cairo_t *context,
                Vec2d pos, const MarkerStyle &style);


void DrawPolygon(cairo_surface_t *surface, cairo_t *context,
                 const std::vector<Vec2d> points,
                 const LineStyle &line_style,
                 Color fill_color);


void DrawRect(cairo_surface_t *surface, cairo_t *context,
              Rect rect, const LineStyle &line_style,
              Color fill_color);


void DrawText(cairo_surface_t *surface, cairo_t *context,
              const std::vector<const char *> &text,
              Vec2d anchor_position, TextAnchor anchor,
              const TextStyle &text_style, const Vec2d &padding,
              double rotation, const LineStyle &box_line_style,
              const Color &box_fill_color, double box_corner_radius,
              const Vec2d &fixed_box_size);


/** Creates a path for a rectangle with rounded corners.
 *  Assumes that the viewport is already translated (and optionally
 *  rotated).
 */
void PathHelperRoundedRect(cairo_t *context, Rect rect);

} // namespace helpers
} // namespace viren2d

#endif // __VIREN2D_DRAWING_HELPERS_H__
