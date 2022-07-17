#ifndef __VIREN2D_STYLES_H__
#define __VIREN2D_STYLES_H__

#include <vector>
#include <map>
#include <string>
#include <ostream>

#include <viren2d/colors.h>
#include <viren2d/primitives.h>
#include <viren2d/positioning.h>

//TODO(snototter) add tasks and progress to all:
//TODO [ ] add documentation
//TODO [ ] add C++ test (tests/xxx_test.cpp)
//TODO [ ] add Python bindings
//TODO [ ] add Python test (tests/test_xxx.py)
//TODO [ ] add C++ demo
//TODO [ ] add Python demo


//FIXME: should implement a ToDetailedString() for all styles (to be called whenever tests fail)
//FIXME switch to coding/documentation style

namespace viren2d {
//-------------------------------------------------  Line endpoints
/// How to render the endpoints of a line.
enum class LineCap : unsigned char {
  Butt = 0,  ///< Start/stop the line exactly at the start/end point.
  Round,     ///< Rounded end, where the center of the circle is the line's start/end point.
  Square     ///< Squared end, where the center of the square is the line's start/end point.
};


/// Returns the string representation.
std::string LineCapToString(LineCap cap);


/// Returns a LineCap from its string representation.
LineCap LineCapFromString(const std::string &cap);


/// Output stream operator to print a LineCap.
std::ostream &operator<<(std::ostream &os, LineCap cap);


/// Computes how much the line cap will extend the line's start/end.
double LineCapOffset(LineCap cap, double line_width);


//-------------------------------------------------  Line joins/junctions

/// How to render the junction of two lines/segments.
enum class LineJoin : unsigned char {
  Miter = 0,  ///< Sharp/angled corner.
  Round,      ///< Rounded join, where the center of the circle is the joint point.
  Bevel       ///< The join is cut off at half the line width from the joint point.
};


/// Returns the string representation.
std::string LineJoinToString(LineJoin join);


/// Returns a LineJoin from its string representation.
LineJoin LineJoinFromString(const std::string &join);


/// Output stream operator to print a LineJoin.
std::ostream &operator<<(std::ostream &os, LineJoin join);


/// Computes how much a line join will extend the joint.
///
/// The interior_angle is the angle between two line segments in degrees.
/// This requires the miter_limit because Cairo switches from MITER to BEVEL
/// if the miter_limit is exceeded, see
/// https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-miter-limit
double LineJoinOffset(
    LineJoin join, double line_width, double interior_angle,
    double miter_limit);


//-------------------------------------------------  MarkerStyle
/// Marker shape enumeration.
enum class Marker : unsigned char {
  Point = 0,  ///< A point is a filled circle, char representation: '.'.
  Circle,     ///< A circle (not filled), char representation: 'o'.

  Plus,       ///< Plus sign, char representation: '+'.
  Cross,      ///< Cross marker, char representation: 'x'.

  Square,        ///< Square marker, char representation: 's'.
  RotatedSquare, ///< Rotated square marker, char representation: 'r'.
  Diamond,       ///< Diamond marker, char representation: 'd'.

  TriangleUp,    ///< Upward-pointing triangle marker, char representation: '^'.
  TriangleDown,  ///< Downward-pointing triangle marker, char representation: 'v'.
  TriangleLeft,  ///< Left-pointing triangle marker, char representation: '<'.
  TriangleRight, ///< Right-pointing triangle marker, char representation: '>'.

  Star,       ///< Five-pointed star (Asterisk), char representation: '*'.
  Pentagram,  ///< Five-pointed star, char representation: '5'.
  Pentagon,   ///< Five-sided polygon, char representation: 'p'.
  Hexagram,   ///< Six-pointed star, char representation: '6'.
  Hexagon,    ///< Six-sided polygon, char representation: 'h'.
  Heptagram,  ///< Seven-pointed star, char representation: '7'.
  Heptagon,   ///< Seven-sided polygon, char representation: 'H'.
  Octagram,   ///< Eight-pointed star, char representation: '8'.
  Octagon,    ///< Eight-sided polygon, char representation: '0' (zero).
  Enneagram,  ///< Nine-pointed star, char representation: '9'.
  Enneagon    ///< Nine-sided polygon (Nonagon), char representation: 'n'.

