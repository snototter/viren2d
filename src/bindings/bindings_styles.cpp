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

  std::string doc = "Parses a string into a :class:`"
      + FullyQualifiedType("LineCap") + "`.\n\n"
      "**Corresponding C++ API:** ``viren2d::LineCapFromString``.";
  m.def("lcap", LineCapFromString,
        doc.c_str(), py::arg("cap_str"));
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

  std::string doc = "Parses a string into a :class:`"
      + FullyQualifiedType("LineJoin") + "`.\n\n"
      "**Corresponding C++ API:** ``viren2d::LineJoinFromString``.";
  m.def("ljoin", LineJoinFromString,
        doc.c_str(), py::arg("join_str"));
}


void RegisterMarker(pybind11::module &m) {
  py::enum_<Marker>(m, "Marker", "Marker shape enumeration.")
      .value("Point", Marker::Point,
             "Point, *i.e.* a filled circle, char representation: ``'.'``.")
      .value("Circle", Marker::Circle,
             "A circle (not filled), char representation: ``'o'``.")
      .value("Plus", Marker::Plus,
             "Plus sign, char representation: ``'+'``.")
      .value("Cross", Marker::Cross,
             "Cross marker, char representation: ``'x'``.")
      .value("Square", Marker::Square,
             "Square marker, char representation: ``'s'``.")
      .value("RotatedSquare", Marker::RotatedSquare,
             "Rotated square (a thick diamond), char representation: ``'S'``.")
      .value("Diamond", Marker::Diamond,
             "Diamond marker, char representation: ``'d'``.")
      .value("Star", Marker::Star,
             "Five-pointed star (Asterisk), char representation: ``'*'``.")
      .value("Pentagon", Marker::Pentagon,
             "Five-sided polygon, char representation: ``'p'``.")
      .value("Pentagram", Marker::Pentagram,
             "Five-pointed star, char representation: ``'5'``.")
      .value("Hexagon", Marker::Hexagon,
             "Six-sided polygon, char representation: ``'h'``.")
      .value("Hexagram", Marker::Hexagram,
             "Six-pointed star, char representation: ``'6'``.")
      .value("Heptagon", Marker::Heptagon,
             "Seven-sided polygon, char representation: ``'H'``.")
      .value("Heptagram", Marker::Heptagram,
             "Seven-pointed star, char representation: ``'7'``.")
      .value("Octagon", Marker::Octagon,
             "Eight-sided polygon, char representation: ``'O'``.")
      .value("Octagram", Marker::Octagram,
             "Eight-pointed star, char representation: ``'8'``.")
      .value("Enneagon", Marker::Enneagon,
             "Nine-sided polygon (Nonagon), char representation: ``'n'``.")
      .value("Enneagram", Marker::Enneagram,
             "Nine-pointed star, char representation: ``'9'``.")
      .value("TriangleUp", Marker::TriangleUp,
             "Upward-pointing triangle marker, char representation: ``'^'``.")
      .value("TriangleDown", Marker::TriangleDown,
             "Downward-pointing triangle marker, char representation: ``'v'``.")
      .value("TriangleLeft", Marker::TriangleLeft,
             "Left-pointing triangle marker, char representation: ``'<'``.")
      .value("TriangleRight", Marker::TriangleRight,
             "Right-pointing triangle marker, char representation: ``'>'``.");

  std::string doc = "Parses a character into a :class:`"
      + FullyQualifiedType("Marker") + "`.\n\n"
      "See :func:`" + FullyQualifiedType("marker_codes")
      + "` for a list of supported character representations.\n\n"
      "**Corresponding C++ API:** ``viren2d::MarkerFromChar`` and\n"
      "``viren2d::ListMarkers``.";
  m.def("marker", MarkerFromChar, doc.c_str(), py::arg("rep"));
}


void SetMarkerFromPyObject(MarkerStyle &s, py::object &o) {
  if (py::isinstance<py::str>(o)) {
    const auto str = py::cast<std::string>(o);
    s.marker = MarkerFromChar(str[0]);
  } else if (py::isinstance<Marker>(o)) {
    s.marker = py::cast<Marker>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast value of type `"
        << tp << "` to " << FullyQualifiedType("Marker")
        << "!";
    throw std::invalid_argument(str.str());
  }
}

