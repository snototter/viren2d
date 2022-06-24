#ifndef __VIREN2D_DRAWING_H__
#define __VIREN2D_DRAWING_H__

#include <functional>
#include <memory>
#include <string>
//#include <tuple>
#include <utility>
#include <vector>


#include <viren2d/primitives.h>
#include <viren2d/colors.h>
#include <viren2d/styles.h>


namespace viren2d {

/**
 * @brief The Painter supports drawing on a canvas.
 *
 *TODO replace all docstrings by the python interface doc
 */
class Painter {
public:
  virtual ~Painter() {}

  Painter() = default;
  Painter(const Painter &other) = default;
  Painter& operator=(const Painter &other) = default;
  Painter(Painter &&) = default;
  Painter& operator=(Painter &&) = default;


  /** @brief Returns true if the painter's canvas is valid.
   *
   * You have to set up the painter's canvas before drawing
   * or retrieving, @see SetCanvas
   */
  virtual bool IsValid() const = 0;


  /**
   * @brief Initializes the canvas with the given color.
   *
   * This or any overloaded SetCanvas() must be called before
   * any other DrawXXX calls can be performed.
   */
  virtual void SetCanvas(int width, int height, const Color &color) = 0;


  /**
   * @brief Initializes the canvas with the given image file.
   *
   * Uses the stb/stb_image library for loading, which supports
   * the most common formats.
   * This or any overloaded SetCanvas() must be called before
   * any other DrawXXX calls can be performed.
   */
  virtual void SetCanvas(const std::string &image_filename) = 0;

  //TODO doc - memory will be copied; can be
  // grayscale, RGB or RGBA
  // * rgb --> assumes alpha 255
  // * grayscale --> gray,gray,gray, alpha
  virtual void SetCanvas(const ImageBuffer &image_buffer) = 0;

  /** @brief Returns the size of the canvas. */
  virtual Vec2i GetCanvasSize() const = 0;


  /**
   * @brief Returns the current visualization state (canvas) as ImageBuffer
   *
   * If "copy" is true, the canvas memory is copied into the ImageBuffer (i.e.
   * you can modify the buffer however you like). Otherwise, the ImageBuffer
   * shares the Painter's memory (and thus your subsequent memory modifications
   * will directly affect the canvas).
   */
  virtual ImageBuffer GetCanvas(bool copy) const = 0;


  /**
   * @brief Draws a circular arc.
   *
   * @param center:  Center of the arc
   * @param radius:  Radius of the arc in pixels.
   * @param angle1 & angle2:
   *            The arc will be drawn from angle1 to angle2 in clockwise
   *            direction. Both angles are specified in degrees, where 0 degrees
   *            points in the direction of increasing X coordinates.
   * @param line_style:
   *            How to draw the arc's outline. To skip drawing the contour,
   *            pass LineStyle::Invalid (then you must specify a valid 'fill_color').
   * @param include_center:
   *            If True (default), the center of the circle will be included
   *            when drawing the outline and filling the arc.
   * @param fill_color: Provide a valid color to fill
   *            the arc.
   */
  void DrawArc(
      const Vec2d &center, double radius,
      double angle1, double angle2,
      const LineStyle &line_style = LineStyle(),
      bool include_center = true,
      const Color &fill_color = Color::Invalid) {
    DrawArcImpl(
          center, radius, angle1, angle2, line_style,
          include_center, fill_color);
  }


  /**
   * @brief Draws an arrow.
   *
   * @param from: Start point of the arrow shaft.
   * @param to:   End point of the arrow shaft.
   * @param arrow_style: How to draw the arrow (specifies
   *              both line and head/tip style).
   */
  void DrawArrow(
      const Vec2d &from, const Vec2d &to,
      const ArrowStyle &arrow_style = ArrowStyle()) {
    DrawArrowImpl(from, to, arrow_style);
  }


  /**
   * @brief Draws a 2D bounding box.
   */
  void DrawBoundingBox2D(
      const Rect &box, const std::vector<std::string> &label,
      const BoundingBox2DStyle &style = BoundingBox2DStyle()) {
    std::vector<const char*> lines; //FIXME const char vs string!
    for (const auto &line : label) {
      lines.push_back(line.c_str());
    }
    DrawBoundingBox2DImpl(box, lines, style);
  }


