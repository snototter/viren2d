// STL
#include <string>
#include <iomanip>
#include <exception>
#include <cmath>
#include <utility>
#include <tuple>
#include <cstdlib>

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
  // If radius in (0, 0.5], we use the value as percentage.
  if (rect.radius <= 0.5) {
    rect.radius *= std::min(rect.width, rect.height);
  }
  const double half_width = rect.half_width() - rect.radius;
  const double half_height = rect.half_height() - rect.radius;
  cairo_move_to(context, -rect.half_width(), -half_height);
  cairo_arc(
        context, -half_width, -half_height, rect.radius,
        wgu::deg2rad(180), wgu::deg2rad(270));
  cairo_arc(
        context,  half_width, -half_height, rect.radius,
        wgu::deg2rad(-90), 0);
  cairo_arc(
        context,  half_width,  half_height, rect.radius,
        0, wgu::deg2rad(90));
  cairo_arc(
        context, -half_width,  half_height, rect.radius,
        wgu::deg2rad(90), wgu::deg2rad(180));
  cairo_close_path(context);
}


//---------------------------------------------------- Arc/Circle
void DrawArc(
    cairo_surface_t *surface, cairo_t *context,
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

/// Draws an open, solid arrow head.
void HelperDrawSolidHead(
    cairo_t * context, const Vec2d &pointy_end,
    const Vec2d &tip_a, const Vec2d &tip_b) {
  cairo_move_to(context, tip_a.x(), tip_a.y());
  cairo_line_to(context, pointy_end.x(), pointy_end.y());
  cairo_line_to(context, tip_b.x(), tip_b.y());
}


/// Extends the currently active (sub)path by
/// a closed arrow head.
Vec2d HelperClosedHead(
    cairo_t *context, const Vec2d &pointy_end,
    const Vec2d &tip_a, const Vec2d &tip_b,
    const Vec2d &line_from, const Vec2d &line_to) {
  // Compute the intersection between the shaft and
  // the connection line between the tip endpoints
  Vec2d shaft_point = ProjectPointOntoLine(
        tip_a, line_from, line_to);

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


void DrawArrow(
    cairo_surface_t *surface, cairo_t *context,
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

  // Terminology of the endpoints:
  //     "1st" is the tip at the 'to' end of the line.
  //           This will always be drawn.
  //     "2nd" is the tip at the 'from' end - only for
  //           double-headed arrows.
  // Compute orientation of the line:
  auto diff = from - to;
  const double shaft_angle_rad = std::atan2(diff.y(), diff.x());

  // Compute the offset/direction vectors from the line's
  // endpoints to the endpoints of each tip:
  const double tip_length = arrow_style.TipLengthForShaft(from, to);
  const double tip_angle_rad = wgu::deg2rad(arrow_style.tip_angle);
  auto tip_dir_1st_a = tip_length * Vec2d(
        std::cos(shaft_angle_rad + tip_angle_rad),
        std::sin(shaft_angle_rad + tip_angle_rad));
  auto tip_dir_1st_b = tip_length * Vec2d(
        std::cos(shaft_angle_rad - tip_angle_rad),
        std::sin(shaft_angle_rad - tip_angle_rad));
  // Compute endpoints of 1st tip:
  Vec2d tip_1st_a = to + tip_dir_1st_a;
  Vec2d tip_1st_b = to + tip_dir_1st_b;

  // If double-headed, we need a 2nd set of tip points:
  Vec2d tip_2nd_a = arrow_style.double_headed
      ? (from - tip_dir_1st_a)
      : Vec2d();
  Vec2d tip_2nd_b = arrow_style.double_headed
      ? (from - tip_dir_1st_b)
      : Vec2d();

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
      shaft_from = HelperClosedHead(
            context, from, tip_2nd_a, tip_2nd_b, from, to);
      cairo_fill_preserve(context);
      cairo_stroke(context);  // Stroke is currently solid
    }

    // Draw head Add shaft & head (at the line end)
    Vec2d shaft_to = HelperClosedHead(
          context, to, tip_1st_a, tip_1st_b, from, to);
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
    HelperDrawSolidHead(
          context, to, tip_1st_a, tip_1st_b);

    // Path for the second arrow head:
    if (arrow_style.double_headed) {
      HelperDrawSolidHead(
            context, from, tip_2nd_a, tip_2nd_b);
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

/// Computes the adjusted ellipse arc angle s.t. drawing
/// the ellipse in the scaled Cairo context results in
/// the desired user space angles.
double AdjustEllipseAngle(
    double deg, double scale_x, double scale_y) {
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


void DrawEllipse(
    cairo_surface_t *surface, cairo_t *context,
    Ellipse ellipse, const LineStyle &line_style,
    Color fill_color) {
  CheckCanvas(surface, context);
  CheckLineStyleAndFill(line_style, fill_color);

  if (!ellipse.IsValid()) {
    std::string s("Cannot draw an invalid ellipse: ");
    s += ellipse.ToString();
    s += '!';
    throw std::invalid_argument(s);
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
    ellipse.angle_from = AdjustEllipseAngle(
          ellipse.angle_from, scale_x, scale_y);
    is_partially_drawn = true;
  }
  if (!wgu::eps_equal(ellipse.angle_to, 360.0)) {
    ellipse.angle_to = AdjustEllipseAngle(
          ellipse.angle_to, scale_x, scale_y);
    is_partially_drawn = true;
  }

  // Save context twice (the extra save is needed to draw
  // strokes properly after we applied the scaling). For details see
  // https://www.cairographics.org/tutorial/#L2linewidth
  cairo_save(context);
  cairo_save(context);
  cairo_translate(context, ellipse.cx, ellipse.cy);
  cairo_rotate(context, wgu::deg2rad(ellipse.rotation));
  cairo_scale(context, scale_x, scale_y);

  cairo_arc(
        context, 0, 0, 1,
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
void DrawGrid(
    cairo_surface_t *surface, cairo_t *context,
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
    right = static_cast<double>(
          cairo_image_surface_get_width(surface));
    bottom = static_cast<double>(
          cairo_image_surface_get_height(surface));
  }

  // Draw the grid. To support thin lines, we need to
  // shift the coordinates by half a pixel.
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
void DrawLine(
    cairo_surface_t *surface, cairo_t *context,
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
/// Returns the number of steps needed to draw the given n-gon, the rotation
/// angle for the context, and the interior angle.
inline std::tuple<int, double, double> NGonMarkerSteps(Marker m) {
  switch (m) {
    case Marker::Pentagon:
      return std::make_tuple(4, 72.0, 108.0);

    case Marker::Pentagram:
      return std::make_tuple(4, 144.0, 36.0);

    case Marker::Hexagon:
      return std::make_tuple(5, 60.0, 120.0);

    case Marker::Hexagram:
      return std::make_tuple(5, 120.0, 60.0);

    case Marker::Heptagon:
      return std::make_tuple(6, 360.0 / 7.0, 128.57);

    case Marker::Heptagram:
      return std::make_tuple(6, 720.0 / 7.0, 77.14);

    case Marker::Octagon:
      return std::make_tuple(7, 45.0, 135.0);

    case Marker::Octagram:
      return std::make_tuple(7, 135.0, 45.0);

    case Marker::Enneagon:
      return std::make_tuple(8, 40, 140.0);

    case Marker::Enneagram:
      // Returns the steps for the {9/4} stellation
      return std::make_tuple(8, 160.0, 20.0);

    default: {
        std::string s("Marker '");
        s += MarkerToChar(m);
        s += "' is neither an n-sided polygon nor an n-angled star.";
        throw std::invalid_argument(s);
      }
  }
}

void DrawMarker(
    cairo_surface_t *surface, cairo_t *context,
    Vec2d pos, const MarkerStyle &style) {
  // General idea for all markers implemented so far:
  // * Translate the canvas
  // * Create the path(s), i.e. the marker shape's outline
  // * Either fill or stroke (xor! I don't want to deal
  //   with the effects of partially translucent colors
  //   which overlap between fill and stroke)

  // Sanity checks
  CheckCanvas(surface, context);

  if (!style.IsValid()) {
    std::string s("Cannot draw with invalid marker style ");
    s += style.ToString();
    s += '!';
    throw std::invalid_argument(s);
  }

  cairo_save(context);

  // Move to the center of the pixel coordinates, so each
  // marker can be drawn as if it's at the origin:
  pos += 0.5;
  cairo_translate(context, pos.x(), pos.y());

  const double miter_limit = cairo_get_miter_limit(context);
  double half_size = style.size / 2.0;

  // Optionally draw a bubble (or square) behind the marker to improve contrast
  if (style.background_color.IsValid()) {
    ApplyColor(context, style.background_color);
    if (style.marker == Marker::Square) {
      cairo_rectangle(
            context,
            -half_size - style.background_border,
            -half_size - style.background_border,
            style.size + 2 * style.background_border,
            style.size + 2 * style.background_border);
    } else {
      cairo_arc(
            context, 0.0, 0.0, half_size + style.background_border,
            0.0, 2 * M_PI);
    }
    cairo_fill(context);
  }

  ApplyMarkerStyle(context, style);
  cairo_new_path(context);
  switch (style.marker) {
    case Marker::Circle:
    case Marker::Point: {
        if (!style.IsFilled()) {
          half_size -= style.thickness / 2.0;
        }
        cairo_arc(context, 0.0, 0.0, half_size, 0.0, 2 * M_PI);
        break;
      }

    case Marker::Cross:
    case Marker::Plus: {
        half_size -= style.CapOffset();
        if (style.marker == Marker::Cross) {
          cairo_rotate(context, wgu::deg2rad(45.0));
        }
        cairo_move_to(context, -half_size, 0.0);
        cairo_line_to(context, half_size, 0.0);
        cairo_move_to(context, 0.0, -half_size);
        cairo_line_to(context, 0.0, half_size);
        break;
      }

    case Marker::Diamond: {
        if (!style.IsFilled()) {
          half_size -= style.JoinOffset(45.0, miter_limit);
        }
        const double half_diamond = 0.5 * half_size;
        cairo_move_to(context, 0.0, -half_size);
        cairo_line_to(context, half_diamond, 0.0);
        cairo_line_to(context, 0.0, half_size);
        cairo_line_to(context, -half_diamond, 0.0);
        cairo_close_path(context);
        break;
      }

    case Marker::RotatedSquare:
      cairo_rotate(context, wgu::deg2rad(45.0));
      // fall through
    case Marker::Square: {
        double side = style.size;
        if (!style.IsFilled()) {
          side -= 2 * style.JoinOffset(90.0, miter_limit);
        }

        if (style.marker == Marker::RotatedSquare) {
          // Adjust side length of the square, so that the rotated square
          // marker has the same height as all the other markers.
          side /= std::sqrt(2.0);
        }

        cairo_rectangle(context, -side / 2.0, -side / 2.0, side, side);
        break;
      }

    case Marker::TriangleUp:
    case Marker::TriangleDown:
    case Marker::TriangleLeft:
    case Marker::TriangleRight: {
        if (style.marker == Marker::TriangleRight) {
          cairo_rotate(context, wgu::deg2rad(90.0));
        } else if (style.marker == Marker::TriangleDown) {
          cairo_rotate(context, wgu::deg2rad(180.0));
        } else if (style.marker == Marker::TriangleLeft) {
          cairo_rotate(context, wgu::deg2rad(270.0));
        }

        if (!style.IsFilled()) {
          half_size -= style.JoinOffset(60.0, miter_limit);
        }
        cairo_move_to(context, 0, -half_size);
        cairo_rotate(context, wgu::deg2rad(120.0));
        cairo_line_to(context, 0, -half_size);
        cairo_rotate(context, wgu::deg2rad(120.0));
        cairo_line_to(context, 0, -half_size);
        cairo_close_path(context);
        break;
      }

    case Marker::Star: { // Asterisk
        half_size -= style.CapOffset();
        cairo_move_to(context, 0.0, -half_size);
        for (int i = 0; i < 5; ++i) {
          cairo_rotate(context, wgu::deg2rad(72.0));
          cairo_move_to(context, 0.0, 0.0);
          cairo_line_to(context, 0.0, -half_size);
        }
        break;
      }

    case Marker::Enneagon:
    case Marker::Enneagram:
    case Marker::Hexagon:
    case Marker::Heptagon:
    case Marker::Heptagram:
    case Marker::Octagon:
    case Marker::Octagram:
    case Marker::Pentagon:
    case Marker::Pentagram: {
        int steps;
        double ctx_rotation, interior_angle;
        std::tie(steps, ctx_rotation, interior_angle) = NGonMarkerSteps(style.marker);
        if (!style.IsFilled()) {
          half_size -= style.JoinOffset(interior_angle, miter_limit);
        }

        cairo_move_to(context, 0.0, -half_size);
        for (int i = 0; i < steps; ++i) {
          cairo_rotate(context, wgu::deg2rad(ctx_rotation));
          cairo_line_to(context, 0.0, -half_size);
        }
        cairo_close_path(context);
        break;
      }

    case Marker::Hexagram: {
        // Hexagram cannot be drawn by a single continuous path
        if (!style.IsFilled()) {
          half_size -= style.JoinOffset(60.0, miter_limit);
        }
        for (int path_idx = 0; path_idx < 2; ++path_idx) {
          if (path_idx == 1) {
            cairo_rotate(context, wgu::deg2rad(60.0));
          }
          cairo_move_to(context, 0.0, -half_size);
          for (int corner_idx = 0; corner_idx < 2; ++corner_idx) {
            cairo_rotate(context, wgu::deg2rad(120.0));
            cairo_line_to(context, 0.0, -half_size);
          }
          cairo_close_path(context);
        }
        break;
      }
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
                 const std::vector<Vec2d> &points,
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
void DrawRect(
    cairo_surface_t *surface, cairo_t *context,
    Rect rect, const LineStyle &line_style,
    Color fill_color) {
  CheckCanvas(surface, context);
  // Fill color may be changed here if it is 'same'
  CheckLineStyleAndFill(line_style, fill_color);

  if (!rect.IsValid()) {
    std::string s("Cannot draw an invalid rectangle: ");
    s += rect.ToString();
    s += '!';
    throw std::invalid_argument(s);
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
    cairo_rectangle(
          context, -rect.half_width(), -rect.half_height(),
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
