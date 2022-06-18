#include <memory>
#include <sstream>
#include <stdexcept>

#include <viren2d/drawing.h>

#include <pybind11/stl.h>

#include <bindings/binding_helpers.h>

//TODO(doc) Remove FullyQualifiedTypeString from docstr
//    definitions, so we can more easily use raw strings

namespace py = pybind11;

namespace viren2d {
namespace bindings {
TextAnchor TextAnchorFromPyObject(py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return TextAnchorFromString(py::cast<std::string>(o));
  } else if (py::isinstance<TextAnchor>(o)) {
    return py::cast<TextAnchor>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp << "` to `"
        << FullyQualifiedType("TextAnchor") << "`!";
    throw std::invalid_argument(str.str());
  }
}


/**
 * @brief A wrapper for the abstract `Painter`
 *
 * This is necessary because I don't want to expose
 * the `ImagePainter` - I like the current factory method
 * layout because the public headers are quite clean.
 * Thus, I cannot (read "don't want to") use pybind11's
 * trampoline mechanism.
 */
class PainterWrapper {
public:
  PainterWrapper() : painter_(CreatePainter())
  {}


  void SetCanvasColor(int width, int height, const Color &color) {
    painter_->SetCanvas(width, height, color);
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


  void DrawArc(const Vec2d &center, double radius,
               double angle1, double angle2,
               const LineStyle &line_style,
               bool include_center,
               const Color &fill_color) {
    painter_->DrawArc(center, radius, angle1, angle2, line_style,
                      include_center, fill_color);
  }


  void DrawArrow(const Vec2d &from, const Vec2d &to,
                 const ArrowStyle &arrow_style) {
    painter_->DrawArrow(from, to, arrow_style);
  }


  void DrawBoundingBox2D(const Rect &box,
                         const std::vector<std::string> &label,
                         const BoundingBox2DStyle &style) {
    painter_->DrawBoundingBox2D(box, label, style);
  }


  void DrawCircle(const Vec2d &center, double radius,
                  const LineStyle &line_style,
                  const Color &fill_color) {
    painter_->DrawCircle(center, radius, line_style, fill_color);
  }


  void DrawEllipse(const Ellipse &ellipse,
                   const LineStyle &line_style,
                   const Color &fill_color) {
    painter_->DrawEllipse(ellipse, line_style, fill_color);
  }


  void DrawGrid(double spacing_x, double spacing_y,
                const LineStyle &line_style,
                const Vec2d &top_left,
                const Vec2d &bottom_right) {
    painter_->DrawGrid(top_left, bottom_right,
                       spacing_x, spacing_y, line_style);
  }


  void DrawLine(const Vec2d &from, const Vec2d &to,
                const LineStyle &line_style) {
    painter_->DrawLine(from, to, line_style);
  }


  void DrawMarker(const Vec2d &pos, const MarkerStyle &style) {
    painter_->DrawMarker(pos, style);
  }


  void DrawMarkers(const std::vector<std::pair<Vec2d, Color>> &markers,
                   const MarkerStyle &style) {
    painter_->DrawMarkers(markers, style);
  }


  void DrawPolygon(const std::vector<Vec2d> &polygon,
                   const LineStyle &line_style,
                   const Color &fill_color) {
    painter_->DrawPolygon(polygon, line_style, fill_color);
  }


  void DrawRect(const Rect &rect, const LineStyle &line_style,
                const Color &fill_color) {
    painter_->DrawRect(rect, line_style, fill_color);
  }


  void DrawText(const std::vector<std::string> &text,
                const Vec2d &anchor_position, py::object &pyanchor,
                const TextStyle &text_style, const Vec2d &padding,
                double rotation) {
    TextAnchor anchor = TextAnchorFromPyObject(pyanchor);
    painter_->DrawText(text, anchor_position, anchor,
                       text_style, padding, rotation);
  }


  void DrawTextBox(const std::vector<std::string> &text,
                   const Vec2d &anchor_position, py::object &pyanchor,
                   const TextStyle &text_style, const Vec2d &padding,
                   double rotation, const LineStyle &box_line_style,
                   const Color &box_fill_color, double box_corner_radius,
                   const Vec2d &fixed_box_size) {
    TextAnchor anchor = TextAnchorFromPyObject(pyanchor);
    painter_->DrawTextBox(text, anchor_position, anchor, text_style,
                          padding, rotation, box_line_style, box_fill_color,
                          box_corner_radius, fixed_box_size);
  }


  void DrawTrajectory(const std::vector<Vec2d> &points, const LineStyle &style,
                      const Color &color_fade_out, bool oldest_position_first) {
    painter_->DrawTrajectory(points, style, color_fade_out, oldest_position_first);
  }


private:
  std::unique_ptr<Painter> painter_;
};


void RegisterPainter(py::module &m) {
  py::class_<PainterWrapper> painter(m, "Painter",
    R"pbdoc(A :class:`~viren2d.Painter` lets you draw on a canvas.

    Typical usage:

    1. Create a `Painter`:

       >>> import viren2d
       >>> painter = viren2d.Painter()

    2. Initialize its canvas:

      * Paint an empty canvas with a given color via :meth:`set_canvas_rgb`
      * Set up the canvas from image data via :meth:`set_canvas_image`
      * Set up the canvas by loading an image from disk via :meth:`set_canvas_filename`

    3. Draw onto the canvas via the painter's ``draw_xxx(...)`` methods, for example:

       >>> painter.draw_arrow(...)
       >>> painter.draw_bounding_box(...)

    4. After all objects have been drawn, retrieve the
       visualization via :meth:`get_canvas`. For example,
       to get a deeply copied image as ``numpy.ndarray``:

       >>> import numpy as np
       >>> canvas = painter.get_canvas(copy=True)
       >>> img_np = np.array(canvas, copy=False)

    5. Either continue drawing (step 3) or set a new canvas (step 2, the
       same painter instance can be reused).
    )pbdoc");

  painter.def(py::init<>())
      .def("__repr__", [](const Painter &) { return FullyQualifiedType("Painter", true); })
      .def("__str__", [](const Painter &) { return FullyQualifiedType("Painter", false); })
      .def("is_valid", &PainterWrapper::IsValid,
           "Checks if the canvas has been set up correctly.");

  std::string doc = R"docstr(
      Initializes the canvas with the given color.

      Args:
        width: Canvas width in pixels.
        height: Canvas height in pixels.
        color: Background :class:`~viren2d.Color`.

      Example:
        >>> painter = viren2d.Painter()
        >>> painter.set_canvas_rgb(800, 600)
        >>> painter.set_canvas_rgb(800, 600, 'crimson')
      )docstr";
  painter.def("set_canvas_rgb", &PainterWrapper::SetCanvasColor,
      doc.c_str(), py::arg("width"), py::arg("height"),
      py::arg("color") = Color::White);

