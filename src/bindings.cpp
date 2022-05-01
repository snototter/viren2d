#include <sstream>
#include <memory>
#include <stdexcept>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <viren2d/viren2d.h>

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
namespace pickling {
//------------------------------------------------- Color
py::tuple SerializeColor(const viren2d::Color &c) {
  return py::make_tuple(c.red, c.green, c.blue, c.alpha);
}


viren2d::Color DeserializeColor(py::tuple tpl) {
  if (tpl.size() != 4) {
    std::stringstream s;
    s << "Invalid viren2d.Color state - expected 4 values (rgba), got " << tpl.size() << "!";
    throw std::invalid_argument(s.str());
  }
  return viren2d::Color(tpl[0].cast<double>(), tpl[1].cast<double>(),
                     tpl[2].cast<double>(), tpl[3].cast<double>());
}

//------------------------------------------------- Templated Vec(tor) class
template<typename _Tp, int dim>
py::list SerializeVec(const viren2d::Vec<_Tp, dim> &vec) {
  py::list lst;
  for (int i = 0; i < dim; ++i)
    lst.append(vec.val[i]);
  return lst;
}

template<typename _Tp, int dim>
viren2d::Vec<_Tp, dim> DeserializeVec(py::list lst) {
  using VC = viren2d::Vec<_Tp, dim>;
  if (lst.size() != dim) {
    std::stringstream s;
    s << "Invalid viren2d." << VC::TypeName()
      << " state - expected " << dim << " values, found "
      << lst.size() << "!";
    throw std::invalid_argument(s.str());
  }

  VC vec;
  for (int i = 0; i < dim; ++i)
    vec.val[i] = lst[i].cast<_Tp>();
  return vec;
}

//------------------------------------------------- Rectangle
py::tuple SerializeRect(const viren2d::Rect &r) {
  return py::make_tuple(r.cx, r.cy, r.width, r.height, r.angle, r.radius);
}


viren2d::Rect DeserializeRect(py::tuple tpl) {
  if (tpl.size() != 6) {
    std::stringstream s;
    s << "Invalid viren2d.Rect state - expected 6 entries, got " << tpl.size() << "!";
    throw std::invalid_argument(s.str());
  }
  return viren2d::Rect(tpl[0].cast<double>(), tpl[1].cast<double>(),
                    tpl[2].cast<double>(), tpl[3].cast<double>(),
                    tpl[4].cast<double>(), tpl[5].cast<double>());
}


//------------------------------------------------- TODO Pickle others if needed




//------------------------------------------------- LineStyle
py::tuple SerializeLineStyle(const viren2d::LineStyle &c) {
  return py::make_tuple(c.line_width, c.color, c.dash_pattern, c.line_cap, c.line_join);
}


viren2d::LineStyle DeserializeLineStyle(py::tuple tpl) {
  if (tpl.size() != 5) {
    std::stringstream s;
    s << "Invalid viren2d.LineStyle state - expected 5 entries, got " << tpl.size() << "!";
    throw std::invalid_argument(s.str());
  }
  viren2d::LineStyle ls(tpl[0].cast<double>(),
                     tpl[1].cast<viren2d::Color>(),
                     tpl[2].cast<std::vector<double>>(),
                     tpl[3].cast<viren2d::LineCap>(),
                     tpl[4].cast<viren2d::LineJoin>());
  return ls;
}

//------------------------------------------------- ArrowStyle
py::tuple SerializeArrowStyle(const viren2d::ArrowStyle &st) {
  auto ls = static_cast<const viren2d::LineStyle &>(st);
  auto ls_tpl = SerializeLineStyle(ls);

  return py::make_tuple(ls_tpl, st.tip_length,
                        st.tip_angle, st.tip_closed);
}


viren2d::ArrowStyle DeserializeArrowStyle(py::tuple tpl) {
  if (tpl.size() != 4) {
    std::stringstream s;
    s << "Invalid viren2d.ArrowStyle state - expected 4 entries, got " << tpl.size() << "!";
    throw std::invalid_argument(s.str());
  }
  auto ls = tpl[0].cast<viren2d::LineStyle>();
  return viren2d::ArrowStyle(ls.line_width, ls.color,
      tpl[1].cast<double>(), tpl[2].cast<double>(),
      tpl[3].cast<bool>(), ls.dash_pattern,
      ls.line_cap, ls.line_join);
}

} // namespace pickling




