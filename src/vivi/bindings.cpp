#include <sstream>
#include <memory>
#include <stdexcept>

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

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
    throw std::invalid_argument(s.str());
  }
  return vivi::Color(tpl[0].cast<double>(), tpl[1].cast<double>(),
                     tpl[2].cast<double>(), tpl[3].cast<double>());
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
    throw std::invalid_argument(s.str());
  }

  VC vec;
  for (int i = 0; i < dim; ++i)
    vec.val[i] = lst[i].cast<_Tp>();
  return vec;
}

//------------------------------------------------- Rectangle
py::tuple SerializeRect(const vivi::Rect &r)
{
  return py::make_tuple(r.cx, r.cy, r.width, r.height, r.angle, r.radius);
}


vivi::Rect DeserializeRect(py::tuple tpl)
{
  if (tpl.size() != 6)
  {
    std::stringstream s;
    s << "Invalid vivi.Rect state - expected 6 entries, got " << tpl.size() << "!";
    throw std::invalid_argument(s.str());
  }
  return vivi::Rect(tpl[0].cast<double>(), tpl[1].cast<double>(),
                    tpl[2].cast<double>(), tpl[3].cast<double>(),
                    tpl[4].cast<double>(), tpl[5].cast<double>());
}


//------------------------------------------------- TODO Pickle others if needed




//------------------------------------------------- LineStyle
py::tuple SerializeLineStyle(const vivi::LineStyle &c)
{
  return py::make_tuple(c.line_width, c.color, c.dash_pattern, c.line_cap, c.line_join);
}


vivi::LineStyle DeserializeLineStyle(py::tuple tpl)
{
  if (tpl.size() != 5)
  {
    std::stringstream s;
    s << "Invalid vivi.LineStyle state - expected 5 entries, got " << tpl.size() << "!";
    throw std::invalid_argument(s.str());
  }
  vivi::LineStyle ls(tpl[0].cast<double>(),
                     tpl[1].cast<vivi::Color>(),
                     tpl[2].cast<std::vector<double>>(),
                     tpl[3].cast<vivi::LineStyle::Cap>(),
                     tpl[4].cast<vivi::LineStyle::Join>());
  return ls;
}
} // namespace pickling




/** @brief Utilities to simplify module definition. */
namespace moddef
{
//------------------------------------------------- Wrapper for the Painter
// Necessary, because we're not exposing the ImagePainter, so
// we can't use the pybind11 trampoline mechanism.
class Painter
{
public:
  Painter() : painter_(vivi::CreateImagePainter())
  {}

  void SetCanvasColor(int width, int height, const vivi::Color &color)
  {
    painter_->SetCanvas(width, height, color);
  }

  void SetCanvasFilename(const std::string &image_filename)
  {
    painter_->SetCanvas(image_filename);
  }

  void SetCanvasImage(const vivi::ImageBuffer &image)
  {
    painter_->SetCanvas(image);
  }

  vivi::ImageBuffer GetCanvas(bool copy)
  {
    return painter_->GetCanvas(copy);
  }


  void DrawArc(const vivi::Vec2d &center, double radius,
               double angle1, double angle2,
               const vivi::LineStyle &line_style,
               const vivi::Color &fill)
  {
    painter_->DrawArc(center, radius, angle1, angle2, line_style, fill);
  }

  void DrawCircle(const vivi::Vec2d &center, double radius,
                  const vivi::LineStyle &line_style,
                  const vivi::Color &fill)
  {
    painter_->DrawCircle(center, radius, line_style, fill);
  }


  void DrawLine(const vivi::Vec2d &from, const vivi::Vec2d &to,
                const vivi::LineStyle &line_style)
  {
    painter_->DrawLine(from, to, line_style);
  }


  void DrawRect(const vivi::Rect &rect, const vivi::LineStyle &line_style,
                const vivi::Color &fill)
  {
    painter_->DrawRect(rect, line_style, fill);
  }

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
    throw std::invalid_argument(s.str());
  }
  vivi::Color col(tpl[0].cast<double>(),
                  tpl[1].cast<double>(),
                  tpl[2].cast<double>(), 1.0);
  if (tpl.size() == 4)
    col.alpha = tpl[3].cast<double>();
  return col;
}


