#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <vivi/vivi.hpp>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

/** @brief Utilities to support pickling.
 *
 *  Naming convention:
 *  * Serialize<X> --> X.__getstate__
 *  * Deserialize<X> --> X.__setstate__
 *
 *  pybind11 doc: https://pybind11.readthedocs.io/en/stable/advanced/classes.html#pickling-support
 */
namespace pickling
{
py::tuple SerializeColor(const vivi::Color &c)
{
  return py::make_tuple(c.red, c.green, c.blue, c.alpha);
}


vivi::Color DeserializeColor(py::tuple tpl)
{
  if (tpl.size() != 4)
    throw std::runtime_error("Invalid vivi.Color state - expected 4 values (rgba)!");

  return vivi::Color(tpl[0].cast<double>(),
      tpl[1].cast<double>(),
      tpl[2].cast<double>(),
      tpl[3].cast<double>());
}

} // namespace pkl

PYBIND11_MODULE(vivi, m)
{
//  py::module m("vivi");
  m.doc() = R"pbdoc(
        TODO example bindings
  )pbdoc";

  //-----------------------------------------------------------------------------------------
  // PRIMITIVES
  // * Color related
  //TODO nice-to-have +,-,*,... overloading: https://pybind11.readthedocs.io/en/stable/advanced/classes.html#operator-overloading
  py::class_<vivi::Color>(m, "Color")
      .def(py::init<>())
      .def(py::init<double, double, double, double>(),
           py::arg("red"), py::arg("green"), py::arg("blue"),
           py::arg("alpha")=1.0)
      .def("__repr__",
           [](const vivi::Color& c)
           { return "<vivi.Color " + c.ToString() + ">"; },
           "Returns an unambiguous str representation.")
      .def("__str__", &vivi::Color::ToString,
           "Returns a readable str representation.")
      .def(py::pickle(&pickling::SerializeColor,
                      &pickling::DeserializeColor))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("as_RGBA", &vivi::Color::ToRGBA,
           "Returns the corresponding (R, G, B, a) tuple, where R, G, B in [0, 255] and alpha in [0, 1].")
      .def("as_hex", &vivi::Color::ToHexString,
           "Returns the hex web color code representation, e.g. '#0011FF' (alpha is ignored).")
      .def_readwrite("red", &vivi::Color::red)
      .def_readwrite("green", &vivi::Color::green)
      .def_readwrite("blue", &vivi::Color::blue)
      .def_readwrite("alpha", &vivi::Color::alpha)
      .def("inverse", &vivi::Color::Inverse,
           "Returns the inverse color, i.e. (1.0-r, 1.0-g, 1.0-b).\n"
           "Alpha value stays the same.");


  m.def("rgba", vivi::rgba, "Returns a vivi.Color for the given rgba values.\n"
                            "r, g, b and alpha must be within [0, 1]",
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);


  m.def("RGBA", vivi::RGBA, "Returns a vivi.Color for the given RGBA values.\n"
                            "* R,G,B must be within [0, 255]\n"
                            "* alpha must be within [0, 1].",
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);

  //-----------------------------------------------------------------------------------------
  // PRIMITIVES
  // * Geometric primitives

  //TODO point, line, plane, rect

  //-----------------------------------------------------------------------------------------
  // PRIMITIVES
  // * Image representation related

  //-----------------------------------------------------------------------------------------
  // DRAWING
  // * Painter


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
//  return m.ptr();
}
