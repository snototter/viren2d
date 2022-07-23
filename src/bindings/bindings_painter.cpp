#include <memory>
#include <tuple>
#include <sstream>
#include <stdexcept>

#include <viren2d/drawing.h>

#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include <bindings/binding_helpers.h>


namespace py = pybind11;

namespace viren2d {
namespace bindings {
Anchor AnchorFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return AnchorFromString(py::cast<std::string>(o));
  } else if (py::isinstance<Anchor>(o)) {
    return py::cast<Anchor>(o);
  } else {
    std::string s("Cannot cast type `");
    s += py::cast<std::string>(
          o.attr("__class__").attr("__name__"));
    s += "` to `viren2d.Anchor`!";
    throw std::invalid_argument(s);
  }
}


/// A wrapper for the abstract `Painter`
///
/// This is necessary because I don't want to expose
/// the `ImagePainter` - I like the current factory method
/// layout because the public headers are quite clean.
/// Thus, I cannot (read "don't want to") use pybind11's
/// trampoline mechanism.
class PainterWrapper {
public:
  PainterWrapper() : painter_(CreatePainter())
  {}


  PainterWrapper(const ImageBuffer &image)
    : painter_(CreatePainter()) {
    SetCanvasImage(image);
  }


  PainterWrapper(int height, int width, const Color &color)
    : painter_(CreatePainter()) {
    SetCanvasColor(height, width, color);
  }


  void SetCanvasColor(int height, int width, const Color &color) {
    painter_->SetCanvas(height, width, color);
  }


  void SetCanvasFilename(const std::string &image_filename) {
    painter_->SetCanvas(image_filename);
  }


  void SetCanvasImage(const ImageBuffer &image) {
    painter_->SetCanvas(image);
  }


  ImageBuffer GetCanvas(bool copy) {
    return painter_->GetCanvas(copy);
  }


  py::tuple GetCanvasSize() {
    auto sz = painter_->GetCanvasSize();
    return py::make_tuple(sz.x(), sz.y());
  }


  bool IsValid() {
    return (painter_ != nullptr) && (painter_->IsValid());
  }


  void DrawArc(
      const Vec2d &center, double radius, double angle1, double angle2,
      const LineStyle &line_style, bool include_center, const Color &fill_color) {
    painter_->DrawArc(
          center, radius, angle1, angle2, line_style,
          include_center, fill_color);
  }


  void DrawArrow(
      const Vec2d &from, const Vec2d &to, const ArrowStyle &arrow_style) {
    painter_->DrawArrow(from, to, arrow_style);
  }


  void DrawBoundingBox2D(
      const Rect &box, const std::vector<std::string> &label,
      const BoundingBox2DStyle &style) {
    painter_->DrawBoundingBox2D(box, label, style);
  }


  void DrawCircle(
      const Vec2d &center, double radius,
      const LineStyle &line_style, const Color &fill_color) {
    painter_->DrawCircle(center, radius, line_style, fill_color);
  }


  void DrawEllipse(
      const Ellipse &ellipse, const LineStyle &line_style,
      const Color &fill_color) {
    painter_->DrawEllipse(ellipse, line_style, fill_color);
  }


  void DrawGrid(
      double spacing_x, double spacing_y,
      const LineStyle &line_style,
      const Vec2d &top_left,
      const Vec2d &bottom_right) {
    painter_->DrawGrid(
          top_left, bottom_right,
          spacing_x, spacing_y, line_style);
  }


  void DrawImage(
      ImageBuffer &image, const Vec2d &position,
      const py::object &anchor, double alpha, double scale_x, double scale_y,
      double rotation, double clip_factor, const LineStyle &line_style) {
    Anchor a = AnchorFromPyObject(anchor);
    painter_->DrawImage(
          image, position, a, alpha,
          scale_x, scale_y, rotation, clip_factor, line_style);
  }


  void DrawLine(
      const Vec2d &from, const Vec2d &to,
      const LineStyle &line_style) {
    painter_->DrawLine(from, to, line_style);
  }


  void DrawMarker(const Vec2d &pos, const MarkerStyle &style) {
    painter_->DrawMarker(pos, style);
  }


  void DrawMarkers(
      const std::vector<std::pair<Vec2d, Color>> &markers,
      const MarkerStyle &style) {
    painter_->DrawMarkers(markers, style);
  }


  void DrawPolygon(
      const std::vector<Vec2d> &polygon, const LineStyle &line_style,
      const Color &fill_color) {
    painter_->DrawPolygon(polygon, line_style, fill_color);
  }


  void DrawRect(
      const Rect &rect, const LineStyle &line_style,
      const Color &fill_color) {
    painter_->DrawRect(rect, line_style, fill_color);
  }


  Rect DrawText(
      const std::vector<std::string> &text,
      const Vec2d &position, const py::object &pyanchor,
      const TextStyle &text_style, const Vec2d &padding,
      double rotation) {
    Anchor anchor = AnchorFromPyObject(pyanchor);
    return painter_->DrawText(
          text, position, anchor,
          text_style, padding, rotation);
  }


  Rect DrawTextBox(
      const std::vector<std::string> &text,
      const Vec2d &position, const py::object &pyanchor,
      const TextStyle &text_style, const Vec2d &padding,
      double rotation, const LineStyle &box_line_style,
      const Color &box_fill_color, double box_corner_radius,
      const Vec2d &fixed_box_size) {
    Anchor anchor = AnchorFromPyObject(pyanchor);
    return painter_->DrawTextBox(
          text, position, anchor, text_style,
          padding, rotation, box_line_style, box_fill_color,
          box_corner_radius, fixed_box_size);
  }


