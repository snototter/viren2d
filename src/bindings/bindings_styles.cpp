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
  return py::make_tuple(
        ls.width, ls.color, ls.dash_pattern,
        ls.cap, ls.join);
}


py::dict LineStyleToDict(const LineStyle &ls) {
  py::dict d;
  d["width"] = ls.width;
  d["color"] = ls.color;
  d["dash_pattern"] = ls.dash_pattern;
  d["cap"] = ls.cap;
  d["join"] = ls.join;
  return d;
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
    ls.cap = tpl[3].cast<LineCap>();
  }

  if (tpl.size() > 4) {
    ls.join = tpl[4].cast<LineJoin>();
  }

  return ls;
}


void RegisterLineCap(pybind11::module &m) {
  py::enum_<LineCap> cap(m, "LineCap",
             "Enum specifying how to render the endpoints of a line/dash stroke.");
  cap.value("Butt", LineCap::Butt,
          "Start/stop the line exactly at the start/end point.")
      .value("Round", LineCap::Round,
          "Round ending, center of the circle is the end point.")
      .value("Square", LineCap::Square,
          "Square ending, center of the square is the end point.");

  // .export_values() should be skipped for strongly typed enums

  cap.def(
        "__str__", [](LineCap c) -> py::str {
            return py::str(LineCapToString(c));
        }, py::name("__str__"), py::is_method(m));

  cap.def(
        "__repr__", [](LineCap c) -> py::str {
            std::ostringstream s;
            s << "<LineCap." << LineCapToString(c) << '>';
            return py::str(s.str());
        }, py::name("__repr__"), py::is_method(m));
}


LineCap LineCapFromPyObject(py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return LineCapFromString(py::cast<std::string>(o));
  } else if (py::isinstance<LineCap>(o)) {
    return py::cast<LineCap>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp << "` to `"
        << FullyQualifiedType("LineCap") << "`!";
    throw std::invalid_argument(str.str());
  }
}


void RegisterLineJoin(pybind11::module &m) {
  py::enum_<LineJoin> join(m, "LineJoin",
             "Enum specifying how to render the junction of two lines/segments.");

  join.value("Miter", LineJoin::Miter,
             "Sharp (angled) corner.")
      .value("Bevel", LineJoin::Bevel,
             "The join is cut off at half the line width from the joint point.")
      .value("Round", LineJoin::Round,
             "Rounded join, where the center of the circle is the joint point.");

  join.def(
        "__str__", [](LineJoin j) -> py::str {
            return py::str(LineJoinToString(j));
        }, py::name("__str__"), py::is_method(m));

  join.def(
        "__repr__", [](LineJoin j) -> py::str {
            std::ostringstream s;
            s << "<LineJoin." << LineJoinToString(j) << '>';
            return py::str(s.str());
        }, py::name("__repr__"), py::is_method(m));
}


LineJoin LineJoinFromPyObject(py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return LineJoinFromString(py::cast<std::string>(o));
  } else if (py::isinstance<LineJoin>(o)) {
    return py::cast<LineJoin>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp << "` to `"
        << FullyQualifiedType("LineJoin") << "`!";
    throw std::invalid_argument(str.str());
  }
}


void RegisterMarker(pybind11::module &m) {
  py::enum_<Marker>(m, "Marker",
                    "Enum specifying the marker shape.")
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
             "Rotated square (a thick diamond), char representation: ``'r'``.")
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
             "Eight-sided polygon, char representation: ``'0'`` (*i.e.* zero).")
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
}


Marker MarkerFromPyObject(py::object &o) {
  if (py::isinstance<py::str>(o)) {
    const auto str = py::cast<std::string>(o);
    return MarkerFromChar(str[0]);
  } else if (py::isinstance<Marker>(o)) {
    return py::cast<Marker>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp << "` to `"
        << FullyQualifiedType("Marker") << "`!";
    throw std::invalid_argument(str.str());
  }
}


