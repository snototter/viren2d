#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <viren2d/styles.h>
#include <bindings/binding_helpers.h>
namespace py = pybind11;

//TODO(doc) add corresponding c++ api to docstr

namespace viren2d {
namespace bindings {
//-------------------------------------------------  LineStyle
py::tuple LineStyleToTuple(const LineStyle &ls) {
  return py::make_tuple(
        ls.width, ls.color, ls.dash_pattern,
        ls.dash_offset, ls.cap, ls.join);
}


py::dict LineStyleToDict(const LineStyle &ls) {
  py::dict d;
  d["width"] = ls.width;
  d["color"] = ls.color;
  d["dash_pattern"] = ls.dash_pattern;
  d["dash_offset"] = ls.dash_offset;
  d["cap"] = ls.cap;
  d["join"] = ls.join;
  return d;
}


LineStyle LineStyleFromTuple(const py::tuple &tpl) {
  // Convert empty tuple to pre-defined default style
  if (tpl.empty()) {
    return LineStyle();
  }

  if ((tpl.size() < 2) || (tpl.size() > 6)) {
    std::ostringstream s;
    s << "Cannot create `viren2d.LineStyle` from tuple with "
      << tpl.size() << (tpl.size() == 1 ? " entry!" : " entries!");
    throw std::invalid_argument(s.str());
  }

  LineStyle ls(
        tpl[0].cast<double>(),
        tpl[1].cast<Color>());

  if (tpl.size() > 2) {
    ls.dash_pattern = tpl[2].cast<std::vector<double>>();
  }

  if (tpl.size() > 3) {
    ls.dash_offset = tpl[3].cast<double>();
  }

  if (tpl.size() > 4) {
    ls.cap = tpl[4].cast<LineCap>();
  }

  if (tpl.size() > 5) {
    ls.join = tpl[5].cast<LineJoin>();
  }

  return ls;
}


void RegisterLineCap(pybind11::module &m) {
  py::enum_<LineCap> cap(m, "LineCap",
             "Enumeration specifying how to render the endpoints of a line/dash stroke.");
  cap.value(
        "Butt",
        LineCap::Butt, R"docstr(
        Start/stop the line exactly at the start/end point.
        )docstr")
      .value(
        "Round",
        LineCap::Round, R"docstr(
        Round ending, center of the circle is the end point.
        )docstr")
      .value(
        "Square",
        LineCap::Square, R"docstr(
        Square ending, center of the square is the end point.
        )docstr");

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


LineCap LineCapFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return LineCapFromString(py::cast<std::string>(o));
  } else if (py::isinstance<LineCap>(o)) {
    return py::cast<LineCap>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp
        << "` to `viren2d.LineCap`!";
    throw std::invalid_argument(str.str());
  }
}


void RegisterLineJoin(pybind11::module &m) {
  py::enum_<LineJoin> join(m, "LineJoin",
             "Enumeration specifying how to render the junction of two lines/segments.");

  join.value(
        "Miter",
        LineJoin::Miter, R"docstr(
        Sharp (angled) corner.
        )docstr")
      .value(
        "Bevel",
        LineJoin::Bevel, R"docstr(
        The join is cut off at half the line width from the joint point.
        )docstr")
      .value(
        "Round",
        LineJoin::Round, R"docstr(
        Rounded join, where the center of the circle is the joint point.
        )docstr");

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


LineJoin LineJoinFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return LineJoinFromString(py::cast<std::string>(o));
  } else if (py::isinstance<LineJoin>(o)) {
    return py::cast<LineJoin>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp
        << "` to `viren2d.LineJoin`!";
    throw std::invalid_argument(str.str());
  }
}


