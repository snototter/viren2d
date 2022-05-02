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
               Vec2d from, Vec2d to, const ArrowStyle &arrow_style) {
  CheckCanvas(surface, context);

  //FIXME convert other function calls & add dev note (for future draw_xxx implementations)
  from += 0.5;
  to += 0.5;

  // Compute the two end points of the arrow head
  auto diff = from - to;
  const double shaft_angle_rad = std::atan2(diff.y(), diff.x());

  const double tip_length = arrow_style.TipLengthForShaft(from, to);
  const double tip_angle_rad = deg2rad(arrow_style.tip_angle);
  auto tip_dir_a = tip_length * Vec2d(std::cos(shaft_angle_rad + tip_angle_rad),
                                      std::sin(shaft_angle_rad + tip_angle_rad));
  auto tip_dir_b = tip_length * Vec2d(std::cos(shaft_angle_rad - tip_angle_rad),
                                      std::sin(shaft_angle_rad - tip_angle_rad));
  // Compute points of the arrow tip/head
  Vec2d tip_1st_a = to + tip_dir_a;
  Vec2d tip_1st_b = to + tip_dir_b;
  // If double-headed, we need a 2nd set of tip points:
  Vec2d tip_2nd_a = arrow_style.double_headed ? (from - tip_dir_a) : Vec2d();
  Vec2d tip_2nd_b = arrow_style.double_headed ? (from - tip_dir_b) : Vec2d();

  // Switch to given line style
  cairo_save(context);
  helpers::ApplyLineStyle(context, arrow_style);

  // Draw everything as a single path if the head(s) should be filled:
  if (arrow_style.tip_closed) {
    // Create path for the optional 2nd head (at the line start)
    if (arrow_style.double_headed) {
      auto shaft_point_2nd = ProjectPointOntoLine(tip_2nd_a, from, to);
      cairo_move_to(context, shaft_point_2nd.x(), shaft_point_2nd.y());
      cairo_line_to(context, tip_2nd_a.x(), tip_2nd_a.y());
      cairo_line_to(context, from.x(), from.y());
      cairo_line_to(context, tip_2nd_b.x(), tip_2nd_b.y());
      cairo_line_to(context, shaft_point_2nd.x(), shaft_point_2nd.y());
    } else {
      cairo_move_to(context, from.x(), from.y());
    }

    // Add shaft & head (at the line end)
    auto shaft_point_1st = ProjectPointOntoLine(tip_1st_a, from, to);
    cairo_line_to(context, shaft_point_1st.x(), shaft_point_1st.y());
    cairo_line_to(context, tip_1st_a.x(), tip_1st_a.y());
    cairo_line_to(context, to.x(), to.y());
    cairo_line_to(context, tip_1st_b.x(), tip_1st_b.y());
    cairo_line_to(context, shaft_point_1st.x(), shaft_point_1st.y());

    // Fill & draw the path
    cairo_fill_preserve(context);
    cairo_stroke(context);
  } else {
    // Path of the shaft:
    cairo_move_to(context, from.x(), from.y());
    cairo_line_to(context, to.x(), to.y());

    // Path of the arrow head:
    cairo_move_to(context, tip_1st_a.x(), tip_1st_a.y());
    cairo_line_to(context, to.x(), to.y());
    cairo_line_to(context, tip_1st_b.x(), tip_1st_b.y());

    // Add the second head if needed:
    if (arrow_style.double_headed) {
      cairo_move_to(context, tip_2nd_a.x(), tip_2nd_a.y());
      cairo_line_to(context, from.x(), from.y());
      cairo_line_to(context, tip_2nd_b.x(), tip_2nd_b.y());
    }

    // Draw both paths
    cairo_stroke(context);
  }

  // Restore context
  cairo_restore(context);


  // TODO looks weird with highly transparent colors
  // Alternatives were even worse (would require the user to
  // fiddle too much with line joins and manually adjusting
  // the end points. Things tried: draw shaft twice; draw all in a single path; vary the line width (when filling)
  // TODO open ideas:
  // * check operators in detail! https://cairographics.org/operators/
  // * 1) draw opaque (see cairo_stroke_extents; slow but exact bounding box)
  //   2) copy with alpha
  // ---> check also compositions; could be helpful (later on), https://zetcode.com/gfx/cairo/compositing/
  // ---> we'll need something like cairo_surface_create_similar(); draw opaque and then combine them with alpha...
  //
  // * draw & simultaneously draw on a mask; next stroke/fill must be mask aware
  //   sounds too complicated - I'm in favor of the opaque-then-blend solution!
  //
  // so back to the cleanest code version:

  // Works for almost everything, except for filled+transparent arrows:

//  // Switch to given line style
//  cairo_save(context);
//  helpers::ApplyLineStyle(context, arrow_style);
//  // Path of the shaft
//  cairo_move_to(context, from.x(), from.y());
//  cairo_line_to(context, to.x(), to.y());

//  // Path of the arrow head:
//  cairo_move_to(context, tip1.x(), tip1.y());
//  cairo_line_to(context, to.x(), to.y());
//  cairo_line_to(context, tip2.x(), tip2.y());
//  // Fill if needed
//  if (arrow_style.tip_closed) {
//    // Close the path explicitly or we would see the line caps
//    // at tip1/tip2 for very thick line width settings.
//    cairo_close_path(context);
//    cairo_fill_preserve(context);
//  }
//  // Draw the paths
//  //cairo_set_operator(context, CAIRO_OPERATOR_SOURCE); // same effect as ignoring transparency of the path
//  //cairo_set_operator(context, CAIRO_OPERATOR_SOURCE); // "cuts out" the path (i.e. strokes are black)
//  cairo_stroke(context);

//  // Restore context
//  cairo_restore(context);
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
  // We add 0.5 to the coordinates to get sharp lines (a grid
  // will usually be drawn with very thin lines, i.e. 1 px)
  // For details see https://www.cairographics.org/FAQ/#sharp_lines
  auto num_steps = static_cast<int>(std::ceil((right - left) / spacing_x));
  double x = left + 0.5;
  for (int step = 0; step < num_steps; ++step, x += spacing_x) {
    cairo_move_to(context, x, top);
    cairo_line_to(context, x, bottom);
  }

  num_steps = static_cast<int>(std::ceil((bottom - top) / spacing_y));
  double y = top + 0.5;
  for (int step = 0; step < num_steps; ++step, y += spacing_y) {
    cairo_move_to(context, left, y);
    cairo_line_to(context, right, y);
  }
  cairo_stroke(context);
  // Restore previous state
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