py::tuple MarkerStyleToTuple(const MarkerStyle &s) {
  return py::make_tuple(
        s.marker, s.size, s.thickness, s.color,
        s.filled, s.cap, s.join);
}


MarkerStyle MarkerStyleFromTuple(py::tuple tpl) {
  // Convert empty tuple to pre-defined default style
  if (tpl.empty()) {
    return MarkerStyle();
  }

  if (tpl.size() > 7) {
    std::ostringstream s;
    s << "Cannot create " << FullyQualifiedType("MarkerStyle")
      << " from tuple with " << tpl.size()
      << " entries (expected max. 7)!";
    throw std::invalid_argument(s.str());
  }

  MarkerStyle ms;
  ms.marker = tpl[0].cast<Marker>();

  if (tpl.size() > 1) {
    ms.size = tpl[1].cast<double>();
  }

  if (tpl.size() > 2) {
    ms.thickness = tpl[2].cast<double>();
  }

  if (tpl.size() > 3) {
    ms.color = tpl[3].cast<Color>();
  }

  if (tpl.size() > 4) {
    ms.filled = tpl[4].cast<bool>();
  }

  if (tpl.size() > 5) {
    ms.cap = tpl[5].cast<LineCap>();
  }

  if (tpl.size() > 6) {
    ms.join = tpl[6].cast<LineJoin>();
  }

  return ms;
}


