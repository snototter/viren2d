#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <viren2d/styles.h>

#include <bindings/binding_helpers.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {
//-------------------------------------------------  TextStyle
py::tuple TextStyleToTuple(const TextStyle &obj) {
  return py::make_tuple(obj.size, obj.family,
                        ColorToTuple(obj.color),
                        obj.bold, obj.italic,
                        obj.line_spacing, obj.alignment);
}


TextStyle TextStyleFromTuple(py::tuple tpl) {
  // Convert empty tuple to pre-defined default style
  if(tpl.empty()) {
    return TextStyle();
  }

  if (tpl.size() > 7) {
    std::ostringstream s;
    s << "Cannot create " << FullyQualifiedType("TextStyle")
      << " from tuple with "
      << tpl.size() << " entries!";
    throw std::invalid_argument(s.str());
  }

  TextStyle style;
  style.size = tpl[0].cast<int>();

  if (tpl.size() > 1) {
    style.family = tpl[1].cast<std::string>();
  }

  if (tpl.size() > 2) {
    style.color = ColorFromTuple(tpl[2]);
  }

  if (tpl.size() > 3) {
    style.bold = tpl[3].cast<bool>();
  }

  if (tpl.size() > 4) {
    style.italic = tpl[4].cast<bool>();
  }

  if (tpl.size() > 5) {
    style.line_spacing = tpl[5].cast<double>();
  }

  if (tpl.size() > 6) {
    style.alignment = tpl[6].cast<HorizontalAlignment>();
  }

  return style;
}