/** @brief Utilities to simplify module definition. */
namespace moddef {
//------------------------------------------------- Wrapper for the Painter

/**
 * @brief A wrapper to viren2d::Painter
 *
 * This is necessary because I don't want to expose
 * the ImagePainter (I like the factory method; public
 * headers are quite clean). Thus, I cannot use the
 * pybind11 trampoline mechanism.
 */
class Painter {
public:
  Painter() : painter_(viren2d::CreateImagePainter())
  {}

  void SetCanvasColor(int width, int height, const viren2d::Color &color) {
    painter_->SetCanvas(width, height, color);
  }

  void SetCanvasFilename(const std::string &image_filename) {
    painter_->SetCanvas(image_filename);
  }

  void SetCanvasImage(const viren2d::ImageBuffer &image) {
    painter_->SetCanvas(image);
  }

  viren2d::ImageBuffer GetCanvas(bool copy) {
    return painter_->GetCanvas(copy);
  }


  void DrawArc(const viren2d::Vec2d &center, double radius,
               double angle1, double angle2,
               const viren2d::LineStyle &line_style,
               const viren2d::Color &fill) {
    painter_->DrawArc(center, radius, angle1, angle2, line_style, fill);
  }


  void DrawCircle(const viren2d::Vec2d &center, double radius,
                  const viren2d::LineStyle &line_style,
                  const viren2d::Color &fill) {
    painter_->DrawCircle(center, radius, line_style, fill);
  }


  void DrawLine(const viren2d::Vec2d &from, const viren2d::Vec2d &to,
                const viren2d::LineStyle &line_style) {
    painter_->DrawLine(from, to, line_style);
  }