void RegisterMarker(pybind11::module &m) {
  py::enum_<Marker> marker(
        m, "Marker", "Enumeration specifying the marker shape.");

  marker.value(
        "Point",
        Marker::Point, R"docstr(
        Point, *i.e.* a filled circle, char representation: ``'.'``.
        )docstr")
      .value(
        "Circle",
        Marker::Circle, R"docstr(
        A circle (not filled), char representation: ``'o'``.
        )docstr")
      .value(
        "Plus",
        Marker::Plus, R"docstr(
        Plus sign, char representation: ``'+'``.
        )docstr")
      .value(
        "Cross",
        Marker::Cross, R"docstr(
        Cross marker, char representation: ``'x'``.
        )docstr")
      .value(
        "Square",
        Marker::Square, R"docstr(
        Square marker, char representation: ``'s'``.
        )docstr")
      .value(
        "RotatedSquare",
        Marker::RotatedSquare, R"docstr(
        Rotated square (a thick diamond), char representation: ``'S'``.
        )docstr")
      .value(
        "Diamond",
        Marker::Diamond, R"docstr(
        Diamond marker, char representation: ``'d'``.
        )docstr")
      .value(
        "Reticle",
        Marker::Reticle, R"docstr(
        A plus with a :math:`\text{thickness} \times \text{thickness}` hole in
        the middle, char representation: ``'r'``.
        The hole will always be at least :math:`4 \times 4` pixels.
        )docstr")
      .value(
        "RotatedReticle",
        Marker::RotatedReticle, R"docstr(
        A cross with a :math:`\text{thickness} \times \text{thickness}` hole in
        the middle, char representation: ``'R'``.
        The hole will always be at least :math:`4 \times 4` pixels.
        )docstr")
      .value(
        "Star",
        Marker::Star, R"docstr(
        Five-pointed star (Asterisk), char representation: ``'*'``.
        )docstr")
      .value(
        "Pentagon",
        Marker::Pentagon, R"docstr(
        Five-sided polygon, char representation: ``'p'``.
        )docstr")
      .value(
        "Pentagram",
        Marker::Pentagram, R"docstr(
        Five-pointed star, char representation: ``'5'``.
        )docstr")
      .value(
        "Hexagon",
        Marker::Hexagon, R"docstr(
        Six-sided polygon, char representation: ``'h'``.
        )docstr")
      .value(
        "Hexagram",
        Marker::Hexagram, R"docstr(
        Six-pointed star, char representation: ``'6'``.
        )docstr")
      .value(
        "Heptagon",
        Marker::Heptagon, R"docstr(
        Seven-sided polygon, char representation: ``'H'``.
        )docstr")
      .value(
        "Heptagram",
        Marker::Heptagram, R"docstr(
        Seven-pointed star, char representation: ``'7'``.
        )docstr")
      .value(
        "Octagon",
        Marker::Octagon, R"docstr(
        Eight-sided polygon, char representation: ``'0'`` (*i.e.* zero).
        )docstr")
      .value(
        "Octagram",
        Marker::Octagram, R"docstr(
        Eight-pointed star, char representation: ``'8'``.
        )docstr")
      .value(
        "Enneagon",
        Marker::Enneagon, R"docstr(
        Nine-sided polygon (Nonagon), char representation: ``'n'``.
        )docstr")
      .value(
        "Enneagram",
        Marker::Enneagram, R"docstr(
        Nine-pointed star, char representation: ``'9'``.
        )docstr")
      .value(
        "TriangleUp",
        Marker::TriangleUp, R"docstr(
        Upward-pointing triangle marker, char representation: ``'^'``.
        )docstr")
      .value(
        "TriangleDown",
        Marker::TriangleDown, R"docstr(
        Downward-pointing triangle marker, char representation: ``'v'``.
        )docstr")
      .value(
        "TriangleLeft",
        Marker::TriangleLeft, R"docstr(
        Left-pointing triangle marker, char representation: ``'<'``.
        )docstr")
      .value(
        "TriangleRight",
        Marker::TriangleRight, R"docstr(
        Right-pointing triangle marker, char representation: ``'>'``.
        )docstr");

  marker.def(
        "__str__", [](Marker m) -> py::str {
            std::string s("'");
            s += MarkerToChar(m);
            s += "'";
            return py::str(s);
        }, py::name("__str__"), py::is_method(m));

  marker.def(
        "__repr__", [](Marker m) -> py::str {
            std::string s("<Marker '");
            s += MarkerToChar(m);
            s += "'>";
            return py::str(s);
        }, py::name("__repr__"), py::is_method(m));

  std::string doc = R"docstr(
      Returns all :class:`~viren2d.Marker` values.

      Convenience utility to easily iterate all enumeration
      values.

      **Corresponding C++ API:** ``viren2d::ListMarkers``.
      )docstr";
  marker.def_static("list_all", &ListMarkers, doc.c_str());
}


