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

std::string Qualified(const std::string &tpname, bool with_tags = false) {
  std::stringstream s;
  if (with_tags)
    s << '<';
  s << MACRO_STRINGIFY(viren2d_PYMODULE_NAME)
    << '.' << tpname;
  if (with_tags)
    s << '>';
  return s.str();
}


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

  py::tuple GetCanvasSize() {
    auto sz = painter_->GetCanvasSize();
    return py::make_tuple(sz.x(), sz.y());
  }

  bool IsValid() {
    return (painter_ != nullptr) && (painter_->IsValid());
  }


  void DrawArc(const viren2d::Vec2d &center, double radius,
               double angle1, double angle2,
               const viren2d::LineStyle &line_style,
               bool include_center,
               const viren2d::Color &fill_color) {
    painter_->DrawArc(center, radius, angle1, angle2, line_style,
                      include_center, fill_color);
  }


  void DrawArrow(const viren2d::Vec2d &from, const viren2d::Vec2d &to,
                 const viren2d::ArrowStyle &arrow_style) {
    painter_->DrawArrow(from, to, arrow_style);
  }

  void DrawCircle(const viren2d::Vec2d &center, double radius,
                  const viren2d::LineStyle &line_style,
                  const viren2d::Color &fill_color) {
    painter_->DrawCircle(center, radius, line_style, fill_color);
  }

  void DrawEllipse(const viren2d::Ellipse &ellipse,
                   const viren2d::LineStyle &line_style,
                   const viren2d::Color &fill_color) {
    painter_->DrawEllipse(ellipse, line_style, fill_color);
  }

  void DrawGrid(double spacing_x, double spacing_y,
                const viren2d::LineStyle &line_style,
                const viren2d::Vec2d &top_left,
                const viren2d::Vec2d &bottom_right) {
    painter_->DrawGrid(top_left, bottom_right,
                       spacing_x, spacing_y, line_style);
  }

  void DrawLine(const viren2d::Vec2d &from, const viren2d::Vec2d &to,
                const viren2d::LineStyle &line_style) {
    painter_->DrawLine(from, to, line_style);
  }


  void DrawRect(const viren2d::Rect &rect, const viren2d::LineStyle &line_style,
                const viren2d::Color &fill_color) {
    painter_->DrawRect(rect, line_style, fill_color);
  }

private:
  std::unique_ptr<viren2d::Painter> painter_;
};



