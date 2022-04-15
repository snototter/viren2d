#ifndef __VIVI_DRAWING_H__
#define __VIVI_DRAWING_H__

#include <memory>
#include <vector>

//FIXME remove opencv dependency
#include <opencv2/core.hpp>

#include <vivi/primitives.hpp>
#include <vivi/colors.hpp>

namespace vivi
{
class Painter
{
public:
  virtual ~Painter() {}

  // Explicitly default the copy/move constructors
  // and assignment operators:
  Painter() = default;
  Painter(const Painter& other) = default;
  Painter& operator=(const Painter& other) = default;
  Painter(Painter &&) = default;
  Painter& operator=(Painter &&) = default;

  /** @brief Returns true if the painter's canvas is empty/invalid.
   *
   *  Drawing without setting up the canvas via @see SetCanvas
   *  must be avoided (to prevent crashes).
   */
  virtual bool Empty() const = 0;

  virtual void SetCanvas(const cv::Mat &image) = 0;//FIXME remove

  /** @brief Initializes the canvas with the given color. This or
   *  any overloaded SetCanvas() must be called before any other
   *  DrawXXX calls can be performed.
   */
  virtual void SetCanvas(int width, int height, const Color& color) = 0;
  virtual cv::Mat GetCanvas() = 0;

  void DrawLine(const Vec2d& from, const Vec2d& to,
                double line_width, const Color& color,
                const std::vector<double> &dash_style = std::vector<double>())
  { DrawLineImpl(from, to, line_width, color, dash_style); }

  virtual void DummyShow() = 0;//TODO remove
  //TODO DrawLine
  //DrawArrow

protected:
  virtual void DrawLineImpl(const Vec2d& from, const Vec2d& to,
                            double line_width, const Color& color,
                            const std::vector<double> &dash_style) = 0;

};

std::unique_ptr<Painter> CreateImagePainter();
} // namespace vivi

#endif // __VIVI_DRAWING_H__
