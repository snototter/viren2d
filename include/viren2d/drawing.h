#ifndef __VIREN2D_DRAWING_H__
#define __VIREN2D_DRAWING_H__

#include <memory>
#include <vector>
#include <string>

#include <viren2d/primitives.h>
#include <viren2d/colors.h>
#include <viren2d/styles.h>


namespace viren2d {

/**
 * @brief The Painter supports drawing on a canvas.
 *
 * Workflow:
 * 1. Create a Painter
 * 2. SetCanvas()
 * 3. Draw onto the canvas via DrawXXX
 * 4. When all objects have been drawn, retrieve
 *    the visualization via GetCanvas()
 * 5. For the next visualization, start at step 2 to
 *    reuse the allocated resources.
 */
class Painter {
public:
  virtual ~Painter() {}

  // Explicitly default the copy/move constructors
  // and assignment operators:
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

  //TODO doc - memory will be copied; image must be RGBA!
  // TODO support RGB and grayscale images, too:
  // * rgb, set alpha 255
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
   *            How to draw the arc's outline. If line_width is 0, the outline
   *            will not be drawn (then you must define a 'fill_color').
   * @param include_center:
   *            If True (default), the center of the circle will be included
   *            when drawing the outline and filling the arc.
   */
  void DrawArc(const Vec2d &center, double radius,
               double angle1, double angle2,
               const LineStyle &line_style,
               bool include_center = true,
               const Color &fill_color = Color(0, 0, 0, 0)) {
    DrawArcImpl(center, radius, angle1, angle2, line_style,
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
  virtual void DrawArrow(const Vec2d &from, const Vec2d &to,
                         const ArrowStyle &arrow_style) = 0;


  /**
   * @brief Draws a circle.
   *
   * @param center: Center point of the circle.
   * @param radius: Radius in pixels.
   * @param line_style: How to draw the outline. If its
   *                line_width == 0, the outline will not
   *                be drawn.
   * @param fill_color: Provide a valid color to fill
   *                the circle.
   */
  void DrawCircle(const Vec2d &center, double radius,
                  const LineStyle &line_style,
                  const Color &fill_color = Color(0, 0, 0, 0)) {
    DrawCircleImpl(center, radius, line_style, fill_color);
  }


  /**
   * @brief Draws an ellipse.
   *
   * @param ellipse:    The ellipse to be drawn.
   * @param line_style: How to draw the outline. If its
   *                line_width == 0, the outline will not
   *                be drawn.
   * @param fill_color: Provide a valid color to fill
   *                the circle.
   */
  void DrawEllipse(const Ellipse &ellipse, const LineStyle &line_style,
                   const Color &fill_color = Color(0, 0, 0, 0)) {
    DrawEllipseImpl(ellipse, line_style, fill_color);
  }


  /**
   * @brief Draws a grid between top_left and bottom_right.
   *
   * @param top_left & bottom_right:
   *                     If these are not the same, they define
   *                     the canvas region which will be spanned
   *                     by the grid.
   * @param spacing_x:   Horizontal cell size in pixels.
   * @param spacing_y:   Vertical cell size in pixels.
   * @param line_style:  How to draw the grid lines.
   */
  virtual void DrawGrid(const Vec2d &top_left, const Vec2d &bottom_right,
                        double spacing_x, double spacing_y,
                        const LineStyle &line_style) = 0;


  /**
   * @brief Draws a line.
   *
   * @param from:  Start point of the line.
   * @param to:    End point of the line.
   * @param line_style: How to draw the line.
   */
  virtual void DrawLine(const Vec2d &from, const Vec2d &to,
                        const LineStyle &line_style) = 0;


  /**
   * @brief Draws a rectangle in various different shapes.
   *
   * @param rect:   Defines the rectangle (incl. optional
   *                rotation & corner radius)
   * @param line_style: How to draw the outline. If its
   *                line_width == 0, the outline will not
   *                be drawn.
   * @param fill_color: Provide a valid color to fill
   *                the circle.
   */
  void DrawRect(const Rect &rect, const LineStyle &line_style,
                const Color &fill_color = Color(0, 0, 0, 0)) {
    DrawRectImpl(rect, line_style, fill_color);
  }


  //TODO
  void DrawText(const std::string &text, const Vec2d &position,
                TextAnchor text_anchor = TextAnchorFromString("bottom-left"),
                const TextStyle &text_style = GetDefaultTextStyle()) {
    DrawTextImpl(text, position, text_anchor, text_style);
  }


  //TODO DrawPoints - how to handle alternating colors???
  //TODO OverlayImage <-- same size vs different, maybe clip to a circle; maybe add a border, etc
  //            Scaling via cairo context!

protected:
  /** Internal helper to enable default values in public interface. */
  virtual void DrawArcImpl(const Vec2d &center, double radius,
                           double angle1, double angle2,
                           const LineStyle &line_style,
                           bool include_center,
                           const Color &fill_color) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawCircleImpl(const Vec2d &center, double radius,
                              const LineStyle &line_style,
                              const Color &fill_color) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawEllipseImpl(const Ellipse &ellipse, const LineStyle &line_style,
                               const Color &fill_color) = 0;


  /** Internal helper to enable default values in public interface. */
  virtual void DrawRectImpl(const Rect &rect, const LineStyle &line_style,
                            const Color &fill_color) = 0;


  /** Internal helper to allow default values in public interface. */
  virtual void DrawTextImpl(const std::string &text, const Vec2d &position,
                            TextAnchor text_anchor,
                            const TextStyle &text_style) = 0;
};


/** @brief Creates a Painter object for drawing. */
std::unique_ptr<Painter> CreatePainter();

} // namespace viren2d

#endif // __VIREN2D_DRAWING_H__
