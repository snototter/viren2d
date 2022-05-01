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


  /**
   * @brief Returns the current visualization state (canvas) as ImageBuffer
   *
   * If "copy" is true, the canvas memory is copied into the ImageBuffer (i.e.
   * you can modify the buffer however you like). Otherwise, the ImageBuffer
   * shares the Painter's memory (and thus your subsequent memory modifications
   * will directly affect the canvas).
   */
  virtual ImageBuffer GetCanvas(bool copy) = 0;


  /** TODO doc
   *  starts at angle1, draws (increasing angle, i.e. clockwise) until angle2
   *  angle of 0.0 is in the direction of the positive X axis.
 angles in degrees*/
  void DrawArc(const Vec2d &center, double radius,
               double angle1, double angle2,
               const LineStyle &line_style,
               const Color &fill = Color(0, 0, 0, 0)) {
    DrawArcImpl(center, radius, angle1, angle2, line_style, fill);
  }


  void DrawCircle(const Vec2d &center, double radius,
                  const LineStyle &line_style,
                  const Color &fill = Color(0, 0, 0, 0)) {
    DrawCircleImpl(center, radius, line_style, fill);
  }


  virtual void DrawLine(const Vec2d &from, const Vec2d &to,
                        const LineStyle &line_style) = 0;


  void DrawRect(const Rect &rect, const LineStyle &line_style,
                const Color &fill = Color(0, 0, 0, 0)) {
    DrawRectImpl(rect, line_style, fill);
  }


  //TODO DrawPoints - how to handle alternating colors???
  //TODO DrawEllipse <-- (optionally rotated) rect!
  //TODO DrawArrow
  //TODO OverlayImage <-- same size vs different, maybe clip to a circle; maybe add a border, etc

protected:
  virtual void DrawArcImpl(const Vec2d &center, double radius,
                           double angle1, double angle2,
                           const LineStyle &line_style,
                           const Color &fill) = 0;

  virtual void DrawCircleImpl(const Vec2d &center, double radius,
                              const LineStyle &line_style,
                              const Color &fill) = 0;

  virtual void DrawRectImpl(const Rect &rect, const LineStyle &line_style,
                            const Color &fill) = 0;
};


std::unique_ptr<Painter> CreateImagePainter();

} // namespace viren2d

#endif // __VIREN2D_DRAWING_H__