  // If you implement additional markers, you have to consider it within:
  // * MarkerFromChar (there will be NO compilation warnings if you miss it!)
  // * MarkerToChar
  // * MarkerStyle::IsFilled - i.e. AdjustMarkerFill() - and MarkerStyle::IsValid
  // * helpers::DrawMarker
  //
  // Note that "Point" and "Enneagon" are used throughout the code base to
  // denote the first & last enum entries. If you need to change these,
  // also adjust ListMarkers() and the test suite accordingly!
};


/// Returns the Marker from its character representation.
Marker MarkerFromChar(char m);


/// Returns the character representation for the given marker.
char MarkerToChar(Marker marker);


/// Output stream operator to print a marker.
std::ostream &operator<<(std::ostream &os, Marker marker);


/// Returns the char representations of all implemented marker shapes.
std::vector<Marker> ListMarkers();


/// How to render a marker/keypoint.
struct MarkerStyle {
  /// Specifies which type of marker should be drawn.
  Marker marker;

  /// Marker size (i.e. height) in pixels.
  double size;

  /// Contour line width in pixels (will be ignored if the marker is filled).
  double thickness;

  /// Color of the marker's contour or fill.
  Color color;

  /// If true (and the shape allows), the marker will be filled.
  bool filled;

  /// If `background_color` is valid, a circle (or square) will be drawn
  /// behind the actual marker. Size will be `size` + 2 * background_border.
  /// Can be used to improve the contrast of the marker.
  double background_border;

  /// Can be used to improve the contrast, see `border_thickness`.
  Color background_color;

  /// How to render the endpoints.
  LineCap cap;

  /// How to render the junction of two lines/segments.
  LineJoin join;


  /// Returns a library-wide pre-set default style.
  MarkerStyle();


  /// Constructs a custom MarkerStyle.
  /// For non-fillable (circle, cross, etc.) or always-filled (point, etc.)
  /// markers, the value of ``fill`` will be ignored.
  MarkerStyle(
      Marker type,
      double marker_size,
      double thickness_marker,
      const Color &color_marker,
      bool fill = false,
      double border_background = 3.0,
      const Color &color_background = Color::Invalid,
      LineCap line_cap = LineCap::Round,
      LineJoin line_join = LineJoin::Miter);


  /// Returns true if this and the other specify the same text style.
  bool Equals(const MarkerStyle &other) const;


  /// Returns true if this style leads to a renderable marker.
  bool IsValid() const;


  /// Returns true if the underlying marker shape should be filled.
  /// Otherwise, only its contour should be rendered.
  ///
  /// Note that the user can choose for some marker shapes if they
  /// should be filled or not, while for some other shapes, the
  /// fill status is pre-determined (e.g. point, circle, cross, ...)
  bool IsFilled() const;


  /// Computes how much a line join will extend the joint.
  ///
  /// The interior_angle is the angle between two line segments in degrees.
  /// This requires the miter_limit because Cairo switches from MITER to BEVEL
  /// if the miter_limit is exceeded, see
  /// https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-miter-limit
  inline double JoinOffset(double interior_angle, double miter_limit = 10.0) const {
    return LineJoinOffset(join, thickness, interior_angle, miter_limit);
  }


  /// Computes how much the line cap will extend the line's start/end.
  inline double CapOffset() const {
    return LineCapOffset(cap, thickness);
  }


  /// Returns a human-readable string representation.
  std::string ToString() const;


