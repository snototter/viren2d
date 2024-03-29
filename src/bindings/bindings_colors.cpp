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


void RegisterColor(py::module &m) {
  m.def("color_names", &ListNamedColors, R"docstr(
        Returns a list of the predefined color names.

        Each of these names can be used to initialize a
        :class:`~viren2d.Color`. For example:

        >>> text_style.color = 'midnight-blue'    # alpha = 1.0
        >>> line_style.color = 'forest-green!40'  # alpha = 0.4

        **Corresponding C++ API:** ``viren2d::ListNamedColors``.

        Currently, the following color names are available:

        .. viren2d-color-names-table::
        )docstr");


  py::class_<Color> color(m, "Color", R"docstr(
        A color in *rgba* format, *i.e.* :math:`r,g,b,a \in [0,1]`.

        **Corresponding C++ API:** ``viren2d::Color``.

        Important:
           If you initialize a color from a ``(r,g,b,a)`` :class:`tuple`, you
           must ensure that :math:`r,g,b,a \in [0, 1]`.

           This is due to the *saturation cast* performed by
           :class:`~viren2d.Color`. For example, the following :class:`tuple`
           will be converted to ``(1, 1, 1)``, *i.e.* the polygon would
           be actually filled with *white*!

           >>> # Mistakenly specifying a color as `RGB` tuple instead of `rgb`:
           >>> painter.draw_polygon(..., fill_color=(20, 20, 75))

           To specify a color as **RGB**, use :func:`~viren2d.RGBa` instead:

           >>> # Correctly specifying a color as `RGB`:
           >>> painter.draw_polygon(..., fill_color=viren2d.RGBa(20, 20, 75))
        )docstr");

  color.def(
        py::init<>(), R"docstr(
        Initializes an **invalid color**.

        An invalid color, *i.e.* ``r,g,b < 0``, can be used in several
        :class:`~viren2d.Painter` methods to mark *special* color
        handling, *e.g.* to skip filling of a shape.

        This constructor additionally allows the implicit conversion of
        ``None`` to an invalid color.
        )docstr")
      .def(
        py::init<>([](py::none) { return Color::Invalid; }), R"docstr(
        A ``None`` will be interpreted as an **invalid color**.

        An invalid color, *i.e.* ``r,g,b < 0``, can be used in several
        :class:`~viren2d.Painter` methods to mark *special* color
        handling, *e.g.* to skip filling.
        )docstr")
      .def(
        py::init<>(&ColorFromTuple), R"docstr(
        Initializes the color from a *r,g,b* or *r,g,b,a* ``tuple``.

        Each value must be a floating point number within ``[0, 1]``.
        The alpha value will be set to 1 if not provided.

        Example:
          >>> painter.draw_polygon(..., fill_color = (red, green, blue))
          >>> painter.draw_polygon(..., fill_color = (red, green, blue, alpha)
        )docstr", py::arg("tpl"))
      .def(
        py::init<double, double, double, double>(), R"docstr(
        Initializes the color from the given *rgba* components.

        All values **will be clamped** to ``[0, 1]``.
        )docstr",
        py::arg("red"),
        py::arg("green"),
        py::arg("blue"),
        py::arg("alpha") = 1.0)
      .def(
        py::init<const std::string &, double>(),R"docstr(
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
        py::arg("colorspec"),
        py::arg("alpha") = 1.0)
      .def(
        "copy",
        [](const Color &c) { return Color(c); }, R"docstr(
        Returns a deep copy.

        **Corresponding C++ API:** Copy constructor of ``viren2d::Color``.
        )docstr")
      .def(
        "__repr__",
        [](const Color &c) {
          std::ostringstream s;
          s << "viren2d.Color";
          if (c.IsValid()) {
            s << "(red=" << c.red << ", green=" << c.green
              << ", blue=" << c.blue << ", alpha=" << c.alpha << ')';
          } else {
            if (c.IsSpecialSame()) {
              s << ".Same";
              if (c.alpha < 1.0) {
                s << ".with_alpha(" << c.alpha << ')';
              }
            } else {
              s << ".Invalid";
            }
          }
          return s.str();
        })
      .def(
        "__str__",
        [](const Color &c) {return c.ToUInt8String(false); })
      .def(
        py::pickle(&ColorToTuple, &ColorFromTuple),
        ":class:`~viren2d.Color` instances can be pickled.");

  // Operators
  color.def(
        py::self == py::self, R"docstr(
        Checks for equality.

        Returns ``True`` if **all** components are equal.
        )docstr",
        py::arg("other"))
      .def(
        py::self != py::self, R"docstr(
        Checks for inequality.

        Returns ``True`` if **any** components differ.
        )docstr",
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
        )docstr",
        py::arg("rhs"));

