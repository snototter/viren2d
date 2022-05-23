#ifndef __VIREN2D_STYLES_H__
#define __VIREN2D_STYLES_H__

#include <vector>
#include <string>
#include <ostream>
#include <initializer_list>

#include <viren2d/colors.h>
#include <viren2d/primitives.h>

//TODO(snototter) add tasks and progress to all:
//TODO [ ] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo


namespace viren2d {

//-------------------------------------------------  LineStyle

/** @brief How to render the endpoints of a line. */
enum class LineCap : unsigned char {
  Butt = 0,  ///< Start/stop the line exactly at the start/end point.
  Round,     ///< Rounded end, where the center of the circle is the line's start/end point.
  Square     ///< Squared end, where the center of the square is the line's start/end point.
};


/** @brief Returns the string representation. */
std::string LineCapToString(LineCap cap);


/** @brief How to render the junction of two lines/segments. */
enum class LineJoin : unsigned char {
  Miter = 0,  ///< Sharp/angled corner.
  Round,      ///< Rounded join, where the center of the circle is the joint point.
  Bevel       ///< The join is cut off at half the line width from the joint point.
};


/** @brief Returns the string representation. */
std::string LineJoinToString(LineJoin join);


/**
 * @brief Style definitions for lines & contours.
 *
 * Note: depending on the chosen line cap (or line join),
 * the corresponding line (joints) may start/end not exactly
 * where you specified.
 * If you want pixel-accurate start/end in combination with
 * a particular cap/join, use @see CapOffset() or
 * @see JoinOffset(). For example:
 * \code
   // Given a line with start/end as Vec2d:
   auto unit_dir = start.DirectionVector(end).UnitVector();
   const double offset = line_style.CapOffset();
   if (offset > 0.0) {
     start += offset * unit_dir;
     end -= offset * unit_dir;
   }
   \endcode
 */
struct LineStyle {
  double line_width;   /**< Line width (thickness) in pixels. */
  Color color;         /**< Color (rgb & alpha). */
  std::vector<double> dash_pattern; /**< Dash pattern defined as series of on-off segments (lengths in pixels). Line is solid if empty. */
  LineCap line_cap;    /**< How to render the endpoints. */
  LineJoin line_join;  /**< How to render the junction of two lines/segments. */

  /** Returns a library-wide pre-set default style.
   *  To use the painter's default style, you should use LineStyle::Default !!! TODO doc FIXME "special" default, "special " invalid or "some initialized" ??? */
  LineStyle();

//TODO test
  LineStyle(std::initializer_list<double> values);


  LineStyle(double width, const Color &col,
            const std::vector<double> &dash=std::vector<double>(),
            LineCap cap=LineCap::Butt, LineJoin join=LineJoin::Miter);


  // Nothing special about the LineStyle class, so we can have
  // the default copy/assignment/move c'tors/operators:
  LineStyle(const LineStyle &other) = default;
  LineStyle& operator=(const LineStyle &other) = default;
  LineStyle(LineStyle&&) = default;
  LineStyle& operator=(LineStyle &&) = default;

  virtual ~LineStyle() {}


  /** @brief Checks if this line style would lead to a renderable line. */
  virtual bool IsValid() const;

  //TODO doc & test
  virtual bool IsSpecialInvalid() const;
  virtual bool IsSpecialDefault() const;



  /** @brief Returns true if this style contains a dash stroke pattern. */
  bool IsDashed() const;


  /** @brief Computes how much the line cap will extend the line's start/end. */
  double CapOffset() const;


  /**
   * @brief Computes how much a line join will extend the joint.
   *
   * The interior_angle is the angle between two line segments in degrees.
   * This requires the miter_limit because Cairo switches from MITER to BEVEL
   * if the miter_limit is exceeded, see
   *   https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-miter-limit
   */
  double JoinOffset(double interior_angle, double miter_limit = 10.0) const;


  /** @brief Returns true if this and the other specify the same line. */
  bool Equals(const LineStyle &other) const;