Marker MarkerFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    const auto str = py::cast<std::string>(o);
    return MarkerFromChar(str[0]);
  } else if (py::isinstance<Marker>(o)) {
    return py::cast<Marker>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp
        << "` to `viren2d.Marker`!";
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
    s << "Cannot create `viren2d.MarkerStyle` from tuple with "
      << tpl.size() << " entries (expected max. 7)!";
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


/// Convenience construction of a marker style,
/// which accepts marker/cap/join definitions as
/// either enum or char/string representation.
MarkerStyle CreateMarkerStyle(
    const py::object &marker, double size, double thickness, const Color &color,
    bool fill, double background_border, const Color &background_color,
    const py::object &cap, const py::object &join) {
  Marker m = MarkerFromPyObject(marker);
  LineCap c = LineCapFromPyObject(cap);
  LineJoin j = LineJoinFromPyObject(join);
  return MarkerStyle(
        m, size, thickness, color, fill, background_border, background_color,
        c, j);
}


void RegisterMarkerStyle(pybind11::module &m) {
  std::string doc = R"docstr(
      How a marker/keypoint should be drawn.

      Example:
        >>> style = viren2d.MarkerStyle(
        >>>     marker='*', size=10, color='navy-blue!80',
        >>>     thickness=1, filled=True,
        >>>     cap='round', join='miter')
      )docstr";  
  py::class_<MarkerStyle> style(m, "MarkerStyle", doc.c_str());

  style.def(py::pickle(&MarkerStyleToTuple, &MarkerStyleFromTuple),
            ":class:`~viren2d.MarkerStyle` instances can be pickled.");

  MarkerStyle default_style;
  doc = R"docstr(
      Creates a customized marker style.

      Args:
        marker: Shape as :class:`~viren2d.Marker` enumeration value
          or its character representation.
        size: Marker size in pixels as :class:`float`.
        thickness: Width/thickness of the contour in pixels
          as :class:`float`.
        color: The :class:`~viren2d.Color` used for drawing its
          contour or filling.
        filled: If ``True`` (and the shape allwos), the marker
          will be filled.
        bg_border: Can be used to improve the contrast of the marker.
          If ``bg_color`` is valid, a circle (or square for ``'s'``) will be
          drawn behind the actual marker. Size of this circle will be
          ``size + 2 * bg_border``.
        bg_color: The :class:`~viren2d.Color` to improve the contrast,
          see ``bg_thickness``.
        cap: A :class:`~viren2d.LineCap` enum, specifying
          how to render the line endpoints. This parameter
          can also be set via the corresponding string
          representation, *e.g.* ``'round'``.
        join: A :class:`~viren2d.LineJoin` enum, specifying
          how to render the junctions of multi-segment lines.
          This parameter can also be set via the corresponding
          string representation, *e.g.* ``'miter'``.
      )docstr";
  style.def(py::init<>(&CreateMarkerStyle),
            doc.c_str(),
            py::arg("marker") = default_style.marker,
            py::arg("size") = default_style.size,
            py::arg("thickness") = default_style.thickness,
            py::arg("color") = default_style.color,
            py::arg("filled") = default_style.filled,
            py::arg("bg_border") = default_style.background_border,
            py::arg("bg_color") = default_style.background_color,
            py::arg("cap") = default_style.cap,
            py::arg("join") = default_style.join);


  style.def(
    "cap_offset",
    &MarkerStyle::CapOffset,
    "Computes how much the line cap will extend the start/end of the lines.")
  .def(
    "join_offset",
    &MarkerStyle::JoinOffset, //TODO rephrase doc text after implementing get/set miter (see LineStyle bindings)
    "Computes how much a line join will extend the joint.\n\n"
    "The ``interior_angle`` is the angle between two line segments\n"
    "in degrees.\n"
    "This method needs to know the ``miter_limit`` because Cairo switches\n"
    "from ``MITER`` to ``BEVEL`` if the ``miter_limit`` is exceeded.\n"
    "Refer to the "
    "`Cairo documentation <https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-miter-limit>`__ "
    "for details.",
    py::arg("interior_angle"),
    py::arg("miter_limit") = 10.0);


  style.def(
        "copy",
        [](const MarkerStyle &st) { return MarkerStyle(st); }, R"docstr(
        Returns a deep copy.

        **Corresponding C++ API:** Copy constructor of ``viren2d::MarkerStyle``.
        )docstr")
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
      .def("is_filled", &MarkerStyle::IsFilled, R"docstr(
           Returns ``True`` if this marker would be filled.

           Note that this **may differ** from its :attr:`filled`
           member: Some marker shapes *cannot* be filled (*e.g*
           ``'+'`` or ``'o'``), whereas some shapes *must* be
           filled (*e.g.* ``'.'``).
           )docstr")
      .def("is_valid", &MarkerStyle::IsValid,
           "Checks if this style would lead to a drawable marker.");

  doc = ":class:`~viren2d.Color`: Color of the "
        "marker's contour or fill (depending on :attr:`filled`).";
  style.def_readwrite("color", &MarkerStyle::color, doc.c_str());

  doc = R"docstr(
      :class:`~viren2d.Marker`: Marker shape.

        In addition to the enumeration values, you can use
        the character codes to set this member:

        >>> style.marker = viren2d.Marker.Cross
        >>> style.marker = 'x'
      )docstr";
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

  style.def_readwrite(
        "filled",
        &MarkerStyle::filled, R"docstr(
        bool: If ``True``, the marker should be filled with
          :attr:`color`.

          Note that some marker shapes *cannot* be filled (*e.g*
          ``'+'`` or ``'o'``), whereas some shapes *must* be
          filled (*e.g.* ``'.'``). For such shapes, the value
          of :attr:`filled` will be ignored.
        )docstr");

  style.def_readwrite(
        "bg_border",
        &MarkerStyle::background_border, R"docstr(
        :class:`float`: Can be used to improve the contrast of the marker.
          If :attr:`bg_color` is valid, a circle (or
          square for ``'s'``) will be drawn behind the actual marker.
          Size of this circle will be :attr:`size` + 2 * :attr:`bg_border`.
        )docstr");

  style.def_readwrite(
        "bg_color",
        &MarkerStyle::background_color, R"docstr(
        :class:`~viren2d.Color`: Can be used to improve the contrast,
          see :attr:`bg_border`.
        )docstr");

  style.def_property(
        "cap",
        [](MarkerStyle &s) { return s.cap; },
        [](MarkerStyle &s, py::object o) {
            s.cap = LineCapFromPyObject(o);
        }, R"docstr(
        :class:`~viren2d.LineCap`: How to render the endpoints of the marker's
          contour.

          In addition to the enumeration values, you can use the corresponding string
          representation to set this member:

          >>> style.cap = viren2d.LineCap.Round
          >>> style.cap = 'round'
        )docstr");


  doc = ":class:`~viren2d.LineJoin`: "
        "How to render the junctions of the marker's contour.\n\n"
        "In addition to the enumeration values, you can use\n"
        "the corresponding string representation to set this member:\n\n"
        ">>> style.join = viren2d.LineJoin.Miter\n"
        ">>> style.join = 'miter'\n";
  style.def_property("join",
        [](MarkerStyle &s) { return s.join; },
        [](MarkerStyle &s, py::object o) {
            s.join = LineJoinFromPyObject(o);
        }, doc.c_str());
}


