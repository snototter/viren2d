#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <viren2d/styles.h>
#include <bindings/binding_helpers.h>
namespace py = pybind11;

namespace viren2d {
namespace bindings {
//-------------------------------------------------  LineStyle
py::tuple LineStyleToTuple(const LineStyle &ls) {
  return py::make_tuple(ls.line_width, ls.color, ls.dash_pattern,
                        ls.line_cap, ls.line_join);
}


LineStyle LineStyleFromTuple(py::tuple tpl) {
  // Convert empty tuple to pre-defined default style
  if (tpl.empty()) {
    return LineStyle();
  }

  if (tpl.size() > 5) {
    std::ostringstream s;
    s << "Cannot create " << FullyQualifiedType("LineStyle")
      << " from tuple with " << tpl.size()
      << (tpl.size() == 1 ? " entry!" : " entries!");
    throw std::invalid_argument(s.str());
  }

  LineStyle ls(tpl[0].cast<double>(),
                        tpl[1].cast<Color>());

  if (tpl.size() > 2) {
    ls.dash_pattern = tpl[2].cast<std::vector<double>>();
  }

  if (tpl.size() > 3) {
    ls.line_cap = tpl[3].cast<LineCap>();
  }

  if (tpl.size() > 4) {
    ls.line_join = tpl[4].cast<LineJoin>();
  }

  return ls;
}


void RegisterLineCap(pybind11::module &m) {
  py::enum_<LineCap>(m, "LineCap",
             "How to render the endpoints of the line (or dash strokes).")
      .value("Butt", LineCap::Butt,
             "Start/stop the line exactly at the start/end point.")
      .value("Round", LineCap::Round,
             "Round ending, center of the circle is the end point.")
      .value("Square", LineCap::Square,
             "Square ending, center of the square is the end point.");
}

void RegisterLineJoin(pybind11::module &m) {
  py::enum_<LineJoin>(m, "LineJoin",
             "How to render the junction of two lines/segments.")
      .value("Miter", LineJoin::Miter,
             "Sharp (angled) corner.")
      .value("Bevel", LineJoin::Bevel,
             "The join is cut off at half the line width from the joint point.")
      .value("Round", LineJoin::Round,
             "Rounded join, where the center of the circle is the joint point.");
}


void RegisterLineStyle(pybind11::module &m) {
  std::string doc = "How a line/contour should be drawn.\n\nNote the special static members:\n"
        "* Use `LineStyle.Default` to use the painter's default line style.\n"
        "  This can be changed via `Painter.set_default_line_style()`.\n"
        "* Use `LineStyle.Invalid` in supported `draw_xxx()` methods to skip\n"
        "  drawing its outline (if you only want to fill the object).";
  py::class_<LineStyle>line_style(m, "LineStyle", doc.c_str());

  doc = "Initialize from `tuple`:\n"
         "Get the default line style via: ()\n"
         "Only width & color:  `(line_width, color)`"
         "Full configuration:  `(line_width, color, dash_pattern,\n"
         "                      line_cap, line_join)`\n"
         "With data types:\n"
         ":line_width:    `float`\n"
         ":color:         `" + FullyQualifiedType("Color") + "`\n"
         ":dash_pattern:  `List[float]`\n"
         ":line_cap:      `" + FullyQualifiedType("LineCap") + "`\n"
         ":line_join:     `" + FullyQualifiedType("LineJoin") + "`";
  line_style.def(py::init<>(&LineStyleFromTuple), doc.c_str(), py::arg("tpl"));

  doc = "Customize your line style:\n"
        ":line_width:    `float`\n"
        ":color:         `" + FullyQualifiedType("Color") + "`\n"
        ":dash_pattern:  `List[float]`\n"
        ":line_cap:      `" + FullyQualifiedType("LineCap") + "`\n"
        ":line_join:     `" + FullyQualifiedType("LineJoin") + "`";
  line_style.def(py::init<double, Color, std::vector<double>,
                          LineCap, LineJoin>(),
           doc.c_str(),
           py::arg("line_width"),
           py::arg("color"),
           py::arg("dash_pattern") = std::vector<double>(),
           py::arg("line_cap") = LineCap::Butt,
           py::arg("line_join") = LineJoin::Miter);

  line_style.def(py::init<>(), "Creates a default, library-wide preset line style.\n"
           "Note that this default style is NOT the same as the\n"
           "one you can set per painter. For that style,\n"
           "see `Painter.set_default_line_style()FIXME REMOVE/UPDATE DOC`")
      .def("copy", [](const LineStyle &st) { return LineStyle(st); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const LineStyle &st)
           { return "<" + st.ToString() + ">"; })
      .def("__str__", &LineStyle::ToString)
      .def(py::pickle(&LineStyleToTuple,
                      &LineStyleFromTuple))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_valid", &LineStyle::IsValid,
           "Check if the style would lead to a drawable line.")
      .def("is_dashed", &LineStyle::IsDashed,
           "Check if this style contains a dash stroke pattern.")
      .def("cap_offset", &LineStyle::CapOffset,
           "Computes how much the line cap will extend the\n"
           "line's start/end.")
      .def("join_offset", &LineStyle::JoinOffset,
           "Computes how much a line join will extend the joint.\n\n"
           "The `interior_angle` is the angle between two line segments\n"
           "in degrees.\n"
           "This needs to know the `miter_limit` because Cairo switches\n"
           "from `MITER` to `BEVEL` if the `miter_limit` is exceeded,\n"
           "according to [its documentation](https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-miter-limit).",
           py::arg("interior_angle"),
           py::arg("miter_limit") = 10.0)
      .def_readwrite("dash_pattern", &LineStyle::dash_pattern,
           "Dash pattern defined as list of on/off strokes (lengths in\n"
           "pixels), e.g. [20, 10, 40, 10]. If the list is empty, the\n"
           "line will be drawn solid.")
      .def_readwrite("line_cap", &LineStyle::line_cap,
           "How to render the endpoints of the line (or dash strokes).")
      .def_readwrite("line_join", &LineStyle::line_join,
           "How to render the junction of two lines/segments.")
      .def_readwrite("line_width", &LineStyle::line_width,
           "Width/thickness in pixels.")
      .def_readonly_static("Invalid", &LineStyle::Invalid,
            "Pass this to `Painter.draw_xxx()` to skip drawing the contour and\n"
            "only fill the object instead.");

