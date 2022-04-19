#ifndef __VIREN2D_DRAWING_HELPERS_H__
#define __VIREN2D_DRAWING_HELPERS_H__

#include <stdexcept>

#include <math.h>
#include <cairo/cairo.h>

#include <viren2d/colors.hpp>
#include <viren2d/drawing.hpp>


namespace viren2d
{
namespace helpers
{
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
inline void ApplyColor(cairo_t *context, const Color &color)
{
  cairo_set_source_rgba(context, color.blue, color.green,
                        color.red, color.alpha);
}


inline cairo_line_cap_t LineCap2Cairo(const LineStyle &line_style)
{
  switch (line_style.line_cap)
  {
    case LineStyle::Cap::Butt:
      return CAIRO_LINE_CAP_BUTT;

    case LineStyle::Cap::Round:
      return CAIRO_LINE_CAP_ROUND;

    case LineStyle::Cap::Square:
      return CAIRO_LINE_CAP_SQUARE;
  }
  throw std::runtime_error("Line cap style is not yet mapped to Cairo type!");
}


inline cairo_line_join_t LineJoin2Cairo(const LineStyle &line_style)
{
  switch (line_style.line_join)
  {
    case LineStyle::Join::Miter:
      return CAIRO_LINE_JOIN_MITER;

    case LineStyle::Join::Bevel:
      return CAIRO_LINE_JOIN_BEVEL;

    case LineStyle::Join::Round:
      return CAIRO_LINE_JOIN_ROUND;
  }
  throw std::runtime_error("Line join style is not yet mapped to Cairo type!");
}


/** @brief Changes the given Cairo context to use the given LineStyle definitions. */
inline void ApplyLineStyle(cairo_t *context, const LineStyle &line_style)
{
  cairo_set_line_width(context, line_style.line_width);
  cairo_set_line_cap(context, LineCap2Cairo(line_style));
  cairo_set_line_join(context, LineJoin2Cairo(line_style));
  ApplyColor(context, line_style.color);

  //TODO if we're adding patterns (e.g. color gradients), we
  // could return a pointer (or add another parameter); a pattern
  // must be destroyed after use
  //- https://zetcode.com/gfx/cairo/gradients/
  //- https://www.cairographics.org/manual/cairo-cairo-pattern-t.html#cairo-pattern-create-rgba

  if (!line_style.dash_pattern.empty())
  {
    // https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-dash
    const double *dash = &line_style.dash_pattern[0];
    cairo_set_dash(context, dash,
                   static_cast<int>(line_style.dash_pattern.size()),
                   0); // We don't need an offset into the dash pattern
  }
}




inline void CheckCanvas(cairo_surface_t *surface, cairo_t *context)
{
  //TODO also check NIL surface (or if finalized, etc.)
  if (!surface)
    throw std::logic_error("Invalid cairo surface (nullptr)"
                           " - did you forget to set up the canvas first?");

  if (!context)
    throw std::runtime_error("Invalid cairo context (nullptr) - cannot draw anymore.");
}




//---------------------------------------------------- Available drawing helpers
// TODO keep list alphabetically sorted:

void DrawArc(cairo_surface_t *surface, cairo_t *context,
             const Vec2d &center, double radius,
             double angle1, double angle2,
             const LineStyle &line_style, const Color &fill);


inline void DrawCircle(cairo_surface_t *surface, cairo_t *context,
                       const Vec2d &center, double radius,
                       const LineStyle &line_style, const Color &fill)
{
  DrawArc(surface, context, center, radius, 0, 360, line_style, fill);
}



void DrawLine(cairo_surface_t *surface, cairo_t *context,
              const Vec2d &from, const Vec2d &to,
              const LineStyle &line_style);



void DrawRect(cairo_surface_t *surface, cairo_t *context,
              const Rect &rect, const LineStyle &line_style,
              const Color &fill);




} // namespace helpers
} // namespace viren2d

#endif // __VIREN2D_DRAWING_HELPERS_H__
