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
  /// Rectangle (in the contained bounding box' coordinate frame) which
  /// encapsulates the text.
  Rect text_box{};

  /// Holds the label's text and takes care of its rendering.
  MultiLineText text{};

  /// Rotation angle (in radians) of the cairo canvas.
  double canvas_rotation{0.0};

  /// True if this instance has been initialized with a label.
  bool valid{false};


  AlignedLabel()
    : text_box(), text(), canvas_rotation(0.0), valid(false)
  {}

  AlignedLabel(
      Rect rect, MultiLineText txt, double rotation_angle)
    : text_box(rect), text(txt), canvas_rotation(rotation_angle), valid(true)
  {}
};


AlignedLabel PrepareAlignedLabel(
    cairo_t *context, const Rect &bounding_box,
    const BoundingBox2DStyle &style,
    const std::vector<std::string> &label,
    LabelPosition position) {
  // Return if there is no text to be placed.
  if (label.empty()) {
    return AlignedLabel();
  }

  // Set rotation of the canvas & vertical alignment
  double rotation{0.0};
  VerticalAlignment valign{VerticalAlignment::Top};
  switch (position) {
    case LabelPosition::Top:
      valign = VerticalAlignment::Top;
      break;

    case LabelPosition::Bottom:
      valign = VerticalAlignment::Bottom;
      break;

    case LabelPosition::LeftB2T:
      valign = VerticalAlignment::Top;
      rotation = wgu::deg2rad(-90.0);
      break;

    case LabelPosition::LeftT2B:
      valign = VerticalAlignment::Bottom;
      rotation = wgu::deg2rad(+90.0);
      break;

    case LabelPosition::RightB2T:
      valign = VerticalAlignment::Bottom;
      rotation = wgu::deg2rad(-90.0);
      break;

    case LabelPosition::RightT2B:
      valign = VerticalAlignment::Top;
      rotation = wgu::deg2rad(+90.0);
      break;
  }

  // Set the reference position for the text. Note that this is the
  // position *after* rotating the canvas (around the bounding
  // box center).
  Vec2d rotated_anchor_position{0.0, 0.0};
  switch (position) {
    case LabelPosition::Top:
    case LabelPosition::Bottom:
      rotated_anchor_position.SetX(
            (style.text_style.halign == HorizontalAlignment::Left)
            ? (bounding_box.left())
            : ((style.text_style.halign == HorizontalAlignment::Center)
               ? bounding_box.cx
               : bounding_box.right()));
      // Here, valign can only be top or bottom.
      rotated_anchor_position.SetY(
            (valign == VerticalAlignment::Top)
            ? bounding_box.top()
            : bounding_box.bottom());
      break;

    case LabelPosition::RightB2T:
    case LabelPosition::LeftB2T:
      // Both options rotate by -90° to align the text from bottom-to-top.
      rotated_anchor_position.SetX(
            -1.0 * ((style.text_style.halign == HorizontalAlignment::Left)
                    ? (bounding_box.bottom())
                    : ((style.text_style.halign == HorizontalAlignment::Center)
                       ? bounding_box.cy
                       : bounding_box.top())));
      // Here, valign can only be top or bottom.
      rotated_anchor_position.SetY(
            (valign == VerticalAlignment::Top)
            ? bounding_box.left()
            : bounding_box.right());
      break;

    case LabelPosition::RightT2B:
    case LabelPosition::LeftT2B:
      // Both options rotate by +90° to align the text from top-to-bottom.
      rotated_anchor_position.SetX(
            (style.text_style.halign == HorizontalAlignment::Left)
            ? (bounding_box.top())
            : ((style.text_style.halign == HorizontalAlignment::Center)
               ? bounding_box.cy
               : bounding_box.bottom()));
      // Here, valign can only be top or bottom.
      rotated_anchor_position.SetY(
            -1.0 * ((valign == VerticalAlignment::Top)
                    ? bounding_box.right()
                    : bounding_box.left()));
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
        rotated_anchor_position,
        valign | style.text_style.halign,
        style.label_padding, {-1, -1});
  cairo_restore(context);

  // After text alignment, we can set the height (top/bottom edge) or
  // width (left/right edge) of the label's text box properly (i.e. using
  // the correct text dimension).
  Rect text_box;
  switch (position) {
    case LabelPosition::Top:
      text_box = Rect::FromLTWH(
            bounding_box.left(), bounding_box.top(),
            bounding_box.width, mlt.Height());
      break;

    case LabelPosition::Bottom:
      text_box = Rect::FromLTWH(
            bounding_box.left(), bounding_box.bottom() - mlt.Height(),
            bounding_box.width, mlt.Height());
      break;

    case LabelPosition::LeftB2T:
    case LabelPosition::LeftT2B:
      text_box = Rect::FromLTWH(
            bounding_box.left(), bounding_box.top(),
            mlt.Height(), bounding_box.height);
      break;

    case LabelPosition::RightB2T:
    case LabelPosition::RightT2B:
      text_box = Rect::FromLTWH(
            bounding_box.right() - mlt.Height(), bounding_box.top(),
            mlt.Height(), bounding_box.height);
      break;
  }
  return AlignedLabel(text_box, mlt, rotation);
}



