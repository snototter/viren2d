#include <sstream>
#include <memory>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <vivi/vivi.hpp>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;


/** @brief Utilities to support pickling.
 *
 *  Naming convention:
 *  * Serialize<X>   --> X.__getstate__
 *  * Deserialize<X> --> X.__setstate__
 *
 *  See corresponding pybind11 doc:
 *  https://pybind11.readthedocs.io/en/stable/advanced/classes.html#pickling-support
 */
namespace pickling
{
//------------------------------------------------- Color
py::tuple SerializeColor(const vivi::Color &c)
{
  return py::make_tuple(c.red, c.green, c.blue, c.alpha);
}


vivi::Color DeserializeColor(py::tuple tpl)
{
  if (tpl.size() != 4)
  {
    std::stringstream s;
    s << "Invalid vivi.Color state - expected 4 values (rgba), got " << tpl.size() << "!";
    throw std::runtime_error(s.str());
  }
  return vivi::Color(tpl[0].cast<double>(),
      tpl[1].cast<double>(),
      tpl[2].cast<double>(),
      tpl[3].cast<double>());
}

//------------------------------------------------- Templated Vec(tor) class
template<typename _Tp, int dim>
py::list SerializeVec(const vivi::Vec<_Tp, dim> &vec)
{
  py::list lst;
  for (int i = 0; i < dim; ++i)
    lst.append(vec.val[i]);
  return lst;
}

template<typename _Tp, int dim>
vivi::Vec<_Tp, dim> DeserializeVec(py::list lst)
{
  using VC = vivi::Vec<_Tp, dim>;
  if (lst.size() != dim)
  {
    std::stringstream s;
    s << "Invalid vivi." << VC::TypeName()
      << " state - expected " << dim << " values, found "
      << lst.size() << "!";
    throw std::runtime_error(s.str());
  }

  VC vec;
  for (int i = 0; i < dim; ++i)
    vec.val[i] = lst[i].cast<_Tp>();
  return vec;
}

//------------------------------------------------- TODO Pickle others if needed

} // namespace pickling




/** @brief Utilities to simplify module definition. */
namespace moddef
{
//-------------------------------------------------  Wrapper for the Painter
// Necessary, because we're not exposing the ImagePainter, so
// we can't use the pybind11 trampoline mechanism.
class Painter
{
public:
  Painter() : painter_(vivi::CreateImagePainter())
  {}

  void SetCanvas(int width, int height, const vivi::Color &color)
  { painter_->SetCanvas(width, height, color); }

  void DrawLine(const vivi::Vec2d& from, const vivi::Vec2d& to, //FIXME change to linestyle
                double line_width, const vivi::Color& color,
                const std::vector<double> &dash_style)
  { painter_->DrawLine(from, to, line_width, color, dash_style); }