/// Convenience construction of a line style,
/// which accepts cap/join definitions as
/// either enum or string representation.
LineStyle CreateLineStyle(
    double width, const Color &color,
    const std::vector<double> &dash_pattern, double dash_offset,
    const py::object &cap, const py::object &join) {
  LineCap c = LineCapFromPyObject(cap);
  LineJoin j = LineJoinFromPyObject(join);
  return LineStyle(width, color, dash_pattern, dash_offset, c, j);
}


void RegisterLineStyle(pybind11::module &m) {
  std::string doc = R"docstr(
      How a line/contour should be drawn.

      Set the line :attr:`width` to an *odd* value to avoid
      anti-aliasing effects.

      Note that several ``draw_xxx`` methods of the
      :class:`~viren2d.Painter` also accept the special member
      :attr:`LineStyle.Invalid`, which indicates that a shape should
      only be filled, but it's contour should not be drawn.

      Example:
        >>> style = viren2d.LineStyle(
        >>>     width=7, color='crimson',
        >>>     dash_pattern=[20, 10], dash_offset=0.0,
        >>>     cap='round', join='miter')

      Note that depending on the selected :attr:`cap`
      (or :attr:`join`), the corresponding line (or joints)
      may start/end *not exactly* where you specified.
      If you want pixel-accurate start/end in combination with
      a particular cap/join, use
      :meth:`~viren2d.LineStyle.cap_offset` (or
      :meth:`~viren2d.LineStyle.join_offset`). For example:

      >>> # Choose a cap which results in an offset:
      >>> line_style = viren2d.LineStyle(cap='round')
      >>>
      >>> # Endpoints of our line:
      >>> start = viren2d.Vec2d(10, 10)
      >>> end = viren2d.Vec2d(100, 100)
      >>>
      >>> # Now, to draw a line exactly from `start` to `end`,
      >>> # we adjust the endpoints:
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
      unpacking* (aka *splat* operator):

      >>> arrow_style = viren2d.ArrowStyle(
      >>>     **line_style.as_dict(),
      >>>     tip_length=0.3)
      )docstr";
  line_style.def("as_dict", [](const LineStyle &s) -> py::dict {
    return LineStyleToDict(s);
  }, doc.c_str());


  line_style.def("detailed_str", &LineStyle::ToDetailedString,
                 "Returns a verbose string representation to facilitate debugging.");


  doc = R"docstr(
      Creates a customized line style.

      Args:
        width: Width in pixels as :class:`float`.
        color: Line color as :class:`~viren2d.Color`.
        dash_pattern: Dash pattern defined as :class:`list[float]`
          of on/off strokes, refer to the class
          member :attr:`dash_pattern` for details.
        dash_offset: Optional offset into the pattern, at which
          the dash stroke begins (as :class:`float`). Refer to
          the class member :attr:`dash_offset` for details.
        cap: A :class:`~viren2d.LineCap` enum, specifying
          how to render the line endpoints. This parameter
          can also be set using the corresponding string
          representation, *e.g.* ``'round'``.
        join: A :class:`~viren2d.LineJoin` enum, specifying
          how to render the junctions of multi-segment lines.
          This parameter can also be set using the corresponding
          string representation, *e.g.* ``'miter'``.
      )docstr";
  LineStyle default_style;
  line_style.def(
        py::init<>(&CreateLineStyle),
        doc.c_str(),
        py::arg("width") = default_style.width,
        py::arg("color") = default_style.color,
        py::arg("dash_pattern") = default_style.dash_pattern,
        py::arg("dash_offset") = default_style.dash_offset,
        py::arg("cap") = default_style.cap,
        py::arg("join") = default_style.join);


  line_style.def(
        "copy",
        [](const LineStyle &st) { return LineStyle(st); }, R"docstr(
        Returns a deep copy.

        **Corresponding C++ API:** Copy constructor of ``viren2d::LineStyle``.
        )docstr")
      .def(
        "__repr__",
        [](const LineStyle &l) { return "<" + l.ToString() + ">";})
      .def(
        "__str__",
        &LineStyle::ToString)
      .def(
        py::pickle(&LineStyleToTuple, &LineStyleFromTuple),
        ":class:`~viren2d.LineStyle` instances can be pickled.")
      .def(
        py::self == py::self,
        "Checks for equality.")
      .def(
        py::self != py::self,
        "Checks for inequality.")
      .def(
        "is_valid",
        &LineStyle::IsValid,
        "Checks if the style would lead to a drawable line.")
      .def(
        "is_dashed",
        &LineStyle::IsDashed,
        "Checks if this style contains a dash stroke pattern.")
      .def(
        "cap_offset",
        &LineStyle::CapOffset,
        "Computes how much the line cap will extend the\n"
        "line's start/end.")
      .def(
        "join_offset",
        &LineStyle::JoinOffset, //TODO rephrase doc text after implementing get/set miter
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
      .def_readwrite(
        "dash_pattern",
        &LineStyle::dash_pattern, R"docstr(
        list[float]: Dash pattern defined as list of on/off strokes.

          A dash pattern is a list of positive values. Each value defines
          the length (in pixels) of alternating *on* and *off* segments of
          the line. For solid lines, this list must be empty.

          >>> style.dash_pattern = [20, 30, 40, 10] # Would result in:
          '__   ____ __   ____ __   ____ __   ____ __   ____ __   ____ ...'
        )docstr")
      .def_readwrite("dash_offset", &LineStyle::dash_offset, R"docstr(
        float: Offset into the pattern at which the dash stroke begins.

          If the strokes of a :attr:`dash_pattern` do not align nicely,
          adjust this offset (measured in pixels).
        )docstr")
      .def_readwrite("width", &LineStyle::width, R"docstr(
         float: Width/thickness in pixels.

           Due to the discrete pixel grid and the internal
           drawing conventions, *odd* line widths usually avoid
           anti-aliasing effects.
         )docstr")
      //TODO doc string is missing in python package, see: https://github.com/pybind/pybind11/issues/3815
      .def_readonly_static("Invalid", &LineStyle::Invalid, R"docstr(
        :class:`~viren2d.LineStyle`: Special read-only member
          used to skip drawing contours.

          Several drawing methods of the :class:`~viren2d.Painter` support
          only filling a shape (*i.e.* skipping the contour). For example:

          >>> painter.draw_rect(
          >>>     rect, line_style=viren2d.LineStyle.Invalid,
          >>>     fill_color='blue!40')
        )docstr");

  doc = R"docstr(
      :class:`~viren2d.LineCap`: How to render the endpoints"
        of the line (or dash strokes).

        In addition to the enumeration values, you can use
        the corresponding string representation to set this member:

        >>> style.cap = viren2d.LineCap.Round
        >>> style.cap = 'round'
      )docstr";
  line_style.def_property("cap",
        [](LineStyle &s) { return s.cap; },
        [](LineStyle &s, py::object o) {
            s.cap = LineCapFromPyObject(o);
        }, doc.c_str());


  doc = R"docstr(
      :class:`~viren2d.LineJoin`: How to render the junctions
        of the line segments.

        In addition to the enumeration values, you can use
        the corresponding string representation to set this member:

        >>> style.join = viren2d.LineJoin.Miter
        >>> style.cap = 'miter'
      )docstr";
  line_style.def_property("join",
        [](LineStyle &s) { return s.join; },
        [](LineStyle &s, py::object o) {
            s.join = LineJoinFromPyObject(o);
        }, doc.c_str());


  doc = R"docstr(
    :class:`~viren2d.Color`: Color of the line.

    Example:

    >>> style.color = viren2d.Color(1, 0, 1)
    >>> style.color = viren2d.RGBa(255, 0, 255)
    >>> style.color = 'magenta'
    )docstr";
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


