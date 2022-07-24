#ifndef __VIREN2D_DRAWING_H__
#define __VIREN2D_DRAWING_H__

#include <functional>
#include <memory>
#include <string>
//#include <tuple>
#include <utility>
#include <vector>

#include <Eigen/Core>

#include <viren2d/primitives.h>
#include <viren2d/imagebuffer.h>
#include <viren2d/colors.h>
#include <viren2d/styles.h>


namespace viren2d {

using Matrix3x3d = Eigen::Matrix<double, 3, 3, Eigen::RowMajor>;
using Matrix3x4d = Eigen::Matrix<double, 3, 4, Eigen::RowMajor>;


/// The Painter provides functionality to draw on a canvas.
class Painter {
public:
  // The basic interface is trivially con-/destructable, assignable & movable.
  virtual ~Painter() {}
  Painter() = default;
  Painter(const Painter &other) = default;
  Painter& operator=(const Painter &other) = default;
  Painter(Painter &&) = default;
  Painter& operator=(Painter &&) = default;


  /// Returns true if the painter's canvas is valid.
  ///
  /// You have to set up the painter's canvas before drawing
  ///  or retrieving, see the overloaded `SetCanvas` methods.
  virtual bool IsValid() const = 0;


  /// Initializes the canvas with the given color.
  ///
  /// This or any overloaded SetCanvas() must be called before
  /// any other DrawXXX calls can be performed.
  virtual void SetCanvas(int height, int width, const Color &color) = 0;


  /// Initializes the canvas from the given image file.
  /// This functionality uses the
  /// `stb library <https://github.com/nothings/stb/blob/master/stb_image.h>`__
  /// to load the image file. Supported formats are:
  ///    JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM
  ///
  /// This or any overloaded SetCanvas() must be called before
  /// any other DrawXXX calls can be performed.
  virtual void SetCanvas(const std::string &image_filename) = 0;


  /// Initializes the canvas from the given image.
  /// The image can be grayscale (1-channel), RGB or RGBA.
  /// The painter will always create a copy - thus, the image
  /// buffer can be safely destroyed afterwards.
  virtual void SetCanvas(const ImageBuffer &image_buffer) = 0;


  /// Returns the size of the canvas.
  virtual Vec2i GetCanvasSize() const = 0;


  /// Returns the current visualization state (canvas) as ImageBuffer
  ///
  /// If `copy` is true, the canvas memory is copied into the ImageBuffer (i.e.
  /// you can modify the buffer however you like). Otherwise, the ImageBuffer
  /// shares the Painter's memory (and thus your subsequent memory modifications
  /// will directly affect the canvas).
  virtual ImageBuffer GetCanvas(bool copy) const = 0;


  ///  Draws a circular arc.
  ///
  /// Args:
  ///   center: Center position.
  ///   radius: Radius of the arc in pixels.
  ///   angle_from: The arc will be drawn from ``angle_from``
  ///     to ``angle_to`` in clockwise direction. Both angles
  ///     are specified in degrees, where 0 degrees points in the
  ///    direction of increasing *x* coordinates.
  ///    angle_to: See ``angle_from``.
  ///    line_style: How to draw the arc's outline.
  ///       If you pass `LineStyle::Invalid`, the contour will not be
  ///       drawn - then, you must provide a valid ``fill_color``.
  ///    include_center:  If ``true`` (default), the center point
  ///       will be included when drawing the outline and filling.
  ///    fill_color: If you provide a valid color, the arc will be filled.
  bool DrawArc(
      const Vec2d &center, double radius,
      double angle1, double angle2,
      const LineStyle &line_style = LineStyle(),
      bool include_center = true,
      const Color &fill_color = Color::Invalid) {
    return DrawArcImpl(
          center, radius, angle1, angle2, line_style,
          include_center, fill_color);
  }


  /// Draws an arrow.
  ///
  /// Args:
  ///   pt1: Start of the arrow shaft.
  ///   pt2: End of the arrow shaft (the *pointy* end).
  ///   arrow_style: How to draw the arrow.
  bool DrawArrow(
      const Vec2d &from, const Vec2d &to,
      const ArrowStyle &arrow_style = ArrowStyle()) {
    return DrawArrowImpl(from, to, arrow_style);
  }