void RegisterMarkerStyle(pybind11::module &m) {
  std::string doc = R"docstr(
      How a marker/keypoint should be drawn.

      **Example:**

      >>> # Initialize the default style and adjust what you need:
      >>> style = viren2d.MarkerStyle()
      >>> style.marker = '*'
      >>> style.size = 10
      >>> style.color = 'navy-blue!80'
      >>> style.filled = True

      >>> # Alternatively, you would get the same style via:
      >>> style = viren2d.MarkerStyle(
      >>>     marker=viren2d.Marker.Star, size=10,
      >>>     color='navy-blue!80', filled=True)
      )docstr";
  py::class_<MarkerStyle> style(m, "MarkerStyle", doc.c_str());

  style.def(py::pickle(&MarkerStyleToTuple, &MarkerStyleFromTuple),
            ":class:`~viren2d.MarkerStyle` instances can be pickled.");

  MarkerStyle default_style;
  doc = R"docstr(
      Creates a customized marker style.

      Args:
        marker: Shape as :class:`~viren2d.Marker` enum.
        size: Marker size in pixels as :class:`float`.
        thickness: Width/thickness of the contour in pixels
          as :class:`float`.
        color: The :class:`~viren2d.Color` used for drawing its
          contour or filling.
        filled: If ``True`` (and the shape allwos), the marker
          will be filled.
        cap: A :class:`~viren2d.LineCap` enum, specifying
          how to render the line endpoints.
        join: A :class:`~viren2d.LineJoin` enum, specifying
          how to render the junctions of multi-segment lines.
      )docstr";
  style.def(py::init<Marker, double, double, Color, bool, LineCap, LineJoin>(),
            doc.c_str(),
            py::arg("marker") = default_style.marker,
            py::arg("size") = default_style.size,
            py::arg("thickness") = default_style.thickness,
            py::arg("color") = default_style.color,
            py::arg("filled") = default_style.filled,
            py::arg("cap") = default_style.cap,
            py::arg("join") = default_style.join);

  style.def("copy", [](const MarkerStyle &st) { return MarkerStyle(st); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const MarkerStyle &st) {
             return "<" + st.ToString() + ">";
           })
      .def("__str__",
           [](const MarkerStyle &st) {
               std::ostringstream s;
               s << "'" << MarkerToChar(st.marker) << "'";
               return s.str();
           })

      .def(py::self == py::self, "Checks for equality.")
      .def(py::self != py::self, "Checks for inequality.")
      .def("is_filled", &MarkerStyle::IsFilled,
           "Returns ``True`` if this marker would be filled.\n\n"
           "Note that this **may differ** from its :attr:`filled`\n"
           "member: Some marker shapes *cannot* be filled (*e.g*\n"
           "``'+'`` or ``'o'``), whereas some shapes *must* be"
           "filled (*e.g.* ``'.'``).")
      .def("is_valid", &MarkerStyle::IsValid,
           "Checks if this style would lead to a drawable marker.");

  doc = ":class:`~" + FullyQualifiedType("Color") + "`: Color of the marker's contour or fill (see :attr:`filled`).";
  style.def_readwrite("color", &MarkerStyle::color, doc.c_str());

  doc = ":class:`~" + FullyQualifiedType("Marker") + "`: "
        "Marker shape.\n\n"
        "In addition to the enum values, you can use\n"
        "the character codes to set this member:\n\n"
        ">>> style.marker = viren2d.Marker.Cross\n"
        ">>> style.marker = 'x'\n";
  style.def_property("marker",
        [](MarkerStyle &s) { return s.marker; },
        [](MarkerStyle &s, py::object o) {
            s.marker = MarkerFromPyObject(o);
        }, doc.c_str());

  doc = "float: Thickness of the marker's contour. May be ignored if the shape\n"
        "is fillable and you set :attr:`filled`, *i.e.* filling takes\n"
        "precedence over drawing the outline.";
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
        "How to render the endpoints of the marker's contour.\n\n"
        "In addition to the enum values, you can use\n"
        "the corresponding string representation to set this member:\n\n"
        ">>> style.cap = viren2d.LineCap.Round\n"
        ">>> style.cap = 'round'\n";
  style.def_property("cap",
        [](MarkerStyle &s) { return s.cap; },
        [](MarkerStyle &s, py::object o) {
            s.cap = LineCapFromPyObject(o);
        }, doc.c_str());


  doc = ":class:`~" + FullyQualifiedType("LineJoin") + "`: "
        "How to render the junctions of the marker's contour.\n\n"
        "In addition to the enum values, you can use\n"
        "the corresponding string representation to set this member:\n\n"
        ">>> style.join = viren2d.LineJoin.Miter\n"
        ">>> style.join = 'miter'\n";
  style.def_property("join",
        [](MarkerStyle &s) { return s.join; },
        [](MarkerStyle &s, py::object o) {
            s.join = LineJoinFromPyObject(o);
        }, doc.c_str());


  doc = "Lists the character codes of all :class:`~"
      + FullyQualifiedType("Marker") + "` shapes.\n\n"
      "**Corresponding C++ API:** ``viren2d::ListMarkers``.";
  m.def("marker_codes", ListMarkers, doc.c_str());
}