viren2d::ArrowStyle ArrowStyleFromTuple(const py::tuple &tpl) {
  // Convert empty tuple to pre-defined default style
  if(tpl.empty()) {
    return viren2d::ArrowStyle();
  }

  if (tpl.size() > 5) {
    std::ostringstream s;
    s << "Cannot create `viren2d.ArrowStyle` from tuple with "
      << tpl.size() << " entries (expected max. 5)!";
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


/// Convenience construction of an arrow style,
/// which accepts cap/join definitions as
/// either enum or string representation.
ArrowStyle CreateArrowStyle(
    double width, const Color &color, double tip_len, double angle,
    bool fill, bool two_heads, const std::vector<double> &dash_pattern,
    double dash_offset, const py::object &cap, const py::object &join) {
  LineCap c = LineCapFromPyObject(cap);
  LineJoin j = LineJoinFromPyObject(join);
  return ArrowStyle(
        width, color, tip_len, angle, fill, two_heads,
        dash_pattern, dash_offset, c, j);
}


void RegisterArrowStyle(pybind11::module &m) {
  std::string doc = R"docstr(
      How an arrow should be drawn.

      Example:
        >>> style = viren2d.ArrowStyle(
        >>>     width=5, color='black',
        >>>     tip_length=0.3, tip_angle=20,
        >>>     tip_closed=True, double_headed=False,
        >>>     dash_pattern=[], dash_offset=0.0,
        >>>     cap='round', join='miter')
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
        dash_pattern: Dash pattern defined as :class:`list` of :class:`float`,
          specifying the on/off strokes. Refer to the documentation of the
          class member :attr:`dash_pattern` for details.
        dash_offset: Optional offset (length as :class:`float`) into the
          pattern, at which the dash stroke begins. Refer to the
          class member :attr:`dash_offset` for details.
        cap: A :class:`~viren2d.LineCap` enum, specifying
          how to render the line endpoints. This parameter
          can also be set using the corresponding string
          representation, *e.g.* ``'round'``.
        join: A :class:`~viren2d.LineJoin` enum, specifying
          how to render the junctions of multi-segment lines.
          This parameter can also be set using the corresponding
          string representation, *e.g.* ``'miter'``.
        )docstr";
  ArrowStyle default_style;
  arrow_style.def(
        py::init<>(&CreateArrowStyle),
        doc.c_str(),
        py::arg("width") = default_style.width,
        py::arg("color") = default_style.color,
        py::arg("tip_length") = default_style.tip_length,
        py::arg("tip_angle") = default_style.tip_angle,
        py::arg("tip_closed") = default_style.tip_closed,
        py::arg("double_headed") = default_style.double_headed,
        py::arg("dash_pattern") = default_style.dash_pattern,
        py::arg("dash_offset") = default_style.dash_offset,
        py::arg("cap") = default_style.cap,
        py::arg("join") = default_style.join);


  arrow_style.def(
        "as_dict",
        [](const ArrowStyle &s) -> py::dict {
          return ArrowStyleToDict(s);
        }, "Returns a dictionary representation.");

  arrow_style.def(
        "copy",
        [](const ArrowStyle &st) { return ArrowStyle(st); }, R"docstr(
        Returns a deep copy.

        **Corresponding C++ API:** Copy constructor of ``viren2d::ArrowStyle``.
        )docstr")
      .def(
        "__repr__",
        [](const ArrowStyle &a) { return "<" + a.ToString() + ">"; })
      .def(
        "__str__",
        &ArrowStyle::ToString)
      .def(
        py::pickle(&ArrowStyleToTuple, &ArrowStyleFromTuple),
        ":class:`~viren2d.ArrowStyle` instances can be pickled.")
      .def(
        py::self == py::self,
        "Checks for equality.")
      .def(
        py::self != py::self,
        "Checks for inequality.")
      .def(
        "is_valid",
        &ArrowStyle::IsValid,
        "Checks if the style would lead to a drawable arrow.")
      .def(
        "tip_length_for_shaft",
        static_cast<double (ArrowStyle::*)(double) const>(&ArrowStyle::TipLengthForShaft),
        "Returns the length of the arrow head for the given shaft length.",
        py::arg("shaft_length"))
      .def_readwrite(
        "tip_length",
        &ArrowStyle::tip_length, R"docstr(
        float: Length of the arrow tip.

          If the value is between ``[0, 1]``, it is interpreted as
          percentage of the arrow's shaft length. Otherwise, it
          represents the absolute length in pixels.
        )docstr")
      .def_readwrite(
        "tip_angle",
        &ArrowStyle::tip_angle,
        "float: Interior angle (in degrees) between shaft and tip.")
      .def_readwrite(
        "tip_closed",
        &ArrowStyle::tip_closed,
        "bool: If ``True``, the arrow head will be filled.")
      .def_readwrite(
        "double_headed",
        &ArrowStyle::double_headed,
        "bool: If ``True``, arrow heads will be drawn on both ends.");
}


//-------------------------------------------------  BoundingBox2DStyle
LabelPosition LabelPositionFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    return LabelPositionFromString(py::cast<std::string>(o));
  } else if (py::isinstance<LabelPosition>(o)) {
    return py::cast<LabelPosition>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp
        << "` to `viren2d.LabelPosition`!";
    throw std::invalid_argument(str.str());
  }
}