void RegisterAnchors(py::module &m) {
  py::enum_<HorizontalAlignment> halign(m, "HorizontalAlignment",
             "Enum specifying the horizontal alignment.");
  halign.value(
        "Left",
        HorizontalAlignment::Left,
        "Horizontally left-aligned.")
      .value(
        "Center",
        HorizontalAlignment::Center,
        "Horizontally centered.")
      .value(
        "Right",
        HorizontalAlignment::Right,
        "Horizontally right-aligned.");

  halign.def(
        "__str__", [](HorizontalAlignment a) -> py::str {
            return py::str(HorizontalAlignmentToString(a));
        }, py::name("__str__"), py::is_method(m));

  halign.def(
        "__repr__", [](HorizontalAlignment a) -> py::str {
            std::ostringstream s;
            s << "'" << HorizontalAlignmentToString(a) << "'";
            return py::str(s.str());
        }, py::name("__repr__"), py::is_method(m));


  py::enum_<VerticalAlignment> valign(m, "VerticalAlignment",
             "Enum specifying the vertical alignment.");
  valign.value(
        "Top",
        VerticalAlignment::Top,
        "Vertically top-aligned.")
      .value(
        "Center",
        VerticalAlignment::Center,
        "Vertically centered.")
      .value(
        "Bottom",
        VerticalAlignment::Bottom,
        "Vertically bottom-aligned.");

  valign.def(
        "__str__", [](VerticalAlignment a) -> py::str {
            return py::str(VerticalAlignmentToString(a));
        }, py::name("__str__"), py::is_method(m));

  valign.def(
        "__repr__", [](VerticalAlignment a) -> py::str {
            std::ostringstream s;
            s << "'" << VerticalAlignmentToString(a) << "'";
            return py::str(s.str());
        }, py::name("__repr__"), py::is_method(m));


  py::enum_<TextAnchor> anchor(m, "TextAnchor",
             "Enum specifying the location of the text anchor.");
  anchor.value(
        "Center",
        TextAnchor::Center,
        "Horizontally & vertically centered.")
      .value(
        "Left",
        TextAnchor::Left,
        "Horizontally **left-aligned** & vertically **centered**. "
        "Alias string representation: ``west``.")
      .value(
        "Right",
        TextAnchor::Right,
        "Horizontally **right-aligned** & vertically **centered**. "
        "Alias string representation: ``east``.")
      .value(
        "Top",
        TextAnchor::Top,
        "Horizontally **centered** & vertically **top-aligned**. "
        "Alias string representation: ``north``.")
      .value(
        "Bottom",
        TextAnchor::Bottom,
        "Horizontally **centered** & vertically **bottom-aligned**. "
        "Alias string representation: ``south``.")
      .value(
        "TopLeft",
        TextAnchor::TopLeft,
        "Horizontally **left-** & vertically **top-aligned**. "
        "Alias string representation: ``north-west``.")
      .value(
        "TopRight",
        TextAnchor::TopRight,
        "Horizontally **right-** & vertically **top-aligned**. "
        "Alias string representation: ``north-east``.")
      .value(
        "BottomLeft",
        TextAnchor::BottomLeft,
        "Horizontally **left-** & vertically **bottom-aligned**. "
        "Alias string representation: ``south-west``.")
      .value(
        "BottomRight",
        TextAnchor::BottomRight,
        "Horizontally **right-** & vertically **bottom-aligned**. "
        "Alias string representation: ``south-east``.");

  anchor.def(
        "__str__", [](TextAnchor a) -> py::str {
            return py::str(TextAnchorToString(a));
        }, py::name("__str__"), py::is_method(m));

  anchor.def(
        "__repr__", [](TextAnchor a) -> py::str {
            std::ostringstream s;
            s << "'" << TextAnchorToString(a) << "'";
            return py::str(s.str());
        }, py::name("__repr__"), py::is_method(m));


  std::string doc = R"docstr(
      Returns all :class:`~viren2d.TextAnchor` values.

      Convenience utility to easily iterate all enumeration
      values.

      **Corresponding C++ API:** ``viren2d::ListTextAnchors``.
      )docstr";
  anchor.def_static("list_all", &ListTextAnchors, doc.c_str());


  py::enum_<LabelPosition> bblp(
        m, "LabelPosition",
        "Enum specifying where to place a bounding box label.");
  bblp.value(
        "Top",
        LabelPosition::Top,
        "At the **top** of the bounding box.")
      .value(
        "Bottom",
        LabelPosition::Bottom,
        "At the **bottom** of the bounding box.")
      .value(
        "Left",
        LabelPosition::Left,
        "Along the **left edge** of the bounding box, **from bottom "
        "to top**. Alias string representation: ``left-b2t``.")
      .value(
        "LeftT2B",
        LabelPosition::LeftT2B,
        "Along the **left edge** of the bounding box, from **top "
        "to bottom**.")
      .value(
        "Right",
        LabelPosition::Right,
        "Along the **right edge** of the bounding box, from **top "
        "to bottom**. Alias string representation: ``right-t2b``.")
      .value(
        "RightB2T",
        LabelPosition::RightB2T,
        "Along the **right edge** of the bounding box, from **bottom "
        "to top**.");

  bblp.def(
      "__str__", [](LabelPosition lp) -> py::str {
          return py::str(LabelPositionToString(lp));
      }, py::name("__str__"), py::is_method(m));

  bblp.def(
      "__repr__", [](LabelPosition lp) -> py::str {
          std::ostringstream s;
          s << "<LabelPosition."
            << LabelPositionToString(lp) << '>';
          return py::str(s.str());
      }, py::name("__repr__"), py::is_method(m));
}


HorizontalAlignment HorizontalAlignmentFromPyObject(py::object &o) {
  if (py::isinstance<py::str>(o)) {
    const auto str = py::cast<std::string>(o);
    return HorizontalAlignmentFromString(str);
  } else if (py::isinstance<HorizontalAlignment>(o)) {
    return py::cast<HorizontalAlignment>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp << "` to `"
        << FullyQualifiedType("HorizontalAlignment") << "`!";
    throw std::invalid_argument(str.str());
  }
}


