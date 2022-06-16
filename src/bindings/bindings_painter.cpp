#include <memory>
#include <sstream>
#include <stdexcept>

#include <viren2d/drawing.h>

#include <pybind11/stl.h>

#include <bindings/binding_helpers.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {
/**
 * @brief A wrapper for the abstract `Painter`
 *
 * This is necessary because I don't want to expose
 * the `ImagePainter` (I like the current factory method
 * layout because the public headers are quite clean).
 * Thus, I cannot (read "don't want to") use the
 * pybind11's trampoline mechanism.
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


  void DrawRect(const Rect &rect, const LineStyle &line_style,
                const Color &fill_color) {
    painter_->DrawRect(rect, line_style, fill_color);
  }

  void DrawText(const std::vector<std::string> &text,
                const Vec2d &anchor_position, TextAnchor anchor,
                const TextStyle &text_style, const Vec2d &padding,
                double rotation) {
    painter_->DrawText(text, anchor_position, anchor,
                       text_style, padding, rotation);
  }

  void DrawTextBox(const std::vector<std::string> &text,
                   const Vec2d &anchor_position, TextAnchor anchor,
                   const TextStyle &text_style, const Vec2d &padding,
                   double rotation, const LineStyle &box_line_style,
                   const Color &box_fill_color, double box_corner_radius,
                   const Vec2d &fixed_box_size) {
    painter_->DrawTextBox(text, anchor_position, anchor, text_style,
                          padding, rotation, box_line_style, box_fill_color,
                          box_corner_radius, fixed_box_size);
  }


private:
  std::unique_ptr<Painter> painter_;
};


void RegisterPainter(py::module &m) {
  py::class_<PainterWrapper> painter(m, "Painter",
    R"pbdoc(A `Painter` lets you draw on a canvas.

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

  std::string doc = "Initializes the canvas with the given color.\n\n"
       "Args:\n"
       "  width: Canvas width in pixels.\n"
       "  height: Canvas height in pixels.\n"
       "  color: Background :class:`~"
       + FullyQualifiedType("Color") + "`.";
  painter.def(py::init<>())
      .def("__repr__", [](const Painter &) { return FullyQualifiedType("Painter", true); })
      .def("__str__", [](const Painter &) { return FullyQualifiedType("Painter", false); })
      .def("is_valid", &PainterWrapper::IsValid,
           "Checks if the canvas has been set up correctly.")
      .def("set_canvas_rgb", &PainterWrapper::SetCanvasColor,
           doc.c_str(),
           py::arg("width"), py::arg("height"),
           py::arg("color") = Color::White)
      .def("set_canvas_filename", &PainterWrapper::SetCanvasFilename,
           "Initializes the canvas from the given image file.\n\n"
           "This functionality uses the "
           "`stb library <https://github.com/nothings/stb/blob/master/stb_image.h>`_\n"
           "to load the image file. Supported formats are:\n\n"
           "   JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM",
           py::arg("image_filename"));

  doc = "Initializes the canvas from the given image, *i.e.* either\n"
      "a ``numpy.ndarray`` (with ``dtype=uint8``) or an :class:`~"
      + FullyQualifiedType("ImageBuffer") + "`.\n\n"
      "Example:\n"
      "  >>> img_np = np.zeros((480, 640, 3), dtype=np.uint8)\n"
      "  >>> painter.set_canvas_image(img_np)";
  painter.def("set_canvas_image", &PainterWrapper::SetCanvasImage,
              doc.c_str(), py::arg("image"));

  painter.def("get_canvas_size", &PainterWrapper::GetCanvasSize,
           "Returns the size of the canvas in pixels as ``(W, H)`` tuple,\n"
           "where both ``W`` and ``H`` are integers.")
      .def("get_canvas", &PainterWrapper::GetCanvas,
           "Returns the current visualization.\n\n"
           "Args:\n"
           "  copy: If you want a deep copy, set ``copy = True``. Otherwise, the buffer\n"
           "    will just provide a **shared view** on the painter's canvas.\n"
           "    This means: **If you keep on drawing, this view will also\n"
           "    change.**\n\n"
           "Examples:\n"
           "  Get canvas as ``numpy.ndarray``, where the **memory is\n"
           "  shared** with the painter:\n\n"
           "  >>> img_np = np.array(p.get_canvas(copy=False), copy=False)\n\n"
           "  Retrieve a **deep copy** of the canvas as\n"
           "  ``numpy.ndarray``, *i.e.* future ``painter.draw_...`` calls\n"
           "  will not affect this retrieved copy:\n\n"
           "  >>> img_np = np.array(p.get_canvas(copy=True), copy=False)\n\n"
           ".. tip::\n"
           "   If you can ensure that the painter is not destroyed while\n"
           "   you display/process the visualization, use the shared view\n"
           "   (*i.e.* ``copy = False``) on its canvas to avoid unnecessary\n"
           "   memory allocation.",
           py::arg("copy") = true);


//TODO(snototter) use consistent coding style (Google Python Guide) for code in all draw_xxx method docstrings
  //----------------------------------------------------------------------
  doc = "Draws a circular arc.\n\n"
        "Args:\n"
        "  center: Center position as :class:`~" + FullyQualifiedType(Vec2d::TypeName()) + "`.\n"
        "  radius: Radius of the arc in pixels as ``float``.\n"
        "  angle1: The arc will be drawn from ``angle1`` to ``angle2``\n"
        "    in clockwise direction. Both angles are specified as type ``float`` in degrees,\n"
        "    where 0 degrees points in the direction of increasing `x` coordinates.\n"
        "  angle2: See ``angle1``.\n"
        "  line_style: A :class:`~" + FullyQualifiedType("LineStyle") + "` specifying how\n"
        "    to draw the arc's outline.\n"
        "    If you pass :attr:`" + FullyQualifiedType("LineStyle.Invalid") + "`, the\n"
        "    contour will not be drawn (then you must define a ``fill_color``).\n"
        "  include_center:  If ``True`` (default), the center of the circle will be included\n"
        "    when drawing the outline and filling the arc.\n"
        "  fill_color: If you provide a valid :class:`~" + FullyQualifiedType("Color") + "`,\n"
        "    the arc will be filled.";
  painter.def("draw_arc", &PainterWrapper::DrawArc, doc.c_str(),
              py::arg("center"), py::arg("radius"),
              py::arg("angle1"), py::arg("angle2"),
              py::arg("line_style") = LineStyle(),
              py::arg("include_center") = true,
              py::arg("fill_color") = Color::Invalid);

  //----------------------------------------------------------------------
  doc = "Draws an arrow.\n\n"
        "Args:\n"
        "  pt1: Start of the arrow shaft as :class:`~" + FullyQualifiedType(Vec2d::TypeName()) + "`.\n"
        "  pt2: End of the arrow shaft (*i.e.* the *pointy end*) as :class:`~" + FullyQualifiedType(Vec2d::TypeName()) + "`.\n"
        "  arrow_style: An :class:`~" + FullyQualifiedType("ArrowStyle") + "` specifying\n"
        "    how to draw the arrow.";
  painter.def("draw_arrow", &PainterWrapper::DrawArrow, doc.c_str(),
              py::arg("pt1"), py::arg("pt2"),
              py::arg("arrow_style") = ArrowStyle());

  //----------------------------------------------------------------------
  doc = "Draws a circle.\n\n"
        "Args:\n"
        "  center: Center position as :class:`~" + FullyQualifiedType(Vec2d::TypeName()) + "`\n"
        "  radius: Radius of the circle in pixels as ``float``\n"
        "  line_style: A :class:`~" + FullyQualifiedType("LineStyle") + "` specifying how\n"
        "    to draw the circle's outline.\n"
        "    If you pass " + FullyQualifiedType("LineStyle.Invalid") + ", the\n"
        "    contour will not be drawn (then you must define a ``fill_color``).\n"
        "  fill_color: If you provide a valid :class:`~" + FullyQualifiedType("Color") + "`,\n"
        "    the circle will be filled.";
  painter.def("draw_circle", &PainterWrapper::DrawCircle, doc.c_str(),
              py::arg("center"), py::arg("radius"),
              py::arg("line_style") = LineStyle(),
              py::arg("fill_color") = Color::Invalid);


    //TODO coding style//----------------------------------------------------------------------
  doc = "Draws an ellipse.\n\n"
    ":ellipse:  (" + FullyQualifiedType("Ellipse") + ")\n"
    "    The ellipse which should be drawn.\n\n"
    ":line_style:  (" + FullyQualifiedType("LineStyle") + ")\n"
    "    How to draw the ellipse's outline.\n"
    "    If you pass " + FullyQualifiedType("LineStyle.Default") + ", the\n"
    "    painter's default line style will be used.\n"
    "    If you pass " + FullyQualifiedType("LineStyle.Invalid") + ", the\n"
    "    contour will not be drawn (then you must define a 'fill_color').\n\n"
    ":fill_color:  (" + FullyQualifiedType("Color") + ")\n"
    "   Provide a valid color to fill the ellipse.";
  painter.def("draw_ellipse", &PainterWrapper::DrawEllipse, doc.c_str(),
          py::arg("ellipse"),
          py::arg("line_style") = LineStyle(),
          py::arg("fill_color") = Color::Invalid);

        //TODO//----------------------------------------------------------------------
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
              py::arg("line_style") = LineStyle(),
              py::arg("top_left") = Vec2d(),
              py::arg("bottom_right") = Vec2d());

  //----------------------------------------------------------------------
  doc = "Draws a line.\n\n"
        "Args:\n"
        "  pt1: Start position as :class:`~"
        + FullyQualifiedType(Vec2d::TypeName()) + "`.\n"
        "  pt2: End position as :class:`~"
        + FullyQualifiedType(Vec2d::TypeName()) + "`.\n"
        "  style: A :class:`~" + FullyQualifiedType("LineStyle") + "` specifying\n"
        "    how to draw the line.";
  painter.def("draw_line", &PainterWrapper::DrawLine, doc.c_str(),
              py::arg("pt1"), py::arg("pt2"),
              py::arg("style") = LineStyle());

  //----------------------------------------------------------------------
  doc = "Draws a marker/keypoint.\n\n"
        "Args:\n"
        "  pos: Position as :class:`~"
        + FullyQualifiedType(Vec2d::TypeName()) + "`.\n"
        "  style: A :class:`~" + FullyQualifiedType("MarkerStyle") + "` specifying\n"
        "    how to draw the marker.";
  painter.def("draw_marker", &PainterWrapper::DrawMarker, doc.c_str(),
              py::arg("pos"), py::arg("style") = MarkerStyle());

  //TODO doc//----------------------------------------------------------------------
  doc = "Draws a rectangle (axis-aligned/rotated, solid/dashed, etc.)\n\n"
        ":rect:  (" + FullyQualifiedType("Rect") + ")\n"
        "    The rectangle which should be drawn.\n\n"
        ":line_style:  (" + FullyQualifiedType("LineStyle") + ")\n"
        "    How to draw the rectangle's outline.\n"
        "    If you pass " + FullyQualifiedType("LineStyle.Default") + ", the\n"
        "    painter's default line style will be used.\n"
        "    If you pass " + FullyQualifiedType("LineStyle.Invalid") + ", the\n"
        "    contour will not be drawn (then you must define a 'fill_color').\n\n"
        ":fill_color:  (" + FullyQualifiedType("Color") + ")\n"
        "    Provide a valid color to fill the rectangle.";
  painter.def("draw_rect", &PainterWrapper::DrawRect, doc.c_str(),
              py::arg("rect"),
              py::arg("line_style") = LineStyle(),
              py::arg("fill_color") = Color::Invalid);
        //----------------------------------------------------------------------
  doc = "Places the given text on the canvas.\n\n"
        "Args:\n"
        "  text: A list of strings to be drawn.\n"
        "  position: Position of the reference point as :class:`~"
      + FullyQualifiedType(Vec2d::TypeName()) + "`.\n"
        "  anchor: How to orient the text w.r.t. the reference point.\n"
        "    See :class:`~" + FullyQualifiedType("TextAnchor")
      + "` and :func:`~" + FullyQualifiedType("text_anchor") + "`.\n"
                                                              "TODO finish doc.";
  painter.def("draw_text", &PainterWrapper::DrawText, doc.c_str(),
              py::arg("text"), py::arg("position"),
              py::arg("anchor") = TextAnchor::BottomLeft,
              py::arg("text_style") = TextStyle(),
              py::arg("padding") = Vec2d(0.0, 0.0),
              py::arg("rotation") = 0.0);



        //----------------------------------------------------------------------
  doc = "TODO doc";
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


  //TODO(snototter) add draw_xxx methods

  //TODO(snototter) add convenience functions handling multiple inputs (plural draw_xxxS), e.g. via
  //                Painter "for (element in list) : painter_->DrawElement();"
}

} // namespace bindings
} // namespace viren2d
