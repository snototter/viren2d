// STL
#include <string>
#include <exception>
#include <cmath>
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
//---------------------------------------------------- BoundingBox 2D
bool DrawBoundingBox2D(
    cairo_surface_t *surface, cairo_t *context,
    Rect rect, const std::vector<std::string> &label,
    const BoundingBox2DStyle &style) {
  //-------------------- Sanity checks
  if (!CheckCanvas(surface, context)) {
    return false;
  }

  if (!style.IsValid()) {
    std::string s("Cannot draw a bounding box with an invalid style: ");
    s += style.ToString(); //TODO implement and use ToDetailedString?
    s += '!';
    SPDLOG_WARN(s);
    return false;
  }

  if (!rect.IsValid()) {
    std::string s("Cannot draw an invalid bounding box: ");
    s += rect.ToString();
    s += '!';
    SPDLOG_WARN(s);
    return false;
  }

  //-------------------- Drawing
  // In a nutshell:
  // * (optional) Fill the box background
  // * (optional) Fill the text box background (but only
  //   the region which intersects with the box)
  // * Draw the box contour
  // * Draw the label

  // Shift coordinates to the pixel center to
  // correctly draw 1px borders
  rect += 0.5;

  // Shift & rotate the canvas such that we have
  // a (0, 0)-centered, axis-aligned rectangle
  cairo_save(context);
  cairo_translate(context, rect.cx, rect.cy);
  cairo_rotate(context, wgu::deg2rad(rect.rotation));

  // Draw a standard (square) rect or rounded rectangle:
  if (rect.radius > 0.0) {
    PathHelperRoundedRect(context, rect);
  } else {
    cairo_rectangle(
          context, -rect.HalfWidth(), -rect.HalfHeight(),
          rect.width, rect.height);
  }
  // Create a copy of this path to be reused for the
  // contour later on.
  cairo_path_t *bbox_path = cairo_copy_path(context);

  // For labels along the left/right edge, we have to rotate
  // the canvas again. We use a separate context so that we
  // can reuse the copied bbox_path to draw the box' contour
  // later on.
  cairo_save(context);

  // Logic behind the following (quite redundant, I know) code
  // blocks:
  // * If labels should be placed along the left/right edge, we
  //   rotate the canvas by +/-90 degrees. Then, we only have to
  //   differ between top-/bottom-aligned labels.
  // * If we have to rotate, we also have to swap x/y coordinates,
  //   including the label's padding.
  // * To reuse the box path from above, we then have to use the
  //   original context (original x/y coordinates).
  // TODO(cleanup) The following is a prime candidate for refactoring
  //   and simplification
  Rect label_box;
  Vec2d padding = style.label_padding;
  Vec2d oriented_size = rect.Size();
  VerticalAlignment valign;
  double rotation = 0.0;
  switch (style.label_position) {
    case LabelPosition::Top:
      label_box = Rect::FromLTWH(
            -rect.HalfWidth(), -rect.HalfHeight(),
            rect.width, rect.height);
      valign = VerticalAlignment::Top;
      break;

    case LabelPosition::Bottom:
      label_box = Rect::FromLTWH(
            -rect.HalfWidth(), -rect.HalfHeight(),
            rect.width, rect.height);
      valign = VerticalAlignment::Bottom;
      break;

    case LabelPosition::LeftB2T:
      rotation = wgu::deg2rad(-90.0);
      label_box = Rect::FromLTWH(
            -rect.HalfHeight(), -rect.HalfWidth(),
            rect.height, rect.width);
      oriented_size = Vec2d(rect.height, rect.width);
      valign = VerticalAlignment::Top;
      padding = Vec2d(
            style.label_padding.y(), style.label_padding.x());
      break;

    case LabelPosition::LeftT2B://TODO double-check: t2b now seems to work!
      rotation = wgu::deg2rad(90.0);
      label_box = Rect::FromLTWH(
            -rect.HalfHeight(), -rect.HalfWidth(),
            rect.height, rect.width);
      oriented_size = Vec2d(rect.height, rect.width);
      valign = VerticalAlignment::Bottom;
      padding = Vec2d(
            style.label_padding.y(), style.label_padding.x());
      break;

    case LabelPosition::RightB2T:
      rotation = wgu::deg2rad(-90.0);
      label_box = Rect::FromLTWH(
            -rect.HalfHeight(), -rect.HalfWidth(),
            rect.height, rect.width);
      oriented_size = Vec2d(rect.height, rect.width);
      valign = VerticalAlignment::Bottom;
      padding = Vec2d(
            style.label_padding.y(), style.label_padding.x());
      break;

    case LabelPosition::RightT2B:
      rotation = wgu::deg2rad(90.0);
      label_box = Rect::FromLTWH(
            -rect.HalfHeight(), -rect.HalfWidth(),
            rect.height, rect.width);
      oriented_size = Vec2d(rect.height, rect.width);
      valign = VerticalAlignment::Top;
      padding = Vec2d(
            style.label_padding.y(), style.label_padding.x());
      break;
  }

  // In addition to the placement of the label, we also
  // have to check how to align the label text horizontally
  // within the desired region:
  Vec2d text_anchor {
    0.0,
    (valign == VerticalAlignment::Top)
        ? label_box.top()
        : label_box.bottom()};

  switch (style.text_style.halign) {
    case HorizontalAlignment::Left:
      text_anchor.SetX(label_box.left());
      break;

    case HorizontalAlignment::Center:
      text_anchor.SetX(0.0);
      break;

    case HorizontalAlignment::Right:
      text_anchor.SetX(label_box.right());
      break;
  }

  // Now, we compute the text extent and finalize
  // computing the reference point, i.e. where to
  // place the label and its (optional) text box:
  cairo_rotate(context, rotation);
  ApplyTextStyle(context, style.text_style, false);
  MultiLineText mlt(label, style.text_style, context);
  mlt.Align(
        text_anchor,
        valign | style.text_style.halign,
        padding, {-1, -1});
  if (valign == VerticalAlignment::Top) {
    label_box = Rect::FromLTWH(
          label_box.left(), label_box.top(),
          label_box.width, mlt.Height());
  } else if (valign == VerticalAlignment::Bottom) {
    label_box = Rect::FromLTWH(
          label_box.left(), label_box.bottom() - mlt.Height(),
          label_box.width, mlt.Height());
  } else {
    // This exception can remain. Would be caused by an implementation
    // error (in this function) only.
    throw std::logic_error(
          "Internal vertical alignment in "
          "helpers::DrawBoundingBox2d must "
          "be either Top or Bottom!");
  }

  // Optionally, fill the text box
  cairo_clip(context);
  const auto bbox_fill = style.BoxFillColor();
  const auto text_fill = style.TextFillColor();
  if (text_fill.IsValid()) {
    ApplyColor(context, text_fill);
    cairo_rectangle(context, label_box.left(), label_box.top(),
                    label_box.width, label_box.height);
    cairo_fill(context);

    // Fill bounding box
    if (bbox_fill.IsValid()) {
      helpers::ApplyColor(context, bbox_fill);
      auto fill_roi = (valign == VerticalAlignment::Top) ?
            Rect::FromLTWH(
              label_box.left(), label_box.bottom(),
              label_box.width,
              oriented_size.height() - label_box.height)
          : Rect::FromLRTB(
              label_box.left(), label_box.right(),
              -oriented_size.height() / 2.0,
              label_box.top());
      cairo_rectangle(
            context, fill_roi.left(), fill_roi.top(),
            fill_roi.width, fill_roi.height);
      cairo_fill(context);
    }
  } else {
    // We don't have to mask out the label background
    // and can simply fill the currently clipped
    // region
    if (bbox_fill.IsValid()) {
      helpers::ApplyColor(context, bbox_fill);
      cairo_paint(context);
    }
  }
  cairo_reset_clip(context);
  cairo_restore(context);

  // We always draw the box' contour:
  cairo_new_path(context);
  ApplyLineStyle(context, style.line_style);
  cairo_append_path(context, bbox_path);
  cairo_path_destroy(bbox_path);
  if (style.clip_label) {
    cairo_stroke_preserve(context);
    cairo_clip(context);
  } else {
    cairo_stroke(context);
  }

  // Finally, draw the label on top
  cairo_rotate(context, rotation);

  // Since we saved/restored the context, we
  // have to re-apply the text style:
  ApplyTextStyle(context, style.text_style, true);
  mlt.PlaceText(context);

  // Pop the original context
  cairo_restore(context);
  return true;
}