  /// Draws a single 2D bounding box.
  ///
  /// Args:
  ///   rect: The box geometry.
  ///   label: The (potentially multi-line) label.
  ///   box_style: How to draw this bounding box.
  bool DrawBoundingBox2D(
      const Rect &box, const std::vector<std::string> &label,
      const BoundingBox2DStyle &style = BoundingBox2DStyle()) {
    return DrawBoundingBox2DImpl(box, label, style);
  }


  /// Draws a circle.
  ///
  /// Args:
  ///   center: Center position.
  ///   radius: Radius of the circle in pixels.
  ///   line_style: How to draw the circle's outline.
  ///   fill_color: If provided, the circle will be filled.
  bool DrawCircle(
      const Vec2d &center, double radius,
      const LineStyle &line_style = LineStyle(),
      const Color &fill_color = Color::Invalid) {
    return DrawCircleImpl(center, radius, line_style, fill_color);
  }


  /// Draws an ellipse.
  ///
  /// Args:
  ///   ellipse: The ellipse to be drawn.
  ///   line_style: How to draw the ellipse's outline.
  ///   fill_color: If provided, the ellipse will be filled.
  bool DrawEllipse(
      const Ellipse &ellipse,
      const LineStyle &line_style = LineStyle(),
      const Color &fill_color = Color::Invalid) {
    return DrawEllipseImpl(ellipse, line_style, fill_color);
  }


  /// Draws a grid between top_left and bottom_right.
  /// If both points are the same, the grid will span the whole canvas.
  bool DrawGrid(
      const Vec2d &top_left, const Vec2d &bottom_right,
      double spacing_x, double spacing_y,
      const LineStyle &line_style = LineStyle()) {
    return DrawGridImpl(
          top_left, bottom_right,
          spacing_x, spacing_y,
          line_style);
  }


  //TODO doc
  // returns the projected line of horizon - check its IsValid!
  //TODO if defaults change here, also change them in bindings_painter!
  Line2d DrawHorizonLine(
      const Matrix3x3d &K, const Matrix3x3d &R, const Vec3d &t,
      const LineStyle &line_style = LineStyle(
        5, Color::CoordinateAxisColor('z'), {20, 30}, 0.0, LineCap::Round)) {
    return DrawHorizonLineImpl(K, R, t, line_style);
  }


  /// Draws an image.
  ///
  /// Args:
  ///   image: The image.
  ///   anchor: How to orient the text with respect to ``position``.
  ///   position: Reference point where to anchor the image.
  ///   alpha: Opacity as :class:`float` :math:`\in [0,1]`, where ``1`` is
  ///     fully opaque and ``0`` is fully transparent.
  ///   scale_x: Horizontal scaling factor.
  ///   scale_y: Vertical scaling factor.
  ///   rotation: Clockwise rotation in degrees.
  ///   clip_factor: If greater than 0, the corners will be clipped. In
  ///     particular, :math:`0 < \text{clip} \leq 0.5` will result in a
  ///     rounded rectangle, where the corner radius will be ``clip_factor``
  ///     times :math:`\min(\text{width}, \text{height})`.
  ///     If :math:`\text{clip} > 0.5`, the clip region will be an ellipse,
  ///     where major/minor axis length equal the width/height of the image.
  ///   line_style: If provided, the contour/border of the image will be drawn.
  bool DrawImage(
      const ImageBuffer &image,
      const Vec2d &position,
      Anchor anchor = Anchor::TopLeft,
      double alpha = 1.0,
      double scale_x = 1.0, double scale_y = 1.0,
      double rotation = 0.0, double clip_factor = 0.0,
      const LineStyle &line_style = LineStyle::Invalid) {
    return DrawImageImpl(
          image, position, anchor, alpha,
          scale_x, scale_y, rotation, clip_factor, line_style);
  }


  /// Draws a line (segment).
  ///
  /// Args:
  ///   from: Start position.
  ///   to: End position.
  ///     line_style: How to draw the line.
  void DrawLine(
      const Vec2d &from, const Vec2d &to,
      const LineStyle &line_style = LineStyle()) {
    DrawLineImpl(from, to, line_style);
  }


