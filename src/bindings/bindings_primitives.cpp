#include <ostream>
#include <stdexcept>
#include <cstdlib>

#include <pybind11/operators.h>

#include <viren2d/primitives.h>
#include <bindings/binding_helpers.h>

namespace py = pybind11;

//TODO(doc) add corresponding c++ api to docstrs of Ellipse


namespace viren2d {
namespace bindings {
//-------------------------------------------------  Ellipse
py::tuple EllipseToTuple(const Ellipse &obj) {
  return py::make_tuple(obj.cx, obj.cy, obj.major_axis, obj.minor_axis,
                        obj.rotation, obj.angle_from, obj.angle_to,
                        obj.include_center);
}


Ellipse EllipseFromTupleOrList(const py::object &object) {
  const std::string type = py::cast<std::string>(object.attr("__class__").attr("__name__"));

  if (type.compare("tuple") != 0 && type.compare("list") != 0) {
    std::ostringstream s;
    s << "Cannot cast `" << type << "` to `viren2d.Ellipse`, "
      << "expected either a tuple or list.";
    throw std::invalid_argument(s.str());
  }

  // A list can safely be cast into a tuple:
  const py::tuple tpl = object.cast<py::tuple>();
  if (tpl.size() < 2 || tpl.size() > 8) {
    std::ostringstream s;
    s << "Cannot create an Ellipse from tuple/list with "
      << tpl.size() << " entries!";
    throw std::invalid_argument(s.str());
  }

  Ellipse obj;
  std::size_t consumed_elements = 0;
  try {
    // Try initialization from viren2d types first
    auto center = tpl[0].cast<Vec2d>();
    auto size = tpl[1].cast<Vec2d>();
    consumed_elements = 2;
    obj = Ellipse(center, size);
  }  catch (const py::cast_error &) {
    // If casting failed, we should have a
    // (cx, cy, mj, mn, ...) tuple:
    if (tpl.size() < 4) {
      std::ostringstream s;
      s << "You wanted to create a `viren2d.Ellipse` "
        << "from a (cx, cy, major, minor, ...) tuple/list, but provided only "
        << tpl.size() << " entries!";
      throw std::invalid_argument(s.str());
    }
    obj = Ellipse(
        tpl[0].cast<double>(),
        tpl[1].cast<double>(),
        tpl[2].cast<double>(),
        tpl[3].cast<double>());
    consumed_elements = 4;
  }

  if (tpl.size() > consumed_elements) {
    obj.rotation = tpl[consumed_elements].cast<double>();
    ++consumed_elements;
  }

  if (tpl.size() > consumed_elements) {
    obj.angle_from = tpl[consumed_elements].cast<double>();
    ++consumed_elements;
  }

  if (tpl.size() > consumed_elements) {
    obj.angle_to = tpl[consumed_elements].cast<double>();
    ++consumed_elements;
  }

  if (tpl.size() > consumed_elements) {
    obj.include_center = tpl[consumed_elements].cast<bool>();
    ++consumed_elements;
  }

  return obj;
}


Ellipse EllipseFromTuple(const py::tuple &tpl) {
  return EllipseFromTupleOrList(tpl);
}

void RegisterEllipse(pybind11::module &m) {
  std::string doc = R"docstr(
      An ellipse for visualization.

      An ellipse is defined by its center point (:attr:`cx`
      and :attr:`cy`), length of its :attr:`major_axis`,
      length of its :attr:`minor_axis` and it's
      clockwise :attr:`rotation` (in degrees). At 0?? rotation,
      the major axis is aligned with the :math:`x` axis.

      Optionally, an ellipse can be drawn only partially,
      *i.e.* starting at :attr:`angle_from`, drawing clockwise
      (with increasing angles) until :attr:`angle_to`.
      For such partially drawn ellipses, you should consider
      adding the center point to the drawn path via
      :attr:`include_center` (which is the default behavior).

      For convenience, an ellipse can be implicitly created
      from a :class:`list` or a :class:`tuple`:

      >>> # Explicit initialization:
      >>> ellipse = viren2d.Ellipse(center, axes, ...)
      >>> # Implicitly cast from tuple:
      >>> painter.draw_ellipse((center, axes, ...), line_style=...)

      **Corresponding C++ API:** ``viren2d::Ellipse``.
      )docstr";
  py::class_<Ellipse> ellipse(m, "Ellipse", doc.c_str());