  void DrawRect(const viren2d::Rect &rect, const viren2d::LineStyle &line_style,
                const viren2d::Color &fill) {
    painter_->DrawRect(rect, line_style, fill);
  }

private:
  std::unique_ptr<viren2d::Painter> painter_;
};



//-------------------------------------------------  Color from tuple
viren2d::Color CreateColor(py::tuple tpl) {
  if (tpl.size() < 3 || tpl.size() > 4) {
    std::stringstream s;
    s << "Cannot create viren2d.Color: expected 3 or 4 values, found tuple with"
      << tpl.size() << "!";
    throw std::invalid_argument(s.str());
  }
  viren2d::Color col(tpl[0].cast<double>(),
                  tpl[1].cast<double>(),
                  tpl[2].cast<double>(), 1.0);
  if (tpl.size() == 4)
    col.alpha = tpl[3].cast<double>();
  return col;
}


//-------------------------------------------------  LineStyle from tuple
viren2d::LineStyle CreateLineStyle(py::tuple tpl) {
  if (tpl.size() < 2 || tpl.size() > 5) {
    std::stringstream s;
    s << "Cannot create viren2d.LineStyle from tuple with"
      << tpl.size() << " entries!";
    throw std::invalid_argument(s.str());
  }

  viren2d::LineStyle ls(tpl[0].cast<double>(),
                     tpl[1].cast<viren2d::Color>());
  if (tpl.size() > 2)
    ls.dash_pattern = tpl[2].cast<std::vector<double>>();

  if (tpl.size() > 3)
    ls.line_cap = tpl[3].cast<viren2d::LineCap>();

  if (tpl.size() > 4)
    ls.line_join = tpl[4].cast<viren2d::LineJoin>();

  return ls;
}


//-------------------------------------------------  Rectangle from tuple
viren2d::Rect CreateRect(py::tuple tpl) {
  if (tpl.size() < 4 || tpl.size() > 6) {
    std::stringstream s;
    s << "Cannot create viren2d.Rect from tuple with"
      << tpl.size() << " entries!";
    throw std::invalid_argument(s.str());
  }

  viren2d::Rect rect(tpl[0].cast<double>(),
                  tpl[1].cast<double>(),
                  tpl[2].cast<double>(),
                  tpl[3].cast<double>());
  if (tpl.size() > 4)
    rect.angle = tpl[4].cast<double>();
  if (tpl.size() > 5)
    rect.radius = tpl[5].cast<double>();
  return rect;
}


//------------------------------------------------- ImageBuffer from numpy array
// We need a uint8, row-major (C-style) numpy array:
viren2d::ImageBuffer CreateImageBuffer(
    py::array_t<unsigned char, py::array::c_style | py::array::forcecast> buf) {
  // Sanity checks
  if (buf.ndim() < 2 || buf.ndim() > 3)
    throw std::runtime_error("Incompatible image dimension!");

  if (!buf.dtype().is(py::dtype::of<uint8_t>()))
    throw std::runtime_error("Incompatible format: expected a uint8 array!");

  viren2d::ImageBuffer img;
  const int row_stride = static_cast<int>(buf.strides(0));
  const int height = static_cast<int>(buf.shape(0));
  const int width = static_cast<int>(buf.shape(1));
  const int channels = (buf.ndim() == 2) ? 1 : static_cast<int>(buf.shape(2));
  img.CreateSharedBuffer(buf.mutable_data(), width, height, channels, row_stride);
  return img;
}

//------------------------------------------------- Vec(tor) from tuple
template<typename _Tp, int dim>
viren2d::Vec<_Tp, dim> CreateVec(py::tuple tpl) {
  using VC = viren2d::Vec<_Tp, dim>;
  if (tpl.size() != dim) {
    std::stringstream s;
    s << "Cannot create viren2d." << VC::TypeName()
      << ": expected " << dim << " values, found tuple with"
      << tpl.size() << "!";
    throw std::invalid_argument(s.str());
  }

  VC vec;
  for (int i = 0; i < dim; ++i)
    vec.val[i] = tpl[i].cast<_Tp>();
  return vec;
}


template<typename _Tp, int dim>
void RegisterVec(py::module &m) {
  using VC = viren2d::Vec<_Tp, dim>;
  std::stringstream doc;
  doc << "Vector in " << dim << "D space.";

  py::class_<VC> vec_cls(m, VC::TypeName().c_str(), doc.str().c_str());
  //TODO optionally add py::dynamic_attr()
  // (could be useful if we want to use these vecs more extensively in python)

  std::stringstream doc_tpl;
  doc_tpl << "Initialize " << VC::TypeName() << " from "
          << dim << "-element tuple.";

  vec_cls.def(py::init<>())
      .def(py::init<>(&CreateVec<_Tp, dim>),
           doc_tpl.str().c_str())
      .def("__repr__",
           [](const VC &v)
           { return "<viren2d." + v.ToString() + ">"; })
      .def("__str__", &VC::ToString)
      .def_property("x", static_cast<const _Tp &(VC::*)() const>(&VC::x), &VC::SetX,
                    "Accesses the first dimension.")
      .def_property("y", static_cast<const _Tp &(VC::*)() const>(&VC::y), &VC::SetY,
                    "Accesses the second dimension.")
      .def_property_readonly("ndim", [](const VC&) { return dim; },
                             "Number of dimensions.")
      .def("__setitem__", [](VC &self, int index, _Tp v)
                          { self[index] = v; })
      .def("__getitem__", [](const VC &self, int index)
                          { return self[index]; })
      .def("copy", [](const VC &self) { return VC(self); },
           "Returns a copy of this vector.")
      .def("dot", &VC::Dot, py::arg("other"))
      .def("length", &VC::Length)
      .def("length_squared", &VC::LengthSquared)
      .def("distance", &VC::Distance, py::arg("other"))
      .def(py::pickle(&pickling::SerializeVec<_Tp, dim>,
                      &pickling::DeserializeVec<_Tp, dim>))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self + py::self)
      .def(py::self += py::self)
      .def(py::self - py::self)
#ifdef __clang__
// Clang gives false warnings: https://bugs.llvm.org/show_bug.cgi?id=43124
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif  // __clang__
      .def(py::self -= py::self)
#ifdef __clang__
#pragma GCC diagnostic pop
#endif  // __clang__
      .def(py::self * float())
      .def(py::self *= float())
      .def(float() * py::self)
      .def(py::self / float())
      .def(py::self /= float());


  // Specific for 2-dim vectors
  if (dim == 2)
    vec_cls.def(py::init<_Tp, _Tp>(),
                py::arg("x"), py::arg("y"));

  // Specific for 3-dim vectors
  if (dim == 3)
  {
    vec_cls.def(py::init<_Tp, _Tp, _Tp>(),
                py::arg("x"), py::arg("y"), py::arg("z"));
    vec_cls.def("cross", &VC::Cross,
                "Cross product", py::arg("other"));
  }

  if (dim > 2)
    vec_cls.def_property("z", static_cast<const _Tp &(VC::*)() const>(&VC::z), &VC::SetZ,
                         "Accesses the third dimension.");

  // Specific for >=4-dim vectors
  if (dim == 4)
    vec_cls.def(py::init<_Tp, _Tp, _Tp, _Tp>(), py::arg("x"), py::arg("y"), py::arg("z"), py::arg("w"));

  if (dim > 3)
    vec_cls.def_property("w", static_cast<const _Tp &(VC::*)() const>(&VC::w), &VC::SetW,
                         "Accesses the fourth dimension.");

  py::implicitly_convertible<py::tuple, VC>();
}

} // namespace moddef


