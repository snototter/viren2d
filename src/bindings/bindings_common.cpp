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
  // Restore exactly as given. We don't want saturating
  // cast to potentially influence the serialization:
  Color col;
  col.red = tpl[0].cast<double>();
  col.green = tpl[1].cast<double>();
  col.blue = tpl[2].cast<double>();

  if (tpl.size() == 4) {
    col.alpha = tpl[3].cast<double>();
  } else {
    col.alpha = 1.0;
  }

  return col;
}


py::tuple ColorToTuple(const Color &obj) {
  return py::make_tuple(obj.red, obj.green,
                        obj.blue, obj.alpha);
}


void RegisterColor(py::module &m) {
  std::string doc = R"docstr(
      Returns a list of the predefined color names.

      Each of these names can be used to initialize a
      :class:`~viren2d.Color`. For example:

      >>> text_style.color = 'midnight-blue'    # alpha = 1.0
      >>> line_style.color = 'forest-green!40'  # alpha = 0.4

      **Corresponding C++ API:** ``viren2d::ListNamedColors``.
      )docstr";
  m.def("color_names", &ListNamedColors, doc.c_str());

  py::class_<Color>(m, "Color", R"docstr(
      A color in rgba format, where each component is within ``[0, 1]``.

      Important:
         If you initialize a color from a ``tuple(r,g,b,a)``, you **must
         ensure that the r,g,b values are within** ``[0, 1]``.

         The *caveat* lies in *saturation casting* performed by
         :class:`~viren2d.Color`. For example, the following ``tuple``
         will be converted to ``(1, 1, 1)``, *i.e.* the polygon would
         be filled with *white* instead!

         >>> # Incorrectly specifying a color as `RGB` tuple instead of `rgb`:
         >>> painter.draw_polygon(..., fill_color=(20, 20, 75))"
      )docstr")
      .def(py::init<>(), R"docstr(
           Initializes an **invalid color**.

           An invalid color, *i.e.* ``r,g,b < 0``, can be used in several
           :class:`~viren2d.Painter` methods to mark *special* color
           handling, *e.g.* to skip filling.
           )docstr")
      .def(py::init<>(&ColorFromTuple), R"docstr(
           Initializes the color from a *rgb* or *rgba* ``tuple``.

           Each value must be a floating point number within ``[0, 1]``

           Example:

              >>> painter.draw_polygon(..., fill_color = (red, green, blue))
              >>> painter.draw_polygon(..., fill_color = (red, green, blue, alpha)
           )docstr", py::arg("tpl"))
      .def(py::init<double, double, double, double>(), R"docstr(
           Initializes the color from the given rgba components.

           All values **will be clamped** to ``[0, 1]``.
           )docstr", py::arg("red"), py::arg("green"),
           py::arg("blue"), py::arg("alpha") = 1.0)
      .def(py::init<const std::string &, double>(),R"docstr(
           Initializes the color from a string representation (hexcode
           or color name).

           **Hexcode:**
              HTML hex code string as either ``#RRGGBB`` or
              ``#RRGGBBAA``:

              >>> painter.draw_rect(..., fill_color = '#00ff00')
              >>> painter.draw_rect(..., fill_color = '#a0b0c0f0')

           **Color name:**
              See :py:meth:`viren2d.color_names()` for a list of
              available color names.

              >>> painter.draw_rect(..., fill_color = 'black')
              >>> painter.draw_rect(..., fill_color = 'navy-blue')

              A color name can also include an ``alpha`` suffix, which
              must be specified as integer in ``[0, 100]``):

              >>> painter.draw_rect(..., fill_color = 'forest-green!50')

              Color names can also be inverted by prepending either
              ``!`` or ``-``, which results in the *complementary color*,
              *e.g.* ``!blue!30`` is the same as ``yellow!30``:

              >>> painter.draw_rect(..., fill_color = '!blue!30)
            )docstr",
           py::arg("colorspec"), py::arg("alpha")=1.0)
      .def("copy", [](const Color &c) { return Color(c); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const Color &c)
           { return "<" + c.ToString() + ">"; })
      .def("__str__", &Color::ToHexString)
      .def(py::pickle(&ColorToTuple, &ColorFromTuple),
           ":class:`~viren2d.Color` instances can be pickled.")
      .def(py::self == py::self,
           "Checks for equality.\n\nReturns ``True`` if **all** components are equal.",
           py::arg("other"))
      .def(py::self != py::self,
           "Checks for inequality.\n\nReturns ``True`` if **any** components differ.",
           py::arg("other"))
      .def(py::self += py::self, R"docstr(
        Operator ``+=``.

        Adds the other :class:`~viren2d.Color` rgba values and
        performs a saturation cast, *i.e.* the resulting rgba
        values **will be clamped** to ``[0, 1]``.
        )docstr", py::arg("other"))
      .def(py::self + py::self, R"docstr(
        Operator ``lhs + rhs``.

        Adds the rgba values of the two :class:`~viren2d.Color`
        instances (``lhs`` and ``rhs``) and performs a saturating
        cast, *i.e.* the resulting rgba values **will be clamped**
        to ``[0, 1]``.
        )docstr", py::arg("rhs"))
#ifdef __clang__
// Clang gives false warnings: https://bugs.llvm.org/show_bug.cgi?id=43124
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif  // __clang__
      .def(py::self -= py::self, R"docstr(
        Operator ``-=``.

        Subtracts the other :class:`~viren2d.Color` rgba values
        and performs a saturating cast, *i.e.* the resulting
        rgba values **will be clamped** to ``[0, 1]``.
        )docstr", py::arg("other"))