#ifdef __clang__
// Clang gives false warnings: https://bugs.llvm.org/show_bug.cgi?id=43124
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign-overloaded"
#endif  // __clang__
  color.def(
        py::self -= py::self, R"docstr(
        Operator ``-=``.

        Subtracts the other :class:`~viren2d.Color` rgba values
        and performs a saturating cast, *i.e.* the resulting
        rgba values **will be clamped** to ``[0, 1]``.
        )docstr",
        py::arg("other"));
#ifdef __clang__
#pragma GCC diagnostic pop
#endif  // __clang__

  color.def(
        py::self - py::self, R"docstr(
        Operator ``lhs - rhs``.

        Subtracts the rgba values of the :class:`~viren2d.Color`
        instance ``rhs`` from ``lhs`` and performs a saturating
        cast, *i.e.* the resulting rgba values **will be clamped**
        to ``[0, 1]``.
        )docstr",
        py::arg("rhs"))
      .def(
        py::self *= float(), R"docstr(
        Operator ``*= float``.

        Scales **all** components by the given scalar factor and
        performs a saturating cast, *i.e.* the resulting rgba
        values **will be clamped** to ``[0, 1]``.
        )docstr",
        py::arg("scale"))
      .def(
        float() * py::self, R"docstr(
        Operator ``float * Color``.

        Scales **all** components of the right-hand side
        :class:`~viren2d.Color` by the left-hand side scalar
        factor and performs a saturating cast, *i.e.* the
        resulting rgba values **will be clamped** to ``[0, 1]``.
        )docstr",
        py::arg("lhs"))
      .def(
        py::self * float(), R"docstr(
        Operator ``Color * float``.

        Scales **all** components of the left-hand side
        :class:`~viren2d.Color` by the right-hand side scalar
        factor and performs a saturating cast, *i.e.* the
        resulting rgba values **will be clamped** to ``[0, 1]``.
        )docstr",
        py::arg("rhs"))
      .def(
        py::self /= float(), R"docstr(
        Operator ``/= float``.

        Divides **all** components of this :class:`~viren2d.Color`
        by the right-hand side scalar factor and performs a
        saturating cast, *i.e.* the resulting rgba values **will
        be clamped** to ``[0, 1]``.
        )docstr",
        py::arg("rhs"))
      .def(
        py::self / float(), R"docstr(
        Operator ``Color / float``.

        Divides **all** components of the lef-hand side
        :class:`~viren2d.Color` by the right-hand side scalar
        factor and performs a saturating cast, *i.e.* the
        resulting rgba values **will be clamped** to ``[0, 1]``.
        )docstr",
        py::arg("rhs"));

  // Converting to different representations
  color.def(
        "to_RGBa",
        &Color::ToRGBa, R"docstr(
        Returns the corresponding :math:`(R, G, B, \alpha)` tuple.

        The color components will be of type :class:`int`, with
        :math:`R, G, B \in [0, 255]`, whereas :math:`\alpha \in [0, 1]` is
        of type :class:`float`.

        **Corresponding C++ API:** ``viren2d::Color::ToRGBa``.
        )docstr")
      .def(
        "to_RGB",
        &Color::ToRGB, R"docstr(
        Returns the corresponding :math:`(R, G, B)` tuple.

        Each element :math:`R, G, B \in [0, 255]` will be of
        type :class:`int`.

        **Corresponding C++ API:** ``viren2d::Color::ToRGB``.
        )docstr")
      .def(
        "to_rgba",
        [](const Color& c) {
          return py::make_tuple(c.red, c.green, c.blue, c.alpha);
        }, R"docstr(
        Returns the corresponding ``(r, g, b, a)`` tuple.

        Each tuple element will be of type :class:`float`, with
        :math:`r,g,b,a \in [0, 1]`.

        **No corresponding C++ API**, access red, green, blue and alpha
        separately.
        )docstr")
      .def(
        "to_rgb",
        [](const Color& c) {
          return py::make_tuple(c.red, c.green, c.blue);
        }, R"docstr(
        Returns the corresponding ``(r, g, b)`` tuple.

        Each tuple element will be of type :class:`float`, with
        :math:`r,g,b \in [0, 1]`.

        **No corresponding C++ API**, access red, green, blue separately.
        )docstr")
      .def(
        "to_hex",
        &Color::ToHexString, R"docstr(
        Returns the hex code representation.

        If ``with_alpha`` is ``True``, the returned string representation will
        contain 8 digits (+ 1 character for the leading ``#``), and 6 digits
        otherwise. To compute the hexadecimal representation, each component,
        *i.e.* *r,g,b* and optionally *a*, is first scaled to ``[0, 255]``.

        **Corresponding C++ API:** ``viren2d::Color::ToHexString``.

        Args:
          with_alpha: Boolean flag whether to include the alpha value or not.
        )docstr",
        py::arg("with_alpha") = false)
      .def(
        "to_hsv",
        &Color::ToHSV, R"docstr(
        Returns the corresponding ``(h, s, v)`` tuple.

        Each tuple element will be of type :class:`float`, with
        :math:`h \in [0, 360]`, :math:`s \in [0, 1]` and :math:`v \in [0, 1]`.

        **Corresponding C++ API:** ``viren2d::Color::ToHSV``.
        )docstr")
      .def(
        "to_gray",
        &Color::ToGray, R"docstr(
        Returns the grayscale representation of this color.

        The :math:`r,g,b` components of the returned color will be set
        to the luminance, :math:`l = 0.2989*r + 0.5870*g + 0.1141*b`,
        and :math:`\alpha` will stay the same.

        **Corresponding C++ API:** ``viren2d::Color::ToGray``.
        )docstr")
      .def(
        "intensity",
        &Color::GrayscaleIntensity, R"docstr(
        Returns the grayscale intensity as :math:`l = 0.2989*r + 0.5870*g + 0.1141*b`.

        **Corresponding C++ API:** ``viren2d::Color::GrayscaleIntensity``.
        )docstr")
      .def(
        "with_alpha",
        &Color::WithAlpha, R"docstr(
        Returns a color with the same :math:`r,g,b` components, but
        the given :math:`\alpha`.

        **Corresponding C++ API:** ``viren2d::Color::WithAlpha``.

        Args:
          alpha: Opacity as :class:`float` :math:`\in [0, 1]`.
        )docstr",
        py::arg("alpha"))
      .def(
        "inverse",
        &Color::Inverse, R"docstr(
        Returns the inverse/complementary color.

        Except for shades of gray, this returns
        :math:`(1 - r, 1 - g, 1 - b, a)`. For gray values, it will either
        return black or white. In any case, the returned alpha value will stay
        the same.

        Why special handling of gray?
           Complementary colors should be used to provide good
           contrast/highlights. For colors close to medium gray (where
           r,g,b are close to 0.5), the rgb inverse would not be too useful.

        **Corresponding C++ API:** ``viren2d::Color::Inverse``.
        )docstr");

  // Member variables
  color.def_readwrite(
        "red",
        &Color::red, R"docstr(
        float: Red component :math:`\in [0, 1]`.

          **Corresponding C++ API:** ``viren2d::Color::red``.
        )docstr")
      .def_readwrite(
        "green",
        &Color::green, R"docstr(
        float: Green component :math:`\in [0, 1]`.

          **Corresponding C++ API:** ``viren2d::Color::green``.
        )docstr")
      .def_readwrite(
        "blue",
        &Color::blue, R"docstr(
        float: Blue component :math:`\in [0, 1]`.

          **Corresponding C++ API:** ``viren2d::Color::blue``.
        )docstr")
      .def_readwrite(
        "alpha",
        &Color::alpha, R"docstr(
        float: Opacity :math:`\in [0, 1]`, where 0 is fully transparent
          and 1 is fully opaque.

          **Corresponding C++ API:** ``viren2d::Color::alpha``.
        )docstr");

  // TODO(pybind11 bug) documentation of static members is missing in python, see:
  //   https://github.com/pybind/pybind11/issues/3815
  color.def_readonly_static(
        "White",
        &Color::White, R"docstr(
        Read-only white color instantiation.

        **Corresponding C++ API:** ``viren2d::Color::White``.
        )docstr")
      .def_readonly_static(
        "Black",
        &Color::Black, R"docstr(
        "Read-only black color instantiation."

        **Corresponding C++ API:** ``viren2d::Color::Black``.
        )docstr")
      .def_readonly_static(
        "Red",
        &Color::Red, R"docstr(
        Read-only red color instantiation.

        **Corresponding C++ API:** ``viren2d::Color::Red``.
        )docstr")
      .def_readonly_static(
        "Green",
        &Color::Green, R"docstr(
        Read-only green color instantiation.

        **Corresponding C++ API:** ``viren2d::Color::Green``.
        )docstr")
      .def_readonly_static(
        "Blue",
        &Color::Blue, R"docstr(
        Read-only blue color instantiation.

        **Corresponding C++ API:** ``viren2d::Color::Blue``.
        )docstr")
      .def_readonly_static(
        "Cyan",
        &Color::Cyan, R"docstr(
        Read-only cyan color instantiation.

        **Corresponding C++ API:** ``viren2d::Color::Cyan``.
        )docstr")
      .def_readonly_static(
        "Magenta",
        &Color::Magenta, R"docstr(
        Read-only magenta color instantiation.

        **Corresponding C++ API:** ``viren2d::Color::Magenta``.
        )docstr")
      .def_readonly_static(
        "Yellow",
        &Color::Yellow, R"docstr(
        Read-only yellow color instantiation.

        **Corresponding C++ API:** ``viren2d::Color::Yellow``.
        )docstr")
      .def_readonly_static(
        "Invalid",
        &Color::Invalid, R"docstr(
        Read-only special color ``Invalid`` (*e.g.* to skip filling).

        **Corresponding C++ API:** ``viren2d::Color::Invalid``.
        )docstr")
      .def_readonly_static(
        "Same",
        &Color::Same, R"docstr(
        Read-only special color ``Same`` (*e.g.* to use the same
        color for filling as the object's contour).

        **Corresponding C++ API:** ``viren2d::Color::Same``.
        )docstr")
      .def(
        "is_valid",
        &Color::IsValid, R"docstr(
        Returns ``True`` if this is a valid rgba color, where all
        components are :math:`\in [0, 1]`.

        **Corresponding C++ API:** ``viren2d::Color::IsValid``.
        )docstr")
      .def(
        "is_shade_of_gray", &Color::IsShadeOfGray, R"docstr(
        Checks if the *r,g,b* values are almost equal, *i.e.* within
        :math:`\pm \epsilon`.

        **Corresponding C++ API:** ``viren2d::Color::IsShadeOfGray``.
        )docstr",
        py::arg("eps") = 0.02);


  // A Color can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, Color>();


  // A Color can be initialized from a string representation directly.
  py::implicitly_convertible<py::str, Color>();


  // Static initialization methods
  color.def_static(
        "from_object_id",
        &Color::FromObjectID, R"docstr(
        Returns a color for the (numeric) object ID.

        Allows coloring the same object instance consistently, *e.g.* when
        tracking objects.

        **Corresponding C++ API:** ``viren2d::Color::FromObjectId``.

        Args:
          id: The object id as :class:`int`. Note that it must be
            :math:`\geq 0`, or a :class:`TypeError` will be raised.
          colormap: Optionally, select a different categorical
            :class:`~viren2d.ColorMap`. This parameter can be specified both
            via the enumeration value and the color map's string
            representation.

        .. viren2d-color-by-id-defaults::
        )docstr",
        py::arg("id"),
        py::arg("colormap") = ColorMap::GlasbeyDark);


  color.def_static(
        "from_object_category",
        &Color::FromObjectCategory, R"docstr(
        Returns a color for the given category name.

        Allows coloring the same object class consistently, *e.g.* to
        distinguish multiple classes, such as ``car`` or ``person``.

        **Corresponding C++ API:** ``viren2d::Color::FromObjectCategory``.

        Args:
          category: The category name as :class:`str`. See
            :meth:`~viren2d.Color.object_category_names` for a list of
            category names which are explicitly defined. For any other
            category name, a string hash will be computed, which is
            then used to lookup a corresponding color.
          colormap: Optionally, select a different categorical
            :class:`~viren2d.ColorMap`. This parameter can be specified both
            via the enumeration value and the color map's string
            representation.

        Currently, the following category names are available. The shown
        colors are from the default color map:

        .. viren2d-object-category-names::
        )docstr",
        py::arg("category"),
        py::arg("colormap") = ColorMap::GlasbeyDark);
  

  color.def_static(
        "from_hsv",
        &Color::FromHSV, R"docstr(
        Creates a :class:`~viren2d.Color` from its HSV representation.

        Args:
          h: Hue, :math:`h \in [0, 360]`.
          s: Saturation, :math:`s \in [0, 1]`.
          v: Value, :math:`v \in [0, 1]`.
          alpha: Optional opacity of the color, :math:`\alpha \in [0, 1]`.

        **Corresponding C++ API:** ``viren2d::ColorFromHSV``.
        )docstr",
        py::arg("h"),
        py::arg("s"),
        py::arg("v"),
        py::arg("alpha") = 1.0);


  color.def_static(
        "object_category_names",
        &Color::ListObjectCategories, R"docstr(
        Returns a list of the category names which are explicitly
        known to :meth:`~viren2d.Color.from_object_category`.

        **Corresponding C++ API:** ``viren2d::Color::ListObjectCategories``.

        This category list contains all (80+1)
        `COCO <https://cocodataset.org>`__ classes (incl.
        ``background``), plus additional aliases, *e.g.*
        ``human``\ :math:`\leftrightarrow`\ ``person``, or
        ``vehicle``\ :math:`\leftrightarrow`\ ``car``.

        Currently, the following category names are available. The shown
        colors are from the default color map used in
        :meth:`~viren2d.Color.from_object_category`:

        .. viren2d-object-category-names::
        )docstr");


  // Also aliases for typing convenience
  m.def("color_from_object_id",
        &Color::FromObjectID,
        "Alias of :meth:`viren2d.Color.from_object_id`.",
        py::arg("id"),
        py::arg("colormap") = ColorMap::GlasbeyDark);


  m.def("color_from_object_category",
        &Color::FromObjectCategory,
        "Alias of :meth:`viren2d.Color.from_object_category`.",
        py::arg("category"),
        py::arg("colormap") = ColorMap::GlasbeyDark);


  m.def("object_category_names",
        &Color::ListObjectCategories,
        "Alias of :meth:`viren2d.Color.object_category_names`.");


  m.def("axis_color",
        &CoordinateAxisColorFromPyObject, R"docstr(
        Returns a default color for the :math:`x`, :math:`y`, or :math:`z` axis.

        Can be used, for example, to visualize the origin/orientation
        of the world coordinate system via differently colored
        arrows.

        Current axis colors: |axis-colors-html|

        **Corresponding C++ API:** ``viren2d::Color::CoordinateAxisColor``.

        Args:
          axis: Either the character code of the axis (``'x'``, ``'y'``,
            or ``'z'``), or its zero-based index as :class:`int`.
        )docstr", py::arg("axis"));


  m.def("rgba",
        &rgba, R"docstr(
        Creates a :class:`~viren2d.Color` from :math:`r,g,b,a \in [0,1]`.

        **Corresponding C++ API:** ``viren2d::rgba``.
        )docstr",
        py::arg("red"),
        py::arg("green"),
        py::arg("blue"),
        py::arg("alpha") = 1.0);


  m.def("RGBa",
        &RGBa, R"docstr(
        Creates a :class:`~viren2d.Color` from :math:`R,G,B \in [0, 255]` and
        alpha :math:`a \in [0, 1]`.

        **Corresponding C++ API:** ``viren2d::RGBa``.
        )docstr",
        py::arg("red"),
        py::arg("green"),
        py::arg("blue"),
        py::arg("alpha") = 1.0);
  

  m.def("fade_out_linear",
        &ColorFadeOutLinear, R"docstr(
        Color transition function for :meth:`viren2d.Painter.draw_trajectory`.

        Can be used to compute the color mixing weight for the
        color transition of fading trajectories.

        **Corresponding C++ API:** ``viren2d::ColorFadeOutLinear``.

        Returns:
          float: The input value, *i.e.* :math:`y = \text{value}`.
        )docstr",
        py::arg("value"));


  m.def("fade_out_quadratic",
        &ColorFadeOutQuadratic, R"docstr(
        Color transition function for :meth:`viren2d.Painter.draw_trajectory`.

        Can be used to compute the color mixing weight for the
        color transition of fading trajectories.

        **Corresponding C++ API:** ``viren2d::ColorFadeOutQuadratic``.

        Returns:
          float: The quadratic factor :math:`y = \text{value}^2`.
        )docstr", py::arg("value"));


  m.def("fade_out_logarithmic",
        &ColorFadeOutLogarithmic, R"docstr(
        Color transition function for :meth:`viren2d.Painter.draw_trajectory`.

        Can be used to compute the color mixing weight for the
        color transition of fading trajectories.

        **Corresponding C++ API:** ``viren2d::ColorFadeOutLogarithmic``.

        Returns:
          float: The logarithmic factor :math:`y=\operatorname{log}_{10}(0.9 * \text{value} + 1)`.
        )docstr", py::arg("value"));

  py::implicitly_convertible<py::none, Color>();
}

} // namespace bindings
} // namespace viren2d