void RegisterMarkerStyle(pybind11::module &m) {
  std::string doc = "How a marker/keypoint should be drawn.";
  py::class_<MarkerStyle> style(m, "MarkerStyle", doc.c_str());

  //TODO init from tuple
  //TODO serialize
  //TODO implicit conversion
//  doc = "TODO from tuple";
//  style.def(py::init<>(&LineStyleFromTuple), doc.c_str());

  doc = "Create a customized style.\n\n"
        "Args:\n"
        "  marker: Character code of the marker type, see :class:`" + FullyQualifiedType("Marker")
      + "`.\n  size: Marker size in pixels as ``float``.\n"
        "  thickness: Line thickness in pixels as ``float``. Will be\n"
        "    ignored if you choose a ``fill``\\ ed marker.\n"
        "  color: Marker color as :class:`" + FullyQualifiedType("Color") + "`.\n"
        "  fill: If the marker shape allows, you can choose between filling\n"
        "    (``True``) or drawing only the outline (``False``). For *non-fillable*\n"
        "    (*e.g.* ``'+'``, ``'x'``, *etc.*) or *always-filled* shapes (*e.g.* ``'.'``), the\n"
        "    value of ``fill`` will be ignored.\n"
        "  line_cap: How to draw the line endpoints, as :class:`" + FullyQualifiedType("LineCap") + "`.\n"
        "  line_join: How to draw line junctions, as :class:`" + FullyQualifiedType("LineJoin") + "`.";
  style.def(py::init<char, double, double, Color, bool,
                          LineCap, LineJoin>(),
            doc.c_str(),
            py::arg("marker") = 'o',
            py::arg("size") = 20.0,
            py::arg("thickness") = 3.0,
            py::arg("color") = Color(NamedColor::MidnightBlue),
            py::arg("fill") = false,
            py::arg("line_cap") = LineCap::Butt,
            py::arg("line_join") = LineJoin::Miter);

  style.def("copy", [](const MarkerStyle &st) { return MarkerStyle(st); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const MarkerStyle &st) {
             return "<" + st.ToString() + ">";
           })
      .def("__str__",
           [](const MarkerStyle &st) {
             return MarkerToChar(st.marker);
           })
//      .def(py::pickle(&MarkerStyleToTuple,//FIXME implement!
//                      &MarkerStyleFromTuple))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_filled", &MarkerStyle::IsFilled,
           "Returns ``True`` if this marker will be filled.\n\n"
           "Note that this **may differ** from its :attr:`filled`\n"
           "member: Some marker shapes *cannot* be filled (*e.g*\n"
           "``'+'`` or ``'o'``), whereas some shapes *must* be"
           "filled (*e.g.* ``'.'``).")
      .def("is_valid", &MarkerStyle::IsValid,
           "Checks if this style would lead to a drawable marker.");

  doc = ":class:`~" + FullyQualifiedType("Color") + "`: Color of the marker's contour or fill (see :attr:`filled`).";
  style.def_readwrite("color", &MarkerStyle::color, doc.c_str());
//FIXME FIXME FIXME
  // use a lambda to set it!
  // lambda --> markerstyle --> ok; character --> call marker!
  // todo can we use the same for other methods (like text anchor conversion?) - don't think so :/
  doc = ":class:`~" + FullyQualifiedType("Marker") + "`: "
        "Marker shape.\n\n"
"In addition to the enum values, you can use\n"
"the corresponding marker code to set this member:\n"
"  >>> style.marker = viren2d.Marker.Cross\n"
"  >>> style.marker = 'x'";
  style.def_property("marker",
        [](MarkerStyle &s) { return s.marker; },
        [](MarkerStyle &s, py::object o) { SetMarkerFromPyObject(s, o); },
        doc.c_str());
//  style.def_readwrite("marker", &MarkerStyle::marker, doc.c_str());

  doc = "float: Thickness of the marker's contour. May be ignored if the shape\n"
        "is fillable and you set :attr:``filled`, *i.e.* fill will be prefered\n"
        "over just drawing the outline.";
  style.def_readwrite("thickness", &MarkerStyle::thickness, doc.c_str());

  style.def_readwrite("size", &MarkerStyle::size,
        "float: Marker size in pixels.");

  style.def_readwrite("filled", &MarkerStyle::filled,
        "bool: If ``True``, the marker should be filled with\n"
        ":attr:`color`.\n\n"
        "Note that some marker shapes *cannot* be filled (*e.g*\n"
        "``'+'`` or ``'o'``), whereas some shapes *must* be"
        "filled (*e.g.* ``'.'``). For these shapes, the value\n"
        "of :attr:`filled` will be ignored.");

  doc = ":class:`~" + FullyQualifiedType("LineCap") + "`: "
        "How to render the endpoints contour lines.";
  style.def_readwrite("line_cap", &MarkerStyle::line_cap, doc.c_str());

  doc = ":class:`~" + FullyQualifiedType("LineJoin") + "`: "
        "How to render the junctions of the contour segments.";
  style.def_readwrite("line_cap", &MarkerStyle::line_cap, doc.c_str());

//  // A LineStyle can be initialized from a given tuple.
//  py::implicitly_convertible<py::tuple, MarkerStyle>();


  doc = "Lists the character codes of all :class:~`"
      + FullyQualifiedType("Marker") + "` shapes.\n\n"
      "**Corresponding C++ API:** ``viren2d::ListMarkers``.";
  m.def("marker_codes", ListMarkers, doc.c_str());
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
  line_style.def(py::init<>(&LineStyleFromTuple), doc.c_str());

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
      .def_readwrite("line_cap", &LineStyle::line_cap,//TODO doc with class-ref!
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
  arrow_style.def(py::init<>(&ArrowStyleFromTuple), doc.c_str());

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
} // namespace bindings
} // namespace viren2d

