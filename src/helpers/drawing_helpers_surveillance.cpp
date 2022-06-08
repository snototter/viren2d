// STL
#include <string>
#include <sstream>
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
//---------------------------------------------------- BoundingBox 2D
void DrawBoundingBox2D(cairo_surface_t *surface, cairo_t *context,
                       Rect rect, const std::vector<const char*> &label,
                       const BoundingBox2DStyle &style) {
  //-------------------- Sanity checks
  CheckCanvas(surface, context);

  if (!style.IsValid()) {
    std::ostringstream s;
    s << "Cannot draw a bounding box with an invalid style: "
      << style;
    throw std::invalid_argument(s.str());
  }

  if (!rect.IsValid()) {
    std::ostringstream s;
    s << "Cannot draw an invalid bounding box: " << rect << "!";
    throw std::invalid_argument(s.str());
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
    cairo_rectangle(context, -rect.half_width(), -rect.half_height(),
                    rect.width, rect.height);
  }

  // We need a copy of the box path because we switch between box fill,
  // (optional) text box fill, box contour & label (+ the optional clipping)
  cairo_path_t *bbox_path = cairo_copy_path(context);

  // Fill bounding box
  const auto bbox_fill = style.BoxFillColor();
  if (bbox_fill.IsValid()) {
    helpers::ApplyColor(context, bbox_fill);
    cairo_fill_preserve(context);
  }

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
  VerticalAlignment valign;
  double rotation = 0.0;
  switch (style.label_position) {
    case BoundingBoxLabelPosition::Top:
      label_box = Rect::FromLTWH(-rect.half_width(),
                                 -rect.half_height(),
                                 rect.width, rect.height);
      valign = VerticalAlignment::Top;
      break;

    case BoundingBoxLabelPosition::Bottom:
      label_box = Rect::FromLTWH(-rect.half_width(),
                                 -rect.half_height(),
                                 rect.width, rect.height);
      valign = VerticalAlignment::Bottom;
      break;

    case BoundingBoxLabelPosition::LeftB2T:
      rotation = wgu::deg2rad(-90.0);
      label_box = Rect::FromLTWH(-rect.half_height(),
                                 -rect.half_width(),
                                 rect.height, rect.width);
      valign = VerticalAlignment::Top;
      padding = Vec2d(style.label_padding.y(), style.label_padding.x());
      break;

    case BoundingBoxLabelPosition::LeftT2B:
      rotation = wgu::deg2rad(90.0);
      label_box = Rect::FromLTWH(-rect.half_height(),
                                 -rect.half_width(),
                                 rect.height, rect.width);
      valign = VerticalAlignment::Bottom;
      padding = Vec2d(style.label_padding.y(), style.label_padding.x());
      break;

    case BoundingBoxLabelPosition::RightB2T:
      rotation = wgu::deg2rad(-90.0);
      label_box = Rect::FromLTWH(-rect.half_height(),
                                 -rect.half_width(),
                                 rect.height, rect.width);
      valign = VerticalAlignment::Bottom;
      padding = Vec2d(style.label_padding.y(), style.label_padding.x());
      break;

    case BoundingBoxLabelPosition::RightT2B:
      rotation = wgu::deg2rad(90.0);
      label_box = Rect::FromLTWH(-rect.half_height(),
                                 -rect.half_width(),
                                 rect.height, rect.width);
      valign = VerticalAlignment::Top;
      padding = Vec2d(style.label_padding.y(), style.label_padding.x());
      break;
  }

  // In addition to the placement of the label, we also
  // have to check how to align the label text horizontally
  // within the desired region:
  Vec2d text_anchor {0.0, (valign == VerticalAlignment::Top) ? label_box.top()
                                                             : label_box.bottom()};
  switch (style.text_style.alignment) {
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
  MultilineText mlt(label, style.text_style, context);
  mlt.Align(text_anchor, valign | style.text_style.alignment, padding, {-1, -1});
  if (valign == VerticalAlignment::Top) {
    label_box = Rect::FromLTWH(label_box.left(), label_box.top(),
                               label_box.width, mlt.Height());
  } else if (valign == VerticalAlignment::Bottom) {
    label_box = Rect::FromLTWH(label_box.left(), label_box.bottom() - mlt.Height(),
                               label_box.width, mlt.Height());
  } else {
    throw std::runtime_error("Internal vertical alignment in "
                             "helpers::DrawBoundingBox2d must "
                             "be either Top or Bottom!");
  }

  // Optionally, fill the text box
  if (style.TextFillColor().IsValid()) {
    SPDLOG_CRITICAL("FIXME text fill: {:s} --> {:s}", style.text_fill_color, style.TextFillColor());
    cairo_clip(context);
    ApplyColor(context, style.TextFillColor());
    cairo_rectangle(context, label_box.left(), label_box.top(),
                    label_box.width, label_box.height);

    cairo_fill(context);
    cairo_reset_clip(context);
  }
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
}

} // namespace helpers
} // namespace viren2d