  painter.def("set_canvas_filename", &PainterWrapper::SetCanvasFilename, R"docstr(
      Initializes the canvas from the given image file.

      This functionality uses the
      `stb library <https://github.com/nothings/stb/blob/master/stb_image.h>`__
      to load the image file. Supported formats are:

         JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM

      )docstr", py::arg("image_filename"));


  doc = R"docstr(
      Initializes the canvas from the given image.

      The input image can be either a ``numpy.ndarray``
      (currently, only ``dtype=uint8`` is supported) or an
      :class:`~viren2d.ImageBuffer`.

      Example:
        >>> img_np = np.zeros((480, 640, 3), dtype=np.uint8)
        >>> painter.set_canvas_image(img_np)
      )docstr";
  painter.def("set_canvas_image", &PainterWrapper::SetCanvasImage,
      doc.c_str(), py::arg("image"));


  //----------------------------------------------------------------------
  //DONE [x] add documentation
  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  //TODO [ ] add Python bindings
  //TODO [ ] add Python test (tests/test_xxx.py)
  //TODO [ ] add C++ demo
  //TODO [ ] add Python demo
  painter.def("get_canvas_size", &PainterWrapper::GetCanvasSize, R"docstr(
      Returns the size of the canvas in pixels.

      Returns the tuple ``(W, H)``, which specifies the
      width and height of the canvas in pixels (as ``int``),
      respectively.
      )docstr");