  /// Draws a single marker/keypoint.
  ///
  /// Args:
  ///   pos: Position of the marker.
  ///   marker_style: How to draw the marker.
  void DrawMarker(
      const Vec2d &position,
      const MarkerStyle &style = MarkerStyle()) {
    DrawMarkerImpl(position, style);
  }


  /// Draws multiple (similarly styled) markers/keypoints.
  ///
  /// Args:
  ///   markers: Holds the position and color of each marker.
  ///     If a marker's color is invalid, it will be drawn using
  ///     ``style``'s color specification instead.
  ///   marker_style: How to draw the markers (except for the color).
  void DrawMarkers(
      const std::vector<std::pair<Vec2d, Color>> &markers,
      const MarkerStyle &style = MarkerStyle()) {
    DrawMarkersImpl(markers, style);
  }


  /// Draws a polygon.
  ///
  /// Args:
  ///   polygon: Points of the polygon.
  ///   line_style: How to draw the polygon's outline.
  ///     If you pass `LineStyle::Invalid`, the contour will not be
  ///     drawn - then, you must provide a valid ``fill_color``.
  ///   fill_color: If you provide a valid color, the polygon will be filled.
  void DrawPolygon(
      const std::vector<Vec2d> &points,
      const LineStyle &line_style = LineStyle(),
      const Color &fill_color = Color::Invalid) {
    DrawPolygonImpl(points, line_style, fill_color);
  }


  /// Draws a rectangle.
  ///
  /// Args:
  ///   rect: The :class:`~viren2d.Rect` which should be drawn.
  ///   line_style: A :class:`~viren2d.LineStyle` specifying how
  ///     to draw the rectangle's outline.
  ///
  ///     If you pass :attr:`viren2d.LineStyle.Invalid`, the
  ///     contour will not be drawn - then, you must provide
  ///     a valid ``fill_color``.
  ///   fill_color: If you provide a valid :class:`~viren2d.Color`,
  ///     the rectangle will be filled.
  void DrawRect(
      const Rect &rect,
      const LineStyle &line_style = LineStyle(),
      const Color &fill_color = Color::Invalid) {
    DrawRectImpl(rect, line_style, fill_color);
  }


  /// Draws single- or multi-line text.
  /// See documentation of `DrawTextBox` for details on the parameters.
  Rect DrawText(
      const std::vector<std::string> &text,
      const Vec2d &position,
      Anchor anchor = Anchor::BottomLeft,
      const TextStyle &text_style = TextStyle(),
      const Vec2d &padding = {0.0, 0.0},
      double rotation = 0.0) {
    return DrawTextImpl(
          text, position, anchor,
          text_style, padding, rotation);
  }


  /// Draws a single- or multi-line text box.
  ///
  /// Args:
  ///   text: The text lines to be drawn.
  ///   position: Position of the reference point where
  ///     to anchor the text.
  ///   anchor: How to orient the text with respect to ``position``.
  ///   text_style: How to render the text.
  ///   padding: Optional padding between text and the edges
  ///     of the box. Specified in pixels.
  ///   rotation: Rotation angle (clockwise around ``position``) in
  ///     degrees.
  ///   line_style: How to render the border of the text box.
  ///   fill_color: If you provide a valid color, the box will be filled.
  ///   radius: Corner radius of the box. Refer to Rect::radius for
  ///     details on valid value ranges.
  ///   fixed_size: Forces the box to be of the specified size. If the
  ///     size is smaller than the text extent, the text will overflow.
  ///
  /// Returns:
  ///   The bounding box of the drawn text.
  Rect DrawTextBox(
      const std::vector<std::string> &text,
      const Vec2d &position,
      Anchor anchor = Anchor::BottomLeft,
      const TextStyle &text_style = TextStyle(),
      const Vec2d &padding = {6.0, 6.0},
      double rotation = 0.0,
      const LineStyle &box_line_style = LineStyle::Invalid,
      const Color &box_fill_color = Color::White.WithAlpha(0.6),
      double box_corner_radius = 0.2,
      const Vec2d &fixed_box_size = {-1.0, -1.0}) {
    return DrawTextBoxImpl(
          text, position, anchor, text_style,
          padding, rotation, box_line_style,
          box_fill_color, box_corner_radius, fixed_box_size);
  }


