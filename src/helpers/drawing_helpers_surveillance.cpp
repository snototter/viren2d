// STL
#include <string>
#include <exception>
#include <cmath>
#include <cstdlib>
#include <tuple>

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
struct AlignedLabel {
  Rect bg_rect;
  MultiLineText text;
  double rotation;
  bool valid;

  AlignedLabel(
      Rect rect, MultiLineText txt, double rotation_angle)
    : bg_rect(rect), text(txt), rotation(rotation_angle), valid(true)
  {}

  AlignedLabel()
    : bg_rect(), text(), rotation(0.0), valid(false)
  {}
};


AlignedLabel PrepareAlignedLabel(
    cairo_t *context, Rect bounding_box, const BoundingBox2DStyle &style,
    const std::vector<std::string> &label, LabelPosition position) {
  if (label.empty()) {
    return AlignedLabel();
  }

  Rect label_box;
  Vec2d padding = style.label_padding;
  Vec2d oriented_size = bounding_box.Size();
  VerticalAlignment valign;
  double rotation = 0.0;
  switch (position) {
    case LabelPosition::Top:
      label_box = Rect::FromLTWH(
            -bounding_box.HalfWidth(), -bounding_box.HalfHeight(),
            bounding_box.width, bounding_box.height);
      valign = VerticalAlignment::Top;
      break;

    case LabelPosition::Bottom:
      label_box = Rect::FromLTWH(
            -bounding_box.HalfWidth(), -bounding_box.HalfHeight(),
            bounding_box.width, bounding_box.height);
      valign = VerticalAlignment::Bottom;
      break;

    case LabelPosition::LeftB2T:
      rotation = wgu::deg2rad(-90.0);
      label_box = Rect::FromLTWH(
            -bounding_box.HalfHeight(), -bounding_box.HalfWidth(),
            bounding_box.height, bounding_box.width);
      oriented_size = Vec2d(bounding_box.height, bounding_box.width);
      valign = VerticalAlignment::Top;
      padding = Vec2d(
            style.label_padding.y(), style.label_padding.x());
      break;

    case LabelPosition::LeftT2B://TODO double-check: t2b now seems to work!
      rotation = wgu::deg2rad(90.0);
      label_box = Rect::FromLTWH(
            -bounding_box.HalfHeight(), -bounding_box.HalfWidth(),
            bounding_box.height, bounding_box.width);
      oriented_size = Vec2d(bounding_box.height, bounding_box.width);
      valign = VerticalAlignment::Bottom;
      padding = Vec2d(
            style.label_padding.y(), style.label_padding.x());
      break;

    case LabelPosition::RightB2T:
      rotation = wgu::deg2rad(-90.0);
      label_box = Rect::FromLTWH(
            -bounding_box.HalfHeight(), -bounding_box.HalfWidth(),
            bounding_box.height, bounding_box.width);
      oriented_size = Vec2d(bounding_box.height, bounding_box.width);
      valign = VerticalAlignment::Bottom;
      padding = Vec2d(
            style.label_padding.y(), style.label_padding.x());
      break;

    case LabelPosition::RightT2B:
      rotation = wgu::deg2rad(90.0);
      label_box = Rect::FromLTWH(
            -bounding_box.HalfHeight(), -bounding_box.HalfWidth(),
            bounding_box.height, bounding_box.width);
      oriented_size = Vec2d(bounding_box.height, bounding_box.width);
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
  cairo_save(context);
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
  cairo_restore(context);
  return AlignedLabel(label_box, mlt, rotation);
}


std::tuple<Rect, std::vector<AlignedLabel>>
AlignBoundingBoxLabels(
    cairo_t *context, Rect bounding_box, const BoundingBox2DStyle &style,
    const std::vector<std::string> &label_top,
    const std::vector<std::string> &label_bottom,
    const std::vector<std::string> &label_left, bool left_top_to_bottom,
    const std::vector<std::string> &label_right, bool right_top_to_bottom) {
  Rect bbox_background(0.0, 0.0, bounding_box.width, bounding_box.height);
  std::vector<AlignedLabel> aligned_labels;

  AlignedLabel top = PrepareAlignedLabel(
        context, bounding_box, style, label_top, LabelPosition::Top);
  if (top.valid) {
    aligned_labels.push_back(top);

    bbox_background.cy += top.bg_rect.height / 2.0;
    bbox_background.height -= top.bg_rect.height;
  }

  AlignedLabel bottom = PrepareAlignedLabel(
        context, bounding_box, style, label_bottom, LabelPosition::Bottom);
  if (bottom.valid) {
    aligned_labels.push_back(bottom);

    bbox_background.cy -= bottom.bg_rect.height / 2.0;
    bbox_background.height -= bottom.bg_rect.height;
  }

  // Adjust height of the bounding box background to avoid 1px wide gaps if
  // we also need to place a text box on the left/right:
  if (top.valid || bottom.valid) {
    bbox_background.height += 0.5;
  }

  AlignedLabel left = PrepareAlignedLabel(
        context, bbox_background, style, label_left,
        left_top_to_bottom ? LabelPosition::LeftT2B : LabelPosition::LeftB2T);
  // TODO if top or bottom --> correct left label_box
  if (left.valid) {
//    if (top.valid) {
//      //FIXME shift up/down depends on t2b/b2t!!
//    }
//    if (bottom.valid) {
//      //FIXME
//    }
    aligned_labels.push_back(left);

    bbox_background.cx += left.bg_rect.height / 2.0; // Height because of the context rotation
    bbox_background.width -= left.bg_rect.height;
  }

  AlignedLabel right = PrepareAlignedLabel(
        context, bbox_background, style, label_right,//FIXME pass "free box bg space" instead of full bbox region
        right_top_to_bottom ? LabelPosition::RightT2B : LabelPosition::RightB2T);
  // TODO if top or bottom --> correct right label_box
  if (right.valid) {
//    if (top.valid) {
//      //FIXME
//    }
//    if (bottom.valid) {
//      //FIXME
//    }
    aligned_labels.push_back(right);

    bbox_background.cx -= right.bg_rect.height / 2.0; // Height because of the context rotation
    bbox_background.width -= right.bg_rect.height;
  }

  return std::make_tuple(bbox_background, aligned_labels);
}


bool DrawBoundingBox2D(
    cairo_surface_t *surface, cairo_t *context, Rect bounding_box,
    const BoundingBox2DStyle &style,
    const std::vector<std::string> &label_top,
    const std::vector<std::string> &label_bottom,
    const std::vector<std::string> &label_left, bool left_top_to_bottom,
    const std::vector<std::string> &label_right, bool right_top_to_bottom) {
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

  if (!bounding_box.IsValid()) {
    std::string s("Cannot draw an invalid bounding box: ");
    s += bounding_box.ToString();
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
  // * Draw the label (optionally clipped)

  // Shift coordinates to the pixel center to
  // correctly draw 1px borders
  bounding_box += 0.5;

  // Shift & rotate the canvas such that we have
  // a (0, 0)-centered, axis-aligned rectangle
  cairo_save(context);
  cairo_translate(context, bounding_box.cx, bounding_box.cy);
  cairo_rotate(context, wgu::deg2rad(bounding_box.rotation));

  // Draw a standard (square) rect or rounded rectangle:
  if (bounding_box.radius > 0.0) {
    PathHelperRoundedRect(context, bounding_box);
  } else {
    cairo_rectangle(
          context, -bounding_box.HalfWidth(), -bounding_box.HalfHeight(),
          bounding_box.width, bounding_box.height);
  }
  // Create a copy of this path to be reused for the
  // contour later on.
  cairo_path_t *bbox_path = cairo_copy_path(context);

  // Save the context, so we can reuse the copied bbox_path to draw the box'
  // contour later on. This is needed because we might have to rotate the
  // canvas again if there are labels along the left or right edges.
  cairo_save(context);

  std::vector<AlignedLabel> aligned_labels;
  Rect box_background;
  std::tie(box_background, aligned_labels) = AlignBoundingBoxLabels(
        context, bounding_box, style,
        label_top, label_bottom, label_left, left_top_to_bottom,
        label_right, right_top_to_bottom);

  // Text box backgrounds will always be clipped by the bounding box contour.
  cairo_clip(context);
  // Fill the box background.
  const auto bbox_fill = style.BoxFillColor();
  if (bbox_fill.IsValid()) {
    ApplyColor(context, bbox_fill);
    SPDLOG_WARN("FIXME: filling box background: {:s}", box_background);
    cairo_rectangle(
          context, box_background.left(), box_background.top(),
          box_background.width, box_background.height);
    cairo_fill(context);
  }
  // Then fill the text box background(s).
  const auto text_fill = style.TextFillColor();
  if (text_fill.IsValid()) {
    ApplyColor(context, text_fill);
    for (const auto &aligned : aligned_labels) {
      cairo_save(context);
      cairo_rotate(context, aligned.rotation);
      cairo_rectangle(
            context, aligned.bg_rect.left(), aligned.bg_rect.top(),
            aligned.bg_rect.width, aligned.bg_rect.height);
      cairo_fill(context);
      cairo_restore(context);
    }
  }
  cairo_reset_clip(context);
  /*


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

  */
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

  // Finally, draw the label on top.
  ApplyTextStyle(context, style.text_style, true);
  for (const auto &aligned : aligned_labels) {
    cairo_save(context);
    cairo_rotate(context, aligned.rotation);
    aligned.text.PlaceText(context);
    cairo_restore(context);
  }

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