py::tuple BoundingBox2DStyleToTuple(const BoundingBox2DStyle &st) {
  return py::make_tuple(
        st.line_style, st.text_style, st.box_fill_color, st.text_fill_color,
        st.label_padding, st.clip_label);
}


/// Convenience construction of an arrow style,
/// which accepts cap/join definitions as
/// either enum or string representation.
BoundingBox2DStyle CreateBoundingBox2DStyle(
    const LineStyle &line_style,
    const TextStyle &text_style,
    const Color &bbox_fill_color,
    const Color &label_box_color,
    const Vec2d &text_padding,
    bool clip_lbl) {
  return BoundingBox2DStyle(
        line_style, text_style, bbox_fill_color, label_box_color,
        text_padding, clip_lbl);
}


BoundingBox2DStyle BoundingBox2DStyleFromTuple(const py::tuple &tpl) {
  // Convert empty tuple to pre-defined default style
  if (tpl.empty()) {
    return BoundingBox2DStyle();
  }

  if (tpl.size() > 7) {
    std::ostringstream s;
    s << "Cannot create `viren2d.BoundingBox2DStyle` from tuple with "
      << tpl.size() << (tpl.size() == 1 ? " entry!" : " entries!");
    throw std::invalid_argument(s.str());
  }

  BoundingBox2DStyle bstyle;
  if (tpl.size() > 0) {
    bstyle.line_style = tpl[0].cast<LineStyle>();
  }

  if (tpl.size() > 1) {
    bstyle.text_style = tpl[1].cast<TextStyle>();
  }

  if (tpl.size() > 2) {
    bstyle.box_fill_color = tpl[2].cast<Color>();
  }

  if (tpl.size() > 3) {
    bstyle.text_fill_color = tpl[3].cast<Color>();
  }

  if (tpl.size() > 4) {
    bstyle.label_padding = tpl[4].cast<Vec2d>();
  }

  if (tpl.size() > 5) {
    bstyle.clip_label = tpl[5].cast<bool>();
  }

  return bstyle;
}