  // This docstring cannot(!!) be written as raw string.
  // Otherwise, it messes up the sphinx parser.
  doc = "Creates an ellipse.\n\n"
      "Args:\n"
      "  center: Center position\n"
      "    as :class:`~viren2d.Vec2d`.\n"
      "  axes: Lengths of the major and minor axes\n"
      "    as :class:`~viren2d.Vec2d`. Order\n"
      "    is ``(major, minor)``.\n"
      "  rotation: Clockwise rotation angle in degrees\n"
      "    as :class:`float`.\n"
      "  angle_from: Starting angle in degrees\n"
      "    as :class:`float`.\n"
      "  angle_to: Ending angle in degrees\n"
      "    as :class:`float`.\n"
      "  include_center: If ``True`` and ``angle_from``\n"
      "    or ``angle_to`` differ from their defaults, the\n"
      "    center point will be included in the drawn/filled\n"
      "    ellipse path (type :class:`bool`).";
  ellipse.def(py::init<Vec2d, Vec2d, double, double,
                       double, bool>(), doc.c_str(),
      py::arg("center"),
      py::arg("axes"),
      py::arg("rotation") = 0.0,
      py::arg("angle_from") = 0.0,
      py::arg("angle_to") = 360.0,
      py::arg("include_center") = true);

  doc = R"docstr(
      Creates an ellipse from a :class:`tuple` or :class:`list`.

      This overloaded constructor is required to allow
      implicit casting and supports 2 general tuple/list
      constellations:

      *  A tuple/list holding ``(center, size, rotation,
         angle_from, angle_to, include_center)``, where
         ``center`` & ``size`` are :class:`~viren2d.Vec2d`,
         ``rotation``, ``angle_from`` and ``angle_to`` are
         :class:`float` and ``include_center`` is a :class:`bool`.

      *  A tuple/list holding ``(cx, cy, major, minor, rotation,
         angle_from, angle_to, include_center)``, where all
         entries (except for ``include_center``) are :class:`float`.

      In both cases, ``rotation``, ``angle_from``, ``angle_to``
      and ``include_center`` are optional.
      )docstr";
  ellipse.def(py::init<>(&EllipseFromTupleOrList), doc.c_str(), py::arg("tpl"));

  ellipse.def(
        "copy",
        [](const Ellipse &e) { return Ellipse(e); }, R"docstr(
        Returns a deep copy.

        **Corresponding C++ API:** Copy constructor of ``viren2d::Ellipse``.
        )docstr")
      .def("__repr__",
           [](const Ellipse &e)
           { return "<Ellipse" + e.ToString() + ">"; })
      .def("__str__", &Ellipse::ToString);

