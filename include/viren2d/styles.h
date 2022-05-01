#ifndef __VIREN2D_STYLES_H__
#define __VIREN2D_STYLES_H__

#include <vector>
#include <string>

#include <viren2d/colors.h>


namespace viren2d {

//-------------------------------------------------  LineStyle

/** @brief How to render the endpoints of a line. */
enum class LineCap : unsigned char {
  Butt = 0,
  Round,
  Square
};


/** @brief How to render the junction of two lines/segments. */
enum class LineJoin : unsigned char {
  Miter = 0,
  Round,
  Bevel
};


/** @brief Style definitions for lines & contours. */
struct LineStyle {
  double line_width;
  Color color;
  std::vector<double> dash_pattern;
  LineCap line_cap;
  LineJoin line_join;


  LineStyle(double width=2.0,
            const Color &col=Color(NamedColor::Azure),
            const std::vector<double> &dash=std::vector<double>(),
            LineCap cap=LineCap::Butt, LineJoin join=LineJoin::Miter)
    : line_width(width), color(col), dash_pattern(dash),
      line_cap(cap), line_join(join)
  {}

  virtual ~LineStyle() {}

  /** @brief Checks if this line style would lead to a renderable line. */
  bool IsValid() const;

  std::string ToString() const;
};

bool operator==(const LineStyle &lhs, const LineStyle &rhs);
bool operator!=(const LineStyle &lhs, const LineStyle &rhs);


//-------------------------------------------------  ArrowStyle

} // namespace viren2d

#endif // __VIREN2D_STYLES_H__