  /**
   * @brief Draws a circle.
   *
   * @param center: Center point of the circle.
   * @param radius: Radius in pixels.
   * @param line_style: How to draw the outline. To skip drawing the contour,
   *                pass LineStyle::Invalid (then you must specify a
   *                valid 'fill_color').
   * @param fill_color: Provide a valid color to fill
   *                the circle.
   */
  void DrawCircle(
      const Vec2d &center, double radius,
      const LineStyle &line_style = LineStyle(),
      const Color &fill_color = Color::Invalid) {
    DrawCircleImpl(center, radius, line_style, fill_color);
  }


  /**
   * @brief Draws an ellipse.
   *
   * @param ellipse:    The ellipse to be drawn.
   * @param line_style: How to draw the outline. To skip drawing
   *                    the contour, pass LineStyle::Invalid (then
   *                    you must specify a valid 'fill_color').
   * @param fill_color: Provide a valid color to fill
   *                    the ellipse.
   */
  void DrawEllipse(
      const Ellipse &ellipse,
      const LineStyle &line_style = LineStyle(),
      const Color &fill_color = Color::Invalid) {
    DrawEllipseImpl(ellipse, line_style, fill_color);
  }


  /**
   * @brief Draws a grid between top_left and bottom_right.
   *
   * @param top_left & bottom_right:
   *            If these are not the same, they define
   *            the canvas region which will be spanned
   *            by the grid.
   * @param spacing_x: Horizontal cell size in pixels.
   * @param spacing_y: Vertical cell size in pixels.
   * @param line_style: How to draw the grid lines.
   */
  void DrawGrid(
      const Vec2d &top_left, const Vec2d &bottom_right,
      double spacing_x, double spacing_y,
      const LineStyle &line_style = LineStyle()) {
    DrawGridImpl(
          top_left, bottom_right,
          spacing_x, spacing_y,
          line_style);
  }


  /**
   * @brief Draws a line.
   */
  void DrawLine(
      const Vec2d &from, const Vec2d &to,
      const LineStyle &line_style = LineStyle()) {
    DrawLineImpl(from, to, line_style);
  }


  void DrawMarker(
      const Vec2d &position,
      const MarkerStyle &style = MarkerStyle()) {
    DrawMarkerImpl(position, style);
  }


  void DrawMarkers(
      const std::vector<std::pair<Vec2d, Color>> &markers,
      const MarkerStyle &style = MarkerStyle()) {
    DrawMarkersImpl(markers, style);
  }


  void DrawPolygon(
      const std::vector<Vec2d> &points,
      const LineStyle &line_style = LineStyle(),
      const Color &fill_color = Color::Invalid) {
    DrawPolygonImpl(points, line_style, fill_color);
  }


  /**
   * @brief Draws a rectangle in various different shapes.
   *
   * @param rect:   Defines the rectangle (incl. optional
   *                rotation & corner radius)
   * @param line_style: How to draw the outline. To skip drawing
   *                the contour, pass LineStyle::Invalid (then
   *                you must specify a valid 'fill_color').
   * @param fill_color: Provide a valid color to fill
   *                the rectangle.
   */
  void DrawRect(
      const Rect &rect,
      const LineStyle &line_style = LineStyle(),
      const Color &fill_color = Color::Invalid) {
    DrawRectImpl(rect, line_style, fill_color);
  }


  //TODO doc, test, bind
  void DrawText(
      const std::vector<std::string> &text,
      const Vec2d &anchor_position,
      TextAnchor anchor = TextAnchor::BottomLeft,
      const TextStyle &text_style = TextStyle(),
      const Vec2d &padding = {0.0, 0.0},
      double rotation = 0.0) {
    std::vector<const char*> lines;//FIXME revert to string
    for (const auto &line : text) {
      lines.push_back(line.c_str());
    }
    DrawTextImpl(lines, anchor_position, anchor,
                 text_style, padding, rotation);
  }


  //TODO doc, test, bind
  void DrawTextBox(
      const std::vector<std::string> &text,
      const Vec2d &anchor_position,
      TextAnchor anchor = TextAnchor::BottomLeft,
      const TextStyle &text_style = TextStyle(),
      const Vec2d &padding = {6.0, 6.0},
      double rotation = 0.0,
      const LineStyle &box_line_style = LineStyle::Invalid,
      const Color &box_fill_color = Color::White.WithAlpha(0.6),
      double box_corner_radius = 0.2,
      const Vec2d &fixed_box_size = {-1.0, -1.0}) {
    std::vector<const char*> lines;//FIXME revert to string
    for (const auto &line : text) {
      lines.push_back(line.c_str());
    }
    DrawTextBoxImpl(
          lines, anchor_position, anchor, text_style,
          padding, rotation, box_line_style,
          box_fill_color, box_corner_radius, fixed_box_size);
  }