  ellipse.def(py::pickle(&EllipseToTuple, &EllipseFromTuple),
              ":class:`~viren2d.Ellipse` instances can be pickled.")
      .def(py::self == py::self, "Checks for equality.")
      .def(py::self != py::self, "Checks for inequality.")
      .def_readwrite(
        "cx",
        &Ellipse::cx, R"docstr(
        float: Horizontal center coordinate.

          **Corresponding C++ API:** ``viren2d::Ellipse::cx``.
        )docstr")
      .def_readwrite(
        "cy",
        &Ellipse::cy, R"docstr(
        float: Vertical center coordinate.

          **Corresponding C++ API:** ``viren2d::Ellipse::cy``.
        )docstr")
      .def_readwrite(
        "major_axis",
        &Ellipse::major_axis, R"docstr(
        float: Length of the major axis.

          **Corresponding C++ API:** ``viren2d::Ellipse::major_axis``.
        )docstr")
      .def_readwrite(
        "minor_axis",
        &Ellipse::minor_axis, R"docstr(
        float: Length of the minor axis.

          **Corresponding C++ API:** ``viren2d::Ellipse::minor_axis``.
        )docstr")
      .def_readwrite(
        "rotation",
        &Ellipse::rotation, R"docstr(
        float: Rotation angle (clockwise) in degrees.

          **Corresponding C++ API:** ``viren2d::Ellipse::rotation``.
        )docstr")
      .def_readwrite(
        "angle_from",
        &Ellipse::angle_from, R"docstr(
        float: Drawing the contour/filling starts at this angle (clockwise
          in degrees).

          **Corresponding C++ API:** ``viren2d::Ellipse::angle_from``.
        )docstr")
      .def_readwrite(
        "angle_to",
        &Ellipse::angle_to, R"docstr(
        float: Drawing the contour/filling stops at this angle (clockwise
          in degrees).

          **Corresponding C++ API:** ``viren2d::Ellipse::angle_to``.
        )docstr")
      .def_readwrite(
        "include_center",
        &Ellipse::include_center, R"docstr(
        bool: If you explicitly change :attr:`angle_from` or :attr:`angle_to`,
          you *very likely* also want to include the center point in the
          rendered path. Otherwise, filling can easily lead to *irritating*
          results.

          **Corresponding C++ API:** ``viren2d::Ellipse::include_center``.
        )docstr")
      .def(
        "is_valid",
        &Ellipse::IsValid, R"docstr(
        Returns ``True`` if the ellipse can be drawn.

        **Corresponding C++ API:** ``viren2d::Ellipse::IsValid``.
        )docstr");


  doc = ":class:`~viren2d.Vec2d`: Provides convenience access to\n"
      "  the center position (*i.e.* :attr:`cx` and :attr:`cy`) as\n"
      "  2D vector.\n\n  **Corresponding C++ API:** ``viren2d::Ellipse::Center``.";
  ellipse.def_property(
        "center", &Ellipse::Center,
        [](Ellipse &e, const Vec2d &c) {
            e.cx = c.x();
            e.cy = c.y();
        }, doc.c_str());


  doc = ":class:`~viren2d.Vec2d`: Provides convenience access to\n"
      "  the axes (*i.e.* :attr:`major_axis` and :attr:`minor_axis`) as\n"
      "  2D vector, *i.e.* ``(major, minor)``.\n\n"
      "  **Corresponding C++ API:** ``viren2d::Ellipse::Axes``.";
  ellipse.def_property(
        "axes", &Ellipse::Axes,
        [](Ellipse &e, const Vec2d &c) {
            e.major_axis = c.x();
            e.minor_axis = c.y();
        }, doc.c_str());


  ellipse.def_static(
        "from_endpoints",
        &Ellipse::FromEndpoints, R"docstr(
        Returns an ellipse defined by the endpoints of its major axis.

        Assumes that the given coordinates specify the endpoints of the
        major axis.

        **Corresponding C++ API:** ``viren2d::Ellipse::FromEndpoints``.

        Args:
          pt1: First endpoint of the major axis as :class:`~viren2d.Vec2d`.
          pt2: Second endpoint of the major axis as :class:`~viren2d.Vec2d`.
          width: Length of the minor axis as :class:`float`.
          angle_from: Starting angle in degrees
            as :class:`float`.
          angle_to: Ending angle in degrees
            as :class:`float`.
          include_center: If ``True`` and ``angle_from``
            or ``angle_to`` differ from their defaults, the
            center point will be included in the drawn/filled
            ellipse path (type :class:`bool`).
        )docstr",
        py::arg("pt1"),
        py::arg("pt2"),
        py::arg("width"),
        py::arg("angle_from") = 0.0,
        py::arg("angle_to") = 360.0,
        py::arg("include_center") = true);


  // Add alias for typing convenience
  m.def("ellipse_from_endpoints",
        &Ellipse::FromEndpoints,
        "Alias of :meth:`~viren2d.Ellipse.from_endpoints`.",
        py::arg("pt1"), py::arg("pt2"), py::arg("width"),
        py::arg("angle_from") = 0.0,
        py::arg("angle_to") = 360.0,
        py::arg("include_center") = true);


  // An ellipse can be initialized from a given tuple/list
  py::implicitly_convertible<py::tuple, Ellipse>();
  py::implicitly_convertible<py::list, Ellipse>();
}