void RegisterLineStyle(pybind11::module &m) {
  std::string doc = R"docstr(
      How a line/contour should be drawn.

      Note that several ``draw_xxx`` methods of the
      :class:`~viren2d.Painter` also accept the special
      member :attr:`~viren2d.LineStyle.Invalid`, which
      indicates that a shape should only be filled,
      but it's contour should not be drawn.

      Example:

      >>> # Initialize the default style and adjust what you need:
      >>> style = viren2d.LineStyle()
      >>> style.width = 7
      >>> style.color = 'crimson'
      >>> style.cap = 'round'
      >>> style.dash_pattern = [20, 10]

      >>> # Alternatively, you would get the same style via:
      >>> style = viren2d.LineStyle(
      >>>     width=7, color='crimson',
      >>>     cap='round', dash_pattern=[20, 10])

      Note that depending on the selected :attr:`cap`
      (or :attr:`join`), the corresponding line (or joints)
      may start/end *not exactly* where you specified.
      If you want pixel-accurate start/end in combination with
      a particular cap/join, use
      :meth:`~viren2d.LineStyle.cap_offset` (or
      :meth:`~viren2d.LineStyle.join_offset`). For example:

      >>> # Line style with a cap that would result in an offset:
      >>> line_style = viren2d.LineStyle()
      >>> line_style.cap = 'round'
      >>> # Now, we want to draw a line exactly from `start` to `end`:
      >>> start = viren2d.Vec2d(10, 10)
      >>> end = viren2d.Vec2d(100, 100)
      >>> unit_dir = start.direction_vector(end).unit_vector()
      >>> start += line_style.cap_offset() * unit_dir
      >>> end -= line_style.cap_offset() * unit_dir
      )docstr";
  py::class_<LineStyle>line_style(m, "LineStyle", doc.c_str());

  doc = R"docstr(
      Returns a dictionary representation.

      Convenience method to initialize an
      :class:`~viren2d.ArrowStyle` from a
      :class:`~viren2d.LineStyle` via *dictionary
      unpacking* (aka *splat*):

      >>> arrow_style = viren2d.ArrowStyle(
      >>>     **line_style.as_dict(),
      >>>     tip_length=0.3)
      )docstr";
  line_style.def("as_dict", [](const LineStyle &s) -> py::dict {
    return LineStyleToDict(s);
  }, doc.c_str());


  line_style.def("detailed_str", &LineStyle::ToDetailedString,
                 "Returns a verbose string representation to facilitate deeper inspection.");


  doc = "Creates a customized line style.\n\n"
        "Args:\n"
        "  width: Width in pixels as :class:`float`.\n"
        "  color: Line color as :class:`~" + FullyQualifiedType("Color") + "`.\n"
        "  dash_pattern: Dash pattern defined as ``List[float]`` of on/off strokes,\n"
        "    refer to the class member :attr:`dash_pattern` for details.\n"
        "  cap: A :class:`~" + FullyQualifiedType("LineCap") + "` enum, specifying\n"
        "    how to render the line's endpoints.\n"
        "  join: A :class:`~" + FullyQualifiedType("LineJoin") + "` enum, specifying\n"
        "    how to render the junctions of multi-segment lines.";
  LineStyle default_style;
  line_style.def(py::init<double, Color, std::vector<double>, LineCap, LineJoin>(),
        doc.c_str(),
        py::arg("width") = default_style.width,
        py::arg("color") = default_style.color,
        py::arg("dash_pattern") = default_style.dash_pattern,
        py::arg("cap") = default_style.cap,
        py::arg("join") = default_style.join);


  line_style.def("copy", [](const LineStyle &st) { return LineStyle(st); },
        "Returns a deep copy.")
      .def("__repr__", [](const LineStyle &) { return "<viren2d.LineStyle>";})
      .def("__str__", &LineStyle::ToString)
      .def(py::pickle(&LineStyleToTuple, &LineStyleFromTuple),
           ":class:`~viren2d.LineStyle` instances can be pickled.")
      .def(py::self == py::self, "Checks for equality.")
      .def(py::self != py::self, "Checks for inequality.")
      .def("is_valid", &LineStyle::IsValid,
        "Checks if the style would lead to a drawable line.")
      .def("is_dashed", &LineStyle::IsDashed,
        "Checks if this style contains a dash stroke pattern.")
      .def("cap_offset", &LineStyle::CapOffset,
        "Computes how much the line cap will extend the\n"
        "line's start/end.")
      .def("join_offset", &LineStyle::JoinOffset,
        "Computes how much a line join will extend the joint.\n\n"
        "The ``interior_angle`` is the angle between two line segments\n"
        "in degrees.\n"
        "This method needs to know the ``miter_limit`` because Cairo switches\n"
        "from ``MITER`` to ``BEVEL`` if the ``miter_limit`` is exceeded.\n"
        "Refer to the "
        "`Cairo documentation <https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-miter-limit>`__ "
        "for details.",
        py::arg("interior_angle"),
        py::arg("miter_limit") = 10.0)
      .def_readwrite("dash_pattern", &LineStyle::dash_pattern,
        "list[float]: Dash pattern defined as list of on/off strokes.\n\n"
        "A dash pattern is a list of positive values. Each value defines\n"
        "the length (in pixels) of alternating *on* and *off* segments of\n"
        "the line. For solid lines, this list must be empty.\n\n"
        ">>> style.dash_pattern = [20, 30, 40, 10] # Would result in:\n"
        "'__   ____ __   ____ __   ____ __   ____ __   ____ __   ____ ...'\n")
      .def_readwrite("width", &LineStyle::width,
           "float: Width/thickness in pixels.")
      // missing doc string: https://github.com/pybind/pybind11/issues/3815
      .def_readonly_static("Invalid", &LineStyle::Invalid, R"doc(
        Read-only special member used to skip drawing contours.

        Several drawing methods of the :class:`~viren2d.Painter` support
        only filling a shape (*i.e.* skipping the contour). For example:

        >>> painter.draw_rect(rect, viren2d.LineStyle.Invalid, 'blue!40')
        )doc");

  doc = ":class:`~" + FullyQualifiedType("LineCap") + "`: "
        "How to render the endpoints of the line (or dash strokes).\n\n"
        "In addition to the enum values, you can use\n"
        "the corresponding string representation to set this member:\n\n"
        ">>> style.cap = viren2d.LineCap.Round\n"
        ">>> style.cap = 'round'\n";
  line_style.def_property("cap",
        [](LineStyle &s) { return s.cap; },
        [](LineStyle &s, py::object o) {
            s.cap = LineCapFromPyObject(o);
        }, doc.c_str());


  doc = ":class:`~" + FullyQualifiedType("LineJoin") + "`: "
        "How to render the junctions of the line segments.\n\n"
        "In addition to the enum values, you can use\n"
        "the corresponding string representation to set this member:\n\n"
        ">>> style.join = viren2d.LineJoin.Miter\n"
        ">>> style.cap = 'miter'\n";
  line_style.def_property("join",
        [](LineStyle &s) { return s.join; },
        [](LineStyle &s, py::object o) {
            s.join = LineJoinFromPyObject(o);
        }, doc.c_str());


  doc = ":class:`~" + FullyQualifiedType("Color") + "`: "
        "Color of the line.\n\nExample:\n\n"
        ">>> style.color = viren2d.Color(1, 0, 1)\n" //TODO update example & include in other style definitions!
        ">>> style.color = viren2d.RGB(255, 0, 255)\n"
        ">>> style.color = 'navy-blue'\n";
  line_style.def_readwrite("color", &LineStyle::color,
           doc.c_str());
}