//------------------------------------------------- Module definition
// TODO How to bind a new class X:
// * Implement moddef::CreateX (init from py::tuple/list/whatever)
// * Implement pickling::SerializeX
// * Implement pickling::DeserializeX
// * Implement __str__ & __repr__
// * nice-to-have: operator == and !=
// * Declare it py::implicitly_convertible
// * Check (in python) initialization, pickling, comparison, etc.
// * All this info does not hold for ImageBuffer - which exposes a
//   buffer view (and we need to be able to convert to/from numpy
//   arrays)
//
// TODO low priority: DeserializeX could reuse CreateX (not sure if
//      both are using the same inputs, tuples vs list vs ...)

PYBIND11_MODULE(viren2d_PYMODULE_NAME, m) {
  m.doc() = R"pbdoc(
    Vision & Rendering 2D

    This is a light-weight 2D rendering toolbox to
    easily visualize the results of common computer
    vision tasks, such as detections, trajectories,
    and the like.
  )pbdoc";


  //------------------------------------------------- Color
  m.def("color_names", &viren2d::ListNamedColors,
        "Returns the names of premixed colors. These names\n"
        "can be used to initialize a Color object.");

  py::class_<viren2d::Color>(m, "Color",
           "Represents a color in rgba format, i.e. each component is within [0,1].")
      .def(py::init<>(),
           "Initializes an 'invalid color' (r,g,b < 0) which can\n"
           "be used in several Painter methods to request special\n"
           "color handling (e.g. switching to the inverse color).")
      .def(py::init<const viren2d::Color &>(),
           "Initializes a color as copy. If you want the same color\n"
           "with a different alpha, use with_alpha() instead.",
           py::arg("color"))
      .def(py::init<>(&moddef::CreateColor),
           "Initialize from tuple (all values must\n"
           "be within [0, 1]):\n"
           "* (red, green, blue)\n"
           "* (red, green, blue, alpha)")
      .def(py::init<double, double, double, double>(),
           "Initialize with the given color components. All values\n"
           "will be clamped to [0, 1].",
           py::arg("red"), py::arg("green"), py::arg("blue"),
           py::arg("alpha")=1.0)
      .def(py::init<const std::string &, double>(),
           "Initialize from a string represenation:\n"
           "* Hex/Webcodes:\n '#00ff00', '#a0b0c0f0'\n"
           "* A color name, e.g. 'black', 'navy-blue'.\n"
           "  See color_names() for a list of available\n"
           "  color names.\n"
           "* A color name with alpha suffix (which must\n"
           "  be an integer in [0, 100]):\n  'forest-green!50'"
           "* A color name can also be inverted (to get its\n"
           "   complementary color) by prepending '!' or '-':\n"
           "  '!blue!30' (would be equal to 'yellow!30')",
           py::arg("colorspec"), py::arg("alpha")=1.0)
      .def("__repr__",
           [](const viren2d::Color &c)
           { return "<viren2d.Color " + c.ToHexString() + ">"; })
      .def("__str__", &viren2d::Color::ToHexString)
      .def(py::pickle(&pickling::SerializeColor,
                      &pickling::DeserializeColor))
      .def(py::self == py::self,
           "'==' operator: Returns True if ALL components (r, g,\n"
           "               b & alpha) are equal.")
      .def(py::self != py::self,
           "'!=' operator: Returns True if ANY component (r, g,\n"
           "               b or alpha) differ.")
      .def(py::self += py::self,
           "'+=' operator: Adds the other's r,g,b values and clamps\n"
           "               the result to [0, 1]. Alpha will not be changed.")
      .def(py::self + py::self,
           "'+' operator: Adds the r,g,b values of two Color objects and\n"
           "              clamps the result to [0, 1]. Alpha will not be changed.")
#ifdef __clang__
// Clang gives false warnings: https://bugs.llvm.org/show_bug.cgi?id=43124
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif  // __clang__
      .def(py::self -= py::self,
           "'+=' operator: Subtracts the other's r,g,b values and clamps\n"
           "               the result to [0, 1]. Alpha will not be changed.")