  void DummyShow() { painter_->DummyShow(); } //TODO remove

private:
  std::unique_ptr<vivi::Painter> painter_;
};



//-------------------------------------------------  Color from tuple
vivi::Color CreateColor(py::tuple tpl)
{
  if (tpl.size() < 3 || tpl.size() > 4)
  {
    std::stringstream s;
    s << "Cannot create vivi.Color: expected 3 or 4 values, found tuple with"
      << tpl.size() << "!";
    throw std::runtime_error(s.str());
  }
  vivi::Color col(tpl[0].cast<double>(),
                  tpl[1].cast<double>(),
                  tpl[2].cast<double>(), 1.0);
  if (tpl.size() == 4)
    col.alpha = tpl[3].cast<double>();
  return col;
}


//-------------------------------------------------  Vec(tor) from tuple
template<typename _Tp, int dim>
vivi::Vec<_Tp, dim> CreateVec(py::tuple tpl)
{
  using VC = vivi::Vec<_Tp, dim>;
  if (tpl.size() != dim)
  {
    std::stringstream s;
    s << "Cannot create vivi." << VC::TypeName()
      << ": expected " << dim << " values, found tuple with"
      << tpl.size() << "!";
    throw std::runtime_error(s.str());
  }
  VC vec;
  for (int i = 0; i < dim; ++i)
    vec.val[i] = tpl[i].cast<_Tp>();
  return vec;
}


template<typename _Tp, int dim>
void RegisterVec(py::module &m)
{
  using VC = vivi::Vec<_Tp, dim>;
  auto &vccls = py::class_<VC>(m, VC::TypeName().c_str()) //TODO optional (could be useful if vecs are to be used in python more extensively):, py::dynamic_attr())
      .def(py::init<>())
      .def(py::init<>(&CreateVec<_Tp, dim>))
      .def(py::init<_Tp, _Tp>(), py::arg("x"), py::arg("y"))
      .def(py::init<_Tp, _Tp, _Tp>(), py::arg("x"), py::arg("y"), py::arg("z"))
      .def(py::init<_Tp, _Tp, _Tp, _Tp>(), py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"))
      .def("__repr__",
           [](const VC& v)
           { return "<vivi." + v.ToString() + ">"; },
           "Returns an unambiguous str representation.")
      .def("__str__", &VC::ToString,
           "Returns a readable str representation.")
      .def_property("x", static_cast<const _Tp& (VC::*)() const>(&VC::x), &VC::SetX)
      .def_property("y", static_cast<const _Tp& (VC::*)() const>(&VC::y), &VC::SetY)
      .def_property("z", static_cast<const _Tp& (VC::*)() const>(&VC::z), &VC::SetZ)
      .def_property("w", static_cast<const _Tp& (VC::*)() const>(&VC::w), &VC::SetW)
      .def("__setitem__", [](VC &self, unsigned index, _Tp v)
                          { self[index] = v; })
      .def("__getitem__", [](const VC &self, unsigned index)
                          { return self[index]; })
      .def("dot", &VC::Dot, py::arg("other"))
      .def("length", &VC::Length)
      .def("distance", &VC::Distance, py::arg("other"))
      .def(py::pickle(&pickling::SerializeVec<_Tp, dim>,
                      &pickling::DeserializeVec<_Tp, dim>))
      .def(py::self == py::self)
      .def(py::self != py::self);

  //TODO other operators:
  // +
  // +=
  // -
  // -=
  // *
  // *=
  // x / 3; /=

  if (dim == 3)
  {
    vccls.def("cross", &VC::Cross, "Cross product", py::arg("other"));
  }

  py::implicitly_convertible<py::tuple, VC>();
}

} // namespace moddef






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
      .def(py::init<>(&moddef::CreateColor))// init from tuple
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

  py::implicitly_convertible<py::tuple, vivi::Color>();

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
  // * Geometric primitives - 2d, 3d & 4d vectors/points:
  moddef::RegisterVec<double, 2>(m);
  moddef::RegisterVec<double, 3>(m);
  moddef::RegisterVec<double, 4>(m);
  moddef::RegisterVec<int, 2>(m);
  moddef::RegisterVec<int, 3>(m);

  //TODO TODO TODO
  // * Geometric primitives - lines (?)
  // * Geometric primitives - rectangles
  //TODO plane

  //-----------------------------------------------------------------------------------------
  // PRIMITIVES
  // * Image representation related

  //-----------------------------------------------------------------------------------------
  // DRAWING
  // * LineStyle //TODO

  // * Painter
  py::class_<moddef::Painter>(m, "Painter")
      .def(py::init<>())
      .def("set_canvas_rgb", &moddef::Painter::SetCanvas,
           "Initializes the canvas with the given color. This or\n"
           "set_canvas_image() must be called before anything can\n"
           "be drawn.", py::arg("width"), py::arg("height"),
           py::arg("color")=vivi::Color(0, 0, 0, 1))
//      .def("set_canvas_image")
      .def("draw_line", &moddef::Painter::DrawLine)
//           py::arg("from"), py::arg("to")) //TODO params!
      .def("show", &moddef::Painter::DummyShow, "TODO - Shows the current canvas; only used for initial development!");


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
