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
TextLine::TextLine(const char *line, cairo_t *context,
                   cairo_font_extents_t *font_metrics)
  : text(line),
    reference_point(0.0, 0.0) {
  Init(context, font_metrics);
}


void TextLine::Align(Vec2d anchor_position,
                     TextAnchor anchor) {
  // Default Cairo text `position` is bottom-left,
  // indicating the "reference point".

  // Check these useful resources:
  // https://www.cairographics.org/tutorial/#L1understandingtext
  // https://www.cairographics.org/tutorial/textextents.c
  // https://www.cairographics.org/samples/text_align_center/

  // Adjust horizontal alignment.
  double x = anchor_position.x();
  if (IsFlagSet(anchor, HorizontalAlignment::Center)) {
    x -= (width / 2.0 + bearing_x);
  } else if (IsFlagSet(anchor, HorizontalAlignment::Right)) {
    x -= (width + bearing_x);
  } else {  // Left-aligned
    x -= bearing_x;
  }

  // Adjust vertical alignment.
  double y = anchor_position.y();
  if (IsFlagSet(anchor, VerticalAlignment::Center)) {
    y -= (height / 2.0 + bearing_y);
  } else if (IsFlagSet(anchor, VerticalAlignment::Top)) {
    y -= bearing_y;
  } else {  // Bottom-aligned
    y -= (height + bearing_y);
  }

  reference_point = {x, y};
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
  } else {
    height = std::round(text_extent.height);
    bearing_y = std::round(text_extent.y_bearing);
  }
}


MultilineText::MultilineText(const std::vector<const char *> &text,
                             const TextStyle &text_style, cairo_t *context)
  : top_left(0.0, 0.0), padding(0.0, 0.0), fixed_size(0.0, 0.0),
    width(0.0), height(0.0), style(text_style) {
  cairo_font_extents_t font_extent;
  cairo_font_extents(context, &font_extent);

  for (std::size_t idx = 0; idx < text.size(); ++idx) {
    lines.push_back(TextLine(text[idx], context, &font_extent));
    width = std::max(width, lines[idx].Width());

    height += (lines[idx].Height() * ((idx > 0) ? text_style.line_spacing : 1.0));
  }
}


void MultilineText::Align(Vec2d anchor_point, TextAnchor anchor,
                          Vec2d padding, Vec2d fixed_size) {
  // Store padding & fixed size as we need it for the
  // subsequent PlaceText() call.
  this->fixed_size = fixed_size;
  this->padding = padding;

  // Adjust left corner
  if (IsFlagSet(anchor, HorizontalAlignment::Center)) {
    top_left.SetX(anchor_point.x() - Width() / 2.0);
  } else if (IsFlagSet(anchor, HorizontalAlignment::Right)) {
    top_left.SetX(anchor_point.x() - Width());
  } else {  // Left-aligned
    top_left.SetX(anchor_point.x());
  }

  // Adjust top corner
  if (IsFlagSet(anchor, VerticalAlignment::Center)) {
    top_left.SetY(anchor_point.y() - Height() / 2.0);
  } else if (IsFlagSet(anchor, VerticalAlignment::Top)) {
    top_left.SetY(anchor_point.y());
  } else {  // Bottom-aligned
    top_left.SetY(anchor_point.y() - Height());
  }

  // Compute the horizontal anchor coordinate
  // for each TextLine depending on the
  // user's TextStyle choice:
  double x = 0.0;
  switch(style.alignment) {
    case HorizontalAlignment::Left:
      x = top_left.x() + padding.x();
      break;

    case HorizontalAlignment::Center:
      x = top_left.x() + Width() / 2.0;
      break;

    case HorizontalAlignment::Right:
      x = top_left.x() + Width() - padding.x();
      break;
  }

  // Align each TextLine:
  double y = top_left.y() + padding.y();
  for (std::size_t idx = 0; idx < lines.size(); ++idx) {
    y += (lines[idx].Height() * ((idx == 0) ? 1.0 : style.line_spacing));
    lines[idx].Align(Vec2d(x, y),
                     VerticalAlignment::Bottom | style.alignment);
  }
}