#ifdef __clang__
#pragma GCC diagnostic pop
#endif  // __clang__
      .def(py::self - py::self,
           "'-' operator: Subtracts the r,g,b values of rhs from lhs and\n"
           "              clamps the result to [0, 1]. Alpha will not be changed.")
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
      .def("as_RGBa", &viren2d::Color::ToRGBa,
           "Returns the corresponding (R, G, B, a) tuple,\n"
           "where R, G, B in [0, 255] and alpha in [0, 1].")
      .def("as_rgba", [](viren2d::Color& c)
           { return py::make_tuple(c.red, c.green, c.blue, c.alpha); },
           "Returns the corresponding (r, g, b, a) tuple,\n"
           "where all components are in[0, 1].")
      .def("as_hex", &viren2d::Color::ToHexString,
           "Returns the hex web color code representation,\n"
           "e.g. '#0011ffff' (all components are scaled to [0, 255]).")
      .def("with_alpha", &viren2d::Color::WithAlpha,
           "Return a color with the same r,g,b components, but the given alpha.",
           py::arg("alpha"))
      .def_readwrite("red", &viren2d::Color::red,
                     "Red component within [0, 1].")
      .def_readwrite("green", &viren2d::Color::green,
                     "Green component within [0, 1].")
      .def_readwrite("blue", &viren2d::Color::blue,
                     "Blue component within [0, 1].")
      .def_readwrite("alpha", &viren2d::Color::alpha,
                     "Opacity within [0, 1].")
      // TODO pybind11 bug, documentation of static members is
      //      not carried over to python: https://github.com/pybind/pybind11/issues/3815
      .def_readonly_static("White", &viren2d::Color::White,
                           "Read-only white color instantiation (for convenience).")
      .def_readonly_static("Black", &viren2d::Color::Black,
                           "Read-only black color instantiation (for convenience).")
      .def_readonly_static("Red", &viren2d::Color::Red,
                           "Read-only red color instantiation (for convenience).")
      .def_readonly_static("Green", &viren2d::Color::Green,
                           "Read-only green color instantiation (for convenience).")
      .def_readonly_static("Blue", &viren2d::Color::Blue,
                           "Read-only blue color instantiation (for convenience).")
      .def_readonly_static("Cyan", &viren2d::Color::Cyan,
                           "Read-only cyan color instantiation (for convenience).")
      .def_readonly_static("Magenta", &viren2d::Color::Magenta,
                           "Read-only magenta color instantiation (for convenience).")
      .def_readonly_static("Yellow", &viren2d::Color::Yellow,
                           "Read-only yellow color instantiation (for convenience).")
      .def("is_valid", &viren2d::Color::IsValid,
           "Returns True if this is a valid rgba color, where all\n"
           "components are within [0, 1].")
      .def("inverse", &viren2d::Color::Inverse,
           "Returns the inverse/complementary color.\n\n"
           "Except for shades of gray, this returns (1.0-r, 1.0-g, 1.0-b).\n"
           "For gray values it will either return black or white. The alpha\n"
           "value will always stay the same.\n"
           "Why special handling of gray? Complementary colors should be\n"
           "used to provide good contrast/highlights - thus, having the\n"
           "true inverse (i.e. 1-r|g|b) for medium gray (r|g|b close to 127)\n"
           "would not be too useful.")
      .def("is_shade_of_gray", &viren2d::Color::IsShadeOfGray,
           "Checks if all rgb components almost the same (+/- the given epsilon).",
           py::arg("eps")=0.02);

  // A Color can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, viren2d::Color>();

  // A Color can be initialized from a string representation directly.
  py::implicitly_convertible<py::str, viren2d::Color>();

  m.def("rgba", &viren2d::rgba,
        "Returns a viren2d.Color for the given rgba values.\n"
        "r, g, b and alpha must be within [0, 1]",
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);


  m.def("RGBa", &viren2d::RGBa,
        "Returns a viren2d.Color for the given RGBa values.\n"
        "* R,G,B must be within [0, 255]\n"
        "* alpha must be within [0, 1].",
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);

  //------------------------------------------------- Primitives - Vectors
  moddef::RegisterVec<double, 2>(m);
  moddef::RegisterVec<double, 3>(m);
  //TODO add more vector bindings once we need them
