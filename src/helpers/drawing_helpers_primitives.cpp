// STL
#include <string>
#include <sstream>
#include <iomanip>
#include <exception>
#include <cmath>

#include <iostream> //TODO remove after switching to spdlog

// Custom
#include <viren2d/math.h>
#include <helpers/drawing_helpers.h>


namespace viren2d {
namespace helpers {
void CheckLineStyle(const LineStyle &style) {
  if (!style.IsValid()) {
    std::stringstream s;
    s << "Cannot draw with invalid style: " << style.ToString() << ".";
    throw std::invalid_argument(s.str());
  }
}


void CheckLineStyleAndFill(const LineStyle &style,
                           const Color &fill_color) {
  if (!style.IsValid() && !fill_color.IsValid()) {
    std::stringstream s;
    s << "Cannot draw with invalid line style & invalid fill color: "
      << style.ToString() << " and " << fill_color.ToString() << ".";
    throw std::invalid_argument(s.str());
  }
}

//---------------------------------------------------- Arc/Circle
void DrawArc(cairo_surface_t *surface, cairo_t *context,
             Vec2d center, double radius,
             double angle1, double angle2,
             const LineStyle &line_style,
             bool include_center,
             const Color &fill_color) {
  CheckCanvas(surface, context);
  CheckLineStyleAndFill(line_style, fill_color);

  // Move to the center of the pixel coordinates:
  center += 0.5;

  cairo_save(context);
  cairo_arc(context, center.x(), center.y(), radius,
            deg2rad(angle1), deg2rad(angle2));

  if (include_center) {
    cairo_line_to(context, center.x(), center.y());
    cairo_close_path(context);
  }

  if (fill_color.alpha > 0.0) {
    helpers::ApplyColor(context, fill_color);
    cairo_fill_preserve(context);
  }

  helpers::ApplyLineStyle(context, line_style);
  cairo_stroke(context);
  // Restore previous context
  cairo_restore(context);
}


//---------------------------------------------------- Arrow

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
Vec2d HelperClosedHead(cairo_t *context, const Vec2d &pointy_end,
                      const Vec2d &tip_a, const Vec2d &tip_b,
                      const Vec2d &line_from, const Vec2d &line_to) {
  // Compute the intersection between the shaft and
  // the connection line between the tip endpoints
  Vec2d shaft_point = ProjectPointOntoLine(tip_a, line_from, line_to);

  // Draw the path such that a) we can reuse this function
  // at both ends of the arrow and b) the "pointy end" is
  // rendered as a line joint.
  cairo_line_to(context, shaft_point.x(), shaft_point.y());
  cairo_line_to(context, tip_a.x(), tip_a.y());
  cairo_line_to(context, pointy_end.x(), pointy_end.y());
  cairo_line_to(context, tip_b.x(), tip_b.y());
  cairo_line_to(context, shaft_point.x(), shaft_point.y());

  return shaft_point;
}


void DrawArrow(cairo_surface_t *surface, cairo_t *context,
               Vec2d from, Vec2d to, const ArrowStyle &arrow_style) {
  CheckCanvas(surface, context);
  CheckLineStyle(arrow_style);

  // Add 0.5 (half a pixel) to align the arrow exactly
  // with the given coordinates
  from += 0.5;
  to += 0.5;


  // Adjust endpoints s.t. the "pointy ends" point exactly to
  // the given endpoints. My implementation ensures that for
  // any ArrowStyle, the "pointy end" will always be rendered
  // as a line joint.
  const double tip_offset = arrow_style.TipOffset(cairo_get_miter_limit(context));
  to += tip_offset * to.DirectionVector(from).UnitVector();

  if (arrow_style.double_headed) {
    from += tip_offset * from.DirectionVector(to).UnitVector();
  }

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

  // Start drawing
  cairo_save(context);

  if (arrow_style.tip_closed) {
    // First, draw & fill the head(s). The head contour
    // will always be drawn solid (dashed heads look weird).
    helpers::ApplyLineStyle(context, arrow_style, true);

    // Create path for the optional 2nd head (at the line start)
    Vec2d shaft_from = from;
    if (arrow_style.double_headed) {
      cairo_new_path(context);
      shaft_from = HelperClosedHead(context, from, tip_2nd_a, tip_2nd_b, from, to);
      cairo_fill_preserve(context);
      cairo_stroke(context);  // Stroke is currently solid
    }

    // Draw head Add shaft & head (at the line end)
    Vec2d shaft_to = HelperClosedHead(context, to, tip_1st_a, tip_1st_b, from, to);
    cairo_fill_preserve(context);
    cairo_stroke(context);

    // Switch to dashed line if needed
    if (arrow_style.IsDashed()) {
      helpers::ApplyLineStyle(context, arrow_style);
    }
    cairo_move_to(context, shaft_from.x(), shaft_from.y());
    cairo_line_to(context, shaft_to.x(), shaft_to.y());
    cairo_stroke(context);
  } else {
    // For "open" arrows, we can simply create
    // multiple (sub)paths.
    // Path for the first arrow head:
    HelperDrawSolidHead(context, to, tip_1st_a, tip_1st_b);

    // Path for the second arrow head:
    if (arrow_style.double_headed) {
      HelperDrawSolidHead(context, from, tip_2nd_a, tip_2nd_b);
    }
    // Draw both paths solid
    helpers::ApplyLineStyle(context, arrow_style, true);
    cairo_stroke(context);

    // Finally, draw the shaft (swith to dashed
    // line if needed)
    if (arrow_style.IsDashed()) {
      helpers::ApplyLineStyle(context, arrow_style);
    }
    cairo_move_to(context, from.x(), from.y());
    cairo_line_to(context, to.x(), to.y());
    cairo_stroke(context);
  }
  // Restore context
  cairo_restore(context);
}


//---------------------------------------------------- Ellipse
void DrawEllipse(cairo_surface_t *surface, cairo_t *context,
                 Rect rect, const LineStyle &line_style,
                 const Color &fill_color) {
  CheckCanvas(surface, context);
  CheckLineStyleAndFill(line_style, fill_color);
  // Ensure that the radius doesn't influence the validity
  // check (as it is ignored for drawing ellipses anyhow).
  rect.radius = 0.0;
  if (!rect.IsValid()) {
    throw std::invalid_argument("Cannot draw an invalid ellipse!");
  }

  // Shift to the pixel center (so 1px borders are drawn correctly)
  rect += 0.5;

  cairo_save(context);
  cairo_save(context); // Save context twice
  cairo_translate(context, rect.cx, rect.cy);
  cairo_rotate(context, deg2rad(rect.angle));
  cairo_scale(context, rect.width / rect.height, 1);

  cairo_arc(context, 0, 0, rect.half_height(), 0, deg2rad(360));
  cairo_restore(context);

  if (fill_color.alpha > 0.0) {
    helpers::ApplyColor(context, fill_color);
    cairo_fill_preserve(context);
  }

  helpers::ApplyLineStyle(context, line_style);
  cairo_stroke(context);

  // Restore context
  cairo_restore(context);
}


//---------------------------------------------------- Grid
void DrawGrid(cairo_surface_t *surface, cairo_t *context,
              Vec2d top_left, Vec2d bottom_right,
              double spacing_x, double spacing_y,
              const LineStyle &line_style) {
  // Sanity checks
  CheckCanvas(surface, context);
  CheckLineStyle(line_style);
  if ((spacing_x <= 0.0) || (spacing_y <= 0.0)) {
    throw std::invalid_argument("Cell spacing must be > 0.");
  }

  // Adjust corners if necessary
  if (bottom_right.x() < top_left.x()) {
    std::swap(top_left.val[0], bottom_right.val[0]);
  }
  if (bottom_right.y() < top_left.y()) {
    std::swap(top_left.val[1], bottom_right.val[1]);
  }

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

  // Draw the grid. To support thin lines, we need to shift the coordinates.
  // For details see https://www.cairographics.org/FAQ/#sharp_lines
  auto num_steps = static_cast<int>(std::ceil((right - left) / spacing_x));
  double x = left + 0.5;
  for (int step = 0; step <= num_steps; ++step, x += spacing_x) {
    cairo_move_to(context, x, top);
    cairo_line_to(context, x, bottom);
  }

  num_steps = static_cast<int>(std::ceil((bottom - top) / spacing_y));
  double y = top + 0.5;
  for (int step = 0; step <= num_steps; ++step, y += spacing_y) {
    cairo_move_to(context, left, y);
    cairo_line_to(context, right, y);
  }
  cairo_stroke(context);
  // Restore previous state
  cairo_restore(context);
}


//---------------------------------------------------- Line
void DrawLine(cairo_surface_t *surface, cairo_t *context,
              Vec2d from, Vec2d to, const LineStyle &line_style) {
  CheckCanvas(surface, context);
  CheckLineStyle(line_style);

  // Adjust coordinates to support thin (1px) lines
  from += 0.5;
  to += 0.5;

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
              Rect rect, const LineStyle &line_style,
              const Color &fill_color) {
  CheckCanvas(surface, context);
  CheckLineStyleAndFill(line_style, fill_color);
  if (!rect.IsValid()) {
    throw std::invalid_argument("Cannot draw an invalid rectangle!");
  }

  // Shift to the pixel center (so 1px borders are drawn correctly)
  rect += 0.5;

  cairo_save(context);
  cairo_translate(context, rect.cx, rect.cy);
  cairo_rotate(context, deg2rad(rect.angle));

  // Draw a standard (box) rect or rounded rectangle:
  if (rect.radius > 0.0)
    PathHelperRoundedRect(context, rect);
  else
    cairo_rectangle(context, -rect.half_width(), -rect.half_height(),
                    rect.width, rect.height);

  if (fill_color.alpha > 0.0) {
    helpers::ApplyColor(context, fill_color);
    cairo_fill_preserve(context);
  }

  helpers::ApplyLineStyle(context, line_style);
  cairo_stroke(context);
  // Restore context
  cairo_restore(context);
}


} // namespace helpers
} // namespace viren2d
