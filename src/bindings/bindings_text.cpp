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


TextStyle TextStyleFromTuple(const py::tuple &tpl) {
  // Convert empty tuple to pre-defined default style
  if(tpl.empty()) {
    return TextStyle();
  }

  if (tpl.size() > 7) {
    std::ostringstream s;
    s << "Cannot create `viren2d.TextStyle` from tuple with "
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
             "Options for horizontal alignment.");
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
             "Options for vertical alignment.");
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


  py::enum_<Anchor> anchor(m, "Anchor",
             "Placement options with respect to a reference point.");
  anchor.value(
        "Center",
        Anchor::Center,
        "Horizontally & vertically centered.")
      .value(
        "Left",
        Anchor::Left,
        "Horizontally **left-aligned** & vertically **centered**. "
        "Alias string representation: ``west``.")
      .value(
        "Right",
        Anchor::Right,
        "Horizontally **right-aligned** & vertically **centered**. "
        "Alias string representation: ``east``.")
      .value(
        "Top",
        Anchor::Top,
        "Horizontally **centered** & vertically **top-aligned**. "
        "Alias string representation: ``north``.")
      .value(
        "Bottom",
        Anchor::Bottom,
        "Horizontally **centered** & vertically **bottom-aligned**. "
        "Alias string representation: ``south``.")
      .value(
        "TopLeft",
        Anchor::TopLeft,
        "Horizontally **left-** & vertically **top-aligned**. "
        "Alias string representation: ``north-west``.")
      .value(
        "TopRight",
        Anchor::TopRight,
        "Horizontally **right-** & vertically **top-aligned**. "
        "Alias string representation: ``north-east``.")
      .value(
        "BottomLeft",
        Anchor::BottomLeft,
        "Horizontally **left-** & vertically **bottom-aligned**. "
        "Alias string representation: ``south-west``.")
      .value(
        "BottomRight",
        Anchor::BottomRight,
        "Horizontally **right-** & vertically **bottom-aligned**. "
        "Alias string representation: ``south-east``.");

  anchor.def(
        "__str__", [](Anchor a) -> py::str {
            return py::str(AnchorToString(a));
        }, py::name("__str__"), py::is_method(m));

  anchor.def(
        "__repr__", [](Anchor a) -> py::str {
            std::ostringstream s;
            s << "'" << AnchorToString(a) << "'";
            return py::str(s.str());
        }, py::name("__repr__"), py::is_method(m));


  std::string doc = R"docstr(
      Returns all :class:`~viren2d.Anchor` values.

      Convenience utility to easily iterate all enumeration
      values.

      **Corresponding C++ API:** ``viren2d::ListAnchors``.
      )docstr";
  anchor.def_static("list_all", &ListAnchors, doc.c_str());


  py::enum_<LabelPosition> bblp(
        m, "LabelPosition",
        "Placement options for labels.");
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


HorizontalAlignment HorizontalAlignmentFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    const auto str = py::cast<std::string>(o);
    return HorizontalAlignmentFromString(str);
  } else if (py::isinstance<HorizontalAlignment>(o)) {
    return py::cast<HorizontalAlignment>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp
        << "` to `viren2d.HorizontalAlignment`!";
    throw std::invalid_argument(str.str());
  }
}


/// Convenience construction of a text style,
/// which accepts alignment definition as
/// either enum or string representation.
TextStyle CreateTextStyle(
    unsigned int font_size, const std::string &font_family,
    const Color &font_color, bool font_bold, bool font_italic,
    double spacing, const py::object &align) {
  HorizontalAlignment halign = HorizontalAlignmentFromPyObject(align);
  return TextStyle(
        font_size, font_family, font_color,
        font_bold, font_italic, spacing, halign);
}


void RegisterTextStyle(py::module &m) {
  std::string doc = R"docstr(
      How text should be rendered.

      Example:
        >>> text_style = viren2d.TextStyle(
        >>>     family='monospace', size=18,
        >>>     color='navy-blue', bold=True,
        >>>     italic=False, line_spacing=1.1,
        >>>     alignment='left')
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
          as :class:`~viren2d.HorizontalAlignment` enum. This parameter
          also accepts the corresponding string representation.
      )docstr";

  TextStyle default_style;
  text_style.def(
        py::init<>(&CreateTextStyle),
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
        ``center|middle``, ``right|east``) to set this member:

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