//-------------------------------------------------  ArrowStyle
py::tuple ArrowStyleToTuple(const ArrowStyle &obj) {
  // Re-use LineStyle serialization:
  auto ls = static_cast<const LineStyle &>(obj);
  auto ls_tpl = LineStyleToTuple(ls);

  return py::make_tuple(ls_tpl, obj.tip_length,
                        obj.tip_angle, obj.tip_closed,
                        obj.double_headed);
}


py::dict ArrowStyleToDict(const ArrowStyle &obj) {
  auto lst = static_cast<const LineStyle &>(obj);
  py::dict d = LineStyleToDict(lst);
  d["tip_length"] = obj.tip_length;
  d["tip_angle"] = obj.tip_angle;
  d["tip_closed"] = obj.tip_closed;
  d["double_headed"] = obj.double_headed;
  return d;
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
  std::string doc = R"docstr(
      How an arrow should be drawn.

      **Example:**

      >>> # Initialize the default style and adjust what you need:
      >>> style = viren2d.ArrowStyle()
      >>> style.width = 5
      >>> style.color = 'black'
      >>> style.tip_length = 0.3
      >>> style.tip_angle = 20
      >>> style.tip_closed = True

      >>> # Alternatively, you would get the same style via:
      >>> style = viren2d.ArrowStyle(
      >>>     width=5, color='black',
      >>>     tip_length=0.3, tip_angle=20,
      >>>     tip_closed=True)
      )docstr";
  py::class_<ArrowStyle, LineStyle> arrow_style(m, "ArrowStyle", doc.c_str());


  doc = R"docstr(
      Creates a customized arrow style.

      Args:
        width: Width in pixels as :class:`float`.
        color: Arrow color as :class:`~viren2d.Color`.
        tip_length: Length of the arrow tip as :class:`float`. If it
          is between ``[0, 1]``, it is interpreted as percentage
          of the shaft length. Otherwise, it represents the absolute
          length in pixels.
        tip_angle: Interior angle between shaft and tip, specified
          in degrees as :class:`float`.
        tip_closed: Set ``True`` to fill the tip (type :class:`bool`).
        double_headed: Set `True` to draw arrow tips on both ends
          of the shaft (type :class:`bool`).
        dash_pattern: Dash pattern defined as ``List[float]`` of
          on/off strokes. Refer to the documentation of the class
          member :attr:`dash_pattern` for details.
        cap: A :class:`~viren2d.LineCap` enum, specifying
          how to render the line's endpoints.
        join: A :class:`~viren2d.LineJoin` enum, specifying
          how to render the junctions of multi-segment lines.
        )docstr";
  ArrowStyle default_style;
  arrow_style.def(py::init<double, Color,
                           double, double, bool, bool, std::vector<double>,
                           LineCap, LineJoin>(), doc.c_str(),
         py::arg("width") = default_style.width,
         py::arg("color") = default_style.color,
         py::arg("tip_length") = default_style.tip_length,
         py::arg("tip_angle") = default_style.tip_angle,
         py::arg("tip_closed") = default_style.tip_closed,
         py::arg("double_headed") = default_style.double_headed,
         py::arg("dash_pattern") = default_style.dash_pattern,
         py::arg("cap") = default_style.cap,
         py::arg("join") = default_style.join);


  arrow_style.def("as_dict", [](const ArrowStyle &s) -> py::dict {
    return ArrowStyleToDict(s);
  }, "Returns a dictionary representation.");

  arrow_style.def("copy", [](const ArrowStyle &st) { return ArrowStyle(st); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const ArrowStyle &st)
           { return FullyQualifiedType(st.ToString(), true); })
      .def("__str__", &ArrowStyle::ToString)
      .def(py::pickle(&ArrowStyleToTuple, &ArrowStyleFromTuple),
           ":class:`~viren2d.ArrowStyle` instances can be pickled.")
      .def(py::self == py::self, "Checks for equality.")
      .def(py::self != py::self, "Checks for inequality.")
      .def("is_valid", &ArrowStyle::IsValid,
           "Checks if the style would lead to a drawable arrow.")
      .def("tip_length_for_shaft",
           static_cast<double (ArrowStyle::*)(double) const>(&ArrowStyle::TipLengthForShaft),
           "Returns the length of the arrow head for the given shaft length.", py::arg("shaft_length"))