//  moddef::RegisterVec<double, 4>(m);
//  moddef::RegisterVec<int, 2>(m);
//  moddef::RegisterVec<int, 3>(m);


  //------------------------------------------------- Primitives - Rectangle
  py::class_<viren2d::Rect>(m, "Rect",
           "Rectangle for visualization.\n\n"
           "Note that a rectangle is defined by its CENTER,\n"
           "width, height, angle (clockwise rotation in degrees),\n"
           "and a corner radius (for rounded rectangles).")
      .def(py::init<>(&moddef::CreateRect),
           "Initialize from tuple:\n"
           "* (cx, cy, w, h)\n"
           "* (cx, cy, w, h, angle)\n"
           "* (cx, cy, w, h, angle, radius)",
           py::arg("tpl"))  // init from tuple
      .def(py::init<double, double, double, double>(),
           "Create an axis-aligned square box:\n"
           "  Center coordinates and dimensions.",
           py::arg("cx"), py::arg("cx"), py::arg("w"), py::arg("h"))
      .def(py::init<double, double, double, double, double>(),
           "Create a rotated square box:\n"
           "  Center coordinates, dimensions and\n"
           "  rotation (clockwise, in degrees).",
           py::arg("cx"), py::arg("cx"), py::arg("w"), py::arg("h"),
           py::arg("angle"))
      .def(py::init<double, double, double, double, double, double>(),
           "Create a rotated rounded rectangle:\n"
           "  Center coordinates, dimensions, rotation (clockwise,\n"
           "  in degrees), and corner radius (in pixels).",
           py::arg("cx"), py::arg("cx"), py::arg("w"), py::arg("h"),
           py::arg("angle"), py::arg("radius"))
      .def("__repr__",
           [](const viren2d::Rect &r)
           { return "<viren2d." + r.ToString() + ">"; })
      .def("__str__", &viren2d::Rect::ToString)
      .def(py::pickle(&pickling::SerializeRect,
                      &pickling::DeserializeRect))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_readwrite("cx", &viren2d::Rect::cx, "Horizontal center.")
      .def_readwrite("cy", &viren2d::Rect::cy, "Vertical center.")
      .def_readwrite("width", &viren2d::Rect::width, "Rectangle width.")
      .def_property_readonly("half_width", &viren2d::Rect::half_width, "Half the rectangle width (for us lazy folks).") // I mostly wanted to try pybind11's property definition
      .def_property_readonly("half_height", &viren2d::Rect::half_height, "Half the rectangle height (for us lazy folks).")
      .def_readwrite("height", &viren2d::Rect::height, "Rectangle height.")
      .def_readwrite("angle", &viren2d::Rect::angle, "Angle in degrees (clockwise rotation).")
      .def_readwrite("radius", &viren2d::Rect::radius, "Corner radius (for rounded rectangles).")
      .def("is_valid", &viren2d::Rect::IsValid, "Returns True if both width & height are > 0.");

  // A Rect can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, viren2d::Rect>();


  //TODO plane? maybe - either use vec4d or make a full-blown plane class
  //     preferably reuse vec4d - nobody expects a visualization toolbox to
  //     be the go-to resource for (easy to use) geometric datatypes

  //------------------------------------------------- Primitives - ImageBuffer
  // Info on numpy memory: https://stackoverflow.com/a/53099870/400948
  py::class_<viren2d::ImageBuffer>(m, "ImageBuffer", py::buffer_protocol(),
                                "An ImageBuffer holds 8-bit images (Grayscale, "
                                "RGB or RGBA).")
      .def(py::init(&moddef::CreateImageBuffer),
           "Initialize from numpy array with dtype uint8.")
     .def_buffer([](viren2d::ImageBuffer &img) -> py::buffer_info {
          return py::buffer_info(
              img.data, sizeof(unsigned char), // Pointer to data & size of each element
              py::format_descriptor<unsigned char>::format(), // Python struct-style format descriptor
              3, // We'll always return ndim=3 arrays
              { static_cast<size_t>(img.height),
                static_cast<size_t>(img.width),
                static_cast<size_t>(img.channels) }, // Buffer dimensions
              { static_cast<size_t>(img.stride),
                static_cast<size_t>(img.channels),
                sizeof(unsigned char) } // Strides (in bytes) per dimension
          );
      })
      .def("to_rgb", &viren2d::ImageBuffer::ToRGB,
           "Convert to RGB. Will always return a copy, even if this buffer\n"
           "is already RGB.")
      .def("to_rgba", &viren2d::ImageBuffer::ToRGBA,
           "Convert to RGBA. Will always return a copy, even if this buffer\n"
           "is already RGBA.")
      .def("__repr__",
           [](const viren2d::ImageBuffer &b)
           { return "<viren2d." + b.ToString() + ">"; })
      .def("__str__", &viren2d::ImageBuffer::ToString)
      .def_readonly("width", &viren2d::ImageBuffer::width, "Image width in pixels.")
      .def_readonly("height", &viren2d::ImageBuffer::height, "Image height in pixels.")
      .def_readonly("channels", &viren2d::ImageBuffer::channels, "Number of channels.")
      .def_readonly("stride", &viren2d::ImageBuffer::stride, "Stride per row (in bytes).")
      .def_readonly("owns_data", &viren2d::ImageBuffer::owns_data_,
                    "Boolean flag indicating if it has ownership of the image data,\n"
                    "i.e. whether it is responsible for memory cleanup.");

  // An ImageBuffer can be initialized from a numpy array
  py::implicitly_convertible<py::array, viren2d::ImageBuffer>();


  //------------------------------------------------- Drawing - LineStyle
  py::enum_<viren2d::LineCap>(m, "LineCap",
             "How to render the endpoints of the line (or dash strokes).")
      .value("Butt", viren2d::LineCap::Butt,
             "Start/stop the line exactly at the start/end point.")
      .value("Round", viren2d::LineCap::Round,
             "Round ending, center of the circle is the end point.")
      .value("Square", viren2d::LineCap::Square,
             "Square ending, center of the square is the end point.");


  py::enum_<viren2d::LineJoin>(m, "LineJoin",
             "How to render the junction of two lines/segments.")
      .value("Miter", viren2d::LineJoin::Miter,
             "Sharp (angled) corner.")
      .value("Bevel", viren2d::LineJoin::Bevel,
             "Cut off the join at half the line width from the joint point.")
      .value("Round", viren2d::LineJoin::Round,
             "Rounded join, where the center of the circle is the joint point.");


  py::class_<viren2d::LineStyle> line_style(m, "LineStyle",
                                            "How a line should be drawn.");
  line_style.def(py::init<>(&moddef::CreateLineStyle)) // init from tuple
      .def(py::init<double, viren2d::Color, std::vector<double>,
                    viren2d::LineCap, viren2d::LineJoin>(),
           py::arg("line_width") = 2.0,
           py::arg("color") = viren2d::Color(viren2d::NamedColor::Azure),
           py::arg("dash_pattern")=std::vector<double>(),
           py::arg("line_cap")=viren2d::LineCap::Butt,
           py::arg("line_join")=viren2d::LineJoin::Miter)
      .def("__repr__",
           [](const viren2d::LineStyle &st)
           { return "<viren2d." + st.ToString() + ">"; })
      .def("__str__", &viren2d::LineStyle::ToString)
      .def(py::pickle(&pickling::SerializeLineStyle,
                      &pickling::DeserializeLineStyle))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_valid", &viren2d::LineStyle::IsValid,
           "Check if the style would lead to a drawable line.")
      .def_readwrite("line_width", &viren2d::LineStyle::line_width,
                     "Width in pixels (best results for even values).")
      .def_readwrite("color", &viren2d::LineStyle::color, "Line color (rgba).")
      .def_readwrite("dash_pattern", &viren2d::LineStyle::dash_pattern,
                     "Dash pattern defined as list of on/off strokes (lengths in\n"
                     "pixels), e.g. [20, 10, 40, 10]. If the list is empty, the\n"
                     "line will be drawn solid.")
      .def_readwrite("line_cap", &viren2d::LineStyle::line_cap,
                     "How to render the endpoints of the line (or dash strokes).")
      .def_readwrite("line_join", &viren2d::LineStyle::line_join,
                     "How to render the junction of two lines/segments.");


  // A LineStyle can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, viren2d::LineStyle>();


  //------------------------------------------------- Drawing - ArrowStyle
  py::class_<viren2d::ArrowStyle, viren2d::LineStyle>(m, "ArrowStyle",
           "How an arrow should be drawn.")
      .def(py::init<double, viren2d::Color, double, double, bool,
                    std::vector<double>, viren2d::LineCap, viren2d::LineJoin>(),
           py::arg("line_width") = 2.0,
           py::arg("color") = viren2d::Color(viren2d::NamedColor::Azure),
           py::arg("tip_length") = 0.1,
           py::arg("tip_angle") = 30.0,
           py::arg("tip_closed") = false,
           py::arg("dash_pattern")=std::vector<double>(),
           py::arg("line_cap")=viren2d::LineCap::Butt,
           py::arg("line_join")=viren2d::LineJoin::Miter)
      .def("__repr__",
           [](const viren2d::ArrowStyle &st)
           { return "<viren2d." + st.ToString() + ">"; })
      .def("__str__", &viren2d::ArrowStyle::ToString)
      .def(py::pickle(&pickling::SerializeArrowStyle,
                      &pickling::DeserializeArrowStyle))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_valid", &viren2d::ArrowStyle::IsValid,
           "Check if the style would lead to a drawable arrow.")
      .def_readwrite("tip_length", &viren2d::ArrowStyle::tip_length) //TODO doc
      .def_readwrite("tip_angle", &viren2d::ArrowStyle::tip_angle) //TODO doc
      .def_readwrite("tip_closed", &viren2d::ArrowStyle::tip_closed) //TODO doc
      ;
  //TODO implicit from tuple!

  //------------------------------------------------- Drawing - Painter

  py::class_<moddef::Painter>(m, "Painter",
          R"pbdoc(A Painter lets you draw on a canvas.

          Usage:
          1. Create a Painter
          2. Initialize the canvas via either:
             * set_canvas_rgb(),
             * set_canvas_image(), or
             * set_canvas_filename()
          3. Draw onto the canvas via draw_xxx()
          4. When all objects have been drawn, retrieve
             the visualization via get_canvas()
          5. The painter can be reused for future
             visualizations: start over at set_canvas_xxx()
          )pbdoc")
      .def(py::init<>())
      .def("__repr__", [](const moddef::Painter &) { return "<viren2d.Painter>"; })
      .def("__str__", [](const moddef::Painter &) { return "viren2d.Painter"; })
      .def("set_canvas_rgb", &moddef::Painter::SetCanvasColor,
           "Initializes the canvas with the given color.",
           py::arg("width"), py::arg("height"),
           py::arg("color")=viren2d::Color::White)
      .def("set_canvas_filename", &moddef::Painter::SetCanvasFilename,
           "Initializes the canvas from the given image file.\n"
           "Supported formats are:\n"
           "  JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM\n\n"
           "Relies on the stb library, so check for updates if\n"
           "your format is missing:\n"
           "https://github.com/nothings/stb/blob/master/stb_image.h",
           py::arg("image_filename"))
      .def("set_canvas_image", &moddef::Painter::SetCanvasImage,
           "Initializes the canvas from the given image, i.e. either\n"
           "a numpy array (dtype uint8) or a viren2d.ImageBuffer.\n\n"
           "Example:\n"
           "  img_np = np.zeros((480, 640, 3), dtype=np.uint8)\n"
           "  painter.set_canvas_image(img_np)",
           py::arg("image"))
      .def("get_canvas", &moddef::Painter::GetCanvas,
           "Returns the current state of the visualization.\n\n"
           "If you want a copy, set copy=True. Otherwise, the buffer\n"
           "will just provide a view on the Painter's canvas.\n"
           "--> If you keep on drawing, this will also affect the\n"
           "    previously obtained canvas.\n\n"
           "Examples:\n"
           "* Get canvas as numpy array, memory is\n"
           "  SHARED with the painter:\n"
           "    img_np = np.array(p.get_canvas(), copy=False)\n\n"
           "* Retrieve a deep COPY of the canvas as\n"
           "  numpy array:\n"
           "    img_np = np.array(p.get_canvas(True), copy=False)",
           py::arg("copy")=false)