  doc = "Line color as `" + FullyQualifiedType("Color") + "`.";
  line_style.def_readwrite("color", &LineStyle::color,
           doc.c_str());

  // A LineStyle can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, LineStyle>();
}



//-------------------------------------------------  ArrowStyle
py::tuple ArrowStyleToTuple(const viren2d::ArrowStyle &obj) {
  // Re-use LineStyle serialization:
  auto ls = static_cast<const viren2d::LineStyle &>(obj);
  auto ls_tpl = LineStyleToTuple(ls);

  return py::make_tuple(ls_tpl, obj.tip_length,
                        obj.tip_angle, obj.tip_closed,
                        obj.double_headed);
}


viren2d::ArrowStyle ArrowStyleFromTuple(py::tuple tpl) {
  // Convert empty tuple to pre-defined default style
  if(tpl.empty()) {
    return viren2d::ArrowStyle();
  }

  if (tpl.size() > 5) {
    std::ostringstream s;
    s << "Cannot create " << FullyQualifiedType("ArrowStyle")
      << " from tuple with "
      << tpl.size() << " entries!";
    throw std::invalid_argument(s.str());
  }

  viren2d::ArrowStyle style(LineStyleFromTuple(tpl[0]));
  if (tpl.size() > 1) {
    style.tip_length = tpl[1].cast<double>();
  }

  if (tpl.size() > 2) {
    style.tip_angle = tpl[2].cast<double>();
  }

  if (tpl.size() > 3) {
    style.tip_closed = tpl[3].cast<bool>();
  }

  if (tpl.size() > 4) {
    style.double_headed = tpl[4].cast<bool>();
  }

  return style;
}


void RegisterArrowStyle(pybind11::module &m) {
  py::class_<ArrowStyle, LineStyle> arrow_style(m,
        "ArrowStyle", "How an arrow should be drawn.");

  std::string doc = "A ``tuple`` can be cast into an ArrowStyle.\n\n"
"Examples:\n\nTODO TODO see :attr:`line_width` "
"*Specify only width & color:*\n"
"   ``(line_width, color)``\n\n"
"*Full configuration:*\n"
"   ``(line_width, color, tip_length,\n"
"   tip_angle, tip_closed, double_headed, dash_pattern,\n"
"   line_cap, line_join)``\n\n"
"With data types:\n\n"
"* ``line_width``: ``float``\n"
"* ``color``: :class:`" + FullyQualifiedType("Color") + "`\n"
"* ``tip_length``: ``float`` - Percentage of shaft length if [0, 1].\n"
"  Otherwise, defines the absolute tip length in pixels.\n"
"* ``tip_angle``: ``float`` - Interior angle (in degrees) between shaft\n"
"  and tip.\n"
"* ``tip_closed``: ``bool`` - Set ``True`` to fill the tip.\n"
"* ``double_headed``: ``bool`` Set ``True`` to draw a tip on both ends of\n"
"  the shaft.\n"
"* ``dash_pattern``: ``List[float]`` - see :class:`" + FullyQualifiedType("LineStyle") + "`\n"
":line_cap:      :class:`" + FullyQualifiedType("LineCap") + "`\n"
":line_join:     :class:`" + FullyQualifiedType("LineJoin") + "`";
  arrow_style.def(py::init<>(&ArrowStyleFromTuple), doc.c_str(), py::arg("tpl"));

  doc = "Customize your arrow style:\n"
        ":line_width:    `float`\n"
        ":color:         `" + FullyQualifiedType("Color") + "`\n"
        ":tip_length:    `float`\n"
        "                Percentage of shaft length if [0, 1].\n"
        "                Otherwise, absolute tip length in pixels.\n"
        ":tip_angle:     `float`\n"
        "                Interior angle (in degrees) between shaft\n"
        "                and tip.\n"
        ":tip_closed:    `bool`\n"
        "                Set `True` to fill the tip.\n"
        ":double_headed: `bool`\n"
        "                Set `True` to draw a tip on both ends of\n"
        "                the shaft.\n"
        ":dash_pattern:  `List[float]`\n"
        ":line_cap:      `" + FullyQualifiedType("LineCap") + "`\n"
        ":line_join:     `" + FullyQualifiedType("LineJoin") + "`";
  arrow_style.def(py::init<double, Color,
                    double, double, bool, bool, std::vector<double>,
                    LineCap, LineJoin>(), doc.c_str(),
           py::arg("line_width"),
           py::arg("color"),
           py::arg("tip_length") = 0.1,
           py::arg("tip_angle") = 30.0,
           py::arg("tip_closed") = false,
           py::arg("double_headed") = false,
           py::arg("dash_pattern")=std::vector<double>(),
           py::arg("line_cap")=LineCap::Butt,
           py::arg("line_join")=LineJoin::Miter);


  doc = "Customize your arrow style:\n"
        ":line_style:    `" + FullyQualifiedType("LineStyle") + "`\n"
        "                Customized line style.\n"
        ":tip_length:    `float`\n"
        "                Percentage of shaft length if [0, 1].\n"
        "                Otherwise, absolute tip length in pixels.\n"
        ":tip_angle:     `float`\n"
        "                Interior angle (in degrees) between shaft\n"
        "                and tip.\n"
        ":tip_closed:    `bool`\n"
        "                Set `True` to fill the tip.\n"
        ":double_headed: `bool`\n"
        "                Set `True` to draw a tip on both ends of\n"
        "                the shaft.\n";
  arrow_style.def(py::init<LineStyle,
                  double, double, bool, bool>(), doc.c_str(),
           py::arg("line_style"),
           py::arg("tip_length") = 0.1,
           py::arg("tip_angle") = 30.0,
           py::arg("tip_closed") = false,
           py::arg("double_headed") = false)
      .def(py::init<>(), "Creates a default, library-wide preset arrow style.\n"
                 "Note that this default style is NOT the same as the one\n"
                 "you can set per painter. For that style,\n"
                 "see `Painter.set_default_arrow_style()`.")
      .def("copy", [](const ArrowStyle &st) { return ArrowStyle(st); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const ArrowStyle &st)
           { return FullyQualifiedType(st.ToString(), true); })
      .def("__str__", &ArrowStyle::ToString)
      .def(py::pickle(&ArrowStyleToTuple,
                      &ArrowStyleFromTuple))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_valid", &ArrowStyle::IsValid,
           "Check if the style would lead to a drawable arrow.")
      .def("tip_length_for_shaft",
           static_cast<double (ArrowStyle::*)(double) const>(&ArrowStyle::TipLengthForShaft),
           "Computes the length of the arrow head/tip for the given shaft length.",
           py::arg("shaft_length"))
      .def("tip_length_for_shaft",
           static_cast<double (ArrowStyle::*)(const Vec2d&, const Vec2d&) const>(&ArrowStyle::TipLengthForShaft),
           "Computes the length of the arrow head/tip for the given shaft.",
           py::arg("shaft_from"), py::arg("shaft_to"))
      .def_readwrite("tip_length", &ArrowStyle::tip_length,
           "Length of the tip. If this value is in [0, 1], the tip\n"
           "length will be this fraction of the shaft length.\n"
           "Otherwise, this value specifies the absolute tip length\n"
           "in pixels.")
      .def_readwrite("tip_angle", &ArrowStyle::tip_angle,
           "Angle between tip lines and the shaft in degrees.")
      .def_readwrite("tip_closed", &ArrowStyle::tip_closed,
           "If `True`, the arrow head/tip will be filled. Otherwise,\n"
           "the tip will be open.")
      .def_readwrite("double_headed", &ArrowStyle::double_headed,
           "If `True`, heads/tips will be drawn on both ends of the line.");