//      .def("tip_length_for_shaft",
//           static_cast<double (ArrowStyle::*)(const Vec2d&, const Vec2d&) const>(&ArrowStyle::TipLengthForShaft),
//           "Returns the length of the arrow head for the given shaft.",
//           py::arg("shaft_from"), py::arg("shaft_to"))
      .def_readwrite("tip_length", &ArrowStyle::tip_length, R"docstr(
          float: Length of the arrow tip.

          If the value is between ``[0, 1]``, it is interpreted as
          percentage of the arrow's shaft length. Otherwise, it
          represents the absolute length in pixels.
          )docstr")
      .def_readwrite("tip_angle", &ArrowStyle::tip_angle,
          "float: Interior angle (in degrees) between shaft and tip.")
      .def_readwrite("tip_closed", &ArrowStyle::tip_closed,
           "bool: If ``True``, the arrow head will be filled.")
      .def_readwrite("double_headed", &ArrowStyle::double_headed,
           "bool: If ``True``, arrow heads will be drawn on both ends.");
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
//    ls.cap = tpl[3].cast<LineCap>();
//  }

//  if (tpl.size() > 4) {
//    ls.join = tpl[4].cast<LineJoin>();
//  }

//  return ls;
  throw std::runtime_error("TODO: Not yet implemented");
}


