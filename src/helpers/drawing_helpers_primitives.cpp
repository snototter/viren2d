// STL
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

#include <iostream> //TODO remove after switching to spdlog

// Custom
#include <viren2d/math.h>
#include <helpers/drawing_helpers.h>

namespace viren2d {
namespace helpers {
//---------------------------------------------------- Arc/Circle
void DrawArc(cairo_surface_t *surface, cairo_t *context,
             const Vec2d &center, double radius,
             double angle1, double angle2,
             const LineStyle &line_style, const Color &fill) {
  CheckCanvas(surface, context);

  cairo_save(context);
  cairo_arc(context, center.x(), center.y(), radius,
            deg2rad(angle1), deg2rad(angle2));
  if (fill.alpha > 0.0) {
    helpers::ApplyColor(context, fill);
    cairo_fill_preserve(context);
  }

  helpers::ApplyLineStyle(context, line_style);
  cairo_stroke(context);
  // Restore previous context
  cairo_restore(context);
}


//---------------------------------------------------- Arrow
void DrawArrow(cairo_surface_t *surface, cairo_t *context,
               const Vec2d &from, const Vec2d &to,
               const ArrowStyle &arrow_style) {
  CheckCanvas(surface, context);

  // Compute the two end points of the arrow head
  auto diff = from - to;
  const double shaft_angle_rad = std::atan2(diff.y(), diff.x());

  const double tip_length = arrow_style.TipLengthForShaft(from, to);
  const double tip_angle_rad = deg2rad(arrow_style.tip_angle);
  auto tip1 = to + tip_length * Vec2d(std::cos(shaft_angle_rad + tip_angle_rad),
                                      std::sin(shaft_angle_rad + tip_angle_rad));
  auto tip2 = to + tip_length * Vec2d(std::cos(shaft_angle_rad - tip_angle_rad),
                                      std::sin(shaft_angle_rad - tip_angle_rad));

//  std::cout << "Drawing Arrow: " << tip_length << "; " << tip1 << " -- " << to << " -- " << tip2 << std::endl;
//  std::cout << "--> " << from << " -- " << to << ": l=" << from.Distance(to) << "; tip length: " << arrow_style.tip_length << "; as.tiplenfor()=" << arrow_style.TipLengthForShaft(from, to) << std::endl;

  // TODO looks weird with highly transparent colors
  // Alternatives were even worse (would require the user to
  // fiddle too much with line joins and manually adjusting
  // the end points. Things tried: draw shaft twice; draw all in a single path; vary the line width (when filling)
  // TODO open ideas:
  // * 1) draw opaque (see cairo_stroke_extents; slow but exact bounding box)
  //   2) copy with alpha
  // ---> check also compositions; could be helpful (later on), https://zetcode.com/gfx/cairo/compositing/
  // ---> we'll need something like cairo_surface_create_similar(); draw opaque and then combine them with alpha...
  //
  // * draw & simultaneously draw on a mask; next stroke/fill must be mask aware
  //   sounds too complicated - I'm in favor of the opaque-then-blend solution!
  //
  // so back to the cleanest code version:

  // Switch to given line style
  cairo_save(context);
  helpers::ApplyLineStyle(context, arrow_style);
  // Draw shaft
  cairo_move_to(context, from.x(), from.y());
  cairo_line_to(context, to.x(), to.y());
  cairo_stroke(context);

  // Arrow tip:
  cairo_move_to(context, tip1.x(), tip1.y());
  cairo_line_to(context, to.x(), to.y());
  cairo_line_to(context, tip2.x(), tip2.y());
  // Fill if needed
  if (arrow_style.tip_closed) {
    cairo_fill_preserve(context);
  }
  // The filled area excludes the contour/path, thus we
  // always have to draw the tip stroke:
  cairo_stroke(context);

  //FIXME python bindings grid
  //FIXME python bindings arrow
  //FIXME support transparent arrows via opaque-then-blend

  // Restore context
  cairo_restore(context);
}


//---------------------------------------------------- Grid
void DrawGrid(cairo_surface_t *surface, cairo_t *context,
              const Vec2d &top_left, const Vec2d &bottom_right,
              double spacing_x, double spacing_y,
              const LineStyle &line_style) {
  CheckCanvas(surface, context);

  // Switch to given line style
  cairo_save(context);
  helpers::ApplyLineStyle(context, line_style);

  // Check grid limits
  double left = top_left.x();
  double right = bottom_right.x();
  double top = top_left.y();
  double bottom = bottom_right.y();
  // Should the grid span the whole canvas?
  if (top_left == bottom_right) {
    right = static_cast<double>(cairo_image_surface_get_width(surface));
    bottom = static_cast<double>(cairo_image_surface_get_height(surface));
  }

  // Draw the grid
  for (double x = left; x <= right; x += spacing_x) {
    cairo_move_to(context, x, top);
    cairo_line_to(context, x, bottom);
    cairo_stroke(context);
  }
  for (double y = top; y <= bottom; y += spacing_y) {
    cairo_move_to(context, left, y);
    cairo_line_to(context, right, y);
    cairo_stroke(context);
  }
  // Restore previous context
  cairo_restore(context);
}


//---------------------------------------------------- Line
void DrawLine(cairo_surface_t *surface, cairo_t *context,
              const Vec2d &from, const Vec2d &to,
              const LineStyle &line_style) {
  CheckCanvas(surface, context);
  // Switch to given line style
  cairo_save(context);
  helpers::ApplyLineStyle(context, line_style);
  // Draw line
  cairo_move_to(context, from.x(), from.y());
  cairo_line_to(context, to.x(), to.y());
  cairo_stroke(context);
  // Restore context
  cairo_restore(context);
}




//---------------------------------------------------- Rectangle (box, rounded, rotated)
/** Creates a path for a rectangle with rounded corners.
 *  Assumes that the viewport is already translated (and optionally
 *  rotated).
 */
void PathHelperRoundedRect(cairo_t *context, const Rect &rect) {
  if (rect.radius > std::min(rect.half_height(), rect.half_width())) {
    std::stringstream s;
    s << "Invalid rounded rect: radius " << std::fixed << std::setprecision(2)
      << " must be less than half the smaller dimension (i.e. "
      << std::min(rect.half_height(), rect.half_width()) << ")!";
    throw std::out_of_range(s.str());
  }

  const double half_width = rect.half_width() - rect.radius;
  const double half_height = rect.half_height() - rect.radius;
  cairo_move_to(context, -rect.half_width(), -half_height);
  cairo_arc(context, -half_width, -half_height, rect.radius, deg2rad(180), deg2rad(270));
  cairo_arc(context,  half_width, -half_height, rect.radius, deg2rad(-90), 0);
  cairo_arc(context,  half_width,  half_height, rect.radius, 0, deg2rad(90));
  cairo_arc(context, -half_width,  half_height, rect.radius, deg2rad(90), deg2rad(180));
  cairo_close_path(context);
}


void DrawRect(cairo_surface_t *surface, cairo_t *context,
              const Rect &rect, const LineStyle &line_style,
              const Color &fill) {
  CheckCanvas(surface, context);

  cairo_save(context);
  cairo_translate(context, rect.cx, rect.cy);
  cairo_rotate(context, deg2rad(rect.angle));

  // Draw a standard (box) rect or rounded rectangle:
  if (rect.radius > 0.0)
    PathHelperRoundedRect(context, rect);
  else
    cairo_rectangle(context, -rect.half_width(), -rect.half_height(),
                    rect.width, rect.height);

  if (fill.alpha > 0.0) {
    helpers::ApplyColor(context, fill);
    cairo_fill_preserve(context);
  }

  helpers::ApplyLineStyle(context, line_style);
  cairo_stroke(context);
  // Restore context
  cairo_restore(context);
}


} // namespace helpers
} // namespace viren2d