  /** @brief Returns a human-readable string representation. */
  virtual std::string ToString() const;


  /** @brief Overloaded stream operator. */
  friend std::ostream &operator<<(std::ostream &os, const LineStyle &style) {
    os << style.ToString();
    return os;
  }


  //TODO [x] add documentation
  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  //TODO [ ] add Python bindings
  //TODO [ ] add Python test (tests/test_xxx.py)
  //TODO [ ] add C++ demo
  //TODO [ ] add Python demo
  /**
    FIXME see notes at the top of the header

   * The "invalid" style is used to switch to
   * "special" painter behavior, e.g. not drawing
   * the contour or using a default setting.
   */
  static const LineStyle Invalid;
  static const LineStyle Default;
};

bool operator==(const LineStyle &lhs, const LineStyle &rhs);
bool operator!=(const LineStyle &lhs, const LineStyle &rhs);


//-------------------------------------------------  ArrowStyle
/** @brief Style definitions for arrows. */
struct ArrowStyle : public LineStyle {
  double tip_length;   /**< Length of the tip (percentage if in (0, 1]; else absolute length in pixels). */
  double tip_angle;    /**< Angle between tip lines and the shaft in degrees. */
  bool tip_closed;     /**< How to draw the tip: only lines (false) or as a filled triangle (true). */
  bool double_headed;  /**< Should the head be drawn on both ends of the line? */


  /** FIXME see linestyle doc! */
  ArrowStyle();


  ArrowStyle(double width, const Color &col,
             double tip_len = 0.1, double angle = 20.0,
             bool fill = false, bool two_heads = false,
             const std::vector<double> &dash = std::vector<double>(),
             LineCap cap = LineCap::Butt, LineJoin join = LineJoin::Miter);


  ArrowStyle(const LineStyle &line_style,
             double tip_len = 0.1,
             double angle = 20.0,
             bool fill = false,
             bool two_heads = false)
    : LineStyle(line_style),
      tip_length(tip_len), tip_angle(angle),
      tip_closed(fill), double_headed(two_heads)
  {}


  // Nothing special about the LineStyle class, so we can have
  // the default copy/assignment/move c'tors/operators:
  ArrowStyle(const ArrowStyle &other) = default;
  ArrowStyle& operator=(const ArrowStyle &other) = default;
  ArrowStyle(ArrowStyle&&) = default;
  ArrowStyle& operator=(ArrowStyle &&) = default;

  ~ArrowStyle() {}


  /** @brief Checks if this style would lead to a renderable arrow. */
  bool IsValid() const override;


  //FIXME
  bool IsSpecialInvalid() const override;
  bool IsSpecialDefault() const override;

  /** @brief Computes the length of the arrow head/tip for the given shaft length. */
  double TipLengthForShaft(double shaft_length) const;


  /** @brief Computes the length of the arrow head/tip for the given shaft. */
  double TipLengthForShaft(const Vec2d &from, const Vec2d &to) const;


  /**
   * @brief Computes how much the tip would extend the line's start/end point.
   *
   * This requires the miter_limit because Cairo switches from MITER to BEVEL
   * if the miter_limit is exceeded, see
   *  https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-miter-limit
   *
   *  As this is only intended for internal library use, it will not
   *  be exposed in the Python API.
   */
  double TipOffset(double miter_limit = 10.0) const;


  /** @brief Returns true if this and the other specify the same arrow style. */
  bool Equals(const ArrowStyle &other) const;


  /** @brief Returns a human-readable string representation. */
  virtual std::string ToString() const override;


  /** @brief Overloaded stream operator. */
  friend std::ostream &operator<<(std::ostream &os, const ArrowStyle &style) {
    os << style.ToString();
    return os;
  }