//-------------------------------------------------  Color from tuple
viren2d::Color ColorFromTuple(py::tuple tpl) {
  if (tpl.size() < 3 || tpl.size() > 4) {
    std::stringstream s;
    s << "Cannot create " << Qualified("Color", false)
      << ": expected 3 or 4 values, but got tuple with"
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


py::tuple ColorToTuple(const viren2d::Color &c) {
  return py::make_tuple(c.red, c.green, c.blue, c.alpha);
}


//-------------------------------------------------  LineStyle
py::tuple LineStyleToTuple(const viren2d::LineStyle &c) {
  return py::make_tuple(c.line_width, c.color, c.dash_pattern, c.line_cap, c.line_join);
}


viren2d::LineStyle LineStyleFromTuple(py::tuple tpl) {
  if (tpl.size() == 1 || tpl.size() > 5) {
    std::stringstream s;
    s << "Cannot create " << Qualified("LineStyle", false)
      << " from tuple with "
      << tpl.size() << (tpl.size() == 1 ? " entry!" : " entries!");
    throw std::invalid_argument(s.str());
  }

  if (tpl.empty())
    return viren2d::LineStyle();

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


//-------------------------------------------------  ArrowStyle
py::tuple ArrowStyleToTuple(const viren2d::ArrowStyle &st) {
  auto ls = static_cast<const viren2d::LineStyle &>(st);
  auto ls_tpl = LineStyleToTuple(ls);

  return py::make_tuple(ls_tpl, st.tip_length,
                        st.tip_angle, st.tip_closed,
                        st.double_headed);
}


viren2d::ArrowStyle ArrowStyleFromTuple(py::tuple tpl) {
  if (tpl.size() == 0 || tpl.size() > 5) {
    std::stringstream s;
    s << "Cannot create " << Qualified("ArrowStyle", false)
      << " from tuple with "
      << tpl.size() << " entries!";
    throw std::invalid_argument(s.str());
  }

  viren2d::ArrowStyle style(LineStyleFromTuple(tpl[0]));

  if (tpl.size() > 1)
    style.tip_length = tpl[1].cast<double>();

  if (tpl.size() > 2)
    style.tip_angle = tpl[2].cast<double>();

  if (tpl.size() > 3)
    style.tip_closed = tpl[3].cast<bool>();

  if (tpl.size() > 4)
    style.double_headed = tpl[4].cast<bool>();

  return style;
}



//-------------------------------------------------  Rectangle
py::tuple RectToTuple(const viren2d::Rect &r) {
  return py::make_tuple(r.cx, r.cy, r.width, r.height, r.rotation, r.radius);
}


viren2d::Rect RectFromTupleOrList(py::object object) {
  const std::string type = py::cast<std::string>(object.attr("__class__").attr("__name__"));

  if (type.compare("tuple") != 0 && type.compare("list") != 0) {
    std::stringstream s;
    s << "Cannot cast " << type << " to " << Qualified("Rect")
      << ", we only support casting from tuples and lists.";
    throw std::invalid_argument(s.str());
  }

  py::tuple tpl = object.cast<py::tuple>();
  if (tpl.size() < 2 || tpl.size() > 6) {
    std::stringstream s;
    s << "Cannot create " << Qualified("Rect", false)
      << " from tuple with "
      << tpl.size() << " entries!";
    throw std::invalid_argument(s.str());
  }

  size_t consumed_elements = 0;
  viren2d::Rect rect;
  try {
    // Try initialization from viren2d types first
    auto center = tpl[0].cast<viren2d::Vec2d>();
    auto size = tpl[1].cast<viren2d::Vec2d>();
    consumed_elements = 2;
    rect = viren2d::Rect(center, size);
  }  catch (const py::cast_error &) {
    // If casting failed, we should have a (cx, cy, w, h) tuple
    if (tpl.size() < 4) {
      std::stringstream s;
      s << "You wanted to create a " << Qualified("Rect", false)
        << " from a (cx, cy, w, h) tuple, but provided only "
        << tpl.size() << " entries!";
      throw std::invalid_argument(s.str());
    }
    rect = viren2d::Rect(tpl[0].cast<double>(),
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


viren2d::Rect RectFromTuple(py::tuple tpl) {
  return RectFromTupleOrList(tpl);
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
viren2d::Vec<_Tp, dim> VecFromTupleOrList(py::object object) {
  const std::string type = py::cast<std::string>(object.attr("__class__").attr("__name__"));
  using VC = viren2d::Vec<_Tp, dim>;

  if (type.compare("tuple") != 0 && type.compare("list") != 0) {
    std::stringstream s;
    s << "Cannot cast " << type << " to " << Qualified(VC::TypeName())
      << ", we only support casting from tuples and lists.";
    throw std::invalid_argument(s.str());
  }

  py::tuple tpl = object.cast<py::tuple>();
  if (!tpl.empty() && tpl.size() != dim) {
    std::stringstream s;
    s << "Cannot create " << Qualified(VC::TypeName())
      << " from " << tpl.size() << " values, expected "
      << dim << "!";
    throw std::invalid_argument(s.str());
  }

  if (tpl.empty())
    return VC();

  VC vec;
  for (int i = 0; i < dim; ++i)
    vec.val[i] = tpl[i].cast<_Tp>();
  return vec;
}


template<typename _Tp, int dim>
viren2d::Vec<_Tp, dim> VecFromList(py::list lst) {
  return VecFromTupleOrList<_Tp, dim>(lst.cast<py::tuple>());
}


template<typename _Tp, int dim>
py::list VecToList(const viren2d::Vec<_Tp, dim> &vec) {
  py::list lst;
  for (int i = 0; i < dim; ++i)
    lst.append(vec.val[i]);
  return lst;
}


template<typename _Tp, int dim>
void RegisterVec(py::module &m) {
  using VC = viren2d::Vec<_Tp, dim>;
  std::stringstream doc;
  doc << "Vector in " << dim << "D space.";

  py::class_<VC> vec_cls(m, VC::TypeName().c_str(), doc.str().c_str());

  std::stringstream doc_tpl;
  doc_tpl << "Initialize " << Qualified(VC::TypeName(), false)
          << " from " << dim << "-element tuple.";

  vec_cls.def(py::init<>())
      .def(py::init<>(&VecFromTupleOrList<_Tp, dim>),
           doc_tpl.str().c_str())
      .def("__repr__",
           [](const VC &v)
           { return Qualified(v.ToString(), true); })
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
           "Returns a deep copy of this vector.")
      .def("dot", &VC::Dot,
           "Computes the dot product.",
           py::arg("other"))
      .def("length", &VC::Length,
           "Returns the vector's length (i.e. magnitude).")
      .def("length_squared", &VC::LengthSquared,
           "Returns the squared length.")
      .def("distance", &VC::Distance,
           "Computes the L2 distance between 'self' and the 'other'.",
           py::arg("other"))
      .def("direction_vector", &VC::DirectionVector,
           "Computes the direction vector from 'self' to the 'other'.",
           py::arg("other"))
      .def("unit_vector", &VC::UnitVector,
           "Computes the unit vector of 'self'.")
      .def(py::pickle(&VecToList<_Tp, dim>,
                      &VecFromList<_Tp, dim>))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def(py::self + py::self)
      .def(py::self += py::self)
      .def(py::self - py::self)
      .def(-py::self)
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
  if (dim == 2) {
    vec_cls.def(py::init<_Tp, _Tp>(),
                py::arg("x"), py::arg("y"))
        .def_property("width", static_cast<const _Tp &(VC::*)() const>(&VC::width), &VC::SetWidth,
                      "Accesses the first dimension (use vector to define a size).")
        .def_property("height", static_cast<const _Tp &(VC::*)() const>(&VC::height), &VC::SetHeight,
                      "Accesses the second dimension (use vector to define a size).");
  }

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
  py::implicitly_convertible<py::list, VC>();
}

} // namespace moddef


//------------------------------------------------- Module definition

//
// TODO low priority: DeserializeX could reuse CreateX (not sure if
//      both are using the same inputs, tuples vs list vs ...)


PYBIND11_MODULE(viren2d_PYMODULE_NAME, m) {
  m.doc() = R"pbdoc(
      Computer Vision Results, but Nice-Looking
    ----------------------------------------------
                Vision & Rendering 2D

    This is a light-weight 2D toolbox to simplify
    visualization of common 2D computer vision results,
    such as detections, trajectories, and the like.

    For example:
      import viren2d
      import numpy as np

      painter = viren2d.Painter()
      painter.set_canvas_rgb(300, 400, 'white')
      painter.draw_bounding_box(...)

      img_np = np.array(painter.get_canvas())
  )pbdoc"; // TODO finish documentation once we support bounding boxes


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
      .def(py::init<>(&moddef::ColorFromTuple),
           "Initialize from tuple (all values must\n"
           "be a floating point number within [0, 1]):\n"
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
      .def("copy", [](const viren2d::Color &c) { return viren2d::Color(c); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const viren2d::Color &c)
           { return "<" + Qualified("Color", false) + ", " + (c.IsValid() ? c.ToHexString() : "(invalid)") + ">"; })
      .def("__str__", &viren2d::Color::ToHexString)
      .def(py::pickle(&moddef::ColorToTuple,
                      &moddef::ColorFromTuple))
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
           "e.g. '#0011ffff' (all components are scaled to [0, 255]).\n"
           "If the color is invalid, #???????? will be returned instead.")
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
      // TODO(snototter) pybind11 bug, documentation of static members is missing in python, see
      //     https://github.com/pybind/pybind11/issues/3815
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


  auto doc = "Returns a " + Qualified("Color", false) + " for the given values.\n"
        "red, green, blue and alpha must be within [0, 1]";
  m.def("rgba", &viren2d::rgba, doc.c_str(),
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);

  // Convenience function "rgb"
  doc = "Returns a fully opaque " + Qualified("Color", false) + " for the given\n"
        "values. red, green, and blue must be within [0, 1].";
  m.def("rgb", [](double red, double green, double blue) { return viren2d::Color(red, green, blue, 1.0); },
        doc.c_str(), py::arg("red"), py::arg("green"), py::arg("blue"));


  doc = "Returns a " + Qualified("Color", false) + " for the given RGBa values.\n"
        "R, G, and B must be within [0, 255]; alpha must be within [0, 1].";
  m.def("RGBa", &viren2d::RGBa,
        doc.c_str(),
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);

  // Convenience function "RGB"
  doc = "Returns a fully opaque " + Qualified("Color", false) + " for the given\n"
        "values. red, green, and blue must be within [0, 255]\n"
        "* alpha must be within [0, 1].";
  m.def("RGB",  [](double R, double G, double B) { return viren2d::RGBa(R, G, B, 1.0); },
        doc.c_str(), py::arg("red"), py::arg("green"), py::arg("blue"));

  //------------------------------------------------- Primitives - Vectors
  moddef::RegisterVec<double, 2>(m);
  moddef::RegisterVec<double, 3>(m);
  //TODO add more vector bindings once we need them
//  moddef::RegisterVec<double, 4>(m);
//  moddef::RegisterVec<int, 2>(m);
//  moddef::RegisterVec<int, 3>(m);


  //------------------------------------------------- Primitives - Ellipse
  py::class_<viren2d::Ellipse>(m, "Ellipse", "TODO doc")
//      .def(py::init<>(&moddef::CreateRect), doc.c_str(),
//           py::arg("tpl"))
      .def(py::init<double, double, double, double, double, double, double, bool>(),
           "TODO",
           py::arg("cx"), py::arg("cy"),
           py::arg("major_axis"), py::arg("minor_axis"),
           py::arg("rotation") = 0.0,
           py::arg("angle_from") = 0.0,
           py::arg("angle_to") = 360.0,
           py::arg("include_center") = true)
      .def(py::init<viren2d::Vec2d, viren2d::Vec2d, double, double, double, bool>(),
           "TODO",
           py::arg("center"), py::arg("axes"),
           py::arg("rotation") = 0.0,
           py::arg("angle_from") = 0.0,
           py::arg("angle_to") = 360.0,
           py::arg("include_center") = true)
      .def("copy", [](const viren2d::Ellipse &e) { return viren2d::Ellipse(e); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const viren2d::Ellipse &o)
           { return Qualified(o.ToString(), true); })
      .def("__str__", &viren2d::Ellipse::ToString)
//      .def(py::pickle(&pickling::SerializeEllipse,
//                      &pickling::DeserializeEllipse))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_readwrite("cx", &viren2d::Ellipse::cx,
           "Horizontal center.")
      .def_readwrite("cy", &viren2d::Ellipse::cy,
           "Vertical center.")
      .def_readwrite("major_axis", &viren2d::Ellipse::major_axis,
           "Length of major axis.")
      .def_readwrite("minor_axis", &viren2d::Ellipse::minor_axis,
           "Length of minor axis.")
      .def_readwrite("rotation", &viren2d::Ellipse::rotation,
           "Rotation angle (clockwise) in degrees.")
      .def_readwrite("angle_from", &viren2d::Ellipse::angle_from,
           "Drawing the contour/fill starts at 'angle_from' (clockwise in degrees).")
      .def_readwrite("angle_to", &viren2d::Ellipse::angle_to,
           "Drawing the contour/fill stops at 'angle_from' (clockwise in degrees).")
      .def_readwrite("include_center", &viren2d::Ellipse::include_center,
           "If you explicitly change 'angle_from'/'angle_to', you very likely\n"
           "want to also include the center point in the rendering path. Otherwise,\n"
           "filling can easily lead to irritating results.")
      .def("is_valid", &viren2d::Ellipse::IsValid,
           "Returns True if the ellipse is in a valid/drawable state.");

//  // An ellipse can be initialized from a given tuple.
//  py::implicitly_convertible<py::tuple, viren2d::Ellipse>();//TODO


  //------------------------------------------------- Primitives - Rectangle
  py::class_<viren2d::Rect> rect(m, "Rect",
           "Rectangle for visualization.\n\n"
           "A rectangle is defined by its CENTER, width, height,\n"
           "clockwise rotation (in degrees), and a corner\n"
           "radius (for rounded rectangles).");

  doc = "Initialize from tuple:\n"
         "(center, size)\n"
         "    With types: (" + Qualified(viren2d::Vec2d::TypeName()) + ", "
         + Qualified(viren2d::Vec2d::TypeName()) + ")\n"
         "    Axis-aligned rectangle.\n"
         "(center, size, rotation)\n"
         "    With types: (" + Qualified(viren2d::Vec2d::TypeName()) + ", "
         + Qualified(viren2d::Vec2d::TypeName()) + ", float)\n"
         "    Rotated rectangle.\n"
         "(center, size, rotation, radius)\n"
         "    With types: (" + Qualified(viren2d::Vec2d::TypeName()) + ", "
         + Qualified(viren2d::Vec2d::TypeName()) + ", float, float)\n"
         "    Rotated rectangle with rounded corners.\n"
         "(cx, cy, w, h)\n"
         "    Where each element is a float.\n"
         "    Axis-aligned rectangle.\n"
         "(cx, cy, w, h, rotation)\n"
         "    Where each element is a float.\n"
         "    Rotated rectangle.\n"
         "* (cx, cy, w, h, rotation, radius)\n"
         "    Where each element is a float.\n"
         "    Rotated rectangle with rounded corners.\n";
  rect.def(py::init<>(&moddef::RectFromTupleOrList), doc.c_str(),
           py::arg("tpl"));

  doc = "Create a rectangle from its center, size, rotation & radius.\n"
        "Required:  center and size, each as " + Qualified(viren2d::Vec2d::TypeName()) + ".\n"
        "Optional:  rotation  Clockwise rotation in degres.\n"
        "Optional:  radius    Corner radius in pixels (if > 1) or as\n"
        "                     percentage of min(w, h) if radius in (0, 0.5].";
  rect.def(py::init<double, double, double, double, double, double>(),
           "Create a rectangle from cx, cy, w, h, rotation & radius.\n"
           "Required:  cx, cy, w, h (center and size, in pixels).\n"
           "Optional:  rotation   Clockwise rotation in degres.\n"
           "Optional:  radius:    Corner radius in pixels (if > 1) or as\n"
           "                      percentage of min(w, h) if radius in (0, 0.5].",
           py::arg("cx"), py::arg("cy"), py::arg("w"), py::arg("h"),
           py::arg("rotation") = 0.0,
           py::arg("radius") = 0.0)
      .def(py::init<viren2d::Vec2d, viren2d::Vec2d, double, double>(),
           doc.c_str(),
           py::arg("center"), py::arg("size"),
           py::arg("rotation") = 0.0,
           py::arg("radius") = 0.0)
      .def("copy", [](const viren2d::Rect &r) { return viren2d::Rect(r); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const viren2d::Rect &r)
           { return Qualified(r.ToString(), true); })
      .def("__str__", &viren2d::Rect::ToString)
      .def(py::pickle(&moddef::RectToTuple,
                      &moddef::RectFromTuple))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_readwrite("cx", &viren2d::Rect::cx,
           "Horizontal center.")
      .def_readwrite("cy", &viren2d::Rect::cy,
           "Vertical center.")
      .def_readwrite("width", &viren2d::Rect::width,
           "Rectangle width.")
      .def_property_readonly("half_width", &viren2d::Rect::half_width,
           "Half the rectangle width.")
      .def_property_readonly("half_height", &viren2d::Rect::half_height,
           "Half the rectangle height.")
      .def_readwrite("height", &viren2d::Rect::height,
           "Rectangle height.")
      .def_readwrite("rotation", &viren2d::Rect::rotation,
           "Rotation angle (clockwise) in degrees.")
      .def_readwrite("radius", &viren2d::Rect::radius,
           "Corner radius (> 0 for rounded rectangles).\n"
           "Defined in pixels (if radius > 1) or as percentage\n"
           "of min(w, h) if radius in (0, 0.5].\n"
           "Values within (0.5, 1) lead to invalid rectangles.")
      .def("is_valid", &viren2d::Rect::IsValid,
           "Returns True if both width & height are > 0.");

  // A Rect can be initialized from a given tuple/list.
  py::implicitly_convertible<py::tuple, viren2d::Rect>();
  py::implicitly_convertible<py::list, viren2d::Rect>();


  //TODO plane? maybe - either use vec4d or make a full-blown plane class
  //     preferably reuse vec4d - nobody expects a visualization toolbox to
  //     be the go-to resource for (easy to use) geometric datatypes

  //------------------------------------------------- Primitives - ImageBuffer
  // Info on numpy memory layout: https://stackoverflow.com/a/53099870/400948
  py::class_<viren2d::ImageBuffer>(m, "ImageBuffer", py::buffer_protocol(),
           "An ImageBuffer holds 8-bit images (Grayscale,\n"
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
      .def("copy", [](const viren2d::ImageBuffer &buf) {
             viren2d::ImageBuffer cp;
             cp.CreateCopy(buf.data, buf.width, buf.height, buf.channels, buf.stride);
             return cp;
           }, "Returns a deep copy. This new ImageBuffer will always allocate\n"
           "and copy the memory, even if you call copy() on a \"shared\" buffer.")
      .def("to_rgb", &viren2d::ImageBuffer::ToRGB,
           "Convert to RGB. Will always return a copy, even if this buffer\n"
           "is already RGB.")
      .def("to_rgba", &viren2d::ImageBuffer::ToRGBA,
           "Convert to RGBA. Will always return a copy, even if this buffer\n"
           "is already RGBA.")
      .def("__repr__",
           [](const viren2d::ImageBuffer &b)
           { return Qualified(b.ToString(), true); })
      .def("__str__", &viren2d::ImageBuffer::ToString)
      .def_readonly("width", &viren2d::ImageBuffer::width,
           "Image width in pixels.")
      .def_readonly("height", &viren2d::ImageBuffer::height,
           "Image height in pixels.")
      .def_readonly("channels", &viren2d::ImageBuffer::channels,
           "Number of channels.")
      .def_readonly("stride", &viren2d::ImageBuffer::stride,
           "Stride per row (in bytes).")
      .def_readonly("owns_data", &viren2d::ImageBuffer::owns_data_,
           "Boolean flag indicating if this ImageBuffer is\n"
           "the owner of the image data (and is thus responsible\n"
           "for memory cleanup).");

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
             "The join is cut off at half the line width from the joint point.")
      .value("Round", viren2d::LineJoin::Round,
             "Rounded join, where the center of the circle is the joint point.");



  py::class_<viren2d::LineStyle>line_style(m, "LineStyle", "How a line should be drawn.");
  doc = "Initialize from tuple:\n"
         "Get the default line style via: ()\n"
         "Width & color:       (line_width, color)"
         "Full configuration:  (line_width, color, dash_pattern,\n"
         "                      line_cap, line_join)\n"
         "With data types:\n"
         ":line_width:    float\n"
         ":color:         " + Qualified("Color") + "\n"
         ":dash_pattern:  list[float]\n"
         ":line_cap:      " + Qualified("LineCap") + "\n"
         ":line_join:     " + Qualified("LineJoin");
  line_style.def(py::init<double, viren2d::Color, std::vector<double>,
                    viren2d::LineCap, viren2d::LineJoin>(),
           "TODO doc",
           py::arg("line_width"),
           py::arg("color"),
           py::arg("dash_pattern") = std::vector<double>(),
           py::arg("line_cap") = viren2d::LineCap::Butt,
           py::arg("line_join") = viren2d::LineJoin::Miter)
      .def(py::init<>(&moddef::LineStyleFromTuple),
           doc.c_str())
      .def(py::init<>(), "Initializes the default style.\n"
           "Change the default settings via set_default_line_style().")
      .def("copy", [](const viren2d::LineStyle &st) { return viren2d::LineStyle(st); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const viren2d::LineStyle &st)
           { return Qualified(st.ToString(), true); })
      .def("__str__", &viren2d::LineStyle::ToString)
      .def(py::pickle(&moddef::LineStyleToTuple,
                      &moddef::LineStyleFromTuple))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_valid", &viren2d::LineStyle::IsValid,
           "Check if the style would lead to a drawable line.")
      .def("is_dashed", &viren2d::LineStyle::IsDashed,
           "Check if this style contains a dash stroke pattern.")
      .def("cap_offset", &viren2d::LineStyle::CapOffset,
           "Computes how much the line cap will extend the\n"
           "line's start/end.")
      .def("join_offset", &viren2d::LineStyle::JoinOffset,
           "Computes how much a line join will extend the joint.\n\n"
           "The interior_angle is the angle between two line segments\n"
           "in degrees.\n"
           "This needs to know the miter_limit because Cairo switches\n"
           "from MITER to BEVEL if the miter_limit is exceeded, see\n"
           "https://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-miter-limit\n"
           "for details.",
           py::arg("interior_angle"),
           py::arg("miter_limit") = 10.0)
      .def_readwrite("dash_pattern", &viren2d::LineStyle::dash_pattern,
           "Dash pattern defined as list of on/off strokes (lengths in\n"
           "pixels), e.g. [20, 10, 40, 10]. If the list is empty, the\n"
           "line will be drawn solid.")
      .def_readwrite("line_cap", &viren2d::LineStyle::line_cap,
           "How to render the endpoints of the line (or dash strokes).")
      .def_readwrite("line_join", &viren2d::LineStyle::line_join,
           "How to render the junction of two lines/segments.")
      .def_readwrite("line_width", &viren2d::LineStyle::line_width,
           "Width/thickness in pixels.");

  doc = "Line color as " + Qualified("Color") + ".";
  line_style.def_readwrite("color", &viren2d::LineStyle::color,
           doc.c_str());

  // A LineStyle can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, viren2d::LineStyle>();

  //TODO test
  doc = "Change the default " + Qualified("LineStyle") + " to be\n"
        "used for drawing lines & contours.";
  m.def("set_default_line_style", &viren2d::SetDefaultLineStyle,
        doc.c_str(), py::arg("line_style"));

  //------------------------------------------------- Drawing - ArrowStyle
  py::class_<viren2d::ArrowStyle, viren2d::LineStyle>(m, "ArrowStyle",
           "How an arrow should be drawn.")
      .def(py::init<>(&moddef::ArrowStyleFromTuple), "TODO doc")
      .def(py::init<double, viren2d::Color, double, double, bool, bool,
                    std::vector<double>, viren2d::LineCap, viren2d::LineJoin>(),
           "TODO doc",
           py::arg("line_width") = 2.0,
           py::arg("color") = viren2d::Color(viren2d::NamedColor::Azure),
           py::arg("tip_length") = 0.1,
           py::arg("tip_angle") = 30.0,
           py::arg("tip_closed") = false,
           py::arg("double_headed") = false,
           py::arg("dash_pattern")=std::vector<double>(),
           py::arg("line_cap")=viren2d::LineCap::Butt,
           py::arg("line_join")=viren2d::LineJoin::Miter)
      .def(py::init<viren2d::LineStyle, double, double, bool, bool>(),
           "TODO doc",
           py::arg("line_style"),
           py::arg("tip_length") = 0.1,
           py::arg("tip_angle") = 30.0,
           py::arg("tip_closed") = false,
           py::arg("double_headed") = false)
      .def("copy", [](const viren2d::ArrowStyle &st) { return viren2d::ArrowStyle(st); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const viren2d::ArrowStyle &st)
           { return Qualified(st.ToString(), true); })
      .def("__str__", &viren2d::ArrowStyle::ToString)
      .def(py::pickle(&moddef::ArrowStyleToTuple,
                      &moddef::ArrowStyleFromTuple))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_valid", &viren2d::ArrowStyle::IsValid,
           "Check if the style would lead to a drawable arrow.")
      .def("tip_length_for_shaft",
           static_cast<double (viren2d::ArrowStyle::*)(double) const>(&viren2d::ArrowStyle::TipLengthForShaft),
           "Computes the length of the arrow head/tip for the given shaft length.",
           py::arg("shaft_length"))
      .def("tip_length_for_shaft",
           static_cast<double (viren2d::ArrowStyle::*)(const viren2d::Vec2d&, const viren2d::Vec2d&) const>(&viren2d::ArrowStyle::TipLengthForShaft),
           "Computes the length of the arrow head/tip for the given shaft.",
           py::arg("shaft_from"), py::arg("shaft_to"))
      .def_readwrite("tip_length", &viren2d::ArrowStyle::tip_length,
           "Length of the tip. If this value is in [0, 1], the tip\n"
           "length will be this fraction of the shaft length.\n"
           "Otherwise, this value specifies the absolute tip length\n"
           "in pixels.")
      .def_readwrite("tip_angle", &viren2d::ArrowStyle::tip_angle,
           "Angle between tip lines and the shaft in degrees.")
      .def_readwrite("tip_closed", &viren2d::ArrowStyle::tip_closed,
           "If True, the arrow head/tip will be filled. Otherwise,\n"
           "the tip will be open.")
      .def_readwrite("double_headed", &viren2d::ArrowStyle::double_headed,
           "If True, heads/tips will be drawn on both ends of the line.");

  // An ArrowStyle can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, viren2d::ArrowStyle>();

  //------------------------------------------------- Drawing - Painter
  py::class_<moddef::Painter> painter(m, "Painter",
          R"pbdoc(A Painter lets you draw on a canvas.

          Usage:
          1. Create a Painter:
               import viren2d
               painter = viren2d.Painter()

          2. Initialize the canvas via either:
               painter.set_canvas_rgb(...)
               painter.set_canvas_image(...)
               painter.set_canvas_filename()

          3. Draw onto the canvas via draw_xxx(), for example:
               painter.draw_arrow(...)
               painter.draw_bounding_box(...)

          4. When all objects have been drawn, retrieve the
             visualization via get_canvas(...). For example,
             to get a deeply copied image as numpy array:
               import numpy as np
               canvas = p.get_canvas(copy=True)
               img_np = np.array(canvas, copy=False)

          5. Either continue drawing or set a new canvas (the
             painter object can be reused).
          )pbdoc");

  painter.def(py::init<>())
      .def("__repr__", [](const moddef::Painter &) { return Qualified("Painter", true); })
      .def("__str__", [](const moddef::Painter &) { return Qualified("Painter", false); })
      .def("is_valid", &moddef::Painter::IsValid,
           "Checks if the canvas has been set up correctly.")
      .def("set_canvas_rgb", &moddef::Painter::SetCanvasColor,
           "Initializes the canvas with the given color.",
           py::arg("width"), py::arg("height"),
           py::arg("color")=viren2d::Color::White)
      .def("set_canvas_filename", &moddef::Painter::SetCanvasFilename,
           "Initializes the canvas from the given image file.\n"
           "Supported formats are:\n"
           "   JPEG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM\n\n"
           "Relies on the stb library, so check for its updates\n"
           "if your format is missing:\n"
           "https://github.com/nothings/stb/blob/master/stb_image.h",
           py::arg("image_filename"));

  doc = "Initializes the canvas from the given image, i.e. either\n"
        "a numpy array (dtype uint8) or a " + Qualified("ImageBuffer", false) + ".\n\n"
        "Example:\n"
        "  img_np = np.zeros((480, 640, 3), dtype=np.uint8)\n"
        "  painter.set_canvas_image(img_np)";
  painter.def("set_canvas_image", &moddef::Painter::SetCanvasImage,
           doc.c_str(), py::arg("image"));

  painter.def("get_canvas_size", &moddef::Painter::GetCanvasSize,
           "Returns the size of the canvas in pixels as (W, H) tuple.")
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
           py::arg("copy") = false);

        //----------------------------------------------------------------------
  doc = "Draws a circular arc.\n\n of the given radius using the\n"
        ":center:  (" + Qualified(viren2d::Vec2d::TypeName()) + ")\n"
        ":radius:  (float)"
        "    Radius of the arc in pixels.\n\n"
        ":angle1:  (float)\n"
        ":angle2:  (float)\n"
        "    The arc will be drawn from angle1 to angle2 in clockwise\n"
        "    direction. Both angles are specified in degrees, where 0 degrees\n"
        "    points in the direction of increasing X coordinates.\n\n"
        ":line_style:  (" + Qualified("LineStyle") + ")\n"
        "   How to draw the arc's outline. If line_width is 0, the outline\n"
        "   will not be drawn (then you must define a 'fill_color').\n\n"
        ":include_center:  (bool)\n"
        "    If True (default), the center of the circle will be included\n"
        "    when drawing the outline and filling the arc.\n\n"
        ":fill_color:  (" + Qualified("Color") + ")\n"
        "   Provide a valid color to fill the circle.";
  painter.def("draw_arc", &moddef::Painter::DrawArc, doc.c_str(),
              py::arg("center"), py::arg("radius"),
              py::arg("angle1"), py::arg("angle2"),
              py::arg("line_style") = viren2d::LineStyle(),
              py::arg("include_center") = true,
              py::arg("fill_color") = viren2d::Color(0, 0, 0, 0));

        //----------------------------------------------------------------------
  doc = "Draws an arrow.\n\n"
        ":pt1:  (" + Qualified(viren2d::Vec2d::TypeName()) + ")\n"
        ":pt2:  (" + Qualified(viren2d::Vec2d::TypeName()) + ")\n"
        "   Start/end coordinates of the arrow shaft.\n\n"
        ":arrow_style:  (" + Qualified("ArrowStyle") + ")\n"
        "   How to draw the arrow (specifies both line and head style).";
  painter.def("draw_arrow", &moddef::Painter::DrawArrow, doc.c_str(),
              py::arg("pt1"), py::arg("pt2"),
              py::arg("arrow_style") = viren2d::ArrowStyle());

        //----------------------------------------------------------------------
  doc = "Draws a circle.\n\n"
        ":center:  (" + Qualified("Vec2d") + ")\n"
        "   Center position of the circle.\n\n"
        ":line_style:  (" + Qualified("LineStyle") + ")\n"
        "   How to draw the circle outline. If line_width is 0, the outline\n"
        "   will not be drawn (then you *must* define a 'fill_color').\n\n"
        ":fill_color:  (" + Qualified("Color") + ")\n"
        "   Provide a valid color to fill the circle.";
  painter.def("draw_circle", &moddef::Painter::DrawCircle, doc.c_str(),
              py::arg("center"), py::arg("radius"),
              py::arg("line_style") = viren2d::LineStyle(),
              py::arg("fill_color") = viren2d::Color(0, 0, 0, 0));


    //----------------------------------------------------------------------
  doc = "Draws an ellipse.\n\n"
    ":ellipse:  (" + Qualified("Ellipse") + ")\n"
    "    The ellipse which should be drawn.\n\n"
    ":line_style:  (" + Qualified("LineStyle") + ")\n"
    "   How to draw it's outline. If line_width is 0, the outline\n"
    "   will not be drawn (then you *must* define a 'fill_color').\n\n"
    ":fill_color:  (" + Qualified("Color") + ")\n"
    "   Provide a valid color to fill the ellipse.";
  painter.def("draw_ellipse", &moddef::Painter::DrawEllipse, doc.c_str(),
          py::arg("ellipse"),
          py::arg("line_style") = viren2d::LineStyle(),
          py::arg("fill_color") = viren2d::Color(0, 0, 0, 0));

        //----------------------------------------------------------------------
  doc = "Draws a grid.\n\n:spacing_x:  (float)\n:spacing_y:  (float)\n"
        "    Width & height of each grid cell.\n\n"
        "    The grid will only be drawn within the defined region.\n"
        "    If not provided, the grid will span the whole canvas.\n\n"
        ":line_style:  (" + Qualified("LineStyle") + ")\n"
        "    How to draw the grid lines."
        ":top_left:  (" + Qualified(viren2d::Vec2d::TypeName()) + ")\n"
        ":bottom_right:  (" + Qualified(viren2d::Vec2d::TypeName()) + ")\n";

  painter.def("draw_grid", &moddef::Painter::DrawGrid, doc.c_str(),
              py::arg("spacing_x"), py::arg("spacing_y"),
              py::arg("line_style"),
              py::arg("top_left") = viren2d::Vec2d(),
              py::arg("bottom_right") = viren2d::Vec2d());

        //----------------------------------------------------------------------
  doc = "Draws a line.\n\n"
        ":pt1:  (" + Qualified(viren2d::Vec2d::TypeName()) + ")\n"
        ":pt2:  (" + Qualified(viren2d::Vec2d::TypeName()) + ")\n"
        "   Start/end coordinates of the line.\n\n"
        ":line_style:  (" + Qualified("LineStyle") + ")\n"
        "    How to draw the line (stroke width, color, dash pattern).";
  painter.def("draw_line", &moddef::Painter::DrawLine, doc.c_str(),
              py::arg("pt1"), py::arg("pt2"), py::arg("line_style"));

        //----------------------------------------------------------------------
  doc = "Draws a rectangle (axis-aligned/rotated, solid/dashed, etc.)\n\n"
        ":rect:  (" + Qualified("Rect") + ")\n"
        "    The rectangle which should be drawn.\n\n"
        ":line_style:  (" + Qualified("LineStyle") + ")\n"
        "   How to draw the rectangle's outline. If line_width is 0, the\n"
        "   outline will not be drawn (then you *must* define a 'fill_color').\n\n"
        ":fill_color:  (" + Qualified("Color") + ")\n"
        "   Provide a valid color to fill the rectangle.";
  painter.def("draw_rect", &moddef::Painter::DrawRect, doc.c_str(),
              py::arg("rect"),
              py::arg("line_style") = viren2d::LineStyle(),
              py::arg("fill_color") = viren2d::Color(0, 0, 0, 0));


      //TODO add draw_xxx methods
  //TODO add convenience functions for plural draw_xxxS (e.g. in
  // moddef::Painter "for (element in list) : painter_->DrawElement();"
//FIXME copy python doc to cpp
  //FIXME set default linestyle, arrowstyle, markerstyle, ...
  //FIXME allow the user to change the default styles

#ifdef viren2d_VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(viren2d_VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
