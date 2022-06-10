#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <bindings/binding_helpers.h>
#include <viren2d/colors.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {
std::string FullyQualifiedType(const std::string &name, bool with_tags) {
  std::ostringstream s;
  if (with_tags)
    s << '<';
  s << MACRO_STRINGIFY(viren2d_PYMODULE_NAME)
    << '.' << name;
  if (with_tags)
    s << '>';
  return s.str();
}


Color ColorFromTuple(py::tuple tpl) {
  // Return invalid color for empty tuple
  if (tpl.empty()) {
    return Color();
  }

  if (tpl.size() < 3 || tpl.size() > 4) {
    std::ostringstream s;
    s << "Cannot create " << FullyQualifiedType("Color")
      << " from tuple with " << tpl.size()
      << " values. Expected 3 or 4!";
    throw std::invalid_argument(s.str());
  }
  Color col(tpl[0].cast<double>(),
                  tpl[1].cast<double>(),
                  tpl[2].cast<double>(), 1.0);

  if (tpl.size() == 4) {
    col.alpha = tpl[3].cast<double>();
  }

  return col;
}


py::tuple ColorToTuple(const Color &obj) {
  return py::make_tuple(obj.red, obj.green,
                        obj.blue, obj.alpha);
}


void RegisterColor(py::module &m) {
  auto doc = "Returns a list of all available predefined color names.\n"
             "Each of these names can be used to initialize a\n:class:`"
             + FullyQualifiedType("Color") + "`.";
  m.def("color_names", &ListNamedColors, doc.c_str());

  py::class_<Color>(m, "Color",
           "A color in rgba format, where each component is within [0, 1].")
      .def(py::init<>(),
           "Initializes an *invalid color* (``r,g,b < 0``) which can\n"
           "be used in several :py:class:`viren2d.Painter` methods to request\n"
           "special color handling (e.g. switching to the inverse color or\n"
           "to skip filling).")
      .def(py::init<>(&ColorFromTuple),
           "A ``tuple`` can be implicitly cast into a :py:class:`viren2d.Color`.\n"
           "For this, each value must be a floating point number within [0, 1]."
           "\n\nExample:\n"
           "  >>> painter.draw_polygon(..., fill_color = (red, green, blue))\n"
           "  >>> painter.draw_polygon(..., fill_color = (red, green, blue, alpha)",
           py::arg("tpl"))
      .def(py::init<double, double, double, double>(),
           "Initializes the color from the given rgba components. **All values\n"
           "will be clamped** to [0, 1].",
           py::arg("red"), py::arg("green"), py::arg("blue"),
           py::arg("alpha")=1.0)
      .def(py::init<const std::string &, double>(),
           "Initializes the color from a string representation via its\n"
           "hex/webcode or its color name.\n\n"
           "**Hex/Webcode:**\n"
           "  >>> painter.draw_rect(..., fill_color = '#00ff00')\n"
           "  >>> painter.draw_rect(..., fill_color = '#a0b0c0f0')\n\n"
           "**Color name:**\n"
           "  See :py:meth:`viren2d.color_names()` for\n"
           "  a list of available color names.\n"
           "    >>> painter.draw_rect(..., fill_color = 'black')\n"
           "    >>> painter.draw_rect(..., fill_color = 'navy-blue')\n"
           "  A color name can also include an alpha suffix (which must\n"
           "  be an integer in [0, 100]):\n"
           "      >>> painter.draw_rect(..., fill_color = 'forest-green!50')\n"
           "  Optionally, you can also invert a color name (to use its\n"
           "  complementary color) by prepending ``!`` or ``-``\\:\n"
           "      >>> # '!blue!30 is equal to 'yellow!30'\n"
           "      >>> painter.draw_rect(..., fill_color = '!blue!30')",
           py::arg("colorspec"), py::arg("alpha")=1.0)
      .def("copy", [](const Color &c) { return Color(c); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const Color &c)
           { return "<" + c.ToString() + ">"; })
      .def("__str__", &Color::ToHexString)
      .def(py::pickle(&ColorToTuple,
                      &ColorFromTuple))
      .def(py::self == py::self,
           "'==' operator: Returns True if ALL components (r, g,\n"
           "b & alpha) are equal.")
      .def(py::self != py::self,
           "'!=' operator: Returns True if ANY component (r, g,\n"
           "b or alpha) differs.")
      .def(py::self += py::self,
           "'+=' operator: Adds the other's r,g,b values and clamps\n"
           "the result to [0, 1]. Alpha will not be changed.")
      .def(py::self + py::self,
           "'+' operator: Adds the r,g,b values of two Color objects and\n"
           "clamps the result to [0, 1]. Alpha will not be changed.")
#ifdef __clang__
// Clang gives false warnings: https://bugs.llvm.org/show_bug.cgi?id=43124
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif  // __clang__
      .def(py::self -= py::self,
           "'-=' operator: Subtracts the other's r,g,b values and clamps\n"
           "the result to [0, 1]. Alpha will not be changed.")
#ifdef __clang__
#pragma GCC diagnostic pop
#endif  // __clang__
      .def(py::self - py::self,
           "'-' operator: Subtracts the r,g,b values of rhs from lhs and\n"
           "clamps the result to [0, 1]. Alpha will not be changed.")
      .def(py::self *= float(),
           "Scales r,g,b by the given factor (alpha remains unchanged).")
      .def(float() * py::self,
           "Scales r,g,b by the given factor (alpha remains unchanged).")
      .def(py::self * float(),
           "Scales r,g,b by the given factor (alpha remains unchanged).")
      .def(py::self /= float(),
           "Divides r,g,b by the given factor (alpha remains unchanged).")
      .def(py::self / float(),
           "Divides r,g,b by the given factor (alpha remains unchanged).")
      .def("as_RGBa", &Color::ToRGBa,
           "Returns the corresponding (R, G, B, a) tuple,\n"
           "where R, G, B in [0, 255] and alpha in [0, 1].")
      .def("as_rgba", [](const Color& c)
           { return py::make_tuple(c.red, c.green, c.blue, c.alpha); },
           "Returns the corresponding (r, g, b, a) tuple,\n"
           "where all components are in[0, 1].")
      .def("as_hex", &Color::ToHexString,
           "Returns the hex web color code representation, for\n"
           "example ``#0011ffff`` (all components are scaled to [0, 255]).\n"
           "If the color is invalid, ``#????????`` will be returned instead.")
      .def("with_alpha", &Color::WithAlpha,
           "Return a color with the same rgb components, but the given alpha.",
           py::arg("alpha"))
      .def_readwrite("red", &Color::red,
                     "Red component within [0, 1].")
      .def_readwrite("green", &Color::green,
                     "Green component within [0, 1].")
      .def_readwrite("blue", &Color::blue,
                     "Blue component within [0, 1].")
      .def_readwrite("alpha", &Color::alpha,
                     "Opacity within [0, 1], where 0 is fully transparent\n"
                     "and 1 is fully opaque.")
      // TODO(snototter) pybind11 bug, documentation of static members is missing in python, see https://github.com/pybind/pybind11/issues/3815
      .def_readonly_static("White", &Color::White,
                           "Read-only white color instantiation.")
      .def_readonly_static("Black", &Color::Black,
                           "Read-only black color instantiation.")
      .def_readonly_static("Red", &Color::Red,
                           "Read-only red color instantiation.")
      .def_readonly_static("Green", &Color::Green,
                           "Read-only green color instantiation.")
      .def_readonly_static("Blue", &Color::Blue,
                           "Read-only blue color instantiation.")
      .def_readonly_static("Cyan", &Color::Cyan,
                           "Read-only cyan color instantiation.")
      .def_readonly_static("Magenta", &Color::Magenta,
                           "Read-only magenta color instantiation.")
      .def_readonly_static("Yellow", &Color::Yellow,
                           "Read-only yellow color instantiation.")
      .def_readonly_static("Invalid", &Color::Invalid,
                           "Read-only special color ``Invalid`` (*e.g.* to skip filling).")
      .def_readonly_static("Same", &Color::Same,
                           "Read-only special color ``Same`` (*e.g.* to use the same\n"
                           "color for filling as the object's contour).")
      .def("is_valid", &Color::IsValid,
           "Returns ``True`` if this is a valid rgba color, where all\n"
           "components are within [0, 1].")
      .def("inverse", &Color::Inverse,
           "Returns the inverse/complementary color.\n\n"
           "Except for shades of gray, this returns ``(1.0-r, 1.0-g, 1.0-b)``.\n"
           "For gray values it will either return black or white. The alpha\n"
           "value will always stay the same.\n"
           "Why special handling of gray? Complementary colors should be\n"
           "used to provide good contrast/highlights - thus, having the\n"
           "true inverse (*i.e.* ``1-r|g|b``) for medium gray (r,g,b close to 127)\n"
           "would not be too useful.")
      .def("is_shade_of_gray", &Color::IsShadeOfGray,
           "Checks if all rgb components are almost the same (+/- the given epsilon).",
           py::arg("eps")=0.02);

  // A Color can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, Color>();

  // A Color can be initialized from a string representation directly.
  py::implicitly_convertible<py::str, Color>();


  doc = "Returns a " + FullyQualifiedType("Color") + " for the given values.\n"
        "red, green, blue and alpha must be within [0, 1]";
  m.def("rgba", &rgba, doc.c_str(),
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);

  // Convenience function "rgb"
  doc = "Returns a fully opaque " + FullyQualifiedType("Color") + " for the given\n"
        "values. red, green, and blue must be within [0, 1].";
  m.def("rgb", [](double red, double green, double blue) { return Color(red, green, blue, 1.0); },
        doc.c_str(), py::arg("red"), py::arg("green"), py::arg("blue"));


  doc = "Returns a " + FullyQualifiedType("Color") + " for the given RGBa values.\n"
        "R, G, and B must be within [0, 255]; alpha must be within [0, 1].";
  m.def("RGBa", &RGBa,
        doc.c_str(),
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);

  // Convenience function "RGB"
  doc = "Returns a fully opaque " + FullyQualifiedType("Color") + " for the given\n"
        "values. red, green, and blue must be within [0, 255]\n"
        "* alpha must be within [0, 1].";
  m.def("RGB",  [](double R, double G, double B) { return RGBa(R, G, B, 1.0); },
        doc.c_str(), py::arg("red"), py::arg("green"), py::arg("blue"));
}

} // namespace bindings
} // namespace viren2d