  /// Draws a single trajectory.
  /// Can be used to either draw **a fixed-color path** (if
  /// ``color_fade_out`` is invalid), or **a path which gradually
  /// changes its color** from ``line_style.color`` to ``color_fade_out``.
  /// In the latter case, the color transition can be controlled
  /// by ``mix_factor``.
  ///
  /// Note:
  ///   If a valid ``color_fade_out`` is provided, the trajectory
  ///   has to be drawn via separate line segments. This means that the
  ///   `style.join` setting will have no effect. Additionally, if transparent
  ///   colors are used, the individual segment endpoints will be visible.
  ///
  ///   To avoid this behavior, the trajectory needs to be drawn with
  ///   a single color, *i.e.* pass `Color::Invalid` as ``color_fade_out``.
  bool DrawTrajectory(
      const std::vector<Vec2d> &points,
      const LineStyle &style = LineStyle(),
      const Color &color_fade_out = Color::White.WithAlpha(0.4),
      bool oldest_position_first = false,
      int smoothing_window = 0,
      const std::function<double(double)> &mix_factor = ColorFadeOutQuadratic) {
    return DrawTrajectoryImpl(
          points, style, color_fade_out, oldest_position_first,
          smoothing_window, mix_factor);
  }


  /// Draws multiple (similarly styled) trajectories.
  /// If a trajectory's color is Color::Invalid or Color::Same, the
  /// style's color will be used instead.
  bool DrawTrajectories(
      const std::vector<std::pair<std::vector<Vec2d>, Color>> &trajectories,
      const LineStyle &style = LineStyle(),
      const Color &color_fade_out = Color::White.WithAlpha(0.4),
      bool oldest_position_first = false,
      int smoothing_window = 0,
      const std::function<double(double)> &mix_factor = ColorFadeOutQuadratic) {
    return DrawTrajectoriesImpl(
          trajectories, style, color_fade_out, oldest_position_first,
          smoothing_window, mix_factor);
  }


  /// Draws the coordinate system axes for the pinhole camera calibration.
  ///
  /// Args:
  ///   K: The `3 x 3` camera matrix of type double, which holds the intrinsic
  ///     parameters.
  ///   R: The `3 x 3` extrinsic rotation matrix, again of double precision.
  ///   t: The 3d extrinsic translation vector.
  ///   origin: Center of the world coordinate system.
  ///   lengths: Specifies how far to shift the arrow tips from the origin.
  ///     Each axis tip will be computed as
  ///     :math:`\text{tip}_{\text{axis}} = \text{origin} + \mathbf{e}_{\text{axis}} * \text{lengths}[\text{axis}]`,
  ///     where :math:`\mathbf{e}_{\text{axis}}` is the unit vector for the
  ///     corresponding axis.
  ///     The default value assumes that the calibration is given in
  ///     millimeters and will result in 1 meter long arrows.
  ///   arrow_style: How the axis arrows should be drawn. Note that it's color
  ///     attribute will be ignored.
  ///   color_x: Color of the *x* axis arrow. Default reddish.
  ///   color_y: Color of the *y* axis arrow. Default greenish.
  ///   color_z: Color of the *z* axis arrow. Default bluish.
  ///
  /// Returns:
  ///   True if at least one point (axis arrow tip or the origin) is
  ///   visible within the camera's field-of-view
  bool DrawXYZAxes(
      const Matrix3x3d &K, const Matrix3x3d &R, const Vec3d &t,
      const Vec3d &origin = Vec3d::All(0.0),
      const Vec3d &axes_lengths = Vec3d::All(1e3),
      const ArrowStyle &style = ArrowStyle(),
      const Color &color_x = Color::CoordinateAxisColor('x'),
      const Color &color_y = Color::CoordinateAxisColor('y'),
      const Color &color_z = Color::CoordinateAxisColor('z')) {
    return DrawXYZAxesImpl(
          K, R, t, origin, axes_lengths, style, color_x, color_y, color_z);
  }


protected:
  /// Internal helper to enable default values in public interface.
  virtual bool DrawArcImpl(
      const Vec2d &center, double radius,
      double angle1, double angle2,
      const LineStyle &line_style,
      bool include_center, const Color &fill_color) = 0;


  /// Internal helper to enable default values in public interface.
  virtual bool DrawArrowImpl(
      const Vec2d &from, const Vec2d &to,
      const ArrowStyle &arrow_style) = 0;