  /// Overloaded stream operator.
  friend std::ostream &operator<<(std::ostream &os, const MarkerStyle &style) {
    os << style.ToString();
    return os;
  }
};


// Comparison operators for MarkerStyle objects
bool operator==(const MarkerStyle &lhs, const MarkerStyle &rhs);
bool operator!=(const MarkerStyle &lhs, const MarkerStyle &rhs);


//-------------------------------------------------  LineStyle
/** TODO change docstyle
 * How to draw lines & contours.
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
  ///< Line width (thickness) in pixels.
  double width;

  /// Color (rgb & alpha).
  Color color;

  /// Dash pattern defined as series of on-off segments (lengths in pixels). Line is solid if empty.
  std::vector<double> dash_pattern;

  /// Offset into the pattern at which the stroke begins.
  double dash_offset;

  /// How to render the line/dash stroke endpoints
  LineCap cap;

  /// How to render the junction of two lines/segments.
  LineJoin join;


  /// Creates a library-wide pre-set default style.
  LineStyle();


  /// Standard constructor.
  LineStyle(
      double width, const Color &col,
      const std::vector<double> &dash = std::vector<double>(),
      double offset = 0.0,
      LineCap cap = LineCap::Butt,
      LineJoin join = LineJoin::Miter);


  /// Nothing special about the LineStyle class, so we can have
  /// the default copy/assignment/move c'tors/operators:
  LineStyle(const LineStyle &other) = default;
  LineStyle& operator=(const LineStyle &other) = default;
  LineStyle(LineStyle&&) = default;
  LineStyle& operator=(LineStyle &&) = default;


  /// Explicit destructor needed (`ArrowStyle` derives from `LineStyle`).
  virtual ~LineStyle() {}


  /// Returns true if this line style leads to a renderable line.
  virtual bool IsValid() const;


  /// Returns true if this style equals the special `Invalid` member.
  virtual bool IsSpecialInvalid() const;


  /// Returns true if this style contains a dash stroke pattern.
  bool IsDashed() const;


  /// Computes how much the line cap will extend the line's start/end.
  inline double CapOffset() const {
    return LineCapOffset(cap, width);
  }


  /// Computes how much a line join will extend the joint.
  ///
  /// The interior_angle is the angle between two line segments in degrees.
  /// This requires the miter_limit because Cairo switches from MITER to BEVEL
  /// if the miter_limit is exceeded, see
  /// https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-miter-limit
  inline double JoinOffset(double interior_angle, double miter_limit = 10.0) const {
    return LineJoinOffset(join, width, interior_angle, miter_limit);
  }


  /// Returns true if this and the other specify the same line.
  bool Equals(const LineStyle &other) const;


  /// Returns a human-readable string representation.
  virtual std::string ToString() const;

  //TODO doc
  virtual std::string ToDetailedString() const;

  /// Overloaded stream operator.
  friend std::ostream &operator<<(std::ostream &os, const LineStyle &style) {
    os << style.ToString();
    return os;
  }


  /// In several painter methods, LineStyle::Invalid
  /// is used to skip drawing the outline/contour (and
  /// only fill the corresponding shape instead).
  static const LineStyle Invalid;
};


/// Comparison operators for LineStyle objects
bool operator==(const LineStyle &lhs, const LineStyle &rhs);
bool operator!=(const LineStyle &lhs, const LineStyle &rhs);


//-------------------------------------------------  ArrowStyle
/** How to draw arrows. */
struct ArrowStyle : public LineStyle {
  double tip_length;   /**< Length of the tip (percentage if in (0, 1]; else absolute length in pixels). */
  double tip_angle;    /**< Angle between tip lines and the shaft in degrees. */
  bool tip_closed;     /**< How to draw the tip: only lines (false) or as a filled triangle (true). */
  bool double_headed;  /**< Should the head be drawn on both ends of the line? */


  /** FIXME see linestyle doc! */
  ArrowStyle();


  ArrowStyle(
      double width,
      const Color &col,
      double tip_len = 0.1,
      double angle = 20.0,
      bool fill = false,
      bool two_heads = false,
      const std::vector<double> &dash = std::vector<double>(),
      double offset = 0.0,
      LineCap cap = LineCap::Round,
      LineJoin join = LineJoin::Miter);


  ArrowStyle(
      const LineStyle &line_style,
      double tip_len = 0.1,
      double angle = 20.0,
      bool fill = false,
      bool two_heads = false);


  // Nothing special about the LineStyle class, so we can have
  // the default copy/assignment/move c'tors/operators:
  ArrowStyle(const ArrowStyle &other) = default;
  ArrowStyle& operator=(const ArrowStyle &other) = default;
  ArrowStyle(ArrowStyle&&) = default;
  ArrowStyle& operator=(ArrowStyle &&) = default;

  ~ArrowStyle() {}


  /** Checks if this style would lead to a renderable arrow. */
  bool IsValid() const override;


  /** Computes the length of the arrow head/tip for the given shaft length. */
  double TipLengthForShaft(double shaft_length) const;


  /** Computes the length of the arrow head/tip for the given shaft. */
  double TipLengthForShaft(const Vec2d &from, const Vec2d &to) const;


  /**
   * Computes how much the tip would extend the line's start/end point.
   *
   * This requires the miter_limit because Cairo switches from MITER to BEVEL
   * if the miter_limit is exceeded, see
   * https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-miter-limit
   *
   * As this is only intended for internal library use, it will not
   * be exposed in the Python API.
   */
  double TipOffset(double miter_limit = 10.0) const;


  /** Returns true if this and the other specify the same arrow style. */
  bool Equals(const ArrowStyle &other) const;


  /** Returns a human-readable string representation. */
  std::string ToString() const override;

  //TODO doc
  std::string ToDetailedString() const override;

  /** Overloaded stream operator. */
  friend std::ostream &operator<<(std::ostream &os, const ArrowStyle &style) {
    os << style.ToString();
    return os;
  }
};


// Comparison operators for ArrowStyle objects
bool operator==(const ArrowStyle &lhs, const ArrowStyle &rhs);
bool operator!=(const ArrowStyle &lhs, const ArrowStyle &rhs);


//-------------------------------------------------  TextStyle
/// Specifies how to render text.
struct TextStyle {
  /// Font size (height) in pixels. Note that this can differ from the actual
  /// height of a glyph, i.e. the actual height is font-specific.
  int size;


