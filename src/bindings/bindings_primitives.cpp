#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>

#include <viren2d/primitives.h>
#include <bindings/binding_helpers.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {
//-------------------------------------------------  Ellipse
py::tuple EllipseToTuple(const Ellipse &obj) {
  return py::make_tuple(obj.cx, obj.cy, obj.major_axis, obj.minor_axis,
                        obj.rotation, obj.angle_from, obj.angle_to,
                        obj.include_center);
}


Ellipse EllipseFromTupleOrList(py::object object) {
  const std::string type = py::cast<std::string>(object.attr("__class__").attr("__name__"));

  if (type.compare("tuple") != 0 && type.compare("list") != 0) {
    std::ostringstream s;
    s << "Cannot cast " << type << " to "
      << FullyQualifiedType("Ellipse")
      << ", we only support casting from tuples and lists.";
    throw std::invalid_argument(s.str());
  }

  py::tuple tpl = object.cast<py::tuple>();
  if (tpl.size() < 2 || tpl.size() > 8) {
    std::ostringstream s;
    s << "Cannot create " << FullyQualifiedType("Ellipse")
      << " from tuple with "
      << tpl.size() << " entries!";
    throw std::invalid_argument(s.str());
  }

  Ellipse obj;
  size_t consumed_elements = 0;
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
      s << "You wanted to create a "
        << FullyQualifiedType("Ellipse")
        << " from a (cx, cy, major, minor, ...) tuple, but provided only "
        << tpl.size() << " entries!";
      throw std::invalid_argument(s.str());
    }
    obj = Ellipse(tpl[0].cast<double>(),
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


Ellipse EllipseFromTuple(py::tuple tpl) {
  return EllipseFromTupleOrList(tpl);
}

void RegisterEllipse(pybind11::module &m) {
  py::class_<Ellipse> ellipse(m, "Ellipse",
    "An ellipse for visualization.\n\n"
    "An ellipse is defined by its center point, length of\n"
    "its major axis, length of its minor axes and clockwise\n"
    "rotation (in degrees). At 0° rotation, the major axis\n"
    "is aligned with the X axis.\n"
    "Additionally, you can choose to draw the contour/fill the\n"
    "ellipse only partially, *i.e.* starting at ``angle_from``,\n"
    "drawing clockwise (increasing angles) until ``angle_to``.\n"
    "In this case, you should consider adding the center point\n"
    "to the drawn path via ``include_center`` (which is the default).");

  auto doc = "Create an ellipse from its center, size, rotation & from/to angles.\n"
    "Required:  center & axes, each as " + FullyQualifiedType(Vec2d::TypeName()) + ".\n"
    "Optional:  rotation   Clockwise rotation in degres.\n"
    "Optional:  angle_from & angle_to\n"
    "                      Contour/fill will be rendered from\n"
    "                      angle_from to angle_to. Default 0-360."
    "Optional:  include_center\n"
    "                      If true (and angle_from/to differ from\n"
    "                      their defaults), the center point will\n"
    "                      be included in the drawn/filled path.";
  ellipse.def(py::init<Vec2d, Vec2d,
                double, double, double, bool>(), doc.c_str(),
       py::arg("center"), py::arg("axes"),
       py::arg("rotation") = 0.0,
       py::arg("angle_from") = 0.0,
       py::arg("angle_to") = 360.0,
       py::arg("include_center") = true);

  doc = "Create an ellipse from its center, size, rotation & angle from/to\n"
        "rendering settings.\n"
        "Required:  cx, cy, major_axis & minor_axes\n"
        "                      Center & size in pixels.\n"
        "Optional:  rotation   Clockwise rotation in degres.\n"
        "Optional:  angle_from & angle_to\n"
        "                      Contour/fill will be rendered from\n"
        "                      angle_from to angle_to. Default 0-360."
        "Optional:  include_center\n"
        "                      If true (and angle_from/to differ from\n"
        "                      their defaults), the center point will\n"
        "                      be included in the drawn/filled path.";
  ellipse.def(py::init<double, double, double, double,
                       double, double, double, bool>(), doc.c_str(),
           py::arg("cx"), py::arg("cy"),
           py::arg("major_axis"), py::arg("minor_axis"),
           py::arg("rotation") = 0.0,
           py::arg("angle_from") = 0.0,
           py::arg("angle_to") = 360.0,
           py::arg("include_center") = true);

  doc = "Initialize from tuple or list:\n"
         "(center, size)\n"
         "    With types: (" + FullyQualifiedType(Vec2d::TypeName()) + ", "
         + FullyQualifiedType(Vec2d::TypeName()) + ")\n"
         "    Horizontal ellipse.\n"
         "(center, size, rotation)\n"
         "    With types: (" + FullyQualifiedType(Vec2d::TypeName()) + ", "
         + FullyQualifiedType(Vec2d::TypeName()) + ", float)\n"
         "    Rotated ellipse.\n"
         "(center, size, rotation, angle_from, angle_to, include_center)\n"
         "    With types: (" + FullyQualifiedType(Vec2d::TypeName()) + ", "
         + FullyQualifiedType(Vec2d::TypeName()) + ", float, float, float, bool)\n"
         "    Rotated ellipse, drawn only between these two angles.\n"
         "(cx, cy, major_axis, minor_axis)\n"
         "    Where each element is a float.\n"
         "    Horizontal ellipse.\n"
         "(cx, cy, major_axis, minor_axis, rotation)\n"
         "    Where each element is a float.\n"
         "    Rotated ellipse.\n"
         "(cx, cy, major_axis, minor_axis, rotation,\n"
         " angle_from, angle_to, include_center)\n"
         "    Where each element is a float except for 'include_center',\n"
         "    which is a bool.\n"
         "    Rotated ellipse, drawn only between these two angles.\n";
  ellipse.def(py::init<>(&EllipseFromTupleOrList), doc.c_str(), py::arg("tpl"));

  ellipse.def("copy", [](const Ellipse &e) { return Ellipse(e); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const Ellipse &o)
           { return FullyQualifiedType(o.ToString(), true); })
      .def("__str__", &Ellipse::ToString);

  doc = "A " + FullyQualifiedType("Ellipse") + " can be pickled.";
  ellipse.def(py::pickle(&EllipseToTuple,
                      &EllipseFromTuple), doc.c_str())
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_readwrite("cx", &Ellipse::cx,
           "float: Horizontal center.")
      .def_readwrite("cy", &Ellipse::cy,
           "float: Vertical center.")
      .def_readwrite("major_axis", &Ellipse::major_axis,
           "float: Length of major axis.")
      .def_readwrite("minor_axis", &Ellipse::minor_axis,
           "float: Length of minor axis.")
      .def_readwrite("rotation", &Ellipse::rotation,
           "float: Rotation angle (clockwise) in degrees.")
      .def_readwrite("angle_from", &Ellipse::angle_from,
           "float: Drawing the contour/filling starts at :attr:`angle_from` (clockwise in degrees).")
      .def_readwrite("angle_to", &Ellipse::angle_to,
           "float: Drawing the contour/filling stops at :attr:`angle_from` (clockwise in degrees).")
      .def_readwrite("include_center", &Ellipse::include_center,
           "bool: If you explicitly change :attr:`angle_from`/:attr:`angle_to`, you *very likely*\n"
           "also want to include the center point in the rendered path. Otherwise,\n"
           "filling can easily lead to irritating results.")
      .def("is_valid", &Ellipse::IsValid,
           "Returns ``True`` if the ellipse is in a valid/drawable state.");
//FIXME add convenience fx: create ellipse from corner points
  //FIXME add convencience fx for rects, too (ltwh; lrtb)
  // An ellipse can be initialized from a given tuple/list
  py::implicitly_convertible<py::tuple, Ellipse>();
  py::implicitly_convertible<py::list, Ellipse>();
}


//-------------------------------------------------  Rectangle
py::tuple RectToTuple(const Rect &obj) {
  return py::make_tuple(obj.cx, obj.cy, obj.width, obj.height,
                        obj.rotation, obj.radius);
}


Rect RectFromTupleOrList(py::object object) {
  const std::string type = py::cast<std::string>(object.attr("__class__").attr("__name__"));

  if (type.compare("tuple") != 0 && type.compare("list") != 0) {
    std::ostringstream s;
    s << "Cannot cast " << type << " to " << FullyQualifiedType("Rect")
      << ", we only support casting from tuples and lists.";
    throw std::invalid_argument(s.str());
  }

  py::tuple tpl = object.cast<py::tuple>();
  if (tpl.size() < 2 || tpl.size() > 6) {
    std::ostringstream s;
    s << "Cannot create a " << FullyQualifiedType("Rect")
      << " from tuple with "
      << tpl.size() << " entries! Use (center, size, ...)"
                       " or (cx, cy, w, h, ...)";
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
      s << "You wanted to create a " << FullyQualifiedType("Rect")
        << " from a (cx, cy, w, h, ...) tuple, but provided only "
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


Rect RectFromTuple(py::tuple tpl) {
  return RectFromTupleOrList(tpl);
}


void RegisterRectangle(py::module &m) {
  py::class_<Rect> rect(m, "Rect",
           "A rectangle for visualization.\n\n"
           "A rectangle is defined by its **center**, width, height,\n"
           "clockwise rotation (in degrees), and a corner\n"
           "radius (to draw rounded rectangles).");

  std::string doc = "Initialize from tuple or list:\n"
         "(center, size)\n"
         "    With types: (" + FullyQualifiedType(Vec2d::TypeName()) + ", "
         + FullyQualifiedType(Vec2d::TypeName()) + ")\n"
         "    Axis-aligned rectangle.\n"
         "(center, size, rotation)\n"
         "    With types: (" + FullyQualifiedType(Vec2d::TypeName()) + ", "
         + FullyQualifiedType(Vec2d::TypeName()) + ", float)\n"
         "    Rotated rectangle.\n"
         "(center, size, rotation, radius)\n"
         "    With types: (" + FullyQualifiedType(Vec2d::TypeName()) + ", "
         + FullyQualifiedType(Vec2d::TypeName()) + ", float, float)\n"
         "    Rotated rectangle with rounded corners.\n"
         "(cx, cy, w, h)\n"
         "    Where each element is a float.\n"
         "    Axis-aligned rectangle.\n"
         "(cx, cy, w, h, rotation)\n"
         "    Where each element is a float.\n"
         "    Rotated rectangle.\n"
         "(cx, cy, w, h, rotation, radius)\n"
         "    Where each element is a float.\n"
         "    Rotated rectangle with rounded corners.\n";
  rect.def(py::init<>(&RectFromTupleOrList), doc.c_str(),
           py::arg("tpl"));

  doc = "Create a rectangle from its center, size, rotation & radius.\n"
        "Required:  center & size, each as " + FullyQualifiedType(Vec2d::TypeName()) + ".\n"
        "Optional:  rotation  Clockwise rotation in degres.\n"
        "Optional:  radius    Corner radius in pixels (if > 1) or as\n"
        "                     percentage of min(w, h) if radius in (0, 0.5].";
  rect.def(py::init<double, double, double,
                    double, double, double>(),
           "Create a rectangle from cx, cy, w, h, rotation & radius.\n"
           "Required:  cx, cy, w, h\n"
           "                     Center and size, in pixels.\n"
           "Optional:  rotation  Clockwise rotation in degres.\n"
           "Optional:  radius:   Corner radius in pixels (if > 1) or as\n"
           "                     percentage of min(w, h) if radius in (0, 0.5].",
           py::arg("cx"), py::arg("cy"), py::arg("w"), py::arg("h"),
           py::arg("rotation") = 0.0,
           py::arg("radius") = 0.0)
      .def(py::init<Vec2d, Vec2d, double, double>(),
           doc.c_str(),
           py::arg("center"), py::arg("size"),
           py::arg("rotation") = 0.0,
           py::arg("radius") = 0.0)
      .def("copy", [](const Rect &r) { return Rect(r); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const Rect &r)
           { return FullyQualifiedType(r.ToString(), true); })
      .def("__str__", &Rect::ToString)
      .def(py::pickle(&RectToTuple,
                      &RectFromTuple))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_readwrite("cx", &Rect::cx,
           "float: Horizontal center.")
      .def_readwrite("cy", &Rect::cy,
           "float: Vertical center.")
      .def_readwrite("width", &Rect::width,
           "float: Rectangle width.")
      .def_property_readonly("half_width", &Rect::half_width,
           "float: Half the rectangle width (read-only).")
      .def_property_readonly("half_height", &Rect::half_height,
           "float: Half the rectangle height (read-only).")
      .def_readwrite("height", &Rect::height,
           "float: Rectangle height.")
      .def_readwrite("rotation", &Rect::rotation,
           "float: Rotation angle (clockwise) in degrees.")
      .def_readwrite("radius", &Rect::radius,
           "float: Corner radius (> 0 for rounded rectangles).\n"
           "Defined in pixels (if radius > 1) or as percentage\n"
           "of min(w, h) if radius in (0, 0.5].\n"
           "Values within (0.5, 1) lead to invalid rectangles.")
      .def("is_valid", &Rect::IsValid,
           "Returns True if both width & height are > 0.")
      .def_static("from_ltwh", &Rect::FromLTWH,
           "Initializes a rectangle from the LTWH representation, *i.e.*\n"
           "``left``, ``top``, ``width`` and ``height``.",
           py::arg("left"), py::arg("top"),
           py::arg("width"), py::arg("height"),
           py::arg("rotation") = 0.0, py::arg("radius") = 0.0)
      .def_static("from_lrtb", &Rect::FromLRTB,
           "Initializes a rectangle from the LRTB representation, *i.e.*\n"
           "``left``, ``right``, ``top`` and ``bottom``.",
           py::arg("left"), py::arg("right"),
           py::arg("top"), py::arg("bottom"),
           py::arg("rotation") = 0.0, py::arg("radius") = 0.0);

  // A Rect can be initialized from a given tuple/list.
  py::implicitly_convertible<py::tuple, Rect>();
  py::implicitly_convertible<py::list, Rect>();
}

} // namespace bindings
} // namespace viren2d