  //TODO [x] add documentation
  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  //TODO [ ] add Python bindings
  //TODO [ ] add Python test (tests/test_xxx.py)
  //TODO [ ] add C++ demo
  //TODO [ ] add Python demo
  //FIXME
  static const ArrowStyle Invalid;
  static const ArrowStyle Default;
};

bool operator==(const ArrowStyle &lhs, const ArrowStyle &rhs);
bool operator!=(const ArrowStyle &lhs, const ArrowStyle &rhs);

//-------------------------------------------------  TextStyle
//enum class FontSlant : unsigned char {
//  Normal = 0,  ///< Upright font.
//  Italic = 1   ///< Italic font face.
//};


//enum class FontWeight : unsigned char {
//  Normal = 0,  ///< Default font weight.
//  Bold = 1     ///< Bold font face.
//};


struct TextStyle {
  int font_size;
  std::string font_family;
  Color font_color;
//  FontSlant font_slant;
//  FontWeight font_weight;
  bool font_bold;
  bool font_italic;
  //TODO add line_spacing default 1.2

  /**FIXME see linestyle doc*/
  TextStyle();


  TextStyle(unsigned int size,
            const std::string &family,
            const Color &color = Color::Black,
            bool bold = false, bool italic = false);

//  // Nothing special about the TextStyle class, so we can have
//  // the default copy/assignment/move c'tors/operators:
//  TextStyle(const TextStyle &other) = default;
//  TextStyle& operator=(const TextStyle &other) = default;
//  TextStyle(TextStyle&&) = default;
//  TextStyle& operator=(TextStyle &&) = default;

//  virtual ~TextStyle() {}


  /** @brief Checks if this line style would lead to a renderable line. */
  bool IsValid() const;


  //FIXME
  bool IsSpecialDefault() const;



  /** @brief Returns true if this and the other specify the same text style. */
  bool Equals(const TextStyle &other) const;


  /** @brief Returns a human-readable string representation. */
  std::string ToString() const;


  /** @brief Overloaded stream operator. */
  friend std::ostream &operator<<(std::ostream &os, const TextStyle &style) {
    os << style.ToString();
    return os;
  }

  static const TextStyle Default;
};


bool operator==(const TextStyle &lhs, const TextStyle &rhs);
bool operator!=(const TextStyle &lhs, const TextStyle &rhs);




//TODO bindings!!!!
//TODO to/from string!
enum class HorizontalAlignment : unsigned char {
  Left   = 1,
  Center = 1 << 1,
  Right  = 1 << 2
};

HorizontalAlignment HorizontalAlignmentFromString(const std::string &align);
std::string HorizontalAlignmentToString(HorizontalAlignment align);
std::ostream &operator<<(std::ostream &os, HorizontalAlignment align);

//TODO bindings!!!!
//TODO to/from string!
enum class VerticalAlignment : unsigned char {
  Top    = 1 << 3,
  Center = 1 << 4,
  Bottom = 1 << 5
};

VerticalAlignment VerticalAlignmentFromString(const std::string &align);
std::string VerticalAlignmentToString(VerticalAlignment align);
std::ostream &operator<<(std::ostream &os, VerticalAlignment align);

// Macro to reuse the vertical/horizontal anchors in
// the following position/anchor enum definitions
#define ALIGNMENT(HORZ, VERT) \
  static_cast<unsigned char>(HORZ) \
    | static_cast<unsigned char>(VERT)

// only for draw_text
// TODO doc & test
// public use: prefer Center, Left, ... over combining Horz & Vert
enum class TextAnchor : unsigned char {
  Center = ALIGNMENT(HorizontalAlignment::Center,
                     VerticalAlignment::Center),

  Left =   ALIGNMENT(HorizontalAlignment::Left,
                     VerticalAlignment::Center),
  Right =  ALIGNMENT(HorizontalAlignment::Right,
                     VerticalAlignment::Center),
  Top =    ALIGNMENT(HorizontalAlignment::Center,
                     VerticalAlignment::Top),
  Bottom = ALIGNMENT(HorizontalAlignment::Center,
                     VerticalAlignment::Bottom),