//------------------- TODO keep alphabetic order - easier to maintain!
//----- FIXME: don't use python keywords as argument names!
      .def("draw_arc", &moddef::Painter::DrawArc,
           "Draws a circular arc of the given radius using the\n"
           "LineStyle specification. The arc will be filled if\n"
           "a fill color with alpha > 0 is given.\n"
           "Angles are in degrees, where 0.0 is in the direction\n"
           "of the positive X axis (in user space). The arc will\n"
           "be drawn from angle1 to angle2 in clockwise direction.",
           py::arg("center"), py::arg("radius"),
           py::arg("angle1"), py::arg("angle2"),
           py::arg("line_style"),
           py::arg("fill")=viren2d::Color(0, 0, 0, 0))
      .def("draw_circle", &moddef::Painter::DrawCircle,
           "Draws a circle at the given Vec2d position using the\n"
           "LineStyle specification. The circle will be filled if\n"
           "a fill color with alpha > 0 is given.",
           py::arg("center"), py::arg("radius"), py::arg("line_style"),
           py::arg("fill")=viren2d::Color(0, 0, 0, 0))
      .def("draw_line", &moddef::Painter::DrawLine,
           "Draws a line between the two Vec2d coordinates using the\n"
           "LineStyle specification.",
           py::arg("pt1"), py::arg("pt2"), py::arg("line_style"))
      .def("draw_rect", &moddef::Painter::DrawRect,
           "Draws a rectangle using the LineStyle specification.\n\n"
           "* The rectangle will be filled if fill color has alpha > 0.\n"
           "* Draw a rotated rectangle by defining the rect's angle\n"
           "  in degrees (clockwise rotation).\n"
           "* By defining the rect's corner radius, you can draw\n"
           "  a rounded rectangle.",
           py::arg("rect"), py::arg("line_style"),
           py::arg("fill")=viren2d::Color(0, 0, 0, 0))
      //TODO add draw_xxx methods
      ;

#ifdef viren2d_VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(viren2d_VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