  //----------------------------------------------------------------------
  //DONE [x] add documentation
  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  //TODO [ ] add Python bindings
  //TODO [ ] add Python test (tests/test_xxx.py)
  //TODO [ ] add C++ demo
  //TODO [ ] add Python demo
  painter.def("get_canvas", &PainterWrapper::GetCanvas, R"docstr(
      Returns the current visualization.

      Args:
        copy: If you want a deep copy, set ``copy = True``. Otherwise,
          the buffer will just provide a **shared view** on the
          painter's canvas.
          This means: **If you keep on drawing, this view will also
          change.**

      Examples:
        Get canvas as ``numpy.ndarray``, where the **memory is
        shared** with the painter:

        >>> img_np = np.array(p.get_canvas(copy=False), copy=False)

        Retrieve a **deep copy** of the canvas as
        ``numpy.ndarray``, *i.e.* future ``painter.draw_...`` calls
        will not affect this retrieved copy:

        >>> img_np = np.array(p.get_canvas(copy=True), copy=False)

      .. tip::
          If you can ensure that the painter is not destroyed while
          you display/process the visualization, use the shared view
          (*i.e.* ``copy = False``) on its canvas to avoid unnecessary
          memory allocation.
      )docstr", py::arg("copy") = true);

  //----------------------------------------------------------------------
  //DONE [x] add documentation
  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  //TODO [ ] add Python bindings
  //TODO [ ] add Python test (tests/test_xxx.py)
  //TODO [ ] add C++ demo
  //TODO [ ] add Python demo
  doc = R"docstr(
    Draws a circular arc.