void RegisterBoundingBox2DStyle(py::module &m) {
  std::string doc = "How to draw a 2D bounding box.";
  py::class_<BoundingBox2DStyle>bbox_style(m, "BoundingBox2DStyle", doc.c_str());

  //  //FIXME remove unnecessarily overloaded constructions
    // ---> major workflow: create default style & change members
    // ---> alternative: specify values directly in (a single) constructor

  doc = "Initialize from `tuple`:\n TODO see e.g. TextStyle";
  bbox_style.def(py::init<>(&BoundingBox2DStyleFromTuple), doc.c_str(),
                 py::arg("tpl"));

//  doc = "Customize your text style:\n"
//        ":width:    float\n"
//        ":color:         " + FullyQualifiedType("Color") + "\n"
//        ":dash_pattern:  list[float]\n"
//        ":cap:      " + FullyQualifiedType("LineCap") + "\n"
//        ":join:     " + FullyQualifiedType("LineJoin");
//  text_style.def(py::init<unsigned int, const std::string &,
//                          const Color &, bool, bool, double,
//                          HorizontalAlignment>(),
//                 "TODO doc",
//                 py::arg("size"),
//                 py::arg("family"),
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
      .def(py::pickle(&BoundingBox2DStyleToTuple, &BoundingBox2DStyleFromTuple),
           ":class:`~viren2d.BoundingBox2DStyle` instances can be pickled.")
      .def(py::self == py::self, "Checks for equality.")
      .def(py::self != py::self, "Checks for inequality.")
      .def("is_valid", &BoundingBox2DStyle::IsValid,
           "Check if the style allows rendering a 2D bounding box.");

  doc = ":class:`" + FullyQualifiedType("LineStyle")
      + "`: How to draw the bounding box contour.";
  bbox_style.def_readwrite("line_style", &BoundingBox2DStyle::line_style,
                           doc.c_str());

  doc = ":class:`" + FullyQualifiedType("TextStyle")
      + "`: How to render the label.";
  bbox_style.def_readwrite("text_style", &BoundingBox2DStyle::text_style,
                           doc.c_str());

  doc = ":class:`" + FullyQualifiedType("Color")
      + "`: Fill color of the bounding box.";
  bbox_style.def_readwrite("box_fill_color", &BoundingBox2DStyle::box_fill_color,
                           doc.c_str());

  //FIXME type doc needed for ALL members of all bound classes :/

  doc = ":class:`" + FullyQualifiedType("Color")
      + "`: Fill color of the text box (*i.e.* the label background).";
  bbox_style.def_readwrite("text_fill_color", &BoundingBox2DStyle::text_fill_color,
                           doc.c_str());

  doc = ":class:`" + FullyQualifiedType("BoundingBoxLabelPosition")
      + "`: Where to place the label within the box.";
  bbox_style.def_readwrite("label_position", &BoundingBox2DStyle::label_position,
                           doc.c_str());

  doc = ":class:`" + FullyQualifiedType("Vec2d")
      + "`: Padding between bounding box edges and label text.";
  bbox_style.def_readwrite("label_padding", &BoundingBox2DStyle::label_padding,
                           doc.c_str());

  bbox_style.def_readwrite("clip_label", &BoundingBox2DStyle::clip_label,
           "bool: Whether to clip the label at the bounding box edges.");
}
} // namespace bindings
} // namespace viren2d