void RegisterTextStyle(py::module &m) {
  std::string doc = R"docstr(
      How text should be rendered.

      **Example:**

      >>> # Initialize the default style and adjust what you need:
      >>> style = viren2d.TextStyle()
      >>> style.family = 'monospace'
      >>> style.size = 10
      >>> style.color = 'navy-blue!80'
      >>> style.bold = True
      >>> style.alignment = 'left'
      >>> style.line_spacing = 1.1

      >>> # Alternatively, you would get the same style via:
      >>> style = viren2d.TextStyle(
      >>>     family='monospace', size=10,
      >>>     color='navy-blue!80', bold=True
      >>>     alignment=viren2d.HorizontalAlignment.Left,
      >>>     line_spacing=1.1)
      )docstr";
  py::class_<TextStyle>text_style(m, "TextStyle", doc.c_str());


  doc = R"docstr(
      Creates a customized text style.

      Args:
        size: Font size in pixels as :class:`float`.
        family: Name of the font family. Refer to the class
          member :attr:`family` for details.
        color: Text color as :class:`~viren2d.Color`.
        bold: If ``True``, the font weight will be bold (type :class:`bool`).
        italic: If ``True``, the font slant will be italic (type :class:`bool`).
        line_spacing: Scaling factor of the vertical distance between
          consecutive lines of text.
        alignment: Horizontal alignment of multi-line text
          as :class:`~viren2d.HorizontalAlignment` enum.
      )docstr";
  TextStyle default_style;
  text_style.def(py::init<unsigned int, const std::string &,
                          const Color &, bool, bool, double,
                          HorizontalAlignment>(),
                 doc.c_str(),
         py::arg("size") = default_style.size,
         py::arg("family") = default_style.family,
         py::arg("color") = default_style.color,
         py::arg("bold") = default_style.bold,
         py::arg("italic") = default_style.italic,
         py::arg("line_spacing") = default_style.line_spacing,
         py::arg("alignment") = default_style.alignment);

  text_style.def(
        "copy",
        [](const TextStyle &st) { return TextStyle(st); },
        "Returns a deep copy.")
      .def(
        "__repr__",
        [](const TextStyle &)
        { return FullyQualifiedType("TextStyle", true); })
      .def(
        "__str__",
        &TextStyle::ToString)
      .def(
        py::pickle(&TextStyleToTuple, &TextStyleFromTuple),
        ":class:`~viren2d.TextStyle` instances can be pickled.")
      .def(
        py::self == py::self,
        "Checks for equality.")
      .def(
        py::self != py::self,
        "Checks for inequality.")
      .def(
        "is_valid",
        &TextStyle::IsValid,
        "Returns ``True`` if the style allows rendering text.")
      .def_readwrite(
        "size",
        &TextStyle::size,
        "float: Font size in pixels.")
      .def_readwrite(
        "family",
        &TextStyle::family, R"docstr(
          str: Name of the font family.

          Most available fonts on the system should be supported.
          If you experience issues, try the generic CSS2 family names
          first, *e.g.* ``serif``, ``sans-serif``, or ``monospace``. Refer to the
          `Cairo documentation <https://www.cairographics.org/manual/cairo-text.html#cairo_select_font_face>`__
          for more details.
          )docstr")
      .def_readwrite(
        "bold",
        &TextStyle::bold,
        "bool: If ``True``, the font weight will be bold.")
      .def_readwrite(
        "italic",
        &TextStyle::italic,
        "bool: If ``True``, the font slant will be italic.")
      .def_readwrite(
        "line_spacing",
        &TextStyle::line_spacing,
        "float: Scaling factor of the vertical distance between\n"
        "  consecutive lines of text.");


  doc = ":class:`~viren2d.Color`: Color of the text glyphs.";
  text_style.def_readwrite("color", &TextStyle::color, doc.c_str());


  doc = R"docstr(
      :class:`~viren2d.HorizontalAlignment`:  Horizontal
        alignment of multi-line text.

        In addition to the enum values, you can use
        the string representations (``left|west``,
        ``center``, ``right|east``) to set this member:

        >>> style.alignment = viren2d.HorizontalAlignment.Center
        >>> style.alignment = 'center'
      )docstr";
  text_style.def_property(
        "alignment",
        [](TextStyle &s) { return s.alignment; },
        [](TextStyle &s, py::object o) {
            s.alignment = HorizontalAlignmentFromPyObject(o);
        }, doc.c_str());
}

} // namespace bindings
} // namespace viren2d