//-------------------------------------------------  Rectangle
py::tuple RectToTuple(const Rect &obj) {
  return py::make_tuple(obj.cx, obj.cy, obj.width, obj.height,
                        obj.rotation, obj.radius);
}


Rect RectFromTupleOrList(const py::object &object) {
  const std::string type = py::cast<std::string>(object.attr("__class__").attr("__name__"));

  if (type.compare("tuple") != 0 && type.compare("list") != 0) {
    std::ostringstream s;
    s << "Cannot cast " << type << " to viren2d.Rect, "
      << "expected either tuple or list.";
    throw std::invalid_argument(s.str());
  }

  const py::tuple tpl = object.cast<py::tuple>();
  if (tpl.size() < 2 || tpl.size() > 6) {
    std::ostringstream s;
    s << "Cannot create a viren2d.Rect from tuple/list with "
      << tpl.size()
      << " entries! Use either (center, size, ...) or (cx, cy, w, h, ...).";
    throw std::invalid_argument(s.str());
  }

  size_t consumed_elements = 0;
  Rect rect;
  try {
    // Try initialization from viren2d types first
    auto center = tpl[0].cast<Vec2d>();
    auto size = tpl[1].cast<Vec2d>();
    consumed_elements = 2;
    rect = Rect(center, size);
  }  catch (const py::cast_error &) {
    // If casting failed, we should have a (cx, cy, w, h, ...) tuple
    if (tpl.size() < 4) {
      std::ostringstream s;
      s << "You wanted to create a viren2d.Rect "
        << "from a (cx, cy, w, h, ...) tuple/list, but provided only "
        << tpl.size() << " entries!";
      throw std::invalid_argument(s.str());
    }
    rect = Rect(tpl[0].cast<double>(),
        tpl[1].cast<double>(),
        tpl[2].cast<double>(),
        tpl[3].cast<double>());
    consumed_elements = 4;
  }

  if (tpl.size() > consumed_elements) {
    rect.rotation = tpl[consumed_elements].cast<double>();
    ++consumed_elements;
  }

  if (tpl.size() > consumed_elements) {
    rect.radius = tpl[consumed_elements].cast<double>();
  }
  return rect;
}


Rect RectFromTuple(const py::tuple &tpl) {
  return RectFromTupleOrList(tpl);
}


