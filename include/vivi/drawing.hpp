#ifndef __VIVI_DRAWING_H__
#define __VIVI_DRAWING_H__

#include <memory>
#include <vector>
#include <string>

#include <vivi/primitives.hpp>
#include <vivi/colors.hpp>

namespace vivi
{

//-------------------------------------------------  LineStyle

/** @brief Style definitions for lines & contours. */
struct LineStyle
{
  /** @brief How to render the endpoints of the line (or dash strokes). */
  enum class Cap
  {
    Butt = 0,
    Round,
    Square
  };

  /** @brief How to render the junction of two lines/segments. */
  enum class Join
  {
    Miter = 0,
    Round,
    Bevel
  };


  double line_width;
  Color color;
  std::vector<double> dash_pattern;
  Cap line_cap;
  Join line_join;

  LineStyle(double width, const Color &col,
            const std::vector<double> &dash=std::vector<double>(),
            Cap cap=Cap::Butt, Join join=Join::Miter)
    : line_width(width), color(col), dash_pattern(dash),
      line_cap(cap), line_join(join)
  {}

  std::string ToString() const;
};


//-------------------------------------------------  Comparison operators

bool operator==(const LineStyle &lhs, const LineStyle &rhs);
bool operator!=(const LineStyle &lhs, const LineStyle &rhs);


//-------------------------------------------------  Painter
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
class Painter
{
public:
  virtual ~Painter() {}

  // Explicitly default the copy/move constructors
  // and assignment operators:
  Painter() = default;
  Painter(const Painter &other) = default;
  Painter& operator=(const Painter &other) = default;
  Painter(Painter &&) = default;
  Painter& operator=(Painter &&) = default;


  /** @brief Returns true if the painter's canvas is empty/invalid.
   *
   *  Drawing without setting up the canvas via @see SetCanvas
   *  must be avoided (to prevent crashes).
   */
  virtual bool Empty() const = 0;


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

  //TODO doc - memory will be copied if copy is true (otherwise, we simply
  // reuse the given memory); image must be RGBA!
  virtual void SetCanvas(const ImageBuffer &image_buffer, bool copy) = 0;


  /**
   * @brief Returns the current visualization state (canvas) as ImageBuffer
   *
   * If "copy" is true, the canvas memory is copied into the ImageBuffer (i.e.
   * you can modify the buffer however you like). Otherwise, the ImageBuffer
   * shares the Painter's memory (and thus your subsequent memory modifications
   * will directly affect the canvas).
   */
  virtual ImageBuffer GetCanvas(bool copy) = 0;


  virtual void DrawLine(const Vec2d &from, const Vec2d &to,
                        const LineStyle &line_style) = 0;


  void DrawCircle(const Vec2d &center, double radius,
                  const LineStyle &line_style,
                  const Color &fill = Color(0, 0, 0, 0))
  { DrawCircleImpl(center, radius, line_style, fill); }


  void DrawRect(const Rect &rect, const LineStyle &line_style,
                const Color &fill = Color(0, 0, 0, 0))
  { DrawRectImpl(rect, line_style, fill); }


  //TODO DrawPoints - how to handle alternating colors???
  //TODO DrawEllipse <-- (optionally rotated) rect!
  //TODO DrawArrow
  //TODO OverlayImage <-- same size vs different, maybe clip to a circle; maybe add a border, etc

protected:
  virtual void DrawCircleImpl(const Vec2d &center, double radius,
                              const LineStyle &line_style,
                              const Color &fill) = 0;

  virtual void DrawRectImpl(const Rect &rect, const LineStyle &line_style,
                            const Color &fill) = 0;
};

std::unique_ptr<Painter> CreateImagePainter();
} // namespace vivi

#endif // __VIVI_DRAWING_H__
