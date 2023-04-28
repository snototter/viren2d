// STL
#include <string>
#include <exception>

// Non-STL external
#include <helpers/drawing_helpers.h>
#include <helpers/logging.h>

namespace viren2d {
namespace helpers {
/// Internal helper which is invoked with a 4-channel uint8 ImageBuffer. Thus,
/// no buffer conversion is needed.
bool DrawImageHelper(
    cairo_t *context, const ImageBuffer &img_u8_c4,
    const Vec2d &position, Anchor anchor,
    double alpha, double scale_x, double scale_y,
    double rotation, double clip_factor,
    LineStyle line_style) {
  const int stride = cairo_format_stride_for_width(
      CAIRO_FORMAT_ARGB32, img_u8_c4.Width());
  if (stride != img_u8_c4.RowStride()) {
    SPDLOG_ERROR(
        "ImageBuffer stride {} does not match optimal cairo stride {}!",
        img_u8_c4.RowStride(), stride);
    return false;
  }

  cairo_save(context);
  cairo_translate(context, position.X(), position.Y());
  cairo_rotate(context, rotation * 3.14159 / 180.0);
  cairo_scale(context, scale_x, scale_y);

  Vec2d pattern_offset{0, 0};
  switch(anchor) {
    case Anchor::TopLeft:
      // Cairo's origin of the image surface is at the top-left corner.
      break;

    case Anchor::Top:
      pattern_offset = Vec2d(-img_u8_c4.Width() / 2.0, 0.0);
      break;

    case Anchor::TopRight:
      pattern_offset = Vec2d(-img_u8_c4.Width(), 0.0);
      break;

    case Anchor::Right:
      pattern_offset = Vec2d(-img_u8_c4.Width(), -img_u8_c4.Height() / 2.0);
      break;

    case Anchor::BottomRight:
      pattern_offset = Vec2d(-img_u8_c4.Width(), -img_u8_c4.Height());
      break;

    case Anchor::Bottom:
      pattern_offset = Vec2d(-img_u8_c4.Width() / 2.0, -img_u8_c4.Height());
      break;

    case Anchor::BottomLeft:
      pattern_offset = Vec2d(0.0, -img_u8_c4.Height());
      break;

    case Anchor::Left:
      pattern_offset = Vec2d(0.0, -img_u8_c4.Height() / 2.0);
      break;

    case Anchor::Center:
      pattern_offset = Vec2d(-img_u8_c4.Width() / 2.0, -img_u8_c4.Height() / 2.0);
      break;
  }

  cairo_path_t *image_contour = nullptr;
  const bool need_contour = line_style.IsValid();
  // Clip ROI as a circle/ellipse
  if (clip_factor > 0.5) {
    // We'll scale the context, so we can draw the ellipse as a
    // unit circle.
    const double clip_scale_x = img_u8_c4.Width() / 2.0;
    const double clip_scale_y = img_u8_c4.Height() / 2.0;
    cairo_save(context);
    cairo_translate(
          context,
          pattern_offset.X() + img_u8_c4.Width() / 2.0,
          pattern_offset.Y() + img_u8_c4.Height() / 2.0);
    cairo_scale(context, clip_scale_x, clip_scale_y);
    cairo_arc(context, 0.0, 0.0, 1.0, 0.0, 2 * M_PI);
    cairo_restore(context);

    if (need_contour) {
      image_contour = cairo_copy_path(context);
    }

    cairo_clip(context);

  } else if (clip_factor > 0.0) {
    // Clip rect with rounded corners - the path helper assumes that
    // the rectangle is centered, thus we have to translate the context:
    cairo_save(context);
    cairo_translate(
          context,
          pattern_offset.X() + img_u8_c4.Width() / 2.0,
          pattern_offset.Y() + img_u8_c4.Height() / 2.0);
    helpers::PathHelperRoundedRect(
          context,
          Rect({0.0, 0.0}, Vec2d(img_u8_c4.Size()),0.0, clip_factor));

    cairo_restore(context);

    if (need_contour) {
      image_contour = cairo_copy_path(context);
    }

    cairo_clip(context);

  } else {
    if (need_contour) {
      cairo_rectangle(
            context, pattern_offset.X(), pattern_offset.Y(),
            img_u8_c4.Width(), img_u8_c4.Height());
      image_contour = cairo_copy_path(context);
    }
  }

  // Paint the image onto the (already clipped) canvas.
  // Removing the const-ness is not a problem, because the cairo image
  // surface is only used to copy the data onto the canvas. There will be
  // no write access.
  cairo_surface_t *imsurf = cairo_image_surface_create_for_data(
        const_cast<ImageBuffer &>(img_u8_c4).MutableData(),
        CAIRO_FORMAT_ARGB32,
        img_u8_c4.Width(),
        img_u8_c4.Height(),
        img_u8_c4.RowStride());
  cairo_set_source_surface(
        context, imsurf, pattern_offset.X(), pattern_offset.Y());
  cairo_paint_with_alpha(context, alpha);
  cairo_surface_destroy(imsurf);


  // Draw the contour if requested:
  if (need_contour && (image_contour != nullptr)) {
    cairo_reset_clip(context);
    // Adjust the line thickness since the context is currently
    // scaled
    line_style.width /= std::max(scale_x, scale_y);
    ApplyLineStyle(context, line_style);
    cairo_new_path(context);
    cairo_append_path(context, image_contour);
    cairo_path_destroy(image_contour);
    cairo_stroke(context);
  }

  cairo_restore(context);
  return true;
}


bool DrawImage(
    cairo_surface_t *surface, cairo_t *context,
    const ImageBuffer &image,
    const Vec2d &position, Anchor anchor,
    double alpha, double scale_x, double scale_y,
    double rotation, double clip_factor,
    LineStyle line_style) {
  if (!CheckCanvas(surface, context)) {
    return false;
  }

  if ((image.BufferType() == ImageBufferType::UInt8)
      && (image.Channels() == 4)) {
    return DrawImageHelper(
          context, image, position, anchor,
          alpha, scale_x, scale_y, rotation, clip_factor,
          line_style);
  } else {
    ImageBuffer img_u8_c4 = image.ToUInt8(4);
    return DrawImageHelper(
          context, img_u8_c4, position, anchor,
          alpha, scale_x, scale_y, rotation, clip_factor,
          line_style);
  }
}
} // namespace helpers
} // namespace viren2d