  /// Internal helper to enable default values in public interface.
  virtual bool DrawBoundingBox2DImpl(
      const Rect &box,
      const std::vector<std::string> &label,
      const BoundingBox2DStyle &style) = 0;


  /// Internal helper to enable default values in public interface.
  virtual bool DrawCircleImpl(
      const Vec2d &center, double radius,
      const LineStyle &line_style,
      const Color &fill_color) = 0;


  /// Internal helper to enable default values in public interface.
  virtual bool DrawEllipseImpl(
      const Ellipse &ellipse, const LineStyle &line_style,
      const Color &fill_color) = 0;


  /// Internal helper to enable default values in public interface.
  virtual bool DrawGridImpl(
      const Vec2d &top_left, const Vec2d &bottom_right,
      double spacing_x, double spacing_y,
      const LineStyle &line_style) = 0;


  /// Internal helper to enable default values in public interface.
  virtual Line2d DrawHorizonLineImpl(
      const Matrix3x3d &K, const Matrix3x3d &R, const Vec3d &t,
      const LineStyle &line_style) = 0;


  /// Internal helper to enable default values in public interface.
  virtual bool DrawImageImpl(
      const ImageBuffer &image,
      const Vec2d &position, Anchor anchor,
      double alpha, double scale_x, double scale_y,
      double rotation, double clip_factor, const LineStyle &line_style) = 0;


  /// Internal helper to enable default values in public interface.
  virtual void DrawLineImpl(
      const Vec2d &from, const Vec2d &to,
      const LineStyle &line_style) = 0;


  /// Internal helper to enable default values in public interface.
  virtual void DrawMarkerImpl(
      const Vec2d &pos, const MarkerStyle &style) = 0;


  /// Internal helper to enable default values in public interface.
  virtual void DrawMarkersImpl(
      const std::vector<std::pair<Vec2d, Color>> &markers,
      const MarkerStyle &style) = 0;


  /// Internal helper to enable default values in public interface.
  virtual void DrawPolygonImpl(
      const std::vector<Vec2d> &points,
      const LineStyle &line_style,
      const Color &fill_color) = 0;


  /// Internal helper to enable default values in public interface.
  virtual void DrawRectImpl(
      const Rect &rect, const LineStyle &line_style,
      const Color &fill_color) = 0;


  /// Internal helper to allow default values in public interface.
  virtual Rect DrawTextImpl(
      const std::vector<std::string> &text,
      const Vec2d &position, Anchor text_anchor,
      const TextStyle &text_style,
      const Vec2d &padding, double rotation) = 0;


  /// Internal helper to allow default values in public interface.
  virtual Rect DrawTextBoxImpl(
      const std::vector<std::string> &text,
      const Vec2d &position, Anchor text_anchor,
      const TextStyle &text_style, const Vec2d &padding,
      double rotation, const LineStyle &box_line_style,
      const Color &box_fill_color, double box_corner_radius,
      const Vec2d &fixed_box_size) = 0;


  /// Internal helper to allow default values in public interface.
  virtual bool DrawTrajectoryImpl(
      const std::vector<Vec2d> &points, const LineStyle &style,
      const Color &color_fade_out, bool oldest_position_first,
      int smoothing_window,
      const std::function<double(double)> &mix_factor) = 0;


  /// Internal helper to allow default values in public interface.
  virtual bool DrawTrajectoriesImpl(
      const std::vector<std::pair<std::vector<Vec2d>, Color>> &trajectories,
      const LineStyle &style, const Color &color_fade_out,
      bool oldest_position_first, int smoothing_window,
      const std::function<double(double)> &mix_factor) = 0;


  /// Internal helper to allow default values in public interface.
  virtual bool DrawXYZAxesImpl(
      const Matrix3x3d &K, const Matrix3x3d &R, const Vec3d &t,
      const Vec3d &origin, const Vec3d &axes_lengths, const ArrowStyle &style,
      const Color &color_x, const Color &color_y, const Color &color_z) = 0;
};


/// Creates a Painter object for drawing.
std::unique_ptr<Painter> CreatePainter();

} // namespace viren2d

#endif // __VIREN2D_DRAWING_H__