Rect MultilineText::BoundingBox(double corner_radius) const {
  double w = Width();
  double h = Height();
  return Rect(top_left.x() + w / 2.0,
              top_left.y() + h / 2.0,
              w, h, 0.0, corner_radius);
}


void MultilineText::PlaceText(cairo_t *context) const {
#ifdef VIREN2D_DEBUG_TEXT_EXTENT
  cairo_save(context);
  ApplyLineStyle(context, LineStyle(1, Color::Black));
  auto bb = BoundingBox();
  cairo_rectangle(context, bb.left(), bb.top(), bb.width, bb.height);
  cairo_stroke(context);
  cairo_restore(context);
#endif  // VIREN2D_DEBUG_TEXT_EXTENT

  for (const auto &line : lines) {
    line.PlaceText(context);
  }
}


double MultilineText::Width() const {
  return (fixed_size.width() > 0.0)
      ? fixed_size.width()
      : (width + 2 * padding.x());
}


double MultilineText::Height() const {
  return (fixed_size.height() > 0.0)
      ? fixed_size.height()
      : (height + 2 * padding.y());
}


//---------------------------------------------------- Text (plain & boxed)
void DrawText(cairo_surface_t *surface, cairo_t *context,
              const std::vector<const char*> &text,
              Vec2d anchor_position, TextAnchor anchor,
              const TextStyle &text_style, const Vec2d &padding,
              double rotation, const LineStyle &box_line_style,
              const Color &box_fill_color, double box_corner_radius,
              const Vec2d &fixed_box_size) {
  CheckCanvas(surface, context);

  if (text.empty()) {
    SPDLOG_WARN("Cannot draw an empty text!");
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

  // Shift the context to the desired anchor point
  // and rotate.
  cairo_translate(context, anchor_position.x(), anchor_position.y());
  cairo_rotate(context, wgu::deg2rad(rotation));
  anchor_position = {0, 0};

  // Query the rendered text extents and use them to adjust
  // the position according to the desired text anchor.
  ApplyTextStyle(context, text_style, false);
  MultilineText mlt(text, text_style, context);
  mlt.Align(anchor_position, anchor, padding, fixed_box_size);

//#define VIREN2D_DEBUG_TEXT_EXTENT // TODO(snototter) document this debug flag
#ifdef VIREN2D_DEBUG_TEXT_EXTENT
  // Draw a box showing the text extent
  DrawRect(surface, context, mlt.BoundingBox(),
           LineStyle(1, text_style.font_color), Color::Invalid);

  // Draw a box or circle at the desired location, showing the
  // size of the padded region
  ApplyLineStyle(context, LineStyle(1, Color::Black));
  if (padding.LengthSquared() > 0) {
    cairo_rectangle(context,
                    anchor_position.x() - padding.x() + 0.5,
                    anchor_position.y() - padding.y() + 0.5,
                    2 * padding.x(), 2 * padding.y());
  } else {
    // If the text is not padded, draw a small circle instead:
    cairo_arc(context,
              anchor_position.x() + 0.5,
              anchor_position.y() + 0.5,
              4, 0, 2 * M_PI);
  }
  cairo_stroke(context);
#endif  // VIREN2D_DEBUG_TEXT_EXTENT

  // Reuse DrawRect() if we need to draw a text box:
  if (box_fill_color.IsValid() || box_line_style.IsValid()) {
    DrawRect(surface, context,
             mlt.BoundingBox(box_corner_radius),
             box_line_style, box_fill_color);
  }

  // Now that the optional text box has been drawn, we
  // have to make sure that we plot the text in the
  // correct color.
  ApplyColor(context, text_style.color);
  mlt.PlaceText(context);

  // Pop the original context.
  cairo_restore(context);
}
} // namespace helpers
} // namespace viren2d