  // An ArrowStyle can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, ArrowStyle>();
}


//-------------------------------------------------  BoundingBox2DStyle
py::tuple BoundingBox2DStyleToTuple(const BoundingBox2DStyle &st) {
  return py::make_tuple(st.line_style, st.text_style,
                        st.box_fill_color, st.text_fill_color,
                        st.label_position, st.label_padding,
                        st.clip_label);
}


BoundingBox2DStyle BoundingBox2DStyleFromTuple(py::tuple tpl) {
  // Convert empty tuple to pre-defined default style
  if (tpl.empty()) {
    return BoundingBox2DStyle();
  }

//  if (tpl.size() > 5) {
//    std::ostringstream s;
//    s << "Cannot create " << FullyQualifiedType("LineStyle")
//      << " from tuple with " << tpl.size()
//      << (tpl.size() == 1 ? " entry!" : " entries!");
//    throw std::invalid_argument(s.str());
//  }

//  LineStyle ls(tpl[0].cast<double>(),
//                        tpl[1].cast<Color>());

//  if (tpl.size() > 2) {
//    ls.dash_pattern = tpl[2].cast<std::vector<double>>();
//  }

//  if (tpl.size() > 3) {
//    ls.line_cap = tpl[3].cast<LineCap>();
//  }

//  if (tpl.size() > 4) {
//    ls.line_join = tpl[4].cast<LineJoin>();
//  }

//  return ls;
  throw std::runtime_error("TODO: Not yet implemented");
}


