// STL
#include <cmath>
#include <string>

// non-STL, external
#include <werkzeugkiste/geometry/utils.h>
namespace wgu = werkzeugkiste::geometry;

// Custom
#include <helpers/drawing_helpers.h>
#include <helpers/enum.h>



namespace viren2d {
namespace helpers {
//---------------------------------------------------- Text metrics
TextExtent::TextExtent()
  : width(0.0), height(0.0),
    bearing_x(0.0), bearing_y(0.0)
{}


TextExtent::TextExtent(const std::string &line, cairo_t *context,
                       bool use_font_height) {
  if (use_font_height) {
    cairo_font_extents_t font_metrics;
    cairo_font_extents(context, &font_metrics);
    InitFromText(context, &font_metrics, line);
  } else {
    InitFromText(context, nullptr, line);
  }
}


TextExtent::TextExtent(const std::string &line,
                       cairo_t *context,
                       cairo_font_extents_t font_metrics) {
  InitFromText(context, &font_metrics, line);
}


Vec2d TextExtent::Size() const {
  return Vec2d(width, height);
}


Vec2d TextExtent::ReferencePoint(Vec2d position,
                                 TextAnchor anchor,
                                 Vec2d padding) const {
  //FIXME double-check with adjusted textextent/metrics

  // Default Cairo text `position` is bottom-left,
  // indicating the "reference point".
  // Check these useful resources:
  // https://www.cairographics.org/tutorial/#L1understandingtext
  // https://www.cairographics.org/tutorial/textextents.c
  // https://www.cairographics.org/samples/text_align_center/

  // Adjust horizontal alignment.
  double x = position.x();
  if (IsFlagSet(anchor, HorizontalAlignment::Center)) {
    x -= (width / 2.0 + bearing_x);
  } else if (IsFlagSet(anchor, HorizontalAlignment::Right)) {
    x -= (width + padding.x() + bearing_x);
  } else {  // Left-aligned
    x += padding.x() - bearing_x;
  }
  position.SetX(x);

  // Adjust vertical alignment.
  double y = position.y();
  if (IsFlagSet(anchor, VerticalAlignment::Center)) {
    y -= (height / 2.0 + bearing_y);
  } else if (IsFlagSet(anchor, VerticalAlignment::Top)) {
    y += (padding.y() - bearing_y);
  } else {  // Bottom-aligned
    y -= (height + bearing_y + padding.y());
  }
  position.SetY(y);

  return position;
}


Rect TextExtent::BoundingBox(Vec2d reference_point,
                             Vec2d padding) const {
  return Rect(Center(reference_point),
              Size() + 2.0 * padding);
}

void TextExtent::InitFromText(cairo_t *context,
                              cairo_font_extents_t *font_metrics,
                              const std::string &line) {
  cairo_text_extents_t text_extent;
  cairo_text_extents(context, line.c_str(), &text_extent);

  width = std::round(text_extent.width);
  bearing_x = std::round(text_extent.x_bearing);

  if (font_metrics) {
    height = std::round(font_metrics->ascent + font_metrics->descent);
    bearing_y = -std::round(font_metrics->ascent);
//    SPDLOG_DEBUG("TODO text: \"{}\", Cextent.height {} vs Cfont.height {} vs asc+desc {}",
//                 line, std::round(text_extent.height), std::round(font_metrics->height),
//                 std::round(font_metrics->ascent + font_metrics->descent));
  } else {
    height = std::round(text_extent.height);
    bearing_y = std::round(text_extent.y_bearing);
  }
}


Vec2d TextExtent::TopLeft(Vec2d reference_point) const {
  return reference_point + Vec2d(bearing_x, bearing_y);
}


Vec2d TextExtent::Center(Vec2d reference_point) const {
  return TopLeft(reference_point) + Vec2d(width / 2.0, height / 2.0);
}


//---------------------------------------------------- Text (plain & boxed)
void DrawText(cairo_surface_t *surface, cairo_t *context,
              const std::string &text, Vec2d position, TextAnchor text_anchor,
              const TextStyle &text_style, const Vec2d &padding,
              double rotation, const LineStyle &box_line_style,
              const Color &box_fill_color, double box_corner_radius) {
  CheckCanvas(surface, context);

  if (text.empty()) {
    return;
  }

  if (!text_style.IsValid()) {
    std::ostringstream s;
    s << "Cannot draw text with invalid style: "
      << text_style;
    throw std::invalid_argument(s.str());
  }

  // Push the current context. Now, we just apply styles
  // specific to *this* DrawText call.
  cairo_save(context);

  ApplyTextStyle(context, text_style);

  // Shift the context to the desired anchor point
  // and rotate
  cairo_translate(context, position.x(), position.y());
  cairo_rotate(context, wgu::deg2rad(rotation));
  position = {0, 0};

//#define VIREN2D_DEBUG_TEXT_EXTENT  // TODO document this debug flag
#ifdef VIREN2D_DEBUG_TEXT_EXTENT
  const auto desired_position = position;
#endif  // VIREN2D_DEBUG_TEXT_EXTENT

  // Now that the font face is set up, we can query
  // the rendered text extents and use them to adjust
  // the position according to the desired text anchor.
  TextExtent extent(text, context, text_style.use_font_height);
  position = extent.ReferencePoint(position, text_anchor, padding);

#ifdef VIREN2D_DEBUG_TEXT_EXTENT
  // Draw a box showing the text extent
  DrawRect(surface, context, extent.BoundingBox(position),
           LineStyle(1, text_style.font_color),
           Color::Invalid);

  // Draw a box or circle at the desired location, showing the
  // size of the padded region
  ApplyLineStyle(context, LineStyle(1, Color::Black));
  if (padding.LengthSquared() > 0) {
    cairo_rectangle(context, desired_position.x() - padding.x(),
                    desired_position.y() - padding.y(),
                    2 * padding.x(), 2 * padding.y());
  } else {
    // If we don't have padding, draw a small circle:
    cairo_arc(context, desired_position.x(), desired_position.y(),
              4, 0, 2 * M_PI);
  }
  cairo_stroke(context);
#endif  // VIREN2D_DEBUG_TEXT_EXTENT

  // Reuse DrawRect if we need to draw a text box:
  if (box_fill_color.IsValid() || box_line_style.IsValid()) {
    auto rect = extent.BoundingBox(position, padding);
    rect.radius = box_corner_radius;

    DrawRect(surface, context, rect,
             box_line_style, box_fill_color);
  }

  // We have to always apply font color in this context (as
  // Cairo uses the default source rgba values to render the
  // font - these could have been overwritten by any other
  // `DrawXXX()` call since setting the default font style).
  ApplyColor(context, text_style.font_color);

  // Shift to the pixel center, and move to the origin of the
  // first glyph. Then, let Cairo render the text:
  position += 0.5;
  cairo_move_to(context, position.x(), position.y());
  cairo_show_text(context, text.c_str());

  // Pop the original context
  cairo_restore(context);
}
} // namespace helpers
} // namespace viren2d