void RegisterBoundingBox2DStyle(py::module &m) {
  std::string doc = R"docstr(
      How a 2D bounding box should be drawn."

      Example:
        >>> line_style = viren2d.LineStyle(
        >>>   width=7, color='navy-blue',
        >>>   dash_pattern=[], dash_offset=0.0,
        >>>   cap='round', join='miter')
        >>> text_style = viren2d.TextStyle(
        >>>   family='monospace', size=18,
        >>>   color='navy-blue', bold=True,
        >>>   italic=False, line_spacing=1.1,
        >>>   halign='left', valign='top')
        >>> box_style = viren2d.BoundingBox2DStyle(
        >>>   line_style=line_style, text_style=text_style,
        >>>   box_fill_color='same!20', text_fill_color='white!60',
        >>>   clip_label=True)
      )docstr";
  py::class_<BoundingBox2DStyle> bbox_style(m, "BoundingBox2DStyle", doc.c_str());

  doc = R"docstr(
      Creates a customized bounding box style.

      Args:
        line_style: A :class:`~viren2d.LineStyle` specifying
          how to render the box outline.
        text_style: A :class:`~viren2d.TextStyle` specifying
          how to render the label text.
        box_fill_color: Optional :class:`~viren2d.Color` to
          fill the box.
        text_fill_color: Optional :class:`~viren2d.Color` to
          fill the background of the label.
        label_padding: Padding between the nearest bounding box
          edges and the label as :class:`~viren2d.Vec2d`.
        clip_label: If ``True``, the label will be clipped if it
          exceeds the bounding box.
      )docstr";
  BoundingBox2DStyle default_style;
  bbox_style.def(
        py::init<>(&CreateBoundingBox2DStyle),
        doc.c_str(),
        py::arg("line_style") = default_style.line_style,
        py::arg("text_style") = default_style.text_style,
        py::arg("box_fill_color") = default_style.box_fill_color,
        py::arg("text_fill_color") = default_style.text_fill_color,
        py::arg("label_padding") = default_style.label_padding,
        py::arg("clip_label") = default_style.clip_label);