#ifdef __clang__
#pragma GCC diagnostic pop
#endif  // __clang__
      .def(py::self - py::self, R"docstr(
        Operator ``lhs - rhs``.

        Subtracts the rgba values of the :class:`~viren2d.Color`
        instance ``rhs`` from ``lhs`` and performs a saturating
        cast, *i.e.* the resulting rgba values **will be clamped**
        to ``[0, 1]``.
        )docstr", py::arg("rhs"))
      .def(py::self *= float(), R"docstr(
        Operator ``*= float``.

        Scales **all** components by the given scalar factor and
        performs a saturating cast, *i.e.* the resulting rgba
        values **will be clamped** to ``[0, 1]``.
        )docstr", py::arg("scale"))
      .def(float() * py::self, R"docstr(
        Operator ``float * Color``.

        Scales **all** components of the right-hand side
        :class:`~viren2d.Color` by the left-hand side scalar
        factor and performs a saturating cast, *i.e.* the
        resulting rgba values **will be clamped** to ``[0, 1]``.
        )docstr", py::arg("lhs"))
      .def(py::self * float(), R"docstr(
        Operator ``Color * float``.

        Scales **all** components of the left-hand side
        :class:`~viren2d.Color` by the right-hand side scalar
        factor and performs a saturating cast, *i.e.* the
        resulting rgba values **will be clamped** to ``[0, 1]``.
        )docstr", py::arg("rhs"))
      .def(py::self /= float(), R"docstr(
        Operator ``/= float``.

        Divides **all** components of this :class:`~viren2d.Color`
        by the right-hand side scalar factor and performs a
        saturating cast, *i.e.* the resulting rgba values **will
        be clamped** to ``[0, 1]``.
        )docstr", py::arg("rhs"))
      .def(py::self / float(), R"docstr(
        Operator ``Color / float``.

        Divides **all** components of the lef-hand side
        :class:`~viren2d.Color` by the right-hand side scalar
        factor and performs a saturating cast, *i.e.* the
        resulting rgba values **will be clamped** to ``[0, 1]``.
        )docstr", py::arg("rhs"))
      .def("as_RGBa", &Color::ToRGBa,
           "Returns the corresponding ``(R, G, B, a)`` tuple,\n"
           "where R, G, B in ``[0, 255]`` and ``alpha`` in ``[0, 1]``.")
      .def("as_rgba", [](Color& c)
           { return py::make_tuple(c.red, c.green, c.blue, c.alpha); },
           "Returns the corresponding ``(r, g, b, a)`` tuple,\n"
           "where all components are in ``[0, 1]``.")
      .def("as_hex", &Color::ToHexString, R"docstr(
           Returns the hex code representation.

           The web/hex code representation will contain 8 components,
           *i.e.* it includes the alpha value. To compute the hexadecimal
           representation, each component (*rgba*) is first scaled to
           ``[0, 255]``.
           )docstr")
      .def("grayscale", &Color::Grayscale,
           "Returns the grayscale representation of this color.\n\n"
           "Computes the luminance as :math:`L = 0.2989*R + 0.5870*G + 0.1141*B`.")
      .def("with_alpha", &Color::WithAlpha,
           "Returns a color with the same *rgb* components, but the given ``alpha``.",
           py::arg("alpha"))
      .def_readwrite("red", &Color::red,
           "float: Red component within ``[0, 1]``.")
      .def_readwrite("green", &Color::green,
           "float: Green component within ``[0, 1]``.")
      .def_readwrite("blue", &Color::blue,
           "float: Blue component within ``[0, 1]``.")
      .def_readwrite("alpha", &Color::alpha,
           "float: Opacity within ``[0, 1]``, where ``0`` is fully transparent\n"
           "and ``1`` is fully opaque.")
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
           "components are within ``[0, 1]``.")
      .def("inverse", &Color::Inverse, R"docstr(
           Returns the inverse/complementary color.

           Except for shades of gray, this returns the rgb inverse, *i.e.*
           ``(1-r, 1-g, 1-b, a)``.
           For gray values, it will either return black or white. In both
           cases, the returned alpha value will always stay the same.

           Why special handling of gray?
              Complementary colors should be used to provide good
              contrast/highlights. For colors close to medium gray (where
              r,g,b are close to 0.5), the rgb inverse would not be too useful.
           )docstr")
      .def("is_shade_of_gray", &Color::IsShadeOfGray,
           "Checks if all rgb components are almost the same (+/- the given epsilon).",
           py::arg("eps")=0.02);

  // A Color can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, Color>();

  // A Color can be initialized from a string representation directly.
  py::implicitly_convertible<py::str, Color>();


  doc = "Returns a :class:`~" + FullyQualifiedType("Color") + "` for the given values.\n\n"
        "The red, green, blue and alpha components must be within ``[0, 1]``.\n\n"
        "**Corresponding C++ API:** ``viren2d::rgba``.";
  m.def("rgba", &rgba, doc.c_str(),
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);


  doc = "Returns a :class:`~" + FullyQualifiedType("Color") + "` for the given RGBa values.\n\n"
        "The R, G, and B components must be within ``[0, 255]``, whereas\n"
        "alpha must be within ``[0, 1]``.\n\n"
        "**Corresponding C++ API:** ``viren2d::RGBa``.";
  m.def("RGBa", &RGBa,
        doc.c_str(),
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);
}

} // namespace bindings
} // namespace viren2d