//-------------------------------------------------  LineStyle from tuple
vivi::LineStyle CreateLineStyle(py::tuple tpl)
{
  if (tpl.size() < 2 || tpl.size() > 5)
  {
    std::stringstream s;
    s << "Cannot create vivi.LineStyle from tuple with"
      << tpl.size() << " entries!";
    throw std::invalid_argument(s.str());
  }

  vivi::LineStyle ls(tpl[0].cast<double>(),
                     tpl[1].cast<vivi::Color>());
  if (tpl.size() > 2)
    ls.dash_pattern = tpl[2].cast<std::vector<double>>();

  if (tpl.size() > 3)
    ls.line_cap = tpl[3].cast<vivi::LineStyle::Cap>();

  if (tpl.size() > 4)
    ls.line_join = tpl[4].cast<vivi::LineStyle::Join>();

  return ls;
}


//-------------------------------------------------  Rectangle from tuple
vivi::Rect CreateRect(py::tuple tpl)
{
  if (tpl.size() < 4 || tpl.size() > 6)
  {
    std::stringstream s;
    s << "Cannot create vivi.Rect from tuple with"
      << tpl.size() << " entries!";
    throw std::invalid_argument(s.str());
  }

  vivi::Rect rect(tpl[0].cast<double>(),
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
vivi::ImageBuffer CreateImageBuffer(py::array_t<unsigned char, py::array::c_style | py::array::forcecast> buf)
{
  // Sanity checks
  if (buf.ndim() < 2 || buf.ndim() > 3)
    throw std::runtime_error("Incompatible image dimension!");

  if (!buf.dtype().is(py::dtype::of<uint8_t>()))
    throw std::runtime_error("Incompatible format: expected a uint8 array!");

  vivi::ImageBuffer img;
  const int row_stride = static_cast<int>(buf.strides(0));
  const int height = static_cast<int>(buf.shape(0));
  const int width = static_cast<int>(buf.shape(1));
  const int channels = (buf.ndim() == 2) ? 1 : static_cast<int>(buf.shape(2));
  img.CreateSharedBuffer(buf.mutable_data(), width, height, channels, row_stride);
  return img;
}

//------------------------------------------------- Vec(tor) from tuple
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
    throw std::invalid_argument(s.str());
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
              { return "<vivi." + v.ToString() + ">"; })
         .def("__str__", &VC::ToString)
         .def_property("x", static_cast<const _Tp &(VC::*)() const>(&VC::x), &VC::SetX,
                       "Accesses the first dimension.")
         .def_property("y", static_cast<const _Tp &(VC::*)() const>(&VC::y), &VC::SetY,
                       "Accesses the second dimension.")
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

  //TODO bind other operators:
  // +
  // +=
  // -
  // -=
  // *
  // *=
  // x / 3; /=


  // Specific for 2-dim vectors
  if (dim == 2)
    vec_cls.def(py::init<_Tp, _Tp>(), py::arg("x"), py::arg("y"));

  // Specific for 3-dim vectors
  if (dim == 3)
  {
    vec_cls.def(py::init<_Tp, _Tp, _Tp>(), py::arg("x"), py::arg("y"), py::arg("z"));
    vec_cls.def("cross", &VC::Cross, "Cross product", py::arg("other"));
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

PYBIND11_MODULE(vivi, m)
{ 
  m.doc() = R"pbdoc(
    A visualization tool for computer vision tasks

    Python bindings for the C++ vivi toolbox which
    uses the 2D Cairo graphics library.
  )pbdoc";
  //TODO maybe rename cpp toolbox to vivi++ ? If so, adjust docstring above!


  //------------------------------------------------- Color
  py::module_ color_sub = m.def_submodule("colors",
                                          "Color definitions, named colors and convenience wrappers");
  py::class_<vivi::Color>(color_sub, "Color",
                          "Color in rgba format, i.e. each component is within [0,1].")
      .def(py::init<>())
      .def(py::init<>(&moddef::CreateColor),
           "Initialize from tuple (all values must\n"
           "be within [0, 1]):\n"
           "* (red, green, blue)\n"
           "* (red, green, blue, alpha)")
      .def(py::init<double, double, double, double>(),
           py::arg("red"), py::arg("green"), py::arg("blue"),
           py::arg("alpha")=1.0)
      .def("__repr__",
           [](const vivi::Color &c)
           { return "<vivi.Color " + c.ToString() + ">"; })
      .def("__str__", &vivi::Color::ToString)
      .def(py::pickle(&pickling::SerializeColor,
                      &pickling::DeserializeColor))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("as_RGBA", &vivi::Color::ToRGBA,
           "Returns the corresponding (R, G, B, a) tuple,\n"
           "where R, G, B in [0, 255] and alpha in [0, 1].")
      .def("as_hex", &vivi::Color::ToHexString,
           "Returns the hex web color code representation,\n"
           "e.g. '#0011FF' (alpha is ignored).")
      .def_readwrite("red", &vivi::Color::red,
                     "Red component within [0, 1].")
      .def_readwrite("green", &vivi::Color::green,
                     "Green component within [0, 1].")
      .def_readwrite("blue", &vivi::Color::blue,
                     "Blue component within [0, 1].")
      .def_readwrite("alpha", &vivi::Color::alpha,
                     "Opacity within [0, 1].")
      .def("inverse", &vivi::Color::Inverse,
           "Returns the inverse color, i.e. (1.0-r, 1.0-g, 1.0-b, a).\n"
           "Alpha value stays the same.");

  // A Color can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, vivi::Color>();


  color_sub.def("rgba", vivi::rgba, "Returns a vivi.Color for the given rgba values.\n"
                                    "r, g, b and alpha must be within [0, 1]",
                py::arg("red"), py::arg("green"), py::arg("blue"),
                py::arg("alpha")=1.0);


  color_sub.def("RGBA", vivi::RGBA, "Returns a vivi.Color for the given RGBA values.\n"
                                    "* R,G,B must be within [0, 255]\n"
                                    "* alpha must be within [0, 1].",
                py::arg("red"), py::arg("green"), py::arg("blue"),
                py::arg("alpha")=1.0);

  color_sub.def("black", &vivi::colors::Black, py::arg("alpha")=1.0)
      .def("white", &vivi::colors::White, py::arg("alpha")=1.0)
      .def("crimson", &vivi::colors::Crimson, py::arg("alpha")=1.0)
      .def("maroon", &vivi::colors::Maroon, py::arg("alpha")=1.0)
      .def("purple", &vivi::colors::Purple, py::arg("alpha")=1.0)
      .def("cyan", &vivi::colors::Cyan, py::arg("alpha")=1.0)
      .def("magenta", &vivi::colors::Magenta, py::arg("alpha")=1.0)
      .def("turquoise", &vivi::colors::Turquoise, py::arg("alpha")=1.0)
      .def("orange", &vivi::colors::Orange, py::arg("alpha")=1.0)
      .def("orchid", &vivi::colors::Orchid, py::arg("alpha")=1.0)
      .def("silver", &vivi::colors::Silver, py::arg("alpha")=1.0)
      .def("gold", &vivi::colors::Gold, py::arg("alpha")=1.0)
      .def("forest_green", &vivi::colors::ForestGreen, py::arg("alpha")=1.0)
      .def("teal_green", &vivi::colors::TealGreen, py::arg("alpha")=1.0)
      .def("lime_green", &vivi::colors::LimeGreen, py::arg("alpha")=1.0)
      .def("navy_blue", &vivi::colors::NavyBlue, py::arg("alpha")=1.0)
      .def("indigo", &vivi::colors::Indigo, py::arg("alpha")=1.0)
      .def("copper", &vivi::colors::Copper, py::arg("alpha")=1.0)
      .def("freesia", &vivi::colors::Freesia, py::arg("alpha")=1.0)
      .def("midnight_blue", &vivi::colors::MidnightBlue, py::arg("alpha")=1.0)
      .def("salmon", &vivi::colors::Salmon, py::arg("alpha")=1.0)
      .def("rose_red", &vivi::colors::RoseRed, py::arg("alpha")=1.0)
      .def("olive", &vivi::colors::Olive, py::arg("alpha")=1.0)
      .def("light_blue", &vivi::colors::LightBlue, py::arg("alpha")=1.0)
      .def("lavender", &vivi::colors::Lavender, py::arg("alpha")=1.0)
      .def("ivory", &vivi::colors::Ivory, py::arg("alpha")=1.0);

  //------------------------------------------------- Primitives - Vectors
  moddef::RegisterVec<double, 2>(m);
  moddef::RegisterVec<double, 3>(m);
  //TODO add more vector bindings once we need them
//  moddef::RegisterVec<double, 4>(m);
//  moddef::RegisterVec<int, 2>(m);
//  moddef::RegisterVec<int, 3>(m);


  //------------------------------------------------- Primitives - Rectangle
  py::class_<vivi::Rect>(m, "Rect",
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
           [](const vivi::Rect &r)
           { return "<vivi." + r.ToString() + ">"; })
      .def("__str__", &vivi::Rect::ToString)
      .def(py::pickle(&pickling::SerializeRect,
                      &pickling::DeserializeRect))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_readwrite("cx", &vivi::Rect::cx, "Horizontal center.")
      .def_readwrite("cy", &vivi::Rect::cy, "Vertical center.")
      .def_readwrite("width", &vivi::Rect::width, "Rectangle width.")
      .def_readwrite("height", &vivi::Rect::height, "Rectangle height.")
      .def_readwrite("angle", &vivi::Rect::angle, "Angle in degrees (clockwise rotation).")
      .def_readwrite("radius", &vivi::Rect::radius, "Corner radius (for rounded rectangles).");

  // A Rect can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, vivi::Rect>();


  //TODO line? (overkill, not needed now)
  //TODO plane? maybe - either use vec4d or make a full-blown plane class
  //     preferably reuse vec4d - nobody expects a visualization toolbox to
  //     be the go-to resource for (easy to use) geometric datatypes

  //------------------------------------------------- Primitives - ImageBuffer
  // Info on numpy memory: https://stackoverflow.com/a/53099870/400948
  py::class_<vivi::ImageBuffer>(m, "ImageBuffer", py::buffer_protocol(),
                                "An ImageBuffer holds 8-bit images (Grayscale, "
                                "RGB or RGBA).")
      .def(py::init(&moddef::CreateImageBuffer),
           "Initialize from numpy array with dtype uint8.")
     .def_buffer([](vivi::ImageBuffer &img) -> py::buffer_info {
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
      .def("to_rgb", &vivi::ImageBuffer::ToRGB,
           "Convert to RGB. Will always return a copy, even if this buffer\n"
           "is already RGB.")
      .def("to_rgba", &vivi::ImageBuffer::ToRGBA,
           "Convert to RGBA. Will always return a copy, even if this buffer\n"
           "is already RGBA.")
      .def("__repr__",
           [](const vivi::ImageBuffer &b)
           { return "<vivi." + b.ToString() + ">"; })
      .def("__str__", &vivi::ImageBuffer::ToString)
      .def_readonly("width", &vivi::ImageBuffer::width, "Image width in pixels.")
      .def_readonly("height", &vivi::ImageBuffer::height, "Image height in pixels.")
      .def_readonly("channels", &vivi::ImageBuffer::channels, "Number of channels.")
      .def_readonly("stride", &vivi::ImageBuffer::stride, "Stride per row (in bytes).")
      .def_readonly("owns_data", &vivi::ImageBuffer::owns_data_,
                    "Boolean flag indicating if it has ownership of the image data,\n"
                    "i.e. whether it is responsible for memory cleanup.");

  // An ImageBuffer can be initialized from a numpy array
  py::implicitly_convertible<py::array, vivi::ImageBuffer>();


  //------------------------------------------------- Drawing - LineStyle
  py::class_<vivi::LineStyle> line_style(m, "LineStyle",
                                         "How a line should be drawn.");

  py::enum_<vivi::LineStyle::Cap>(line_style, "Cap",
                                  "How to render the endpoints of the line (or dash strokes).")
      .value("Butt", vivi::LineStyle::Cap::Butt,
             "Start/stop the line exactly at the start/end point.")
      .value("Round", vivi::LineStyle::Cap::Round,
             "Round ending, center of the circle is the end point.")
      .value("Square", vivi::LineStyle::Cap::Square,
             "Square ending, center of the square is the end point.");


  py::enum_<vivi::LineStyle::Join>(line_style, "Join",
                                   "How to render the junction of two lines/segments.")
      .value("Miter", vivi::LineStyle::Join::Miter,
             "Sharp (angled) corner.")
      .value("Bevel", vivi::LineStyle::Join::Bevel,
             "Cut off the join at half the line width from the joint point.")
      .value("Round", vivi::LineStyle::Join::Round,
             "Rounded join, where the center of the circle is the joint point.");


  line_style.def(py::init<>(&moddef::CreateLineStyle))// init from tuple
      .def(py::init<double, vivi::Color, std::vector<double>,
                    vivi::LineStyle::Cap, vivi::LineStyle::Join>(),
           py::arg("line_width"), py::arg("color"),
           py::arg("dash_pattern")=std::vector<double>(),
           py::arg("line_cap")=vivi::LineStyle::Cap::Butt,
           py::arg("line_join")=vivi::LineStyle::Join::Miter)
      .def("__repr__",
           [](const vivi::LineStyle &st)
           { return "<vivi." + st.ToString() + ">"; })
      .def("__str__", &vivi::LineStyle::ToString)
      .def(py::pickle(&pickling::SerializeLineStyle,
                      &pickling::DeserializeLineStyle))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def_readwrite("line_width", &vivi::LineStyle::line_width, "Width in pixels (best results for even values).")
      .def_readwrite("color", &vivi::LineStyle::color, "Line color (rgba).")
      .def_readwrite("dash_pattern", &vivi::LineStyle::dash_pattern, "Dash pattern defined as list of on/off strokes (lengths in\n"
                                                                     "pixels), e.g. [20, 10, 40, 10]. If the list is empty, the\n"
                                                                     "line will be drawn solid.")
      .def_readwrite("line_cap", &vivi::LineStyle::line_cap, "How to render the endpoints of the line (or dash strokes).")
      .def_readwrite("line_join", &vivi::LineStyle::line_join, "How to render the junction of two lines/segments.");


  // A LineStyle can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, vivi::LineStyle>();


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
      .def("__repr__", [](const moddef::Painter &) { return "<vivi.Painter>"; })
      .def("__str__", [](const moddef::Painter &) { return "vivi.Painter"; })
      .def("set_canvas_rgb", &moddef::Painter::SetCanvasColor,
           "Initializes the canvas with the given color.",
           py::arg("width"), py::arg("height"),
           py::arg("color")=vivi::Color(0, 0, 0, 1))
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
           "a numpy array (dtype uint8) or a vivi.ImageBuffer.\n\n"
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
           py::arg("fill")=vivi::Color(0, 0, 0, 0))
      .def("draw_circle", &moddef::Painter::DrawCircle,
           "Draws a circle at the given Vec2d position using the\n"
           "LineStyle specification. The circle will be filled if\n"
           "a fill color with alpha > 0 is given.",
           py::arg("center"), py::arg("radius"), py::arg("line_style"),
           py::arg("fill")=vivi::Color(0, 0, 0, 0))
      .def("draw_line", &moddef::Painter::DrawLine,
           "Draws a line between the two Vec2d coordinates using the\n"
           "LineStyle specification.",
           py::arg("from"), py::arg("to"), py::arg("line_style"))
      .def("draw_rect", &moddef::Painter::DrawRect,
           "Draws a rectangle using the LineStyle specification.\n\n"
           "* The rectangle will be filled if fill color has alpha > 0.\n"
           "* Draw a rotated rectangle by defining the rect's angle\n"
           "  in degrees (clockwise rotation).\n"
           "* By defining the rect's corner radius, you can draw\n"
           "  a rounded rectangle.",
           py::arg("rect"), py::arg("line_style"),
           py::arg("fill")=vivi::Color(0, 0, 0, 0))
      //TODO add draw_xxx methods
      ;


#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