    Args:
      center: Center position as :class:`~viren2d.Vec2d`.
      radius: Radius of the arc in pixels as ``float``
      angle1: The arc will be drawn from ``angle1`` to ``angle2``
        in clockwise direction. Both angles are specified as type
        ``float`` in degrees, where 0 degrees points in the
        direction of increasing *x* coordinates.
      angle2: See ``angle1``
      line_style: A :class:`~viren2d.LineStyle` specifying how
        to draw the arc's outline.
        If you pass :attr:`viren2d.LineStyle.Invalid`, the contour
        will not be drawn (then, you must provide a valid
        ``fill_color``).
      include_center:  If ``True`` (default), the center point
        will be included when drawing the outline and filling.
      fill_color: If you provide a valid :class:`~viren2d.Color`,
        the arc will be filled.
      )docstr";
  painter.def("draw_arc", &PainterWrapper::DrawArc, doc.c_str(),
      py::arg("center"), py::arg("radius"),
      py::arg("angle1"), py::arg("angle2"),
      py::arg("style") = LineStyle(),
      py::arg("include_center") = true,
      py::arg("fill_color") = Color::Invalid);

  //----------------------------------------------------------------------
  //
  doc = R"docstr(
      Draws an arrow.

      Args:
        pt1: Start of the arrow shaft as :class:`~viren2d.Vec2d`.
        pt2: End of the arrow shaft (*i.e.* the *pointy end*) as
          :class:`~viren2d.Vec2d`.
        arrow_style: An :class:`~viren2d.ArrowStyle` specifying
          how to draw the arrow.
      )docstr";
  painter.def("draw_arrow", &PainterWrapper::DrawArrow, doc.c_str(),
      py::arg("pt1"), py::arg("pt2"),
      py::arg("arrow_style") = ArrowStyle());


  //----------------------------------------------------------------------TODO doc
  doc = "TODO doc";
  painter.def("draw_bounding_box_2d", &PainterWrapper::DrawBoundingBox2D,
              doc.c_str(), py::arg("rect"), py::arg("label"),
              py::arg("style") = BoundingBox2DStyle());
  // Create an alias
  painter.def("draw_bbox2d", &PainterWrapper::DrawBoundingBox2D,
              "Alias for :meth:`draw_bounding_box_2d`.", py::arg("rect"), py::arg("label"),
              py::arg("style") = BoundingBox2DStyle());


  //----------------------------------------------------------------------
  doc = "Draws a circle.\n\n"
        "Args:\n"
        "  center: Center position as :class:`~" + FullyQualifiedType(Vec2d::TypeName()) + "`\n"
        "  radius: Radius of the circle in pixels as ``float``\n"
        "  line_style: A :class:`~" + FullyQualifiedType("LineStyle") + "` specifying how\n"
        "    to draw the circle's outline.\n"
        "    If you pass " + FullyQualifiedType("LineStyle.Invalid") + ", the\n"
        "    contour will not be drawn (then you must provide a valid ``fill_color``).\n"
        "  fill_color: If you provide a valid :class:`~" + FullyQualifiedType("Color") + "`,\n"
        "    the circle will be filled.";
  painter.def("draw_circle", &PainterWrapper::DrawCircle, doc.c_str(),
        py::arg("center"), py::arg("radius"),
        py::arg("style") = LineStyle(),
        py::arg("fill_color") = Color::Invalid);


  //----------------------------------------------------------------------
  doc = "Draws an ellipse.\n\n"
    "Args:\n"
    "  ellipse: The :class:`~" + FullyQualifiedType("Ellipse") + "`,\n"
    "    which should be drawn.\n"
    "  line_style:  A :class:`~" + FullyQualifiedType("LineStyle") + "`\n"
    "    specifying how to draw the ellipse's outline.\n"
    "    If you pass :attr:`" + FullyQualifiedType("LineStyle.Invalid") + "`, the\n"
    "    contour will not be drawn (then you must provide a valid ``fill_color``).\n"
    "  fill_color: If you provide a valid :class:`~" + FullyQualifiedType("Color") + "`,\n"
    "    the ellipse will be filled.";
  painter.def("draw_ellipse", &PainterWrapper::DrawEllipse, doc.c_str(),
        py::arg("ellipse"),
        py::arg("style") = LineStyle(),
        py::arg("fill_color") = Color::Invalid);

        //TODO coding style//----------------------------------------------------------------------
  doc = "Draws a grid.\n\n:spacing_x:  (float)\n:spacing_y:  (float)\n"
        "    Width & height of each grid cell.\n\n"
        "    The grid will only be drawn within the defined region.\n"
        "    If not provided, the grid will span the whole canvas.\n\n"
        ":line_style:  (" + FullyQualifiedType("LineStyle") + ")\n"
        "    How to draw the grid lines.\n"
        "    If you pass " + FullyQualifiedType("LineStyle.Default") + ", the\n"
        "    painter's default line style will be used.\n\n"
        ":top_left:  (" + FullyQualifiedType(Vec2d::TypeName()) + ")\n"
        ":bottom_right:  (" + FullyQualifiedType(Vec2d::TypeName()) + ")\n";

  painter.def("draw_grid", &PainterWrapper::DrawGrid, doc.c_str(),
        py::arg("spacing_x"), py::arg("spacing_y"),
        py::arg("style") = LineStyle(),
        py::arg("top_left") = Vec2d(),
        py::arg("bottom_right") = Vec2d());

  //----------------------------------------------------------------------
  doc = R"docstr(
      Draws a line.

      Args:
        pt1: Start position as :class:`~viren2d.Vec2d`.
        pt2: End position as :class:`~viren2d.Vec2d`.
        style: A :class:`~viren2d.LineStyle` specifying
          how to draw the line.
      )docstr";
  painter.def("draw_line", &PainterWrapper::DrawLine, doc.c_str(),
              py::arg("pt1"), py::arg("pt2"),
              py::arg("style") = LineStyle());

  //----------------------------------------------------------------------
  doc = "Draws a single marker/keypoint.\n\n"
        "Args:\n"
        "  pos: Position as :class:`~"
        + FullyQualifiedType(Vec2d::TypeName()) + "`.\n"
        "  style: A :class:`~" + FullyQualifiedType("MarkerStyle") + "` specifying\n"
        "    how to draw the marker.";
  painter.def("draw_marker", &PainterWrapper::DrawMarker, doc.c_str(),
              py::arg("pos"), py::arg("style") = MarkerStyle());


  doc = "Draws multiple (similar) markers/keypoints.\n\n"
        "Args:\n"
        "  markers: Holds the position and color of each marker.\n"
        "    Should be provided as a List[Tuple[:class:`~" + FullyQualifiedType(Vec2d::TypeName()) + "`,\n"
        "    :class:`~" + FullyQualifiedType("Color") + "`]].\n"
        "    If a color is invalid, the corresponding marker will\n"
        "    be drawn using the ``style``'s color specification\n"
        "    instead."
        "  style: A :class:`~" + FullyQualifiedType("MarkerStyle") + "` specifying\n"
        "    how to draw the markers (except for the color).";
  painter.def("draw_markers", &PainterWrapper::DrawMarkers, doc.c_str(),
              py::arg("markers"), py::arg("style") = MarkerStyle());


  //----------------------------------------------------------------------
  doc = "Draws a polygon.\n\n"
        "Args:\n"
        "  polygon: Points of the polygon as ``list`` of :class:`~"
        + FullyQualifiedType(Vec2d::TypeName()) + "`.\n"
        "  line_style: A :class:`~" + FullyQualifiedType("LineStyle") + "` specifying how\n"
        "    to draw the circle's outline.\n"
        "    If you pass " + FullyQualifiedType("LineStyle.Invalid") + ", the\n"
        "    contour will not be drawn (then you must provide a valid ``fill_color``).\n"
        "  fill_color: If you provide a valid :class:`~" + FullyQualifiedType("Color") + "`,\n"
        "    the circle will be filled.";
  painter.def("draw_polygon", &PainterWrapper::DrawPolygon, doc.c_str(),
              py::arg("polygon"), py::arg("style") = LineStyle(),
              py::arg("fill_color") = Color::Invalid);

  //----------------------------------------------------------------------
  doc = "Draws a rectangle (axis-aligned/rotated, solid/dashed, etc.)\n\n"
        "Args:\n  rect: The :class:`~" + FullyQualifiedType("Rect") + "`\n"
        "    object which should be drawn.\n"
        "  line_style: A :class:`~" + FullyQualifiedType("LineStyle") + "` specifying how\n"
        "    to draw the rectangle's outline.\n"
        "    If you pass :attr:`" + FullyQualifiedType("LineStyle.Invalid") + "`, the\n"
        "    contour will not be drawn (then you must provide a valid ``fill_color``).\n"
        "  fill_color: If you provide a valid :class:`~" + FullyQualifiedType("Color") + "`,\n"
        "    the rectangle will be filled.";
  painter.def("draw_rect", &PainterWrapper::DrawRect, doc.c_str(),
              py::arg("rect"),
              py::arg("style") = LineStyle(),
              py::arg("fill_color") = Color::Invalid);

        //TODO doc//----------------------------------------------------------------------
  doc = "Places the given text on the canvas.\n\n"
      "Args:\n"
      "  text: A list of strings to be drawn.\n"
      "  position: Position of the reference point as :class:`~"
      + FullyQualifiedType(Vec2d::TypeName()) + "`.\n"
      "  anchor: How to orient the text w.r.t. the reference point.\n"
      "    Valid inputs are :class:`~" + FullyQualifiedType("TextAnchor")
      + "` enum values and\n"
      "    string representations. A string must correspond to either a\n"
      "    *position specification* (*i.e. ``center``, ``top``, ``top-right``,\n"
      "    ``right``, ``bottom-right``, ``bottom``, ``bottom-left``,\n"
      "    ``left``, ``top-left``) or one of the 8 *compass directions* (*i.e.*\n"
      "    ``north``, ``north-east``, ``east``, ``south-east``, ``south``,\n"
      "    ``south-west``, ``west``, ``north-west``).\n\n"
      "    Before parsing, the input will be converted to lowercase and any\n"
      "    whitespaces, dashes & underscores will be removed.\n"
      "  text_style: A :class:`~" + FullyQualifiedType("TextStyle") +"`,\n"
      "    specifying how to render the text.\n"
      "  padding: Optional distance between the closest glyph and the\n"
      "    reference point. Specified in pixels as :class:`~"
      + FullyQualifiedType("Vec2d") + "`.\n"
      "  rotation: Rotation angle in degrees as ``float``.";
  painter.def("draw_text", &PainterWrapper::DrawText, doc.c_str(),
      py::arg("text"), py::arg("position"),
      py::arg("anchor") = TextAnchor::BottomLeft,
      py::arg("text_style") = TextStyle(),
      py::arg("padding") = Vec2d(0.0, 0.0),
      py::arg("rotation") = 0.0);



        //----------------------------------------------------------------------
  doc = "TODO doc"
      "  anchor: Either enum or string representation";
  painter.def("draw_textbox", &PainterWrapper::DrawTextBox, doc.c_str(),
      py::arg("text"), py::arg("position"),
      py::arg("anchor") = TextAnchor::BottomLeft,
      py::arg("text_style") = TextStyle(),
      py::arg("padding") = Vec2d::All(6.0),
      py::arg("rotation") = 0.0,
      py::arg("line_style") = LineStyle::Invalid,
      py::arg("fill_color") = Color::White.WithAlpha(0.6),
      py::arg("radius") = 0.2,
      py::arg("fixed_size") = Vec2d::All(-1.0));


  //----------------------------------------------------------------------
  doc = "TODO doc";
  painter.def("draw_trajectory", &PainterWrapper::DrawTrajectory, doc.c_str(),
              py::arg("points"), py::arg("style"),
              py::arg("fade_out_color") = Color(NamedColor::LightGray, 0.6),
              py::arg("oldest_first") = false);
  //TODO(snototter) add draw_xxx methods

  //TODO(snototter) add convenience functions handling multiple inputs (plural draw_xxxS), e.g. via
  //                Painter "for (element in list) : painter_->DrawElement();"
}

} // namespace bindings
} // namespace viren2d
