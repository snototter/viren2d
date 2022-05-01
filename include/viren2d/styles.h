#ifndef __VIREN2D_STYLES_H__
#define __VIREN2D_STYLES_H__

#include <vector>
#include <string>
#include <ostream>

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
 public:
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
  virtual bool IsValid() const;

  virtual std::string ToString() const;

  bool Equals(const LineStyle &other) const;

  friend std::ostream &operator<<(std::ostream &os, const LineStyle &style) {
    os << style.ToString();
    return os;
  }
};

bool operator==(const LineStyle &lhs, const LineStyle &rhs);
bool operator!=(const LineStyle &lhs, const LineStyle &rhs);


//-------------------------------------------------  ArrowStyle
struct ArrowStyle : public LineStyle {
  double tip_length; /**< Length of the tip (percentage if in [0, 1]; else absolute in pixels). */
  double tip_angle;  /**< Angle between tip lines and the shaft in degrees. */
  bool tip_closed;   /**< How to draw the tip: only lines (false) or as a filled triangle (true). */

  ArrowStyle(double width = 2.0,
             const Color &col = Color(NamedColor::Azure),
             double tip_len = 0.1,
             double angle = 30.0,
             bool fill = false,
             const std::vector<double> &dash = std::vector<double>(),
             LineCap cap = LineCap::Butt,
             LineJoin join = LineJoin::Miter)
    : LineStyle(width, col, dash, cap, join),
      tip_length(tip_len), tip_angle(angle), tip_closed(fill)
  {}

  ~ArrowStyle() {}

  /** @brief Checks if this line style would lead to a renderable arrow. */
  virtual bool IsValid() const override;

  virtual std::string ToString() const override;

  bool Equals(const ArrowStyle &other) const;

  friend std::ostream &operator<<(std::ostream &os, const ArrowStyle &style) {
    os << style.ToString();
    return os;
  }
};

bool operator==(const ArrowStyle &lhs, const ArrowStyle &rhs);
bool operator!=(const ArrowStyle &lhs, const ArrowStyle &rhs);

} // namespace viren2d

#endif // __VIREN2D_STYLES_H__