  //TODO doc, test, bind
  void DrawTrajectory(
      const std::vector<Vec2d> &points,
      const LineStyle &style = LineStyle(),
      const Color &color_fade_out = Color::White.WithAlpha(0.4),
      bool oldest_position_first = false,
      int smoothing_window = 0,
      const std::function<double(double)> &mix_factor = ColorFadeOutQuadratic) {
    DrawTrajectoryImpl(
          points, style, color_fade_out, oldest_position_first,
          smoothing_window, mix_factor);
  }


  void DrawTrajectories(
      const std::vector<std::pair<std::vector<Vec2d>, Color>> &trajectories,
      const LineStyle &style = LineStyle(),
      const Color &color_fade_out = Color::White.WithAlpha(0.4),
      bool oldest_position_first = false,
      int smoothing_window = 0,
      const std::function<double(double)> &mix_factor = ColorFadeOutQuadratic) {
    DrawTrajectoriesImpl(
          trajectories, style, color_fade_out, oldest_position_first,
          smoothing_window, mix_factor);
  }

  //TODO DrawPoints - how to handle alternating colors???
  //TODO OverlayImage <-- same size vs different, maybe clip to a circle; maybe add a border, etc
  //            Scaling via cairo context!

protected:
  /** Internal helper to enable default values in public interface. */
  virtual void DrawArcImpl(
      const Vec2d &center, double radius,
      double angle1, double angle2,
      const LineStyle &line_style,
      bool include_center, const Color &fill_color) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawArrowImpl(
      const Vec2d &from, const Vec2d &to,
      const ArrowStyle &arrow_style) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawBoundingBox2DImpl(
      const Rect &box,
      const std::vector<const char*> &label,
      const BoundingBox2DStyle &style) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawCircleImpl(
      const Vec2d &center, double radius,
      const LineStyle &line_style,
      const Color &fill_color) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawEllipseImpl(
      const Ellipse &ellipse, const LineStyle &line_style,
      const Color &fill_color) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawGridImpl(
      const Vec2d &top_left, const Vec2d &bottom_right,
      double spacing_x, double spacing_y,
      const LineStyle &line_style) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawLineImpl(
      const Vec2d &from, const Vec2d &to,
      const LineStyle &line_style) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawMarkerImpl(
      const Vec2d &pos, const MarkerStyle &style) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawMarkersImpl(
      const std::vector<std::pair<Vec2d, Color>> &markers,
      const MarkerStyle &style) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawPolygonImpl(
      const std::vector<Vec2d> &points,
      const LineStyle &line_style,
      const Color &fill_color) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawRectImpl(
      const Rect &rect, const LineStyle &line_style,
      const Color &fill_color) = 0;


  /** Internal helper to allow default values in public interface. */
  virtual void DrawTextImpl(
      const std::vector<const char*> &text,
      const Vec2d &position, TextAnchor text_anchor,
      const TextStyle &text_style,
      const Vec2d &padding, double rotation) = 0;


  /** Internal helper to allow default values in public interface. */
  virtual void DrawTextBoxImpl(
      const std::vector<const char*> &text,
      const Vec2d &position, TextAnchor text_anchor,
      const TextStyle &text_style, const Vec2d &padding,
      double rotation, const LineStyle &box_line_style,
      const Color &box_fill_color, double box_corner_radius,
      const Vec2d &fixed_box_size) = 0;


  /** Internal helper to allow default values in public interface. */
  virtual void DrawTrajectoryImpl(
      const std::vector<Vec2d> &points, const LineStyle &style,
      const Color &color_fade_out, bool oldest_position_first,
      int smoothing_window,
      const std::function<double(double)> &mix_factor) = 0;


  /** Internal helper to allow default values in public interface. */
  virtual void DrawTrajectoriesImpl(
      const std::vector<std::pair<std::vector<Vec2d>, Color>> &trajectories,
      const LineStyle &style, const Color &color_fade_out,
      bool oldest_position_first, int smoothing_window,
      const std::function<double(double)> &mix_factor) = 0;
};


/** @brief Creates a Painter object for drawing. */
std::unique_ptr<Painter> CreatePainter();

} // namespace viren2d

#endif // __VIREN2D_DRAWING_H__
