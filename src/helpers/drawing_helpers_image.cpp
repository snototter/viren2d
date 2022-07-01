// STL
#include <string>
#include <exception>

// Non-STL external
#include <helpers/drawing_helpers.h>

namespace viren2d {
namespace helpers {
void DrawImage(
    cairo_surface_t *surface, cairo_t *context,
    const ImageBuffer &image,
    const Vec2d &anchor_position, Anchor anchor,
    double alpha, double scale_x, double scale_y,
    double rotation, double clip_factor) {
  CheckCanvas(surface, context);

  //TODO ToUInt8(num_channels) would be useful
  // floats will be scaled, others are taken as is
  // or only accept uint8 here?

  ImageBuffer img4 = image.ToUInt8(4); //image.ToChannels(4);

  cairo_save(context);
  cairo_translate(context, anchor_position.x(), anchor_position.y());
  cairo_rotate(context, rotation * 3.14159 / 180.0);
  cairo_scale(context, scale_x, scale_y);

  Vec2d pattern_offset{0, 0};
  switch(anchor) {
    case Anchor::TopLeft:
      // Cairo's origin of the image surface is at the top-left corner.
      break;

    case Anchor::Top:
      pattern_offset = Vec2d(-img4.Width() / 2.0, 0.0);
      break;

    case Anchor::TopRight:
      pattern_offset = Vec2d(-img4.Width(), 0.0);
      break;

    case Anchor::Right:
      pattern_offset = Vec2d(-img4.Width(), -img4.Height() / 2.0);
      break;

    case Anchor::BottomRight:
      pattern_offset = Vec2d(-img4.Width(), -img4.Height());
      break;

    case Anchor::Bottom:
      pattern_offset = Vec2d(-img4.Width() / 2.0, -img4.Height());
      break;

    case Anchor::BottomLeft:
      pattern_offset = Vec2d(0.0, -img4.Height());
      break;

    case Anchor::Left:
      pattern_offset = Vec2d(0.0, -img4.Height() / 2.0);
      break;

    case Anchor::Center:
      pattern_offset = Vec2d(-img4.Width() / 2.0, -img4.Height() / 2.0);
      break;
  }

  // Clip ROI as a circle/ellipse
  if (clip_factor > 0.5) {
    // We'll scale the context, so we can draw the ellipse as a
    // unit circle.
    const double clip_scale_x = img4.Width() / 2.0;
    const double clip_scale_y = img4.Height() / 2.0;
    cairo_save(context);
    cairo_translate(
          context,
          pattern_offset.x() + img4.Width() / 2.0,
          pattern_offset.y() + img4.Height() / 2.0);
    cairo_scale(context, clip_scale_x, clip_scale_y);
    cairo_arc(context, 0.0, 0.0, 1.0, 0.0, 2 * M_PI);
    cairo_restore(context);
    cairo_clip(context);
  } else if (clip_factor > 0.0) {
    // Clip rect with rounded corners
    Vec2d center = pattern_offset + (Vec2d(img4.Size()) / 2.0);
    helpers::PathHelperRoundedRect(
          context, Rect(center, Vec2d(img4.Size()), 0.0, 0.5));
    cairo_clip(context);
  }

  cairo_surface_t *imsurf = cairo_image_surface_create_for_data(
        img4.MutableData(), CAIRO_FORMAT_ARGB32,
        img4.Width(), img4.Height(), img4.RowStride());
  cairo_set_source_surface(
        context, imsurf, pattern_offset.x(), pattern_offset.y());
  cairo_paint_with_alpha(context, alpha);
  cairo_restore(context);
  cairo_surface_destroy(imsurf);
}

} // namespace helpers
} // namespace viren2d