void RegisterBoundingBox2DStyle(py::module &m) {
  std::string doc = "How to draw a 2D bounding box.";
  py::class_<BoundingBox2DStyle>bbox_style(m, "BoundingBox2DStyle", doc.c_str());

  doc = "Initialize from `tuple`:\n TODO see e.g. TextStyle";
  bbox_style.def(py::init<>(&BoundingBox2DStyleFromTuple), doc.c_str(),
                 py::arg("tpl"));

//  //FIXME
//  doc = "Customize your text style:\n"
//        ":line_width:    float\n"
//        ":color:         " + FullyQualifiedType("Color") + "\n"
//        ":dash_pattern:  list[float]\n"
//        ":line_cap:      " + FullyQualifiedType("LineCap") + "\n"
//        ":line_join:     " + FullyQualifiedType("LineJoin");
//  text_style.def(py::init<unsigned int, const std::string &,
//                          const Color &, bool, bool, double,
//                          HorizontalAlignment>(),
//                 "TODO doc",
//                 py::arg("font_size"),
//                 py::arg("font_family"),
//                 py::arg("color") = Color::Black,
//                 py::arg("bold") = false,
//                 py::arg("italic") = false,
//                 py::arg("line_spacing") = 1.2,
//                 py::arg("alignment") = HorizontalAlignment::Left);

  bbox_style.def(py::init<>(), "Creates a default, library-wide preset text style.")
      .def("copy", [](const BoundingBox2DStyle &st) { return BoundingBox2DStyle(st); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const BoundingBox2DStyle &st)
           { return "<" + st.ToString() + ">"; })
      .def("__str__", &BoundingBox2DStyle::ToString)
      .def(py::pickle(&BoundingBox2DStyleToTuple,
                      &BoundingBox2DStyleFromTuple))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_valid", &BoundingBox2DStyle::IsValid,
           "Check if the style allows rendering a 2D bounding box.")
      .def_readwrite("line_style", &BoundingBox2DStyle::line_style,
           "How to draw the bounding box contour.")
      .def_readwrite("text_style", &BoundingBox2DStyle::text_style,
           "How to render the label.")
      .def_readwrite("box_fill_color", &BoundingBox2DStyle::box_fill_color,
           "Fill color of the bounding box.")
      .def_readwrite("box_fill_color", &BoundingBox2DStyle::box_fill_color,
           "Fill color of the bounding box.")
      .def_readwrite("text_fill_color", &BoundingBox2DStyle::text_fill_color,
           "Fill color of the text box (label background).")
      .def_readwrite("label_position", &BoundingBox2DStyle::label_position,
           "Where to place the label within the box.")
      .def_readwrite("label_padding", &BoundingBox2DStyle::label_padding,
           "Padding between bounding box edges and label text.")
      .def_readwrite("clip_label", &BoundingBox2DStyle::clip_label,
           "Whether to clip the label at the bounding box edges.");
}
} // namespace bindings
} // namespace viren2d