void RegisterRectangle(py::module &m) {
  py::class_<Rect> rect(m, "Rect", R"docstr(
        A rectangle for visualization.

        A rectangle is defined by its :attr:`center`,
        :attr:`width`, :attr:`height`, :attr:`rotation`
        (clockwise, in degrees), and a corner
        :attr:`radius`.

        For convenience, a rectangle can be implicitly created
        from a :class:`list` or a :class:`tuple`:

        >>> # Explicit initialization:
        >>> rect = viren2d.Rect(center, size, ...)
        >>> # Implicitly cast from tuple:
        >>> painter.draw_rect((center, size, ...), line_style=...)

        Alternatively, an axis-aligned rectangle can also be initialized
        from the ``L,T,W,H`` and ``L,R,T,B`` representations:

        >>> # If top-left and dimensions are given:
        >>> rect = viren2d.Rect.rom_ltwh(left, top, width, height)
        >>> # If top-left and bottom-right corners are given:
        >>> rect = viren2d.Rect.rom_lrtb(left, right, top, bottom)

        **Corresponding C++ API:** ``viren2d::Rect``.
        )docstr");


  rect.def(
        py::init<>(&RectFromTupleOrList), R"docstr(
        Creates a rectangle from a :class:`tuple` or :class:`list`.

        This overloaded constructor is required to allow
        implicit casting and supports 2 general tuple/list
        constellations:

        *  A tuple/list holding ``(center, size, rotation,
           radius)``, where ``center`` & ``size`` are
           :class:`~viren2d.Vec2d`\ s, and ``rotation``
           & ``radius`` are :class:`float`.

        *  A tuple/list holding ``(cx, cy, w, h, rotation,
           radius)``, where all entries are :class:`float`.

        In both cases, ``rotation`` and ``radius`` are
        optional.
        )docstr",
        py::arg("tpl"));

  // This docstring cannot(!!) be written as raw string.
  // Otherwise, it messes up the sphinx parser.
  std::string doc = "Creates a rectangle.\n\n"
      "Args:\n"
      "  center: Center position as :class:`~viren2d.Vec2d`.\n"
      "  size: Size, *i.e.* (width, height) of the rectangle\n"
      "    as :class:`~viren2d.Vec2d`.\n"
      "  rotation: Clockwise rotation angle in degrees\n"
      "    as :class:`float`.\n"
      "  radius: Corner radius as :class:`float`, see documentation\n"
      "    of the :attr:`radius` attribute.\n";
  rect.def(
        //py::init<Vec2d, Vec2d, double, double>(), doc.c_str(),
        py::init<Vec2d, Vec2d, double, double>(),
        doc.c_str(),
        py::arg("center"),
        py::arg("size"),
        py::arg("rotation") = 0.0,
        py::arg("radius") = 0.0);


  rect.def(
        "copy",
        [](const Rect &r) { return Rect(r); }, R"docstr(
        Returns a deep copy.

        **Corresponding C++ API:** Copy constructor of ``viren2d::Rect``.
        )docstr")
      .def(
        "__repr__",
        [](const Rect &) { return FullyQualifiedType("Rect", true); })
      .def("__str__", &Rect::ToString)
      .def(
        py::pickle(&RectToTuple, &RectFromTuple),
        ":class:`~viren2d.Rect` instances can be pickled.")
      .def(py::self == py::self, "Checks for equality.")
      .def(py::self != py::self, "Checks for inequality.")
      .def_readwrite(
        "cx",
        &Rect::cx, R"docstr(
        float: Horizontal center coordinate.

          **Corresponding C++ API:** ``viren2d::Rect::cx``.
        )docstr")
      .def_readwrite(
        "cy",
        &Rect::cy, R"docstr(
        float: Vertical center coordinate.

          **Corresponding C++ API:** ``viren2d::Rect::cy``.
        )docstr")
      .def_readwrite(
        "width",
        &Rect::width, R"docstr(
        float: Rectangle width.

          **Corresponding C++ API:** ``viren2d::Rect::width``.
        )docstr")
      .def_property_readonly(
        "half_width",
        &Rect::HalfWidth, R"docstr(
        float: Half the rectangle width (read-only).

          **Corresponding C++ API:** ``viren2d::Rect::HalfWidth``.
        )docstr")
      .def_property_readonly(
        "half_height",
        &Rect::HalfHeight, R"docstr(
        float: Half the rectangle height (read-only).

          **Corresponding C++ API:** ``viren2d::Rect::HalfHeight``.
        )docstr")
      .def_readwrite(
        "height",
        &Rect::height, R"docstr(
        float: Rectangle height.

          **Corresponding C++ API:** ``viren2d::Rect::height``.
        )docstr")
      .def_readwrite(
        "rotation",
        &Rect::rotation, R"docstr(
        float: Clockwise rotation angle in degrees.

          **Corresponding C++ API:** ``viren2d::Rect::rotation``.
        )docstr")
      .def_readwrite(
        "radius",
        &Rect::radius, R"docstr(
        float: Corner radius.

          If :math:`0 \leq \text{radius} \leq 0.5`, the actural corner radius
          will be computed as
          :math:`\text{radius} * \min(\text{width}, \text{height})`.

          If :math:`\text{radius} > 1`, it denotes the absolute
          corner radius in pixels.

          Otherwise, *i.e.* :math:`0.5 < \text{radius} < 1`, the rectangle
          will be invalid.

          **Corresponding C++ API:** ``viren2d::Rect::radius``.
        )docstr")
      .def(
        "is_valid",
        &Rect::IsValid, R"docstr(
        Returns ``True`` if the rectangle can be drawn.

        **Corresponding C++ API:** ``viren2d::Rect::IsValid``.
        )docstr")
      .def_static(
        "from_ltwh",
        &Rect::FromLTWH, R"docstr(
        Returns an axis-aligned rectangle for the ``L,T,W,H`` representation.

        **Corresponding C++ API:** ``viren2d::Rect::FromLTWH``.

        Args:
          left: :math:`x` coordinate of the left edge as :class:`float`.
          top: :math:`y` coordinate of the top edge as :class:`float`.
          width: Width (extent along the :math:`x` axis) as :class:`float`.
          height: Height (extent along the :math:`y` axis) as :class:`float`.
          radius: Corner radius as :class:`float`, see documentation
            of the :attr:`radius` attribute.
        )docstr",
        py::arg("left"),
        py::arg("top"),
        py::arg("width"),
        py::arg("height"),
        py::arg("radius") = 0.0)
      .def_static(
        "from_lrtb",
        &Rect::FromLRTB, R"docstr(
        Returns an axis-aligned rectangle for the ``L,R,T,B`` representation.

        **Corresponding C++ API:** ``viren2d::Rect::FromLRTB``.

        Args:
          left: :math:`x` coordinate of the left edge as :class:`float`.
          right: :math:`x` coordinate of the right edge as :class:`float`.
          top: :math:`y` coordinate of top edge as :class:`float`.
          bottom: :math:`y` coordinate of bottom edge as :class:`float`.
          radius: Corner radius as :class:`float`, see documentation
            of the :attr:`radius` attribute.
        )docstr",
        py::arg("left"),
        py::arg("right"),
        py::arg("top"),
        py::arg("bottom"),
        py::arg("radius") = 0.0)
      .def_static(
        "from_cwh",
        &Rect::FromCWH, R"docstr(
        Returns a rectangle for the ``Cx,Cy,W,H`` representation.

        **Corresponding C++ API:** ``viren2d::Rect::FromCWH``.

        Args:
          cx: Horizontal center coordinate as :class:`float`.
          cy: Vertical center coordinate as :class:`float`.
          width: Width (extent along the :math:`x` axis if the rectangle is
            not rotated) as :class:`float`.
          height: Height (extent along the :math:`y` axis if the rectangle is
            not rotated) as :class:`float`.
          rotation: Clockwise rotation in degrees as :class:`float`.
          radius: Corner radius as :class:`float`, see documentation
            of the :attr:`radius` attribute.
        )docstr",
        py::arg("cx"),
        py::arg("cy"),
        py::arg("width"),
        py::arg("height"),
        py::arg("rotation") = 0.0,
        py::arg("radius") = 0.0);


  // Add aliases of the static initialization methods (for typing convenience)
  m.def("rect_from_cwh",
        &Rect::FromCWH,
        "Alias of :meth:`~viren2d.Rect.from_cwh`.",
        py::arg("cx"),
        py::arg("cy"),
        py::arg("width"),
        py::arg("height"),
        py::arg("rotation") = 0.0,
        py::arg("radius") = 0.0);

  m.def("rect_from_lrtb",
        &Rect::FromLRTB,
        "Alias of :meth:`~viren2d.Rect.from_lrtb`.",
        py::arg("left"),
        py::arg("right"),
        py::arg("top"),
        py::arg("bottom"),
        py::arg("radius") = 0.0);

  m.def("rect_from_ltwh",
        &Rect::FromLTWH,
        "Alias of :meth:`~viren2d.Rect.from_ltwh`.",
        py::arg("left"),
        py::arg("top"),
        py::arg("width"),
        py::arg("height"),
        py::arg("radius") = 0.0);


  rect.def_property(
        "center", &Rect::Center,
        [](Rect &r, const Vec2d &c) {
            r.cx = c.x();
            r.cy = c.y();
        }, R"docstr(
        :class:`~viren2d.Vec2d`: Provides convenience access to
          the center position (*i.e.* :attr:`cx` and :attr:`cy`) as
          2D vector.

          **Corresponding C++ API:** ``viren2d::Rect::Size``.
        )docstr");


  rect.def_property(
        "size", &Rect::Size,
        [](Rect &r, const Vec2d &c) {
            r.width = c.width();
            r.height = c.height();
        }, R"docstr(
        :class:`~viren2d.Vec2d`: Provides convenience access to
          the size (*i.e.* :attr:`width` and :attr:`height`) as
          2D vector.

          **Corresponding C++ API:** ``viren2d::Rect::Size``.
        )docstr");

  // A Rect can be initialized from a given tuple/list.
  py::implicitly_convertible<py::tuple, Rect>();
  py::implicitly_convertible<py::list, Rect>();
}


