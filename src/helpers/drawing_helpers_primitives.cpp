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
#include <helpers/enum.h>


namespace viren2d {
namespace helpers {
void CheckLineStyle(const LineStyle &style) {
  if (!style.IsValid()) {
    std::stringstream s;
    s << "Cannot draw with invalid line style " << style.ToString() << "!";
    throw std::invalid_argument(s.str());
  }
}


void CheckLineStyleAndFill(const LineStyle &style,
                           const Color &fill_color) {
  if (!style.IsValid() && !fill_color.IsValid()) {
    std::stringstream s;
    s << "Cannot draw with invalid line style & invalid fill color: "
      << style.ToString() << " and " << fill_color.ToString() << "!";
    throw std::invalid_argument(s.str());
  }
}


//---------------------------------------------------- Math/Geometry Helpers
//TODO(snototter) These should be moved to math.h

/** @brief Project point onto line. */
Vec2d ProjectPointOntoLine(const Vec2d &pt, const Vec2d &line_from, const Vec2d &line_to) {
  // Vector from line start to point:
  const Vec2d v = line_from.DirectionVector(pt);
  // Project point onto line via dot product:
  const Vec2d unit_direction = line_from.DirectionVector(line_to).UnitVector();
  const double lambda = unit_direction.Dot(v);
  return line_from + lambda * unit_direction;
}


/** Computes the determinant of the two 2d vectors. */
double Determinant(const Vec2d &a, const Vec2d &b) {
  return a.x() * b.y() - b.x() * a.y();
}


/** Computes the angle (in radians) of a 2d direction vector w.r.t. the positive X axis. */
double AngleRadFromDirectionVec(const Vec2d &vec) {
  // Dot product is proportional to the cosine, whereas
  // the determinant is proportional to the sine.
  // See: https://math.stackexchange.com/a/879474
  Vec2d ref(1, 0);
  Vec2d unit = vec.UnitVector();
  return std::atan2(Determinant(ref, unit), ref.Dot(unit));
}


/** Computes the angle (in degrees) of a 2d direction vector w.r.t. the positive X axis. */
double AngleDegFromDirectionVec(const Vec2d &vec) {
  return rad2deg(AngleRadFromDirectionVec(vec));
}


/** Computes the direction vector given its angle (in radians) w.r.t. the positive X axis. */
Vec2d DirectionVecFromAngleRad(double rad) {
  return Vec2d(std::cos(rad), std::sin(rad)); // TODO verify it's unit length
}


/** Computes the direction vector given its angle (in radians) w.r.t. the positive X axis. */
Vec2d DirectionVecFromAngleDeg(double deg) {
  return DirectionVecFromAngleRad(deg2rad(deg));
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

  if (radius <= 0.0) {
    throw std::invalid_argument("Radius must be > 0.0!");
  }

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
/**
 * Computes the adjusted ellipse arc angle s.t. drawing
 * the ellipse in the scaled Cairo context results in
 * the desired user space angles.
 */
double AdjustEllipseAngle(double deg, double scale_x, double scale_y) {
  // Compute the direction vector corresponding to
  // the desired angle.
  auto dir = DirectionVecFromAngleDeg(deg);
  // Apply the inverse transformation (scaling).
  dir.SetX(dir.x() / scale_x);
  dir.SetY(dir.y() / scale_y);
  // Compute the angle (w.r.t. to the positive X axis)
  // which should be used to draw the path in cairo_arc
  // after the context is transformed.
  return AngleDegFromDirectionVec(dir);
}


void DrawEllipse(cairo_surface_t *surface, cairo_t *context,
                 Ellipse ellipse, const LineStyle &line_style,
                 const Color &fill_color) {
  CheckCanvas(surface, context);
  CheckLineStyleAndFill(line_style, fill_color);

  if (!ellipse.IsValid()) {
    std::stringstream s;
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
  if (!eps_zero(ellipse.angle_from)) {
    //TODO log debug: adjusting angle1
    ellipse.angle_from = AdjustEllipseAngle(ellipse.angle_from,
                                            scale_x, scale_y);
    is_partially_drawn = true;
  }
  if (!eps_equal(ellipse.angle_to, 360.0)) {
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
  cairo_rotate(context, deg2rad(ellipse.rotation));
  cairo_scale(context, scale_x, scale_y);

  cairo_arc(context, 0, 0, 1,
            deg2rad(ellipse.angle_from),
            deg2rad(ellipse.angle_to));

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
    std::stringstream s;
    s << "Cannot draw an invalid rectangle: " << rect << "!";
    throw std::invalid_argument(s.str());
  }

  // Shift to the pixel center (so 1px borders are drawn correctly)
  rect += 0.5;

  cairo_save(context);
  cairo_translate(context, rect.cx, rect.cy);
  cairo_rotate(context, deg2rad(rect.rotation));

  // Draw a standard (box) rect or rounded rectangle:
  if (rect.radius > 0.0) {
    // If radius in [0, 1], we use it as a percentage
    // Actually, due to the IsValid() check, it will
    // either be (0, 0.5] or >= 1
    if (rect.radius < 1.0) {
      rect.radius *= std::min(rect.width, rect.height);
    }
    PathHelperRoundedRect(context, rect);
  } else {
    cairo_rectangle(context, -rect.half_width(), -rect.half_height(),
                    rect.width, rect.height);
  }

  if (fill_color.alpha > 0.0) {
    helpers::ApplyColor(context, fill_color);
    cairo_fill_preserve(context);
  }

  helpers::ApplyLineStyle(context, line_style);
  cairo_stroke(context);
  // Restore context
  cairo_restore(context);
}


//---------------------------------------------------- Text (plain & boxed)
Vec2d GetTextAnchorPosition(Vec2d position, TextAnchor anchor,
                            const cairo_text_extents_t &extents,
                            double padding) {
  //TODO add padding (default = 0) parameter, then we can reuse it for bounding box & text box!
  // Default Cairo text position is bottom-left

  // Adjust horizontal alignment.
  double x = position.x();
  if (IsFlagSet(anchor, TextAnchor::CenterHorizontal)) {
    x -= (extents.width / 2.0);
  } else if (IsFlagSet(anchor, TextAnchor::Right)) {
    x -= (extents.width + padding);
  } else {  // Left-aligned
    x += padding;
  }
  position.SetX(x);

  // Adjust vertical alignment.
  double y = position.y();
  if (IsFlagSet(anchor, TextAnchor::CenterVertical)) {
    y += (extents.height / 2.0);
  } else if (IsFlagSet(anchor, TextAnchor::Top)) {
    y += (extents.height + padding);
  } else {  // Bottom-aligned
    y -= padding;
  }
  position.SetY(y);

  return position;
}


void DrawText(cairo_surface_t *surface, cairo_t *context,
              const std::string &text, Vec2d position,
              TextAnchor text_anchor,
              const TextStyle &desired_text_style,
              const TextStyle &current_context_style) {
  CheckCanvas(surface, context);

  if (text.empty()) {
    // Nothing to do
    return;
  }

  if (!desired_text_style.IsValid()) {
    std::stringstream s;
    s << "Cannot draw text with invalid style: "
      << desired_text_style;
    throw std::invalid_argument(s.str());
  }

  // Push the current context. Now, we just apply styles
  // specific to *this* DrawText call.
  cairo_save(context);

  if (desired_text_style != current_context_style) {
    ApplyTextStyle(context, desired_text_style);
  }

  // We have to always apply font color in this context (as
  // Cairo uses the default source rgba values, which could
  // have been overwritten since setting the default font
  // style)
  ApplyColor(context, desired_text_style.font_color);

#define VIREN2D_DEBUG_TEXT_EXTENT  // TODO undef & document the debug flag
#ifdef VIREN2D_DEBUG_TEXT_EXTENT
  const auto desired_position = position;
#endif  // VIREN2D_DEBUG_TEXT_EXTENT
  double padding = 0; // TODO parametrization!
  //TODO extend drawText to support text boxes

  //FIXME default styles:
  //viren2d - some sane presets for each style; not changeable!
  //painter - init with presets; customizable by user!

  // Now that the font face is set up, we can query
  // the rendered text extents and use them to adjust
  // the position according to the desired text anchor:
  cairo_text_extents_t extents;
  cairo_text_extents(context, text.c_str(), &extents);
  position = GetTextAnchorPosition(position, text_anchor,
                                   extents, padding);

  // Shift to the pixel center, and move to the origin of the
  // first glyph. Then, we can let Cairo render the text
  position += 0.5;
  cairo_move_to(context, position.x(), position.y());
  cairo_show_text(context, text.c_str());

#ifdef VIREN2D_DEBUG_TEXT_EXTENT
  // Draw a box showing the text extent
  ApplyLineStyle(context, LineStyle(1, desired_text_style.font_color));
  auto tl = position + Vec2d(0, -extents.height);
  cairo_rectangle(context, tl.x(), tl.y(), extents.width, extents.height);
  cairo_stroke(context);

  // Draw a box or circle at the desired location, showing the
  // size of the padded region
  ApplyLineStyle(context, LineStyle(1, Color::Black));
  if (padding > 0) {
    cairo_rectangle(context, desired_position.x() - padding,
                    desired_position.y() - padding,
                    2 * padding, 2 * padding);
  } else {
    // If we don't have padding, draw a small circle:
    cairo_arc(context, desired_position.x(), desired_position.y(),
              4, 0, 2 * M_PI);
  }
  cairo_stroke(context);

  // how a text box with that padding would look like:
  ApplyColor(context, "gray!40");
  cairo_rectangle(context, tl.x() - padding, tl.y() - padding,
                  extents.width + 2 * padding,
                  extents.height + 2 * padding);
  cairo_fill(context);
#endif  // VIREN2D_DEBUG_TEXT_EXTENT

  // Pop the original context
  cairo_restore(context);
}
} // namespace helpers
} // namespace viren2d
