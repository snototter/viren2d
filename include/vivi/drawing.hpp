#ifndef __VIVI_DRAWING_H__
#define __VIVI_DRAWING_H__

#include <memory>
#include <vector>
#include <string>

//FIXME remove opencv dependency
#include <opencv2/core.hpp>

#include <vivi/primitives.hpp>
#include <vivi/colors.hpp>

namespace vivi
{

//-------------------------------------------------  LineStyle

/** @brief Style definitions for lines & contours. */
struct LineStyle
{
  double line_width;
  Color color;
  std::vector<double> dash_pattern;

  LineStyle(double width, const Color &col)
    : line_width(width), color(col),
      dash_pattern(std::vector<double>())
  {}

  LineStyle(double width, const Color &col, const std::vector<double> &dash)
    : line_width(width), color(col),
      dash_pattern(dash)
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


  virtual void SetCanvas(const cv::Mat &image) = 0;//FIXME remove - replace by stb or buffer...


  /**
   * @brief Initializes the canvas with the given color.
   *
   * This or any overloaded SetCanvas() must be called before
   * any other DrawXXX calls can be performed.
   */
  virtual void SetCanvas(int width, int height, const Color& color) = 0;

  /**
   * @brief Initializes the canvas with the given image file.
   *
   * Uses the stb/stb_image library for loading, which supports
   * the most common formats.
   * This or any overloaded SetCanvas() must be called before
   * any other DrawXXX calls can be performed.
   */
  virtual void SetCanvas(const std::string &image_filename) = 0;


  /**
   * @brief Saves the canvas to disk as either JPEG or PNG.
   *
   * Uses the stb/stb_image_write library for writing. Note
   * that PNG output is not optimal (20-50% larger file size
   * than using an optimized library).
   * I consider writing to disk only a nice-to-have feature,
   * thus I'm not including any other specialized third-party
   * libraries for that.
   */
  virtual void SaveCanvas(const std::string &image_filename) = 0;
  virtual cv::Mat GetCanvas() = 0; //FIXME remove - replace by stb or buffer, or ...

  virtual void DrawLine(const Vec2d& from, const Vec2d& to,
                        const LineStyle& line_style) = 0;

  void DrawCircle(const Vec2d &center, double radius,
                  const LineStyle &line_style,
                  const Color &fill = Color(0, 0, 0, 0))
  { DrawCircleImpl(center, radius, line_style, fill); }


  void DrawRect(const Rect &rect, const LineStyle &line_style,
                const Color &fill = Color(0, 0, 0, 0))
  { DrawRectImpl(rect, line_style, fill); }

  virtual void DummyShow() = 0;//TODO remove

  //TODO DrawPoints - how to handle alternating colors???
  //TODO DrawEllipse <-- (optionally rotated) rect!
  //TODO DrawArrow
  //TODO OverlayImage <-- same size vs different, maybe clip to a circle; maybe add a border, ppp

protected:
  virtual void DrawCircleImpl(const Vec2d& center, double radius,
                              const LineStyle &line_style,
                              const Color &fill) = 0;

  virtual void DrawRectImpl(const Rect &rect, const LineStyle &line_style,
                            const Color &fill) = 0;
};

std::unique_ptr<Painter> CreateImagePainter();
} // namespace vivi

#endif // __VIVI_DRAWING_H__