/// Returns:
///   * a flag indicating whether any text should be drawn at all
///   * a rectangle which defines the portion of the bounding box which is
///     not covered by a label's text box background (if the text box should
///     be filled)
///   * a vector containing the text placement details for each drawable
///     label
std::tuple<bool, Rect, std::vector<AlignedLabel>>
AlignBoundingBoxLabels(
    cairo_t *context, const Rect &bounding_box, const BoundingBox2DStyle &style,
    const std::vector<std::string> &label_top,
    const std::vector<std::string> &label_bottom,
    const std::vector<std::string> &label_left, bool left_top_to_bottom,
    const std::vector<std::string> &label_right, bool right_top_to_bottom) {
  // Indicates if there is any text to draw at all.
  bool has_text {false};

  // Holds the properly aligned labels (using the chosen TextStyle).
  std::vector<AlignedLabel> aligned_labels;

  // Remaining "free" area (i.e. the bbox "background") if we would fill all
  // text boxes.
  Rect bbox_without_text {bounding_box};

  AlignedLabel top = PrepareAlignedLabel(
        context, bounding_box, style, label_top,
        LabelPosition::Top);
  if (top.valid) {
    has_text = true;
    aligned_labels.push_back(top);

    // Adjust the remaining "free" area.
    bbox_without_text.cy += top.text_box.height / 2.0;
    bbox_without_text.height -= top.text_box.height;
  }

  AlignedLabel bottom = PrepareAlignedLabel(
        context, bounding_box, style, label_bottom,
        LabelPosition::Bottom);
  if (bottom.valid) {
    has_text = true;
    aligned_labels.push_back(bottom);

    bbox_without_text.cy -= bottom.text_box.height / 2.0;
    bbox_without_text.height -= bottom.text_box.height;
  }

  // After "placing" the top/bottom labels, ensure that left/right labels can
  // only occupy the remaining "free" space:
  const Rect available_text_region {bbox_without_text};

  AlignedLabel left = PrepareAlignedLabel(
        context, available_text_region, style, label_left,
        left_top_to_bottom ? LabelPosition::LeftT2B : LabelPosition::LeftB2T);
  if (left.valid) {
    has_text = true;
    aligned_labels.push_back(left);

    bbox_without_text.cx += left.text_box.width / 2.0;
    bbox_without_text.width -= left.text_box.width;
  }

  // Similar computation for text to be placed along the right edge of the box:
  AlignedLabel right = PrepareAlignedLabel(
        context, available_text_region, style, label_right,
        right_top_to_bottom ? LabelPosition::RightT2B : LabelPosition::RightB2T);
  if (right.valid) {
    has_text = true;
    aligned_labels.push_back(right);

    bbox_without_text.cx -= right.text_box.width / 2.0;
    bbox_without_text.width -= right.text_box.width;
  }

  return std::make_tuple(
        has_text,
        style.TextFillColor().IsValid() ? bbox_without_text : bounding_box,
        aligned_labels);
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
  // * Draw the label(s) (optionally clipped)

  // Shift coordinates to the pixel center to
  // correctly draw 1px borders
  bounding_box += 0.5;

  // Shift & rotate the canvas such that we have
  // a (0, 0)-centered, axis-aligned rectangle
  cairo_save(context);
  cairo_translate(context, bounding_box.cx, bounding_box.cy);
  cairo_rotate(context, wgu::deg2rad(bounding_box.rotation));
  bounding_box.cx = 0.0;
  bounding_box.cy = 0.0;

  // Draw a standard (square) rect or rounded rectangle:
  if (bounding_box.radius > 0.0) {
    PathHelperRoundedRect(context, bounding_box);
  } else {
    cairo_rectangle(
          context, -bounding_box.HalfWidth(), -bounding_box.HalfHeight(),
          bounding_box.width, bounding_box.height);
  }
  // Create a copy of this path which will be reused to draw the
  // contour after we finished with filling the background.
  cairo_path_t *bbox_path = cairo_copy_path(context);

  // Save the context, so we can reuse the copied bbox_path to draw the box'
  // contour later on. This is needed because we might have to rotate the
  // canvas again if there are labels along the left or right edges.
  cairo_save(context);

  bool has_text;
  Rect box_background;
  std::vector<AlignedLabel> aligned_labels;
  std::tie(has_text, box_background, aligned_labels) = AlignBoundingBoxLabels(
        context, bounding_box, style,
        label_top, label_bottom,
        label_left, left_top_to_bottom,
        label_right, right_top_to_bottom);

  // Since the (potentially filled) text box backgrounds will always be
  // clipped by the bounding box contour, we don't have to draw rounded
  // corners here.
  cairo_clip(context);
  // First, fill the box background.
  const auto bbox_fill = style.BoxFillColor();
  if (bbox_fill.IsValid()) {
    ApplyColor(context, bbox_fill);
    cairo_rectangle(
          context, box_background.left(), box_background.top(),
          box_background.width, box_background.height);
    cairo_fill(context);
  }
  // Then fill the text box background(s).
  const auto text_fill = style.TextFillColor();
  if (has_text && text_fill.IsValid()) {
    ApplyColor(context, text_fill);
    for (const auto &aligned : aligned_labels) {
      // The label's text box is defined in the bounding box' reference
      // frame, i.e. we don't need to transform the canvas for filling
      // the text box:
      cairo_rectangle(
            context, aligned.text_box.left(), aligned.text_box.top(),
            aligned.text_box.width, aligned.text_box.height);
      cairo_fill(context);
    }
  }
  cairo_reset_clip(context);
  cairo_restore(context);

  // I wanted to always see the box contour, thus the only check needed
  // is whether the user wants to clip the box labels or let them overflow
  // (in case the text is too long).
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

  // Finally, draw the label(s) on top.
  if (has_text) {
    ApplyTextStyle(context, style.text_style, true);
    for (const auto &aligned : aligned_labels) {
      cairo_save(context);
      cairo_rotate(context, aligned.canvas_rotation);
      aligned.text.PlaceText(context);
      cairo_restore(context);
    }
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
