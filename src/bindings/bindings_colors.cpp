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


Color ColorFromTuple(const pybind11::tuple &tpl) {
  // Return invalid color for empty tuple
  if (tpl.empty()) {
    return Color();
  }

  if (tpl.size() < 3 || tpl.size() > 4) {
    std::ostringstream s;
    s << "Cannot create `viren2d.Color` from tuple with "
      << tpl.size() << " values. Expected 3 or 4!";
    throw std::invalid_argument(s.str());
  }

  // Restore exactly as given. We don't want saturating
  // casts to potentially influence the serialization:
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
  return py::make_tuple(
        obj.red, obj.green, obj.blue, obj.alpha);
}


Color CoordinateAxisColorFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    const auto str = py::cast<std::string>(o);
    return Color::CoordinateAxisColor(str[0]);
  } else if (py::isinstance<py::int_>(o)) {
    return Color::CoordinateAxisColor(
          static_cast<char>(py::cast<int>(o)));
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp
        << "` to `int` or `char`!";
    throw std::invalid_argument(str.str());
  }
}


Color FromObjectIDHelper(std::size_t id, const py::object &colormap) {
  ColorMap cm = ColorMapFromPyObject(colormap);
  return Color::FromObjectID(id, cm);
}


Color FromObjectCategoryHelper(
    const std::string &category, const py::object &colormap) {
  ColorMap cm = ColorMapFromPyObject(colormap);
  return Color::FromObjectCategory(category, cm);
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

  py::class_<Color> color(m, "Color", R"docstr(
        A color in rgba format, *i.e.* :math:`r,g,b,a \in [0,1]`.

        Important:
           If you initialize a color from a ``(r,g,b,a)`` :class:`tuple`, you
           must ensure that :math:`r,g,b,a \in [0, 1]`.

           This is due to the *saturation cast* performed by
           :class:`~viren2d.Color`. For example, the following :class:`tuple`
           will be converted to ``(1, 1, 1)``, *i.e.* the polygon would
           be actually filled with *white*!

           >>> # Mistakenly specifying a color as `RGB` tuple instead of `rgb`:
           >>> painter.draw_polygon(..., fill_color=(20, 20, 75))

           To specify a color as *RGB*, use :func:`~viren2d.RGBa` instead:

           >>> # Correctly specifying a color as `RGB`:
           >>> painter.draw_polygon(..., fill_color=viren2d.RGBa(20, 20, 75))
        )docstr");

  color.def(py::init<>(), R"docstr(
        Initializes an **invalid color**.

        An invalid color, *i.e.* ``r,g,b < 0``, can be used in several
        :class:`~viren2d.Painter` methods to mark *special* color
        handling, *e.g.* to skip filling.
        )docstr")
      .def(py::init<>(&ColorFromTuple), R"docstr(
        Initializes the color from a *rgb* or *rgba* ``tuple``.

        Each value must be a floating point number within ``[0, 1]``.
        The alpha value will be set to 1 if not provided.

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
           See :py:meth:`~viren2d.color_names()` for a list of
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
      .def(
        "copy", [](const Color &c) { return Color(c); },
        "Returns a deep copy.")
      .def(
        "__repr__",
        [](const Color &c) { return "<Color" + c.ToUInt8String() + '>'; })
      .def(
        "__str__",
        &Color::ToUInt8String)
      .def(
        py::pickle(&ColorToTuple, &ColorFromTuple),
        ":class:`~viren2d.Color` instances can be pickled.");

  // Operators
  color.def(
        py::self == py::self,
        "Checks for equality.\n\nReturns ``True`` if **all** components are equal.",
        py::arg("other"))
      .def(
        py::self != py::self,
        "Checks for inequality.\n\nReturns ``True`` if **any** components differ.",
        py::arg("other"))
      .def(
        py::self += py::self, R"docstr(
        Operator ``+=``.

        Adds the other :class:`~viren2d.Color` rgba values and
        performs a saturation cast, *i.e.* the resulting rgba
        values **will be clamped** to ``[0, 1]``.
        )docstr", py::arg("other"))
      .def(
        py::self + py::self, R"docstr(
        Operator ``lhs + rhs``.

        Adds the rgba values of the two :class:`~viren2d.Color`
        instances (``lhs`` and ``rhs``) and performs a saturating
        cast, *i.e.* the resulting rgba values **will be clamped**
        to ``[0, 1]``.
        )docstr", py::arg("rhs"));

#ifdef __clang__
// Clang gives false warnings: https://bugs.llvm.org/show_bug.cgi?id=43124
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif  // __clang__
  color.def(py::self -= py::self, R"docstr(
        Operator ``-=``.

        Subtracts the other :class:`~viren2d.Color` rgba values
        and performs a saturating cast, *i.e.* the resulting
        rgba values **will be clamped** to ``[0, 1]``.
        )docstr", py::arg("other"));
#ifdef __clang__
#pragma GCC diagnostic pop
#endif  // __clang__

  color.def(py::self - py::self, R"docstr(
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
        )docstr", py::arg("rhs"));

  // Converting to different representations
  color.def(
        "as_RGBa",
        &Color::ToRGBa, R"docstr(
        Returns the corresponding ``(R, G, B, a)`` tuple.

        R, G, B will be :math:`\in [0, 255]` and alpha :math:`a \in [0, 1]`.
        )docstr")
      .def(
        "as_rgba",
        [](const Color& c) {
          return py::make_tuple(c.red, c.green, c.blue, c.alpha);
        }, R"docstr(
        Returns the corresponding ``(r, g, b, a)`` tuple.

        Each tuple element will be of type :class:`float`, with
        :math:`r,g,b,a \in [0, 1]`.
        )docstr")
      .def(
        "as_hex",
        &Color::ToHexString, R"docstr(
        Returns the hex code representation.

        The web/hex code representation will contain 8 components,
        *i.e.* it includes the alpha value. To compute the hexadecimal
        representation, each component (*rgba*) is first scaled to
        ``[0, 255]``.
        )docstr")
      .def(
        "as_hsv",
        &Color::ToHSV, R"docstr(
        Returns the corresponding ``(h, s, v)`` tuple.

        Each tuple element will be of type :class:`float`, with
        :math:`h \in [0, 360]`, :math:`s \in [0, 1]` and :math:`v \in [0, 1]`.
        )docstr")
      .def(
        "grayscale",
        &Color::Grayscale, R"docstr(
        Returns the grayscale representation of this color.

        The r,g,b components of the returned color will be set
        to the luminance, :math:`L = 0.2989*R + 0.5870*G + 0.1141*B`.
        Alpha will stay the same.
        )docstr")
      .def(
        "with_alpha",
        &Color::WithAlpha, R"docstr(
        Returns a color with the same *r,g,b* components, but the given ``alpha``.
        )docstr",
        py::arg("alpha"))
      .def(
        "inverse",
        &Color::Inverse, R"docstr(
        Returns the inverse/complementary color.

        Except for shades of gray, this returns
        :math:`(1 - R, 1 - G, 1 - B, A)`. For gray values, it will either
        return black or white. In any case, the returned alpha value will stay
        the same.

        Why special handling of gray?
           Complementary colors should be used to provide good
           contrast/highlights. For colors close to medium gray (where
           r,g,b are close to 0.5), the rgb inverse would not be too useful.
        )docstr");

  // Member variables
  color.def_readwrite(
        "red", &Color::red,
        "float: Red component :math:`\\in [0, 1]`.")
      .def_readwrite(
        "green", &Color::green,
        "float: Green component :math:`\\in [0, 1]`.")
      .def_readwrite(
        "blue", &Color::blue,
        "float: Blue component :math:`\\in [0, 1]`.")
      .def_readwrite(
        "alpha", &Color::alpha, R"docstr(
        float: Opacity :math:`\in [0, 1]`, where 0 is fully transparent
          and 1 is fully opaque.
        )docstr");

  // TODO(pybind11 bug) documentation of static members is missing in python, see:
  //   https://github.com/pybind/pybind11/issues/3815
  color.def_readonly_static(
        "White", &Color::White,
        "Read-only white color instantiation.")
      .def_readonly_static(
        "Black", &Color::Black,
        "Read-only black color instantiation.")
      .def_readonly_static(
        "Red", &Color::Red,
        "Read-only red color instantiation.")
      .def_readonly_static(
        "Green", &Color::Green,
        "Read-only green color instantiation.")
      .def_readonly_static(
        "Blue", &Color::Blue,
        "Read-only blue color instantiation.")
      .def_readonly_static(
        "Cyan", &Color::Cyan,
        "Read-only cyan color instantiation.")
      .def_readonly_static(
        "Magenta", &Color::Magenta,
        "Read-only magenta color instantiation.")
      .def_readonly_static(
        "Yellow", &Color::Yellow,
        "Read-only yellow color instantiation.")
      .def_readonly_static(
        "Invalid", &Color::Invalid,
        "Read-only special color ``Invalid`` (*e.g.* to skip filling).")
      .def_readonly_static(
        "Same", &Color::Same,
        "Read-only special color ``Same`` (*e.g.* to use the same\n"
        "color for filling as the object's contour).")
      .def(
        "is_valid",
        &Color::IsValid, R"docstr(
        Returns ``True`` if this is a valid rgba color, where all
        components are :math:`\in [0, 1]`.
        )docstr")
      .def(
        "is_shade_of_gray", &Color::IsShadeOfGray, R"docstr(
        Checks if the *r,g,b* values are almost equal, *i.e.* :math:`\pm \epsilon`.
        )docstr",
        py::arg("eps") = 0.02);

  // A Color can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, Color>();

  // A Color can be initialized from a string representation directly.
  py::implicitly_convertible<py::str, Color>();


  // Static initialization methods
  doc = R"docstr(
        Returns a color for the (numeric) object ID.

        Useful to consistently use the same color for the
        same object or object class.

        Args:
          id: The object id as :class:`int`. Note that it must be
            :math:`\geq 0`, or a :class:`TypeError` will be raised.
          colormap: Optionally, select a different categorical
            :class:`~viren2d.ColorMap`. This parameter can be specified both
            via the enum value and the color map's string representation.
        )docstr";
  color.def_static(
        "from_object_id",
        &FromObjectIDHelper,
        doc.c_str(),
        py::arg("id"),
        py::arg("colormap") = ColorMap::GlasbeyDark);


  doc = R"docstr(
        Returns a color for the given category name.

        Useful to consistently use the same :class:`~viren2d.Color`
        for the same object class, *e.g.* ``car`` or ``person``.

        Args:
          category: The category name as :class:`str`. See
            :meth:`~viren2d.Color.object_category_names` for a list of
            category names which are explicitly defined. For any other
            category name, a string hash will be computed, which is
            then used to lookup a corresponding color.
          colormap: Optionally, select a different categorical
            :class:`~viren2d.ColorMap`. This parameter can be specified both
            via the enum value and the color map's string representation.
        )docstr";
  color.def_static(
        "from_object_category",
        &FromObjectCategoryHelper,
        doc.c_str(),
        py::arg("category"),
        py::arg("colormap") = ColorMap::GlasbeyDark);


  color.def_static(
        "object_category_names",
        &Color::ListObjectCategories, R"docstr(
        Returns a list of the category names which are explicitly
        known :meth:`~viren2d.Color.from_object_category`.

        Currently, this list contains all
        `COCO <https://cocodataset.org>`__ classes (80+1, *i.e.*
        ``background``), plus some additional aliases, *e.g.*
        ``human``\ :math:`\leftrightarrow`\ ``person``, or
        ``vehicle``\ :math:`\leftrightarrow`\ ``car``.

        **Corresponding C++ API:** ``viren2d::Color::ListObjectCategories``.
        )docstr");


  // Also aliases for typing convenience
  m.def("color_from_object_id",
        &FromObjectIDHelper,
        "Alias of :meth:`~viren2d.Color.from_object_id`.",
        py::arg("id"),
        py::arg("colormap") = ColorMap::GlasbeyDark);


  m.def("color_from_object_category",
        &FromObjectCategoryHelper,
        "Alias of :meth:`~viren2d.Color.from_object_category`.",
        py::arg("category"),
        py::arg("colormap") = ColorMap::GlasbeyDark);


  m.def("object_category_names",
        &Color::ListObjectCategories,
        "Alias of :meth:`~viren2d.Color.object_category_names`.");


  m.def("axis_color",
        &CoordinateAxisColorFromPyObject, R"docstr(
        Returns a color for the *x*, *y*, or *z* axis.

        Can be used, for example, to visualize the origin/orientation
        of the world coordinate system via differently colored
        arrows.

        Args:
          axis: Either the character code of the axis (``'x'``, ``'y'``,
            or ``'z'``), or its zero-based index as :class:`int`.
        )docstr", py::arg("axis"));


  doc = "Creates a :class:`~viren2d.Color` from\n"
        ":math:`r,g,b,a \\in [0,1]`.\n\n"
        "**Corresponding C++ API:** ``viren2d::rgba``.";
  m.def("rgba", &rgba, doc.c_str(),
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);


  doc = "Creates a :class:`~viren2d.Color` from\n"
        ":math:`R,G,B \\in [0, 255]` and alpha :math:`a \\in [0, 1]`.\n\n"
        "**Corresponding C++ API:** ``viren2d::RGBa``.";
  m.def("RGBa", &RGBa,
        doc.c_str(),
        py::arg("red"), py::arg("green"), py::arg("blue"),
        py::arg("alpha")=1.0);


  m.def("fade_out_linear",
        &ColorFadeOutLinear, R"docstr(
        Color transition function for :meth:`~viren2d.Painter.draw_trajectory`.

        Can be used to compute the color mixing weight for the
        color transition of fading trajectories.

        Returns:
          The input value, *i.e.* :math:`y=value`.
        )docstr", py::arg("value"));


  m.def("fade_out_quadratic",
        &ColorFadeOutQuadratic, R"docstr(
        Color transition function for :meth:`~viren2d.Painter.draw_trajectory`.

        Can be used to compute the color mixing weight for the
        color transition of fading trajectories.

        Returns:
          The quadratic factor :math:`y=value*value`.
        )docstr", py::arg("value"));


  m.def("fade_out_logarithmic",
        &ColorFadeOutLogarithmic, R"docstr(
        Color transition function for :meth:`~viren2d.Painter.draw_trajectory`.

        Can be used to compute the color mixing weight for the
        color transition of fading trajectories.

        Returns:
          The logarithmic factor :math:`y=\operatorname{log}_{10}(0.9 * value + 1)`.
        )docstr", py::arg("value"));
}

} // namespace bindings
} // namespace viren2d
