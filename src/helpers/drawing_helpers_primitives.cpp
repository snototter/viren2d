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

/** @brief Draws an open, dashed arrow head */
void HelperDrawDashedHead(cairo_t *context, const Vec2d &pointy_end,
                          const Vec2d &tip_dir_a, const Vec2d &tip_pt_a,
                          const Vec2d &tip_dir_b, const Vec2d &tip_pt_b,
                          double length_first_dash) {
  // If the line is dashed, we need to create 2 separate
  // paths, or the result may easily look quite ugly.
  // ("Ugly": the "pointy end" could be "drawn" as
  // the off-stroke of the dash pattern. Thus, there
  // would be no "pointy end").
  // Additionally, we need to ensure that the "pointy end" is
  // rendered as a line joint (so that DrawArrow shows the
  // same behavior with all style configurations).
  // For this, we need 2 additional points:
  Vec2d towards_b = pointy_end
      + std::max(2.0, 0.5 * length_first_dash) * tip_dir_b.UnitVector();
  // This assumes that you're dash pattern starts with at
  // least a 3-4 pixel long dash (which is reasonable imho).

  cairo_move_to(context, towards_b.x(), towards_b.y());
  cairo_line_to(context, pointy_end.x(), pointy_end.y());
  cairo_line_to(context, tip_pt_a.x(), tip_pt_a.y());

  Vec2d towards_a = pointy_end
      + std::max(2.0, 0.5 * length_first_dash) * tip_dir_a.UnitVector();

  cairo_move_to(context, towards_a.x(), towards_a.y());
  cairo_line_to(context, pointy_end.x(), pointy_end.y());
  cairo_line_to(context, tip_pt_b.x(), tip_pt_b.y());
}

/** @brief Draws an open, solid arrow head. */
void HelperDrawSolidHead(cairo_t * context, const Vec2d &pointy_end,
                         const Vec2d &tip_a, const Vec2d &tip_b) {
  // With plain old solid lines, everything is much
  // easier and straightforward :-)
  cairo_move_to(context, tip_a.x(), tip_a.y());
  cairo_line_to(context, pointy_end.x(), pointy_end.y());
  cairo_line_to(context, tip_b.x(), tip_b.y());
}


/**
 * @brief Extends the currently active (sub)path by a closed
 * arrow head.
 */
void HelperClosedHead(cairo_t *context, const Vec2d &pointy_end,
                      const Vec2d &tip_a, const Vec2d &tip_b,
                      const Vec2d &line_from, const Vec2d &line_to) {
  // Compute the intersection between the shaft and
  // the connection line between the tip endpoints
  auto shaft_point = ProjectPointOntoLine(tip_a, line_from, line_to);

  // Draw the path such that a) we can reuse this function
  // at both ends of the arrow and b) the "pointy end" is
  // rendered as a line joint.
  cairo_line_to(context, shaft_point.x(), shaft_point.y());
  cairo_line_to(context, tip_a.x(), tip_a.y());
  cairo_line_to(context, pointy_end.x(), pointy_end.y());
  cairo_line_to(context, tip_b.x(), tip_b.y());
  cairo_line_to(context, shaft_point.x(), shaft_point.y());
}


void DrawArrow(cairo_surface_t *surface, cairo_t *context,
               Vec2d from, Vec2d to, const ArrowStyle &arrow_style) {
  CheckCanvas(surface, context);

  //FIXME convert other function calls & add dev note (for future draw_xxx implementations)
  from += 0.5;
  to += 0.5;

  // Adjust endpoints s.t. the "pointy ends" point exactly to
  // the given endpoints:
  double joint_offset = (arrow_style.line_join == LineJoin::Miter)
      ? (arrow_style.line_width)  // FIXME compute & move to linestyle! see diagram at: https://github.com/freedesktop/cairo/blob/9bb1cbf7249d12dd69c8aca3825711645da20bcb/src/cairo-path-stroke.c#L432
      : (arrow_style.line_width / 2.0);

  if (arrow_style.double_headed) {
    from += joint_offset * from.DirectionVector(to).UnitVector();
  }
  to += joint_offset * to.DirectionVector(from).UnitVector();


  // Compute the two end points of the arrow head.
  // Terminology: "1st" is the tip at the 'to' end of the line.
  //                    This will always be drawn.
  //              "2nd" is the tip at the 'from' end - only for
  //                    double-headed arrows.
  // Compute orientation of the line:
  auto diff = from - to;
  const double shaft_angle_rad = std::atan2(diff.y(), diff.x());

  // Compute the offset/direction vectors from the line's
  // endpoints to the endpoints of each tip:
  const double tip_length = arrow_style.TipLengthForShaft(from, to);
  const double tip_angle_rad = deg2rad(arrow_style.tip_angle);
  auto tip_dir_1st_a = tip_length * Vec2d(std::cos(shaft_angle_rad + tip_angle_rad),
                                          std::sin(shaft_angle_rad + tip_angle_rad));
  auto tip_dir_1st_b = tip_length * Vec2d(std::cos(shaft_angle_rad - tip_angle_rad),
                                          std::sin(shaft_angle_rad - tip_angle_rad));
  // Compute endpoints of 1st tip:
  Vec2d tip_1st_a = to + tip_dir_1st_a;
  Vec2d tip_1st_b = to + tip_dir_1st_b;
  // If double-headed, we need a 2nd set of tip points:
  Vec2d tip_2nd_a = arrow_style.double_headed ? (from - tip_dir_1st_a) : Vec2d();
  Vec2d tip_2nd_b = arrow_style.double_headed ? (from - tip_dir_1st_b) : Vec2d();

  // Switch to the given line style
  cairo_save(context);
  helpers::ApplyLineStyle(context, arrow_style);

  // If the head(s) should be filled, we want a single
  // path, s.t. it also looks good when using a
  // transparent color.
  if (arrow_style.tip_closed) {
    // Create path for the optional 2nd head (at the line start)
    if (arrow_style.double_headed) {
      cairo_new_path(context);
      HelperClosedHead(context, from, tip_2nd_a, tip_2nd_b, from, to);
    } else {
      cairo_move_to(context, from.x(), from.y());
    }

    // Add shaft & head (at the line end)
    HelperClosedHead(context, to, tip_1st_a, tip_1st_b, from, to);

    // Fill & draw the path
    cairo_fill_preserve(context);
    cairo_stroke(context);
  } else {
    // For "open" arrows, we can simply create
    // multiple (sub)paths. First, the path of
    // the shaft:
    cairo_move_to(context, from.x(), from.y());
    cairo_line_to(context, to.x(), to.y());
    // First arrow head:
    if (arrow_style.IsDashed()) {
      HelperDrawDashedHead(context, to,
                           tip_dir_1st_a, tip_1st_a,
                           tip_dir_1st_b, tip_1st_b,
                           arrow_style.dash_pattern[0]);
    } else {
      HelperDrawSolidHead(context, to, tip_1st_a, tip_1st_b);
    }
    // Second arrow head:
    if (arrow_style.double_headed) {
      if (arrow_style.IsDashed()) {
        HelperDrawDashedHead(context, from,
                             -tip_dir_1st_a, tip_2nd_a,
                             -tip_dir_1st_b, tip_2nd_b,
                             arrow_style.dash_pattern[0]);
      } else {
        HelperDrawSolidHead(context, from, tip_2nd_a, tip_2nd_b);
      }
    }
    // Draw both paths
    cairo_stroke(context);
  }
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