// FIXME add doc to draw_bounding_box (changed label parameter!)
// label_position: A :class:`~viren2d.LabelPosition` specifying
//  where to place the label. In addition to the enumeration value,
//  this parameter can be set using its string representation.
  bbox_style.def(
        "copy",
        [](const BoundingBox2DStyle &st) { return BoundingBox2DStyle(st); }, R"docstr(
        Returns a deep copy.

        **Corresponding C++ API:** Copy constructor of ``viren2d::BoundingBox2DStyle``.
        )docstr")
      .def(
        "__repr__",
        [](const BoundingBox2DStyle &bb)
        { return "<" + bb.ToString() + ">"; })
      .def(
        "__str__",
        &BoundingBox2DStyle::ToString)
      .def(
        py::pickle(&BoundingBox2DStyleToTuple, &BoundingBox2DStyleFromTuple),
        ":class:`~viren2d.BoundingBox2DStyle` instances can be pickled.")
      .def(
        py::self == py::self,
        "Checks for equality.")
      .def(
        py::self != py::self,
        "Checks for inequality.")
      .def(
        "is_valid",
        &BoundingBox2DStyle::IsValid,
        "Returns ``True`` if the style allows rendering a 2D bounding box.");


  doc = R"docstr(
      :class:`~viren2d.LineStyle`: How to draw the bounding box contour.
      )docstr";
  bbox_style.def_readwrite(
        "line_style",
        &BoundingBox2DStyle::line_style,
        doc.c_str());


  doc = R"docstr(
      :class:`~viren2d.TextStyle`: How to render the label.
      )docstr";
  bbox_style.def_readwrite(
        "text_style",
        &BoundingBox2DStyle::text_style,
        doc.c_str());


  doc = R"docstr(
      :class:`~viren2d.Color`: Fill color of the bounding box.
      )docstr";
  bbox_style.def_readwrite(
        "box_fill_color",
        &BoundingBox2DStyle::box_fill_color,
        doc.c_str());


  doc = R"docstr(
      :class:`~viren2d.Color`: Fill color of the text
      box, *i.e.* the label background.
      )docstr";
  bbox_style.def_readwrite(
        "text_fill_color",
        &BoundingBox2DStyle::text_fill_color,
        doc.c_str());


//  doc = R"docstr(
//      :class:`~viren2d.LabelPosition`: Where to place
//      the label within the box.

//      In addition to the enumeration values, you can use
//      the string representation to set this member:

//      >>> style.label_position = viren2d.LabelPosition.Left
//      >>> style.label_position = 'left'
//      )docstr";
//  bbox_style.def_property(
//        "label_position",
//        [](BoundingBox2DStyle &s) { return s.label_position; },
//        [](BoundingBox2DStyle &s, py::object o) {
//            s.label_position = LabelPositionFromPyObject(o);
//        }, doc.c_str());
  //FIXME remove


  doc = R"docstr(
      :class:`~viren2d.Vec2d`: Padding between
      bounding box edges and label text.
      )docstr";
  bbox_style.def_readwrite(
        "label_padding",
        &BoundingBox2DStyle::label_padding,
        doc.c_str());

  bbox_style.def_readwrite(
        "clip_label",
        &BoundingBox2DStyle::clip_label,
        "bool: Set to ``True`` to clip the label at "
        "the bounding box edges.");
}
} // namespace bindings
} // namespace viren2d