  TopLeft =     ALIGNMENT(HorizontalAlignment::Left,
                          VerticalAlignment::Top),
  TopRight =    ALIGNMENT(HorizontalAlignment::Right,
                          VerticalAlignment::Top),
  BottomLeft =  ALIGNMENT(HorizontalAlignment::Left,
                          VerticalAlignment::Bottom),
  BottomRight = ALIGNMENT(HorizontalAlignment::Right,
                          VerticalAlignment::Bottom)
};

//TODO doc, test, etc
//TODO bindings!!!!
TextAnchor TextAnchorFromString(const std::string &anchor);
std::string TextAnchorToString(TextAnchor anchor);
std::ostream &operator<<(std::ostream &os, TextAnchor anchor);


enum class BoundingBoxLabelPosition : unsigned char {
  Top = 1,
  Bottom = 1 << 1,

  Right = 1 << 2,
  RightT2B = Right,
  RightB2T = 1 << 3,

  Left = 1 << 4,
  LeftB2T = Left,
  LeftT2B = 1 << 5
};

//TODO doc, test, etc
//TODO bindings!!!!
BoundingBoxLabelPosition BoundingBoxLabelPositionFromString(const std::string &pos);
std::string BoundingBoxLabelPositionToString(BoundingBoxLabelPosition pos);
std::ostream &operator<<(std::ostream &os, BoundingBoxLabelPosition pos);

//FIXME rename to 2d!
struct BoundingBox2DStyle {
  LineStyle line_style;
  TextStyle text_style;
  double alpha_box_fill;
  double alpha_text_fill;
  HorizontalAlignment text_alignment;
  BoundingBoxLabelPosition label_position;
  double label_padding;
  bool clip_label;

  /** TODO Returns a library-wide pre-set default style.
   *  To use the painter's default style, you should use LineStyle::Default !!! TODO doc FIXME "special" default, "special " invalid or "some initialized" ??? */
  BoundingBox2DStyle();

  BoundingBox2DStyle(const LineStyle &contour,
                     const TextStyle &label_style,
                     double bounding_box_alpha, double label_box_alpha,
                     HorizontalAlignment label_alignment,
                     BoundingBoxLabelPosition label_pos,
                     double text_padding, bool clip_lbl);

////TODO test
//  BoundingBoxStyle(std::initializer_list<double> values);
//  LineStyle(double width, const Color &col,
//            const std::vector<double> &dash=std::vector<double>(),
//            LineCap cap=LineCap::Butt, LineJoin join=LineJoin::Miter);


  // Nothing special about the BoundingBoxStyle class, so we can have
  // the default copy/assignment/move c'tors/operators:
  BoundingBox2DStyle(const BoundingBox2DStyle &other) = default;
  BoundingBox2DStyle& operator=(const BoundingBox2DStyle &other) = default;
  BoundingBox2DStyle(BoundingBox2DStyle&&) = default;
  BoundingBox2DStyle& operator=(BoundingBox2DStyle &&) = default;

  ~BoundingBox2DStyle() {}


  /** @brief Checks if this style would lead to a renderable bounding box. */
  bool IsValid() const;

  Vec2d LabelPadding() const;
  Color BoxFillColor() const;
  Color TextFillColor() const;


  /** @brief Returns true if this and the other specify the same BoundingBoxStyle. */
  bool Equals(const BoundingBox2DStyle &other) const;


  /** @brief Returns a human-readable string representation. */
  std::string ToString() const;


  /** @brief Overloaded stream operator. */
  friend std::ostream &operator<<(std::ostream &os, const BoundingBox2DStyle &style) {
    os << style.ToString();
    return os;
  }


  /**
    FIXME add a default/customizable bbox style per painter
   */
//  static const BoundingBox2DStyle Default;
};

bool operator==(const BoundingBox2DStyle &lhs, const BoundingBox2DStyle &rhs);
bool operator!=(const BoundingBox2DStyle &lhs, const BoundingBox2DStyle &rhs);



} // namespace viren2d

#endif // __VIREN2D_STYLES_H__

