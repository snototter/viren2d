#ifndef __VIREN2D_DRAWING_HELPERS_H__
#define __VIREN2D_DRAWING_HELPERS_H__

#include <stdexcept>
#include <sstream>

#include <math.h>
#include <cairo/cairo.h>

#include <viren2d/colors.h>
#include <viren2d/styles.h>
#include <viren2d/drawing.h>


namespace viren2d {
namespace helpers {
//---------------------------------------------------- Used by all drawing helpers

/**
 * @brief Sets the source color. Should be used by all
 * drawing methods (unless you know what you are doing).
 *
 * Issue in a nutshell: Cairo's ARGB format uses the same
 * memory layout as OpenCV's BGRA format. We, however,
 * want to work with RGB(A) images. Thus, we simply flip
 * red and blue when setting the color.
 *
 * This seemed to be the easiest/least confusing option.
 */
inline void ApplyColor(cairo_t *context, const Color &color) {
  cairo_set_source_rgba(context, color.blue, color.green,
                        color.red, color.alpha);
}


inline cairo_line_cap_t LineCap2Cairo(const LineStyle &line_style) {
  switch (line_style.line_cap) {
    case LineCap::Butt:
      return CAIRO_LINE_CAP_BUTT;

    case LineCap::Round:
      return CAIRO_LINE_CAP_ROUND;

    case LineCap::Square:
      return CAIRO_LINE_CAP_SQUARE;
  }
  std::stringstream s;
  s << "Line cap style \"" << static_cast<int>(line_style.line_cap)
    << "\" is not yet mapped to Cairo type!";
  throw std::runtime_error(s.str());
}


inline cairo_line_join_t LineJoin2Cairo(const LineStyle &line_style) {
  switch (line_style.line_join) {
    case LineJoin::Miter:
      return CAIRO_LINE_JOIN_MITER;

    case LineJoin::Bevel:
      return CAIRO_LINE_JOIN_BEVEL;

    case LineJoin::Round:
      return CAIRO_LINE_JOIN_ROUND;
  }
  std::stringstream s;
  s << "Line join style \"" << static_cast<int>(line_style.line_join)
    << "\" is not yet mapped to Cairo type!";
  throw std::runtime_error(s.str());
}


/** @brief Changes the given Cairo context to use the given LineStyle definitions. */
inline void ApplyLineStyle(cairo_t *context,
                           const LineStyle &line_style,
                           bool ignore_dash = false) {
  cairo_set_line_width(context, line_style.line_width);
  cairo_set_line_cap(context, LineCap2Cairo(line_style));
  cairo_set_line_join(context, LineJoin2Cairo(line_style));
  ApplyColor(context, line_style.color);

  //TODO if we're adding patterns (e.g. color gradients), we
  // could return a pointer (or add another parameter); a pattern
  // must be destroyed after use
  //- https://zetcode.com/gfx/cairo/gradients/
  //- https://www.cairographics.org/manual/cairo-cairo-pattern-t.html#cairo-pattern-create-rgba

  if (!line_style.dash_pattern.empty() && !ignore_dash) {
    // https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-dash
    const double *dash = &line_style.dash_pattern[0];
    cairo_set_dash(context, dash,
                   static_cast<int>(line_style.dash_pattern.size()),
                   0); // We don't need an offset into the dash pattern
  }
}


/** Ensure that the canvas is set up correctly. Should be called within each drawing helper function. */
inline void CheckCanvas(cairo_surface_t *surface, cairo_t *context) {
  if (!surface) {
    throw std::logic_error("Invalid cairo surface (nullptr)."
                           " Did you forget to set up the canvas first?");
  }

  cairo_status_t surf_stat = cairo_surface_status(surface);
  if (surf_stat != CAIRO_STATUS_SUCCESS) {
    std::stringstream s;
    s << "Invalid Cairo surface status (" << surf_stat
      << "), check https://www.cairographics.org/manual/cairo-Error-handling.html#cairo-status-t for details.";
    throw std::runtime_error(s.str());
  }

  if (!context) {
    throw std::runtime_error("Invalid Cairo context (nullptr), cannot draw anymore.");
  }
}




//---------------------------------------------------- Available drawing helpers
// TODO keep list alphabetically sorted:

void DrawArc(cairo_surface_t *surface, cairo_t *context,
             Vec2d center, double radius,
             double angle1, double angle2,
             const LineStyle &line_style,
             const Color &fill);


void DrawArrow(cairo_surface_t *surface, cairo_t *context,
               Vec2d from, Vec2d to, const ArrowStyle &arrow_style);


inline void DrawCircle(cairo_surface_t *surface, cairo_t *context,
                       Vec2d center, double radius,
                       const LineStyle &line_style,
                       const Color &fill) {
  DrawArc(surface, context, center, radius, 0, 360, line_style, fill);
}


void DrawGrid(cairo_surface_t *surface, cairo_t *context,
              const Vec2d &top_left, const Vec2d &bottom_right,
              double spacing_x, double spacing_y,
              const LineStyle &line_style);


void DrawLine(cairo_surface_t *surface, cairo_t *context,
              Vec2d from, Vec2d to, const LineStyle &line_style);


void DrawRect(cairo_surface_t *surface, cairo_t *context,
              Rect rect, const LineStyle &line_style,
              const Color &fill);

} // namespace helpers
} // namespace viren2d

#endif // __VIREN2D_DRAWING_HELPERS_H__