  void DrawTrajectory(
      const std::vector<Vec2d> &trajectory, const LineStyle &style,
      const Color &color_fade_out, bool oldest_position_first,
      int smoothing_window,
      const std::function<double(double)> &fading_factor) {
    painter_->DrawTrajectory(
          trajectory, style, color_fade_out, oldest_position_first,
          smoothing_window, fading_factor);
  }


  void DrawTrajectories(
      const std::vector<std::pair<std::vector<Vec2d>, Color>> &trajectories,
      const LineStyle &style, const Color &color_fade_out,
      bool oldest_position_first, int smoothing_window,
      const std::function<double(double)> &fading_factor) {
    painter_->DrawTrajectories(
          trajectories, style, color_fade_out, oldest_position_first,
          smoothing_window, fading_factor);
  }


  bool DrawXYZAxes(
      const Matrix3x3d &K, const Matrix3x3d &R, const Vec3d &t,
      const Vec3d &origin, const Vec3d &axes_lengths,
      const ArrowStyle &style, const Color &color_x,
      const Color &color_y, const Color &color_z) {
    return painter_->DrawXYZAxes(
          K, R, t, origin, axes_lengths, style, color_x, color_y, color_z);
  }


  /// String representation used to bind __str__ and __repr__.
  std::string StringRepresentation(bool tag) const {
    std::ostringstream s;
    if (tag) {
      s << '<';
    }

    s << "viren2d.Painter(";

    if (painter_->IsValid()) {
      auto size = painter_->GetCanvasSize();
      s << size.width() << 'x' << size.height();
    } else {
      s << "canvas not initialized";
    }

    s << ')';

    if (tag) {
      s << '>';
    }
    return s.str();
  }


private:
  std::unique_ptr<Painter> painter_;
};