//-------------------------------------------------  Line2d
void RegisterLine2d(pybind11::module &m) {
  py::class_<Line2d> line(m, "Line2d", R"docstr(
        A line or line segment in 2D space.

        TODO doc

        **Corresponding C++ API:** ``viren2d::Line2d``.
        )docstr");
  
  //FIXME implement bindings, implicit casts, etc.

  line.def(
        py::init<Vec2d, Vec2d>(), R"docstr(
        Creates a line from 2 points.
        
        Args:
          pt1: Start point as :class:`~viren2d.Vec2d`.
          pt2: End point as :class:`~viren2d.Vec2d`.
        )docstr",
        py::arg("pt1"),
        py::arg("pt2"));

  
  line.def(
        "__str__",
        [](const Line2d &l) {
          std::ostringstream s;
          s << l;
          return s.str();
        })
      .def(
        "__repr__",
        [](const Line2d &l) {
          std::ostringstream s;
          s << '<' << l << '>';
          return s.str();
        });

  line.def(
        "is_valid",
        &Line2d::IsValid, R"docstr(
        Returns ``True`` if the line is valid, *i.e.* :math:`\text{length} > 0`.

        **Corresponding C++ API:** ``viren2d::Line2d::IsValid``.
        )docstr");
  
  line.def(
        "reversed",
        &Line2d::Reversed, R"docstr(
        Returns a line with flipped start/end points.

        **Corresponding C++ API:** ``viren2d::Line2d::Reversed``.
        )docstr");
  

  line.def(
        "direction",
        &Line2d::Direction, R"docstr(
        Returns the **non-normalized** direction vector from the start point
        to the end point.

        **Corresponding C++ API:** ``viren2d::Line2d::Direction``.
        )docstr")
      .def(
        "unit_direction",
        &Line2d::UnitDirection, R"docstr(
        Returns the unit direction vector from the start point to the end point.

        **Corresponding C++ API:** ``viren2d::Line2d::UnitDirection``.
        )docstr");


  line.def_property_readonly(
        "pt1",
        &Line2d::From, R"docstr(
        float: Starting point.

          **Corresponding C++ API:** ``viren2d::Line2d::From``.
        )docstr")
      .def_property_readonly(
        "pt2",
        &Line2d::To, R"docstr(
        float: End point.

          **Corresponding C++ API:** ``viren2d::Line2d::To``.
        )docstr")
      .def_property_readonly(
        "mid_point",
        &Line2d::MidPoint, R"docstr(
        float: Mid point between :attr:`~viren2d.Line2d.pt1`
          and :attr:`~viren2d.Line2d.pt2`.

          **Corresponding C++ API:** ``viren2d::Line2d::MidPoint``.
        )docstr")
      .def_property_readonly(
        "length",
        &Line2d::Length, R"docstr(
        float: Length of the segment between :attr:`~viren2d.Line2d.pt1`
          and :attr:`~viren2d.Line2d.pt2`.

          **Corresponding C++ API:** ``viren2d::Line2d::Length``.
        )docstr");

  line.def(
        "homogeneous",
        &Line2d::HomogeneousForm, R"docstr(
        Returns the representation of this line in :math:`\mathbb{P}^2`.
        
        For more details on lines in projective space, refer to
        `Bob Fisher's CVonline <http://homepages.inf.ed.ac.uk/rbf/CVonline/LOCAL_COPIES/BEARDSLEY/node2.html>`__,
        or
        `Stan Birchfield's lecture notes <http://robotics.stanford.edu/~birch/projective/node4.html>`__.

        **Corresponding C++ API:** ``viren2d::Line2d::HomogeneousForm``.

        Returns:
          The :class:`~viren2d.Vec3d` as the result of
          :math:`\text{pt1} \times \text{pt2}`.
        )docstr");

  line.def(
        "closest_point_on_line",
        &Line2d::ClosestPointOnLine, R"docstr(
        Returns the closest point on the **line**.
        
        This computes the projection of the given point onto this line.

        **Corresponding C++ API:** ``viren2d::Line2d::ClosestPointOnLine``.
        )docstr",
        py::arg("pt"))
      .def(
        "closest_point_on_segment",
        &Line2d::ClosestPointOnSegment, R"docstr(
        Returns the closest point on the **segment**.
        
        This computes the projection of the given point onto this segment.

        **Corresponding C++ API:** ``viren2d::Line2d::ClosestPointOnSegment``.
        )docstr",
        py::arg("pt"));


  // TODO add bindings for:
  // Angle
  // DistancePointToLine / DistancePointToSegment
  // IsCollinear
  // IsPointLeftOfLine
  // IntersectionLineLine / IntersectionLineLineSegment / IntersectionLineSegmentLineSegment
  // IntersectionLineCircle / IntersectionLineSegmentCircle
  
  line.def(
        "clip_line_by_rectangle",
        &Line2d::ClipLineByRectangle, R"docstr(
        Clips this **line** against the given **axis-aligned rectangle**.

        **Corresponding C++ API:** ``viren2d::Line2d::ClipLineByRectangle``.
        
        Args:
          top_left: Position of the rectangle's *top-left* corner as
            :class:`~viren2d.Vec2d`.
          size: Dimension of the rectangle as :class:`~viren2d.Vec2d`.

        Returns:
          The clipped :class:`~viren2d.Line2d`. If the line did not intersect
          the rectangle, the result will be invalid. Check this
          via :meth:`~viren2d.Line2d.is_valid`.
        )docstr",
        py::arg("top_left"),
        py::arg("size"))
      .def(
        "clip_segment_by_rectangle",
        &Line2d::ClipLineSegmentByRectangle, R"docstr(
        Clips this **segment** against the given **axis-aligned rectangle**.
        
        **Corresponding C++ API:** ``viren2d::Line2d::ClipLineSegmentByRectangle``.

        Args:
          top_left: Position of the rectangle's *top-left* corner as
            :class:`~viren2d.Vec2d`.
          size: Dimension of the rectangle as :class:`~viren2d.Vec2d`.

        Returns:
          The clipped :class:`~viren2d.Line2d`. If the segment did not intersect
          the rectangle, the result will be invalid. Check this
          via :meth:`~viren2d.Line2d.is_valid`.
        )docstr",
        py::arg("top_left"),
        py::arg("size"));
}

} // namespace bindings
} // namespace viren2d

