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
TextLine::TextLine()
  : text(nullptr), reference_point(0.0, 0.0),
    width(0.0), height(0.0),
    bearing_x(0.0), bearing_y(0.0)
{}


TextLine::TextLine(const char *line, cairo_t *context,
                   bool use_font_height)
  : text(line),
    reference_point(0.0, 0.0) {
  if (use_font_height) {
    cairo_font_extents_t font_metrics;
    cairo_font_extents(context, &font_metrics);
    Init(context, &font_metrics);
  } else {
    Init(context, nullptr);
  }
}


TextLine::TextLine(const char *line,
                   cairo_t *context,
                   cairo_font_extents_t font_metrics)
  : text(line),
    reference_point(0.0, 0.0) {
  Init(context, &font_metrics);
}


void TextLine::Align(Vec2d desired_position,
                     TextAnchor anchor, Vec2d padding) {
  //FIXME double-check with adjusted textextent/metrics

  // Default Cairo text `position` is bottom-left,
  // indicating the "reference point".
  // Check these useful resources:
  // https://www.cairographics.org/tutorial/#L1understandingtext
  // https://www.cairographics.org/tutorial/textextents.c
  // https://www.cairographics.org/samples/text_align_center/

  // Adjust horizontal alignment.
  double x = desired_position.x();
  if (IsFlagSet(anchor, HorizontalAlignment::Center)) {
    x -= (width / 2.0 + bearing_x);
  } else if (IsFlagSet(anchor, HorizontalAlignment::Right)) {
    x -= (width + padding.x() + bearing_x);
  } else {  // Left-aligned
    x += padding.x() - bearing_x;
  }

  // Adjust vertical alignment.
  double y = desired_position.y();
  if (IsFlagSet(anchor, VerticalAlignment::Center)) {
    y -= (height / 2.0 + bearing_y);
  } else if (IsFlagSet(anchor, VerticalAlignment::Top)) {
    y += (padding.y() - bearing_y);
  } else {  // Bottom-aligned
    y -= (height + bearing_y + padding.y());
  }

  reference_point = {x, y};
}

Rect TextLine::BoundingBox(Vec2d padding, double corner_radius) const {
  const auto center = reference_point + Vec2d(bearing_x, bearing_y)
      + Vec2d(width / 2.0, height / 2.0);
  return Rect(center, Vec2d(width, height) + 2.0 * padding,
              0.0, corner_radius);
}


void TextLine::PlaceText(cairo_t *context) const {
  // Shift to the pixel center, and move to the origin of the
  // first glyph. Then, let Cairo render the text:
  auto position = reference_point + 0.5;
  cairo_move_to(context, position.x(), position.y());
  cairo_show_text(context, text);
}


void TextLine::Init(cairo_t *context,
                    cairo_font_extents_t *font_metrics) {
  cairo_text_extents_t text_extent;
  cairo_text_extents(context, text, &text_extent);

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


//Vec2d TextExtent::TopLeft(Vec2d reference_point) const {
//  return reference_point + Vec2d(bearing_x, bearing_y);
//}


//Vec2d TextExtent::Center(Vec2d reference_point) const {
//  return TopLeft(reference_point) + Vec2d(width / 2.0, height / 2.0);
//}


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
  // and rotate.
  cairo_translate(context, position.x(), position.y());
  cairo_rotate(context, wgu::deg2rad(rotation));
  position = {0, 0};

  // Now that the font face is set up, we can query
  // the rendered text extents and use them to adjust
  // the position according to the desired text anchor.
  TextLine extent(text.c_str(), context, text_style.use_font_height);
  extent.Align(position, text_anchor, padding);

//#define VIREN2D_DEBUG_TEXT_EXTENT // TODO(snototter) document this debug flag
#ifdef VIREN2D_DEBUG_TEXT_EXTENT
  // Draw a box showing the text extent
  DrawRect(surface, context, extent.BoundingBox(),
           LineStyle(1, text_style.font_color), Color::Invalid);

  // Draw a box or circle at the desired location, showing the
  // size of the padded region
  ApplyLineStyle(context, LineStyle(1, Color::Black));
  if (padding.LengthSquared() > 0) {
    cairo_rectangle(context,
                    position.x() - padding.x() + 0.5,
                    position.y() - padding.y() + 0.5,
                    2 * padding.x(), 2 * padding.y());
  } else {
    // If the text is not padded, draw a small circle instead:
    cairo_arc(context, position.x() + 0.5, position.y() + 0.5,
              4, 0, 2 * M_PI);
  }
  cairo_stroke(context);
#endif  // VIREN2D_DEBUG_TEXT_EXTENT

  // Reuse DrawRect if we need to draw a text box:
  if (box_fill_color.IsValid() || box_line_style.IsValid()) {
    DrawRect(surface, context,
             extent.BoundingBox(padding, box_corner_radius),
             box_line_style, box_fill_color);
  }

  // Finally, place the aligned text onto the canvas
  ApplyColor(context, text_style.font_color);
  extent.PlaceText(context);

  // Pop the original context
  cairo_restore(context);
}
} // namespace helpers
} // namespace viren2d