void RegisterPainter(py::module &m) {
  py::class_<PainterWrapper> painter(m, "Painter", R"docstr(
        A *Painter* lets you draw on its canvas.

        Typical workflow:

        1. Create a painter with an empty canvas:

           >>> import viren2d
           >>> painter = viren2d.Painter()

        2. Initialize its canvas:

           * Paint an empty canvas with a given color
             via :meth:`set_canvas_rgb`
           * Set up the canvas from image data
             via :meth:`set_canvas_image`
           * Set up the canvas by loading an image from disk
             via :meth:`set_canvas_filename`

           Note that the overloaded *Painter* constructors allow
           combining these steps:

           >>> # Set up from image data, e.g. `numpy.ndarray`:
           >>> image = np.zeros((600, 800, 3), dtype=np.uint8)
           >>> painter = viren2d.Painter(image)
           >>>
           >>> # Or create a custom, blue canvas:
           >>> painter = viren2d.Painter(
           >>>     width=1920, height=1080, color=(0.0, 0.0, 0.8))

        3. Draw onto the canvas via the painter's ``draw_xxx(...)``
           methods, for example:

           >>> painter.draw_arrow(...)
           >>> painter.draw_bounding_box(...)

        4. After all objects have been drawn, retrieve the
           visualization via :meth:`get_canvas`. For example,
           to get a deeply copied image as :class:`numpy.ndarray`:

           >>> import numpy as np
           >>> canvas = painter.get_canvas(copy=True)
           >>> img_np = np.array(canvas, copy=False)

           Alternatively, if access via properties is preferred,
           a *shared memory* image buffer can also be via its
           :attr:`~viren2d.Painter.canvas` attribute. To obtain
           a deeply copied image as before, we can leverage the
           :class:`numpy.ndarray` constructor:

           >>> img_np = np.array(painter.canvas, copy=True)

        5. Either continue drawing (step 3) or set up a new
           canvas (step 2), *i.e.* it is safe to reuse the
           same painter instance.
        )docstr");

  painter.def(
        py::init<>(), R"docstr(
        Default constructor.

        Initializes an empty canvas, *i.e.* :meth:`~viren2d.Painter.is_valid`
        will return ``False`` until the canvas has been properly set up
        via :meth:`~viren2d.Painter.set_canvas_image`, *etc.*
        )docstr")
      .def(
        py::init<const ImageBuffer&>(), R"docstr(
        Creates a painter and initializes its canvas from an image.

        Initializes the painter's canvas with the given image.
        See :meth:`~viren2d.Painter.set_canvas_image` for supported
        image formats and parameter types.
        )docstr",
        py::arg("image"))
      .def(
        py::init<int, int, Color>(), R"docstr(
        Creates a painter with a customized canvas.

        Initializes the painter's canvas and fills it
        with the given :class:`~viren2d.Color`.
        )docstr",
        py::arg("height"), py::arg("width"),
        py::arg("color") = Color::White)
      .def(
        "__repr__",
        [](const PainterWrapper &p) { return p.StringRepresentation(true); })
      .def(
        "__str__",
        [](const PainterWrapper &p) { return p.StringRepresentation(false); })
      .def(
        "is_valid",
        &PainterWrapper::IsValid, R"docstr(
        Checks if the canvas has been set up correctly.

        **Corresponding C++ API:** ``viren2d::Painter::IsValid``.
        )docstr");


  painter.def(
        "set_canvas_rgb",
        &PainterWrapper::SetCanvasColor, R"docstr(
        Initializes the canvas with the given color.

        **Corresponding C++ API:** ``viren2d::Painter::SetCanvas``.

        Args:
          width: Canvas width in pixels.
          height: Canvas height in pixels.
          color: Background :class:`~viren2d.Color`.

        Examples:
          >>> painter = viren2d.Painter()
          >>> painter.set_canvas_rgb(height=600, width=800)
          >>> painter.set_canvas_rgb(width=800, height=600, color='crimson')
          >>> painter.set_canvas_rgb(width=800, height=600, color=(0.5, 0.3, 0.9))
        )docstr",
        py::arg("height"), py::arg("width"),
        py::arg("color") = Color::White);


  painter.def(
        "set_canvas_filename",
        &PainterWrapper::SetCanvasFilename, R"docstr(
        Initializes the canvas from the given image file.

        This functionality uses the
        `stb library <https://github.com/nothings/stb/blob/master/stb_image.h>`__
        to load the image file. Supported formats are:

           JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM

        **Corresponding C++ API:** ``viren2d::Painter::SetCanvas``.
        )docstr",
        py::arg("image_filename"));


  painter.def(
        "set_canvas_image",
        &PainterWrapper::SetCanvasImage, R"docstr(
        Initializes the canvas from the given image.

        **Corresponding C++ API:** ``viren2d::Painter::SetCanvas``.

        Args:
          img_np: Image as either a :class:`numpy.ndarray` (currently,
            only :class:`numpy.uint8` is supported) or an :class:`~viren2d.ImageBuffer`.
            The image can either be grayscale, RGB or RGBA.

        Example:
          >>> img_np = np.zeros((480, 640, 3), dtype=np.uint8)
          >>> painter.set_canvas_image(img_np)
        )docstr",
        py::arg("image"));


  //----------------------------------------------------------------------
  painter.def(
        "get_canvas_size",
        &PainterWrapper::GetCanvasSize, R"docstr(
        Returns the size of the canvas in pixels.

        **Corresponding C++ API:** ``viren2d::Painter::GetCanvasSize``.

        Returns:
          The canvas width & height as the :class:`tuple` ``(W, H)``,
          where ``W`` and ``H`` denote pixels (type :class:`int`).
        )docstr");

  painter.def_property_readonly(
        "width",
        [](PainterWrapper &pw) -> py::int_ {
          return pw.GetCanvasSize()[0];
        }, R"docstr(
        int: Width in pixels of the painter's canvas (read-only).

          **Corresponding C++ API:** ``viren2d::Painter::GetCanvasSize``.
        )docstr");


  painter.def_property_readonly(
        "height",
        [](PainterWrapper &pw) -> py::int_ {
          return pw.GetCanvasSize()[1];
        }, R"docstr(
        int: Height in pixels of the painter's canvas (read-only).

          **Corresponding C++ API:** ``viren2d::Painter::GetCanvasSize``.
        )docstr");


  painter.def_property_readonly(
        "canvas",
        [](PainterWrapper &pw) -> ImageBuffer { return pw.GetCanvas(false); }, R"docstr(
        :class:`~viren2d.ImageBuffer`: Provides a **shared memory** view
          on the painter's canvas for convenience.

          See :meth:`~viren2d.Painter.get_canvas` for details about the
          image format of the canvas. Can be used to convert the current
          visualization into a :class:`numpy.ndarray` via:

          >>> img_np = np.array(painter.canvas, copy=True)

          Although this property is read-only (you cannot assign a newly
          allocated memory), it could still be used to change the individual
          pixels of the canvas (as these are not write-protected):

          >>> img_np = np.array(painter.canvas, copy=False)
          >>> img_np[:, :, :3] = 0  # Now, the canvas is black

          **Corresponding C++ API:** ``viren2d::Painter::GetCanvas``.
        )docstr");


  //----------------------------------------------------------------------
  painter.def(
        "get_canvas",
        &PainterWrapper::GetCanvas, R"docstr(
        Returns the current visualization in RGBA format.

        Returns an :class:`~viren2d.ImageBuffer`, which implements
        the Python buffer protocol. This means, it can be easily
        converted to other buffer types, such as :class:`numpy.ndarray`,
        see the examples below.

        **Corresponding C++ API:** ``viren2d::Painter::GetCanvas``.

        Args:
          copy: If you want a **deep copy**, set ``copy = True``.
            Otherwise, the buffer will just provide a **shared
            view** on the painter's canvas.

            Be aware that if you keep on drawing after obtaining
            a shared view, this view will also change. You could
            even externally modify the canvas pixels.

        Returns:
          The current visualization as a 4-channel, ``uint8``
          :class:`~viren2d.ImageBuffer` with pixel format **RGBA**.

        Examples:
          Get canvas as :class:`numpy.ndarray`, where the **memory is
          shared** with the painter:

          >>> img_np = np.array(p.get_canvas(copy=False), copy=False)

          Retrieve a **deep copy** of the canvas as :class:`numpy.ndarray`,
          *i.e.* future ``painter.draw_...`` calls will not affect this
          retrieved copy:

          >>> img_np = np.array(p.get_canvas(copy=True), copy=False)

          If we need a 3-channel image, we can leverage the
          :meth:`~viren2d.ImageBuffer.to_rgb` method of the buffer:

          >>> # We only need a shared view on the canvas...
          >>> img_buf = p.get_canvas(copy=False)
          >>> # ... because the following call copies memory anyways:
          >>> img_np = img_buf.to_rgb()

        .. tip::
            If you can ensure that the painter is not destroyed while
            you display/process the visualization, use the shared view
            (*i.e.* ``copy = False``) on its canvas to avoid unnecessary
            memory allocation.
        )docstr", py::arg("copy") = true);


  //----------------------------------------------------------------------
  painter.def(
        "draw_arc",
        &PainterWrapper::DrawArc, R"docstr(
        Draws a circular arc.

        **Corresponding C++ API:** ``viren2d::Painter::DrawArc``.

        Args:
          center: Center position as :class:`~viren2d.Vec2d`.
          radius: Radius of the arc in pixels as :class:`float`.
          angle_from: The arc will be drawn from ``angle_from``
            to ``angle_to`` in clockwise direction. Both angles
            are specified in degrees as :class:`float`, where 0
            degrees points in the direction of increasing *x* coordinates.
          angle_to: See ``angle_from``.
          line_style: A :class:`~viren2d.LineStyle` specifying how
            to draw the arc's outline.

            If you pass :attr:`LineStyle.Invalid`, the
            contour will not be drawn - then, you must provide a
            valid ``fill_color``.
          include_center:  If ``True`` (default), the center point
            will be included when drawing the outline and filling.
          fill_color: If you provide a valid :class:`~viren2d.Color`,
            the arc will be filled.

        Example:
          >>> line_style = viren2d.LineStyle(
          >>>     width=5, color='maroon',
          >>>     dash_pattern=[], dash_offset=0.0,
          >>>     cap='round', join='miter')
          >>> painter.draw_arc(
          >>>     center=(50, 50), radius=20, angle_from=30, angle_to=330,
          >>>     line_style=line_style, include_center=True,
          >>>     fill_color='same!30')
        )docstr",
        py::arg("center"),
        py::arg("radius"),
        py::arg("angle_from"),
        py::arg("angle_to"),
        py::arg("line_style") = LineStyle(),
        py::arg("include_center") = true,
        py::arg("fill_color") = Color::Invalid);


  //----------------------------------------------------------------------
  ArrowStyle default_arrow_style;
  painter.def(
        "draw_arrow",
        &PainterWrapper::DrawArrow, R"docstr(
        Draws an arrow.

        **Corresponding C++ API:** ``viren2d::Painter::DrawArrow``.

        Args:
          pt1: Start of the arrow shaft as :class:`~viren2d.Vec2d`.
          pt2: End of the arrow shaft (*i.e.* the *pointy end*) as
            :class:`~viren2d.Vec2d`.
          arrow_style: An :class:`~viren2d.ArrowStyle` specifying
            how to draw the arrow.

        Example:
          >>> arrow_style = viren2d.ArrowStyle(
          >>>     width=3, color='black',
          >>>     tip_length=0.3, tip_angle=20,
          >>>     tip_closed=True, double_headed=False,
          >>>     dash_pattern=[], dash_offset=0.0,
          >>>     cap='round', join='miter')
          >>> painter.draw_arrow(
          >>>     pt1=(10, 10), pt2=(42, 42), arrow_style=arrow_style)

        Note:
          Arrows should always be drawn **fully opaque**. Otherwise,
          you'll experience visible blending in the crossing path
          segments (*i.e.* at the tip).
        )docstr",
        py::arg("pt1"),
        py::arg("pt2"),
        py::arg("arrow_style") = default_arrow_style);


  //----------------------------------------------------------------------
  painter.def(
        "draw_bounding_box_2d",
        &PainterWrapper::DrawBoundingBox2D, R"docstr(
        Draws a single 2D bounding box.

        **Corresponding C++ API:** ``viren2d::Painter::DrawBoundingBox2D``.

        Args:
          rect: The box geometry as :class:`~viren2d.Rect`.
          label: The label as :class:`list` of :class:`str`, since multi-line
            labels are supported.
          box_style: A :class:`~viren2d.BoundingBox2DStyle` specifying how
            to draw this bounding box.

        Example:
          >>> #TODO
        )docstr",
        py::arg("rect"),
        py::arg("label"),
        py::arg("box_style") = BoundingBox2DStyle());

  //TODO multiple 2d bounding boxes


  //----------------------------------------------------------------------
  painter.def(
        "draw_circle",
        &PainterWrapper::DrawCircle, R"docstr(
        Draws a circle.

        **Corresponding C++ API:** ``viren2d::Painter::DrawCircle``.

        Args:
          center: Center position as :class:`~viren2d.Vec2d`.
          radius: Radius of the circle in pixels as :class:`float`.
          line_style: A :class:`~viren2d.LineStyle` specifying how
            to draw the circle's outline.

            If you pass :attr:`LineStyle.Invalid`, the
            contour will not be drawn - then, you must provide a
            valid ``fill_color``.
          fill_color: If you provide a valid :class:`~viren2d.Color`,
            the circle will be filled.

        Example:
          >>> line_style = viren2d.LineStyle(
          >>>     width=5, color='maroon',
          >>>     dash_pattern=[], dash_offset=0.0,
          >>>     cap='round', join='miter')
          >>> painter.draw_circle(
          >>>     center=(50, 50), radius=30,
          >>>     line_style=line_style, fill_color='same!30')
        )docstr",
        py::arg("center"),
        py::arg("radius"),
        py::arg("line_style") = LineStyle(),
        py::arg("fill_color") = Color::Invalid);


  //----------------------------------------------------------------------
  painter.def(
        "draw_ellipse",
        &PainterWrapper::DrawEllipse, R"docstr(
        Draws an ellipse.

        **Corresponding C++ API:** ``viren2d::Painter::DrawEllipse``.

        Args:
          ellipse: The :class:`~viren2d.Ellipse`, which should
            be drawn.
          line_style: A :class:`~viren2d.LineStyle` specifying
            how to draw the ellipse's outline.

            If you pass :attr:`LineStyle.Invalid`, the
            contour will not be drawn - then, you must provide a
            valid ``fill_color``.
          fill_color: If you provide a valid :class:`~viren2d.Color`,
            the ellipse will be filled.

        Example:
          >>> line_style = viren2d.LineStyle(
          >>>     width=3, color='forest-green',
          >>>     dash_pattern=[], dash_offset=0.0,
          >>>     cap='round', join='miter')
          >>> ellipse = viren2d.Ellipse(
          >>>     center=(100, 60), axes=(180, 50), rotation=60)
          >>> painter.draw_ellipse(ellipse, line_style, 'same!20')
          >>> # Or via named arguments:
          >>> painter.draw_ellipse(
          >>>     ellipse=ellipse, line_style=line_style,
          >>>     fill_color='same!20')
        )docstr",
        py::arg("ellipse"),
        py::arg("line_style") = LineStyle(),
        py::arg("fill_color") = Color::Invalid);


  //----------------------------------------------------------------------
  painter.def(
        "draw_grid",
        &PainterWrapper::DrawGrid, R"docstr(
        Draws a grid.

        **Corresponding C++ API:** ``viren2d::Painter::DrawGrid``.

        Args:
          spacing_x: Width of each grid cell as :class:`float`.
          spacing_y: Height of each grid cell as :class:`float`.
          line_style: A :class:`~viren2d.LineStyle` specifying how
            to render the grid lines.
          top_left: Top-left corner as :class:`~viren2d.Vec2d`. If
            provided and ``top_left != `bottom_right``, the grid will
            only be drawn within this rectangular region. Otherwise,
            the grid will span the whole canvas.
          bottom_right: Bottom-right corner as :class:`~viren2d.Vec2d`.
            See ``top_left``.

        Example:
          >>> line_style = viren2d.LineStyle(width=1, color='light-gray!80')
          >>> painter.draw_grid(
          >>>     spacing_x=50, spacing_y=50, line_style=line_style,
          >>>     top_left=(50, 50), bottom_right=(150, 150))
        )docstr",
        py::arg("spacing_x"),
        py::arg("spacing_y"),
        py::arg("line_style") = LineStyle(),
        py::arg("top_left") = Vec2d(),
        py::arg("bottom_right") = Vec2d());


  //----------------------------------------------------------------------
  painter.def(
        "draw_image",
        &PainterWrapper::DrawImage, R"docstr(
        Draws an image onto the canvas.

        **Corresponding C++ API:** ``viren2d::Painter::DrawImage``.

        Args:
          image: The image as :class:`~viren2d.ImageBuffer`. Note that a
            :class:`numpy.ndarray` will be implicitly converted if it is
            C-contiguous - which can be verified via its
            :attr:`numpy.ndarray.flags`).
          position: The position of the reference point where
            to anchor the image as :class:`~viren2d.Vec2d`.
          anchor: How to orient the text with respect to ``position``.
            Valid inputs are :class:`~viren2d.Anchor` values
            and their string representations. For details, refer to the
            ``anchor`` parameter of :meth:`~viren2d.Painter.draw_text`.
          alpha: Opacity as :class:`float` :math:`\in [0,1]`, where ``1`` is fully
            opaque and ``0`` is fully transparent.
          scale_x: Horizontal scaling factor as :class:`float`.
          scale_y: Vertical scaling factor as :class:`float`.
          rotation: Clockwise rotation in degrees as :class:`float`.
          clip_factor: If greater than 0, the corners will be clipped. In
            particular, :math:`0 < \text{clip} \leq 0.5` will result in a
            rounded rectangle, where the corner radius will be ``clip_factor``
            times :math:`\min(\text{width}, \text{height})`.
            If :math:`\text{clip} > 0.5`, the clip region will be an ellipse,
            where major/minor axis length equal the width/height of the image.
          line_style: A :class:`~viren2d.LineStyle` specifying how to draw the
            contour. If set to :attr:`LineStyle.Invalid`, the contour will not
            be drawn.

        Example:
          >>> painter.draw_image(
          >>>     image=img, position=(10, 20), anchor='top-left',
          >>>     alpha=0.8, scale_x=1.0, scale_y=1.0, rotation=0,
          >>>     clip_factor=0.3, line_style=viren2d.LineStyle.Invalid)
        )docstr",
        py::arg("image"),
        py::arg("position"),
        py::arg("anchor") = Anchor::TopLeft,
        py::arg("alpha") = 1.0,
        py::arg("scale_x") = 1.0,
        py::arg("scale_y") = 1.0,
        py::arg("rotation") = 0.0,
        py::arg("clip_factor") = 0.0,
        py::arg("line_style") = LineStyle::Invalid);


  //----------------------------------------------------------------------
  painter.def(
        "draw_line",
        &PainterWrapper::DrawLine, R"docstr(
        Draws a line.

        **Corresponding C++ API:** ``viren2d::Painter::DrawLine``.

        Args:
          pt1: Start position as :class:`~viren2d.Vec2d`.
          pt2: End position as :class:`~viren2d.Vec2d`.
          line_style: A :class:`~viren2d.LineStyle` specifying
            how to draw the line.

        Example:
          >>> line_style = viren2d.LineStyle(
          >>>     width=7, color='crimson!80',
          >>>     dash_pattern=[20, 10], dash_offset=0.0,
          >>>     cap='round', join='miter')
          >>> painter.draw_line(
          >>>     pt1=(42, 42), pt2=(86, 86), line_style=line_style)
        )docstr",
        py::arg("pt1"),
        py::arg("pt2"),
        py::arg("line_style") = LineStyle());


  //----------------------------------------------------------------------
  painter.def(
        "draw_marker",
        &PainterWrapper::DrawMarker, R"docstr(
        Draws a single marker/keypoint.

        **Corresponding C++ API:** ``viren2d::Painter::DrawMarker``.

        Args:
          pos: Position as :class:`~viren2d.Vec2d`.
          marker_style: A :class:`~viren2d.MarkerStyle` specifying
            how to draw the marker.

        Example:
          >>> marker_style = viren2d.MarkerStyle(
          >>>     marker='7', size=20, color='navy-blue!80',
          >>>     thickness=1, filled=True,
          >>>     cap='round', join='miter')
          >>> painter.draw_marker(pt=(42, 70), marker_style=marker_style)
        )docstr",
        py::arg("pt"),
        py::arg("marker_style") = MarkerStyle());


  painter.def(
        "draw_markers",
        &PainterWrapper::DrawMarkers, R"docstr(
        Draws multiple (similarly styled) markers/keypoints.

        **Corresponding C++ API:** ``viren2d::Painter::DrawMarkers``.

        Args:
          markers: Holds the position and color of each marker.
            Should be provided as a :class:`list` of :class:`tuple`\ s, where
            each :class:`tuple` holds the position and color of a marker
            as (:class:`~viren2d.Vec2d`, :class:`~viren2d.Color`).
            If a marker's color is invalid, it will be drawn using
            ``marker_style``'s color specification instead.
          marker_style: A :class:`~viren2d.MarkerStyle` specifying
            how to draw the markers (except for the color).

        Example:
          >>> marker_style = viren2d.MarkerStyle(color='crimson')
          >>> markers = [
          >>>     ((10, 10), 'blue'),
          >>>     ((20, 10), (0.5, 0, 0.5)),
          >>>     ((30, 10), viren2d.RGBa(200, 0, 180)),
          >>>     ((40, 10), 'invalid'),   # Will use marker_style.color
          >>>     ((50, 34), (-1, -1, -1)) # This one too
          >>> ]
          >>> painter.draw_markers(markers, marker_style)
        )docstr",
        py::arg("markers"),
        py::arg("marker_style") = MarkerStyle());


  //----------------------------------------------------------------------
  painter.def(
        "draw_polygon",
        &PainterWrapper::DrawPolygon, R"docstr(
        Draws a polygon.

        **Corresponding C++ API:** ``viren2d::Painter::DrawPolygon``.

        Args:
          polygon: Points of the polygon as :class:`list` of
            :class:`~Vec2d`.
          line_style: A :class:`~viren2d.LineStyle` specifying how
            to draw the polygon's outline.

            If you pass :attr:`~viren2d.LineStyle.Invalid`, the
            contour will not be drawn - then, you must provide a
            valid ``fill_color``.
          fill_color: If you provide a valid :class:`~viren2d.Color`,
            the polygon will be filled.

        Example:
          >>> points = [(0, 0), (10, 20), (42, 30), ...]
          >>> line_style = viren2d.LineStyle(
          >>>     width=5, color='forest-green',
          >>>     dash_pattern=[], dash_offset=0.0,
          >>>     cap='round', join='round')
          >>> painter.draw_polygon(
          >>>     polygon=points, line_style=line_style,
          >>>     fill_color='same!40')
        )docstr",
        py::arg("polygon"),
        py::arg("line_style") = LineStyle(),
        py::arg("fill_color") = Color::Invalid);


  //----------------------------------------------------------------------
  painter.def(
        "draw_rect",
        &PainterWrapper::DrawRect, R"docstr(
        Draws a rectangle.

        **Corresponding C++ API:** ``viren2d::Painter::DrawRect``.

        Args:
          rect: The :class:`~viren2d.Rect` which should be drawn.
          line_style: A :class:`~viren2d.LineStyle` specifying how
            to draw the rectangle's outline.

            If you pass :attr:`viren2d.LineStyle.Invalid`, the
            contour will not be drawn - then, you must provide
            a valid ``fill_color``.
          fill_color: If you provide a valid :class:`~viren2d.Color`,
            the rectangle will be filled.

        Example:
          >>> line_style = viren2d.LineStyle()
          >>> painter.draw_rect(
          >>>   rect=rect, line_style=line_style, fill_color='same!20')
        )docstr",
        py::arg("rect"),
        py::arg("line_style") = LineStyle(),
        py::arg("fill_color") = Color::Invalid);


  //----------------------------------------------------------------------
  painter.def(
        "draw_text",
        &PainterWrapper::DrawText, R"docstr(
        Draws single- or multi-line text.

        **Corresponding C++ API:** ``viren2d::Painter::DrawText``.

        Args:
          text: A :class:`list` of :class:`str` to be drawn.
            For a single line, simply pass a :class:`list` which
            holds only a single :class:`str`.
          position: Position of the reference point where
            to anchor the text as :class:`~viren2d.Vec2d`.
          anchor: How to orient the text with respect to the ``position``.
            Valid inputs are :class:`~viren2d.Anchor` values
            and their string representations.

            A string must correspond either to a *position
            specification* - *i.e.* ``center``, ``top``, ``top-right``,
            ``right``, ``bottom-right``, ``bottom``, ``bottom-left``,
            ``left``, or ``top-left`` - or one of the 8 *compass
            directions* - *i.e.* ``north``, ``north-east``, ``east``,
            ``south-east``, ``south``, ``south-west``, ``west``,
            or ``north-west``.

            Before parsing, the input string will be converted to
            lowercase and any whitespaces, dashes & underscores will
            be removed.
          text_style: A :class:`~viren2d.TextStyle`, specifying
            how to render the text.
          padding: Optional distance between the closest glyph and the
            ``position``. Specified in pixels as :class:`~viren2d.Vec2d`.
          rotation: Rotation angle (clockwise around ``position``) in
            degrees as :class:`float`.

        Returns:
          The bounding box of the drawn text as :class:`~viren2d.Rect`.

        Example:
          >>> text_style = viren2d.TextStyle(
          >>>     family='sans-serif', size=15, color='#1a1c1d', bold=True)
          >>> bbox = painter.draw_text(
          >>>     text=['Hello World!'], position=(200, 50), anchor='center',
          >>>     text_style=text_style, padding=(5, 5), rotation=0)
        )docstr",
        py::arg("text"),
        py::arg("position"),
        py::arg("anchor") = Anchor::BottomLeft,
        py::arg("text_style") = TextStyle(),
        py::arg("padding") = Vec2d(0.0, 0.0),
        py::arg("rotation") = 0.0);



  //----------------------------------------------------------------------
  painter.def(
        "draw_text_box",
        &PainterWrapper::DrawTextBox, R"docstr(
        Draws a single- or multi-line text box.

        **Corresponding C++ API:** ``viren2d::Painter::DrawTextBox``.

        Args:
          text: A :class:`list` of :class:`str` to be drawn.
            For a single line, simply pass a :class:`list` which
            holds a single :class:`str`.
          position: Position of the reference point where
            to anchor the text as :class:`~viren2d.Vec2d`.
          anchor: How to orient the text with respect to ``position``.
            Valid inputs are :class:`~viren2d.Anchor` values
            and their string representations. For details, refer to the
            ``anchor`` parameter of :meth:`~viren2d.Painter.draw_text`.
          text_style: A :class:`~viren2d.TextStyle`, specifying
            how to render the text.
          padding: Optional padding between text and the edges
            of the box. Specified in pixels as :class:`~viren2d.Vec2d`.
          rotation: Rotation angle (clockwise around ``position``) in
            degrees as :class:`float`.
          line_style: A :class:`~viren2d.LineStyle`, specifying
            how to render the border of the text box.
          fill_color: If you provide a valid :class:`~viren2d.Color`,
            the box will be filled.
          radius: Corner radius of the box. Refer to
            :attr:`~viren2d.Rect.radius` for details on valid
            value ranges.
          fixed_size: Forces the box to be of the specified size as
            :class:`~viren2d.Vec2d`. If the size is smaller than the text
            extent, the text will overflow the box.

        Returns:
          The bounding box of the drawn text as :class:`~viren2d.Rect`.

        Example:
          >>> text_style = viren2d.TextStyle(
          >>>     family='sans-serif', size=15, color='#1a1c1d', bold=True)
          >>> line_style = viren2d.LineStyle(
          >>>     width=3, color='#1a1c1d')
          >>> bbox = painter.draw_text_box(
          >>>     text=['Hello World!'], position=(200, 50), anchor='center',
          >>>     text_style=text_style, padding=(5, 5), rotation=0,
          >>>     line_style=line_style, fill_color='#c0bab1', radius=0.2)
        )docstr",
        py::arg("text"),
        py::arg("position"),
        py::arg("anchor") = Anchor::BottomLeft,
        py::arg("text_style") = TextStyle(),
        py::arg("padding") = Vec2d::All(6.0),
        py::arg("rotation") = 0.0,
        py::arg("line_style") = LineStyle::Invalid,
        py::arg("fill_color") = Color::White.WithAlpha(0.6),
        py::arg("radius") = 0.2,
        py::arg("fixed_size") = Vec2d::All(-1.0));


  //----------------------------------------------------------------------
  LineStyle default_trajectory_style;
  default_trajectory_style.width = 5;
  default_trajectory_style.cap = LineCap::Round;
  default_trajectory_style.join = LineJoin::Round;

  const Color default_trajectory_fade_out_color(NamedColor::LightGray, 0.6);

  painter.def(
        "draw_trajectory",
        &PainterWrapper::DrawTrajectory, R"docstr(
        Draws a single trajectory.

        **Corresponding C++ API:** ``viren2d::Painter::DrawTrajectory``.

        Can be used to either draw **a fixed-color path** (if
        ``fade_out_color`` is invalid), or **a path which gradually
        changes its color** from ``line_style.color`` to ``fade_out_color``.
        In the latter case, the color transition can be controlled
        by ``fading_factor``.

        Args:
          trajectory: A :class:`list` of :class:`~viren2d.Vec2d` which
            specifies the trajectory's coordinates.
          line_style: A :class:`~viren2d.LineStyle` specifying how
            to draw the trajectory (except for the color gradient).
          fade_out_color: If this is a valid :class:`~viren2d.Color`,
            the trajectory's tail will be drawn with this color.
          tail_first: Set to ``True`` if the first point, *i.e.*
            ``points[0]``, is the *oldest* point, *i.e.* the trajectory's
            tail. Otherwise, it is assumed to be the *most recent* point.
          smoothing_window: Specifies the window size to optionally
            smooth the trajectory via moving average. Disable smoothing
            by passing a value ``<= 0``.
          fading_factor: A function handle which will be invoked for
            each segment of the trajectory to compute the mixing ratios
            for the color gradient.

            Its **single input** is a :class:`float` :math:`\in [0,1]`, which
            denotes the *drawing progress* along the trajectory, from head
            (*i.e.* :math:`0`) to tail.
            Its **return value** must also be a :class:`float` :math:`\in [0,1]`,
            which specifies the amount of the ``fade_out_color`` to be
            applied for this *drawing progress*.

            For example, to get a *linear* color transition between head
            and tail, we simply use the identity function.
            For convenience, ``viren2d`` already provides :func:`~viren2d.fade_out_linear`,
            :func:`~viren2d.fade_out_quadratic`, and :func:`~viren2d.fade_out_logarithmic`.
            The default ``fading_factor`` function is :func:`~viren2d.fade_out_quadratic`.

        Example:
          >>> points = [(0, 0), (10, 20), (42, 30), ...]
          >>> line_style = viren2d.LineStyle(
          >>>     width=5, color='navy-blue',
          >>>     cap=viren2d.LineCap.Round,
          >>>     join=viren2d.LineJoin.Round)
          >>>
          >>> painter.draw_trajectory(
          >>>     trajectory=points, line_style=line_style,
          >>>     fade_out_color=(0.8, 0.8, 0.8, 0.4),
          >>>     smoothing_window=5, tail_first=True,
          >>>     fading_factor=viren2d.fade_out_linear)

        Note:
          If a valid ``fade_out_color`` is provided, the trajectory
          has to be drawn via separate line segments. This means that the
          :attr:`~viren2d.LineStyle.join` setting of ``line_style``
          parameter will have no effect. Additionally, if transparent
          colors are used, the individual segment endpoints will be visible.

          To avoid this behavior, the trajectory needs to be drawn with
          a single color, *i.e.* pass :attr:`Color.Invalid` as
          ``fade_out_color``.
        )docstr",
        py::arg("trajectory"),

        py::arg("line_style") = default_trajectory_style,
        py::arg("fade_out_color") = default_trajectory_fade_out_color,
        py::arg("tail_first") = true,
        py::arg("smoothing_window") = 0,
        py::arg("fading_factor") = std::function<double(double)>(ColorFadeOutQuadratic));
  // TODO(doc) Double-check the docstring if we change anything, because it includes
  // the current default color mix/fading factor function(!)


  //TODO test example
  painter.def(
        "draw_trajectories",
        &PainterWrapper::DrawTrajectories, R"docstr(
        Draws multiple (similarly styled) trajectories.

        **Corresponding C++ API:** ``viren2d::Painter::DrawTrajectories``.

        Allows rendering multiple trajectories with a common
        :class:`~viren2d.LineStyle`.

        Args:
          trajectories: A :class:`list` of :class:`tuple`,
            where each :class:`tuple` holds ``(trajectory, color)``:

            * ``trajectory`` is a :class:`list` of :class:`~viren2d.Vec`,
              *i.e.* the coordinates.
            * ``color`` is the corresponding :class:`~viren2d.Color`.
              If :attr:`Color.Invalid` or :attr:`Color.Same`, the color of the
              ``line_style`` parameter will be used instead.
          others: For details on all other parameters, refer to the
            documentation of :meth:`~viren2d.Painter.draw_trajectory`.

        Example:
          >>> points1 = [(20,  0), (10, 20), (42, 30), ...]
          >>> points2 = [(70, 70), (50, 20), (23, 30), ...]
          >>> trajs = [(points1, 'maroon'), (points2, 'invalid')]
          >>>
          >>> line_style = viren2d.LineStyle(
          >>>     width=5, color='navy-blue',
          >>>     cap=viren2d.LineCap.Round,
          >>>     join=viren2d.LineJoin.Round)
          >>>
          >>> painter.draw_trajectories(
          >>>     trajectories=trajs, line_style=line_style,
          >>>     fade_out_color=(0.8, 0.8, 0.8, 0.4),
          >>>     smoothing_window=5, tail_first=True,
          >>>     fading_factor=viren2d.fade_out_linear)
        )docstr",
        py::arg("trajectories"),
        py::arg("line_style") = default_trajectory_style,
        py::arg("fade_out_color") = default_trajectory_fade_out_color,
        py::arg("tail_first") = true,
        py::arg("smoothing_window") = 0,
        py::arg("fading_factor") = std::function<double(double)>(ColorFadeOutQuadratic));


  painter.def(
        "draw_xyz_axes",
        &PainterWrapper::DrawXYZAxes, R"docstr(
        Draws the coordinate system axes for the pinhole camera calibration.

        **Corresponding C++ API:** ``viren2d::Painter::DrawXYZAxes``.

        Args:
          K: The :math:`3 \times 3` camera matrix as :class:`numpy.ndarray` of
            type :class:`numpy.float64`, which holds the intrinsic parameters.
          R: The :math:`3 \times 3` extrinsic rotation matrix, again as
            :class:`numpy.ndarray` of type :class:`numpy.float64`.
          t: The 3d extrinsic translation vector as :class:`~viren2d.Vec3d`.
          origin: Center of the world coordinate system as
            :class:`~viren2d.Vec3d`.
          lengths: A :class:`~viren2d.Vec3d` specifying how far to shift the
            arrow tips from the origin. Each axis tip will be computed as
            :math:`\text{tip}_{\text{axis}} = \text{origin} + \mathbf{e}_{\text{axis}} * \text{lengths}[axis]`,
            where :math:`\mathbf{e}_{\text{axis}}` is the unit vector for the
            corresponding axis.
            The default value assumes that the calibration is given in
            millimeters and will result in 1 meter long arrows.
          arrow_style: The :class:`~viren2d.ArrowStyle` specifying how the
            axis arrows should be drawn. Note that it's color attribute
            will be ignored.
          color_x: :class:`~viren2d.Color` of the *x*-axis arrow. Default reddish.
          color_y: :class:`~viren2d.Color` of the *y*-axis arrow. Default greenish.
          color_z: :class:`~viren2d.Color` of the *z*-axis arrow. Default bluish.

        Returns:
          ``True`` if at least one point (axis arrow tip or the origin) is
          visible within the camera's field-of-view.

        Example:
          >>> K = np.array(
          >>>     [[523.2, 0.0, 341.0],
          >>>      [0.0, 523.2, 256.0],
          >>>      [0.0, 0.0, 1.0]], dtype=np.float64)
          >>> R = np.array(
          >>>     [[ 0.99013141,  0.14006482, -0.00465153],
          >>>      [ 0.05439048, -0.41465762, -0.90835056],
          >>>      [-0.12915675,  0.89913342, -0.41818372]], dtype=np.float64)
          >>> t = np.array([-51.8, 17.3, 82.5], dtype=np.float64)
          >>> arrow_style = viren2d.ArrowStyle(
          >>>     width=3, tip_length=0.3, tip_angle=20,
          >>>     tip_closed=True, double_headed=False,
          >>>     dash_pattern=[], dash_offset=0.0,
          >>>     cap='round', join='miter')
          >>> painter.draw_xyz_axes(
          >>>     K=K, R=R, t=t, origin=(0, 0, 0),
          >>>     lengths=(1e3, 1e3, 1e3), arrow_style=arrow_style,
          >>>     color_x='red', color_y='green', color_z='blue')
        )docstr",
        py::arg("K"),
        py::arg("R"),
        py::arg("t"),
        py::arg("origin") = Vec3d::All(0.0),
        py::arg("lengths") = Vec2d::All(1e3),
        py::arg("arrow_style") = default_arrow_style,
        py::arg("color_x") = Color::CoordinateAxisColor('x'),
        py::arg("color_y") = Color::CoordinateAxisColor('y'),
        py::arg("color_z") = Color::CoordinateAxisColor('z'));
}

} // namespace bindings
} // namespace viren2d