  /// Name of the font family.
  /// Most available fonts on the system should be supported.
  /// If you experience issues, try the generic CSS2 family names
  /// first, *e.g.* ``serif``, ``sans-serif``, or ``monospace``. Refer to the
  /// `Cairo documentation <https://www.cairographics.org/manual/cairo-text.html#cairo_select_font_face>`__
  /// for more details.
  std::string family;


  /// Color of the glyphs.
  Color color;


  /// Flag to switch between normal and bold font weight.
  bool bold;


  /// Flag to switch between normal and italic slant.
  bool italic;


  /// Scaling factor of the vertical distance between consecutive lines of text
  double line_spacing;


  HorizontalAlignment halign;
  VerticalAlignment valign;

  /// Creates a library-wide default style.
  TextStyle();


  /// Creates a customized style.
  TextStyle(
      unsigned int font_size,
      const std::string &font_family,
      const Color &font_color = Color::Black,
      bool font_bold = false,
      bool font_italic = false,
      double spacing = 1.2,
      HorizontalAlignment horz_alignment = HorizontalAlignment::Left,
      VerticalAlignment vert_alignment = VerticalAlignment::Top);


  // Nothing special about the TextStyle class, so we can have
  // the default copy/assignment/move c'tors/operators:
  TextStyle(const TextStyle &other) = default;
  TextStyle& operator=(const TextStyle &other) = default;
  TextStyle(TextStyle&&) = default;
  TextStyle& operator=(TextStyle &&) = default;


  /// Checks if this style would lead to renderable text.
  bool IsValid() const;


  /// Returns true if this and the other specify the same text style.
  bool Equals(const TextStyle &other) const;


  /// Returns a human-readable string representation.
  std::string ToString() const;


  /// Overloaded stream operator.
  friend std::ostream &operator<<(std::ostream &os, const TextStyle &style) {
    os << style.ToString();
    return os;
  }
};


// Comparison operators for TextStyle objects
bool operator==(const TextStyle &lhs, const TextStyle &rhs);
bool operator!=(const TextStyle &lhs, const TextStyle &rhs);


//-------------------------------------------------  BoundingBox2DStyle
/// How to draw a 2D bounding box.
struct BoundingBox2DStyle {
  LineStyle line_style; //TODO documentation
  TextStyle text_style;

  /// Optional fill color of the bounding box.
  /// This is a public member for user convenience. Drawing
  /// methods, however, should use @see BoxFillColor() which
  /// takes care of "special" color choices (like "use the same
  /// color as the contour but with a different alpha").
  Color box_fill_color;
  Color text_fill_color;
  LabelPosition label_position;
  Vec2d label_padding;
  bool clip_label;

  BoundingBox2DStyle();

  BoundingBox2DStyle(
      const LineStyle &contour,
      const TextStyle &label_style,
      const Color &bounding_box_fill_color,
      const Color &label_box_color,
      LabelPosition label_pos,
      const Vec2d &text_padding,
      bool clip_lbl);

  // Nothing special about the BoundingBoxStyle class, so we can have
  // the default copy/assignment/move c'tors/operators:
  BoundingBox2DStyle(const BoundingBox2DStyle &other) = default;
  BoundingBox2DStyle& operator=(const BoundingBox2DStyle &other) = default;
  BoundingBox2DStyle(BoundingBox2DStyle&&) = default;
  BoundingBox2DStyle& operator=(BoundingBox2DStyle &&) = default;

  ~BoundingBox2DStyle() {}


  /** Checks if this style would lead to a renderable bounding box. */
  bool IsValid() const;


  /** Returns the fill color of the bounding box (or an invalid color to skip filling). */
  Color BoxFillColor() const;


  /** Returns the fill color of the label's text box (or an invalid color to skip filling) .*/
  Color TextFillColor() const;


  /** Returns true if this and the other specify the same BoundingBoxStyle. */
  bool Equals(const BoundingBox2DStyle &other) const;


  /** Returns a human-readable string representation. */
  std::string ToString() const;


  /** Overloaded stream operator. */
  friend std::ostream &operator<<(std::ostream &os, const BoundingBox2DStyle &style) {
    os << style.ToString();
    return os;
  }
};


// Comparison operators for BoundingBox2DStyle objects
bool operator==(const BoundingBox2DStyle &lhs, const BoundingBox2DStyle &rhs);
bool operator!=(const BoundingBox2DStyle &lhs, const BoundingBox2DStyle &rhs);

} // namespace viren2d

#endif // __VIREN2D_STYLES_H__

