// STL
#include <string>
#include <sstream>
#include <iomanip>
#include <exception>
#include <cmath>

// non-STL, external
#include <werkzeugkiste/geometry/utils.h>
#include <werkzeugkiste/geometry/vector.h>
namespace wgu = werkzeugkiste::geometry;

// Custom
#include <helpers/drawing_helpers.h>
#include <helpers/enum.h>


namespace viren2d {
namespace helpers {

void PathHelperRoundedRect(cairo_t *context, Rect rect) {
  // If radius in (0, 0.5], we use it as a percentage.
  if (rect.radius <= 0.5) {
    rect.radius *= std::min(rect.width, rect.height);
  }
  const double half_width = rect.half_width() - rect.radius;
  const double half_height = rect.half_height() - rect.radius;
  cairo_move_to(context, -rect.half_width(), -half_height);
  cairo_arc(context, -half_width, -half_height, rect.radius,
            wgu::deg2rad(180), wgu::deg2rad(270));
  cairo_arc(context,  half_width, -half_height, rect.radius,
            wgu::deg2rad(-90), 0);
  cairo_arc(context,  half_width,  half_height, rect.radius,
            0, wgu::deg2rad(90));
  cairo_arc(context, -half_width,  half_height, rect.radius,
            wgu::deg2rad(90), wgu::deg2rad(180));
  cairo_close_path(context);
}


void CheckLineStyle(const LineStyle &style) {
  if (!style.IsValid()) {
    std::ostringstream s;
    s << "Cannot draw with invalid line style " << style.ToString() << "!";
    throw std::invalid_argument(s.str());
  }
}


void CheckLineStyleAndFill(const LineStyle &style,
                           Color &fill_color) {
  if (fill_color.IsSpecialSame()) {
    fill_color = style.color.WithAlpha(fill_color.alpha);
  }
  // FIXME check usage of Color::Same in all fill... calls!
  if (!style.IsValid() && !fill_color.IsValid()) {
    std::ostringstream s;
    s << "Cannot draw with both invalid line style and invalid fill color: "
      << style.ToString() << " and " << fill_color.ToString() << "!";
    throw std::invalid_argument(s.str());
  }
}



//---------------------------------------------------- Arc/Circle
void DrawArc(cairo_surface_t *surface, cairo_t *context,
             Vec2d center, double radius,
             double angle1, double angle2,
             const LineStyle &line_style,
             bool include_center,
             Color fill_color) {
  CheckCanvas(surface, context);
  CheckLineStyleAndFill(line_style, fill_color);

  if (radius <= 0.0) {
    throw std::invalid_argument("Radius must be > 0.0!");
  }

  // Move to the center of the pixel coordinates:
  center += 0.5;

  cairo_save(context);
  cairo_arc(context, center.x(), center.y(), radius,
            wgu::deg2rad(angle1), wgu::deg2rad(angle2));

  if (include_center) {
    cairo_line_to(context, center.x(), center.y());
    cairo_close_path(context);
  }

  if (fill_color.IsValid()) {
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


  // Adjust endpoints s.t. the "pointy end" points exactly to
  // the given endpoint. My implementation ensures that for
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
  const double tip_angle_rad = wgu::deg2rad(arrow_style.tip_angle);
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
/**
 * Computes the adjusted ellipse arc angle s.t. drawing
 * the ellipse in the scaled Cairo context results in
 * the desired user space angles.
 */
double AdjustEllipseAngle(double deg, double scale_x, double scale_y) {
  // Compute the direction vector corresponding to
  // the desired angle.
  auto dir = wgu::DirectionVecFromAngleDeg(deg);
  // Apply the inverse transformation (scaling).
  dir.SetX(dir.x() / scale_x);
  dir.SetY(dir.y() / scale_y);
  // Compute the angle (w.r.t. to the positive X axis)
  // which should be used to draw the path in cairo_arc
  // after the context is transformed.
  return wgu::AngleDegFromDirectionVec(dir);
}


void DrawEllipse(cairo_surface_t *surface, cairo_t *context,
                 Ellipse ellipse, const LineStyle &line_style,
                 Color fill_color) {
  CheckCanvas(surface, context);
  CheckLineStyleAndFill(line_style, fill_color);

  if (!ellipse.IsValid()) {
    std::ostringstream s;
    s << "Cannot draw an invalid ellipse: " << ellipse << "!";
    throw std::invalid_argument(s.str());
  }

  // Shift to the pixel center (so 1px borders are drawn correctly).
  ellipse += 0.5;

  // We'll scale the context, so we can draw the ellipse as a
  // unit circle.
  const double scale_x = ellipse.major_axis / 2.0;
  const double scale_y = ellipse.minor_axis / 2.0;

  // If only parts of the ellipse are drawn/filled, we
  // have to adjust the angles, because we scale the
  // Cairo context. Otherwise, the angles would be
  // quite different from what the user expected.
  bool is_partially_drawn = false;
  if (!wgu::eps_zero(ellipse.angle_from)) {
    //TODO log debug: adjusting angle1
    ellipse.angle_from = AdjustEllipseAngle(ellipse.angle_from,
                                            scale_x, scale_y);
    is_partially_drawn = true;
  }
  if (!wgu::eps_equal(ellipse.angle_to, 360.0)) {
    //TODO log debug: adjusting angle2
    ellipse.angle_to = AdjustEllipseAngle(ellipse.angle_to,
                                          scale_x, scale_y);
    is_partially_drawn = true;
  }

  // Save context twice (the extra save is needed to draw even
  // strokes after we applied the scaling). For details see
  // https://www.cairographics.org/tutorial/#L2linewidth
  cairo_save(context);
  cairo_save(context);
  cairo_translate(context, ellipse.cx, ellipse.cy);
  cairo_rotate(context, wgu::deg2rad(ellipse.rotation));
  cairo_scale(context, scale_x, scale_y);

  cairo_arc(context, 0, 0, 1,
            wgu::deg2rad(ellipse.angle_from),
            wgu::deg2rad(ellipse.angle_to));

  // If we shouldn't draw a full circle in the scaled context,
  // the user can decide whether to include the center point
  // in the contour/fill or not. Similar to DrawArc(), this is
  // needed because filling a "partial ellipse" without including
  // the center can look irritating.
  if (is_partially_drawn && ellipse.include_center) {
    cairo_line_to(context, 0, 0);
    cairo_close_path(context);
  }
  cairo_restore(context);

  if (fill_color.IsValid()) {
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
    throw std::invalid_argument("Cell spacing for grid must be > 0.");
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
  auto num_steps = static_cast<int>(std::floor((right - left) / spacing_x));
  double x = left + 0.5;
  for (int step = 0; step <= num_steps; ++step, x += spacing_x) {
    cairo_move_to(context, x, top);
    cairo_line_to(context, x, bottom);
  }

  num_steps = static_cast<int>(std::floor((bottom - top) / spacing_y));
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


//---------------------------------------------------- Marker
void DrawMarker(cairo_surface_t *surface, cairo_t *context,
                Vec2d pos, const MarkerStyle &style) {
  // General idea for all markers implemented so far:
  // * Translate the canvas
  // * Create the path(s), i.e. the marker shape's outline
  // * Either fill or stroke (xor! I don't want to deal
  //   with the effects of partially translucent colors
  //   which overlap between fill and stroke)

  //TODO usually, we want to draw multiple markers at once (e.g. keypoints)
  // --> think about how to do that easily
  // * Marker class = Marker style + location
  //   different from all other primitives so far (where we
  //   have separate style & geometric definition)
  // vs
  // * position vector + color vector + single marker style?
  //   (could use marker_style.color as fallback if a corresponding
  //   color is "invalid")
  CheckCanvas(surface, context);

  if (!style.IsValid()) {
    std::ostringstream s;
    s << "Cannot draw with invalid marker style " << style.ToString() << "!";
    throw std::invalid_argument(s.str());
  }

  cairo_save(context);
  //TODO should markerstyle have a linestyle instead of only size + thickness?
  // --> would allow to specify join & cap (or draw dashed markers)
  //     might be worth it!
//  ApplyMarkerStyle(style);

  // Move to the center of the pixel coordinates, so each
  // marker can be drawn as if it's at the origin:
  pos += 0.5;
  cairo_translate(context, pos.x(), pos.y());
  cairo_new_path(context);

  const double half_size = style.size / 2.0;

  if ((style.marker == Marker::Circle)
      || (style.marker == Marker::Point)) {
    // '.' and 'o'
    cairo_arc(context, 0.0, 0.0, half_size, 0.0, 2 * M_PI);
  } else if ((style.marker == Marker::Cross)
             || (style.marker == Marker::Plus)) {
    // '+' and 'x'
    if (style.marker == Marker::Cross) {
      cairo_rotate(context, wgu::deg2rad(45.0));
    }
    cairo_move_to(context, -half_size, 0.0);
    cairo_line_to(context, half_size, 0.0);
    cairo_move_to(context, 0.0, -half_size);
    cairo_line_to(context, 0.0, half_size);
  } else if ((style.marker == Marker::Diamond)
             || (style.marker == Marker::Square)) {
    // 's' and 'd'
    if (style.marker == Marker::Diamond) {
      cairo_rotate(context, wgu::deg2rad(45.0));
    }
    cairo_rectangle(context, -half_size, -half_size,
                    style.size, style.size);
  }

  if (style.IsFilled()) {
    cairo_fill(context);
  } else {
    cairo_stroke(context);
  }

  cairo_restore(context);
}


//---------------------------------------------------- Polygon
void DrawPolygon(cairo_surface_t *surface, cairo_t *context,
                 const std::vector<Vec2d> points,
                 const LineStyle &line_style,
                 Color fill_color) {
  CheckCanvas(surface, context);
  CheckLineStyleAndFill(line_style, fill_color);

  if (points.size() < 3) {
    throw std::invalid_argument("A polygon must have at least 3 points!");
  }

  cairo_save(context);
  auto from = points[0] + 0.5;
  cairo_move_to(context, from.x(), from.y());
  for (std::size_t idx = 1; idx < points.size(); ++idx) {
    auto to = points[idx] + 0.5;
    cairo_line_to(context, to.x(), to.y());
    from = to;
  }
  if (fill_color.IsValid()) {
    helpers::ApplyColor(context, fill_color);
    cairo_fill_preserve(context);
  }

  helpers::ApplyLineStyle(context, line_style);
  cairo_stroke(context);

  cairo_restore(context);
}


//---------------------------------------------------- Rectangle (box, rounded, rotated)
void DrawRect(cairo_surface_t *surface, cairo_t *context,
              Rect rect, const LineStyle &line_style,
              Color fill_color) {
  CheckCanvas(surface, context);
  CheckLineStyleAndFill(line_style, fill_color);

  if (!rect.IsValid()) {
    std::ostringstream s;
    s << "Cannot draw an invalid rectangle: " << rect << "!";
    throw std::invalid_argument(s.str());
  }

  // Shift to the pixel center (so 1px borders are drawn correctly)
  rect += 0.5;

  cairo_save(context);
  cairo_translate(context, rect.cx, rect.cy);
  cairo_rotate(context, wgu::deg2rad(rect.rotation));

  // Draw a standard (box) rect or rounded rectangle:
  if (rect.radius > 0.0) {
    PathHelperRoundedRect(context, rect);
  } else {
    cairo_rectangle(context, -rect.half_width(), -rect.half_height(),
                    rect.width, rect.height);
  }

  if (fill_color.IsValid()) {
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