//---------------------------------------------------- Trajectory 2D
bool DrawTrajectory(
      cairo_surface_t *surface, cairo_t *context,
      const std::vector<Vec2d> &points, const LineStyle &style,
      Color color_fade_out, bool oldest_position_first,
      const std::function<double(double)> &mix_factor) {
  if (!CheckCanvas(surface, context)) {
    return false;
  }

  if (!style.IsValid()) {
    std::string s(
          "Cannot draw a trajectory with an invalid line style: ");
    s += style.ToDetailedString();
    s += '!';
    SPDLOG_WARN(s);
    return false;
  }

  if (points.size() < 2) {
    SPDLOG_WARN("Input trajectory must have at least 2 points!");
    return false;
  }

  if (color_fade_out.IsSpecialSame()) {
    color_fade_out = style.color.WithAlpha(color_fade_out.alpha);
  }
  const bool fade_out = color_fade_out.IsValid() && (color_fade_out != style.color);

  cairo_save(context);
  ApplyLineStyle(context, style);
  if (fade_out) {
    const double total_length = wgu::LengthPolygon(points);
    double processed_length = 0.0;
    double proportion_color_head = mix_factor(0.0);
    const Color &color_first = oldest_position_first
        ? color_fade_out
        : style.color;
    const Color &color_last = oldest_position_first
        ? style.color
        : color_fade_out;
    Color color_from = color_first.Mix(
          color_last, mix_factor(proportion_color_head));
    Color color_to;

    // Fading out requires a separate path for each line segment,
    // so that we can apply the color gradient.
    for (std::size_t idx = 1; idx < points.size(); ++idx) {
      cairo_pattern_t *pattern = cairo_pattern_create_linear(
          points[idx-1].x(), points[idx-1].y(),
          points[idx].x(), points[idx].y());
      // See ApplyColor() on why we have to use bgra:
      cairo_pattern_add_color_stop_rgba(pattern, 0.0,
          color_from.blue, color_from.green,
          color_from.red, color_from.alpha);

      // The stop color of the current segment's color gradient
      // depends on how far we are along the trajectory:
      processed_length += points[idx-1].Distance(points[idx]);
      proportion_color_head = mix_factor(processed_length / total_length);
      color_to = oldest_position_first
          ? color_fade_out.Mix(style.color, proportion_color_head)
          : style.color.Mix(color_fade_out, proportion_color_head);
      cairo_pattern_add_color_stop_rgba(
            pattern, 1.0,
            color_to.blue, color_to.green,
            color_to.red, color_to.alpha);

      // Draw the current line segment with this linear color gradient:
      cairo_move_to(context, points[idx-1].x(), points[idx-1].y());
      cairo_line_to(context, points[idx].x(), points[idx].y());
      cairo_set_source(context, pattern);
      cairo_stroke(context);
      cairo_pattern_destroy(pattern);
      color_from = color_to;
    }
  } else {
    // The whole trajectory should be drawn with the same
    // color. Thus, we can create a single path:
    cairo_move_to(context, points[0].x(), points[0].y());
    for (std::size_t idx = 1; idx < points.size(); ++idx) {
      cairo_line_to(context, points[idx].x(), points[idx].y());
    }
    cairo_stroke(context);
  }
  cairo_restore(context);

  return true;
}

} // namespace helpers
} // namespace viren2d
