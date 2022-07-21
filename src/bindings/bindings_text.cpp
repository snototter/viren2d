#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <viren2d/styles.h>

#include <bindings/binding_helpers.h>

//TODO(doc) add corresponding c++ api to docstr

namespace py = pybind11;

namespace viren2d {
namespace bindings {
//-------------------------------------------------  TextStyle
/// Convert to tuple (used for pickling)
py::tuple TextStyleToTuple(const TextStyle &obj) {
  return py::make_tuple(
        obj.size, obj.family, ColorToTuple(obj.color), obj.bold, obj.italic,
        obj.line_spacing, obj.halign, obj.valign);
}


/// Convert from tuple (used for pickling and overloaded constructor).
TextStyle TextStyleFromTuple(const py::tuple &tpl) {
  // Convert empty tuple to pre-defined default style
  if(tpl.empty()) {
    return TextStyle();
  }

  if (tpl.size() > 8) {
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
    style.halign = tpl[6].cast<HorizontalAlignment>();
  }

  if (tpl.size() > 7) {
    style.valign = tpl[7].cast<VerticalAlignment>();
  }

  return style;
}


void RegisterAnchors(py::module &m) {
  py::enum_<HorizontalAlignment> halign(m, "HorizontalAlignment", R"docstr(
            Enumeration for horizontal alignment.

            **Corresponding C++ API:** ``viren2d::HorizontalAlignment``.
            )docstr");
  halign.value(
        "Left",
        HorizontalAlignment::Left, R"docstr(
        Horizontally left-aligned.

        String representation ``'left'`` or ``'west'``.
        )docstr")
      .value(
        "Center",
        HorizontalAlignment::Center, R"docstr(
        Horizontally centered.

        String representation ``'center'`` or ``'middle'``.
        )docstr")
      .value(
        "Right",
        HorizontalAlignment::Right, R"docstr(
        Horizontally right-aligned.

        String representation ``'right'`` or ``'east'``.
        )docstr");

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


  py::enum_<VerticalAlignment> valign(m, "VerticalAlignment", R"docstr(
            Options for vertical alignment.

            **Corresponding C++ API:** ``viren2d::VerticalAlignment``.
            )docstr");
  valign.value(
        "Top",
        VerticalAlignment::Top, R"docstr(
        Vertically top-aligned.

        String representation ``'top'`` or ``'north'``.
        )docstr")
      .value(
        "Center",
        VerticalAlignment::Center, R"docstr(
        Vertically centered.

        String representation ``'center'`` or ``'middle'``.
        )docstr")
      .value(
        "Bottom",
        VerticalAlignment::Bottom, R"docstr(
        Vertically bottom-aligned.

        String representation ``'bottom'`` or ``'south'``.
        )docstr");

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


  py::enum_<Anchor> anchor(m, "Anchor", R"docstr(
            Placement options with respect to a reference point.

            **Corresponding C++ API:** ``viren2d::Anchor``.
            )docstr");
  anchor.value(
        "Center",
        Anchor::Center, R"docstr(
        Horizontally & vertically centered.
        Alias string representation: ``'middle'``.
        )docstr")
      .value(
        "Left",
        Anchor::Left, R"docstr(
        Horizontally **left-aligned** & vertically **centered**.
        Alias string representation: ``west``.
        )docstr")
      .value(
        "Right",
        Anchor::Right, R"docstr(
        Horizontally **right-aligned** & vertically **centered**.
        Alias string representation: ``east``.
        )docstr")
      .value(
        "Top",
        Anchor::Top, R"docstr(
        Horizontally **centered** & vertically **top-aligned**.
        Alias string representation: ``north``.
        )docstr")
      .value(
        "Bottom",
        Anchor::Bottom, R"docstr(
        Horizontally **centered** & vertically **bottom-aligned**.
        Alias string representation: ``south``.
        )docstr")
      .value(
        "TopLeft",
        Anchor::TopLeft, R"docstr(
        Horizontally **left-** & vertically **top-aligned**.
        Alias string representation: ``north-west``.
        )docstr")
      .value(
        "TopRight",
        Anchor::TopRight, R"docstr(
        Horizontally **right-** & vertically **top-aligned**.
        Alias string representation: ``north-east``.
        )docstr")
      .value(
        "BottomLeft",
        Anchor::BottomLeft, R"docstr(
        Horizontally **left-** & vertically **bottom-aligned**.
        Alias string representation: ``south-west``.
        )docstr")
      .value(
        "BottomRight",
        Anchor::BottomRight, R"docstr(
        Horizontally **right-** & vertically **bottom-aligned**.
        Alias string representation: ``south-east``.
        )docstr");

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
        m, "LabelPosition", R"docstr(
        Placement options for labels.

        **Corresponding C++ API:** ``viren2d::LabelPosition``.
        )docstr");
  bblp.value(
        "Top",
        LabelPosition::Top, R"docstr(
        At the **top** of the bounding box.
        )docstr")
      .value(
        "Bottom",
        LabelPosition::Bottom, R"docstr(
        At the **bottom** of the bounding box.
        )docstr")
      .value(
        "Left",
        LabelPosition::Left, R"docstr(
        Along the **left edge** of the bounding box, **from bottom
        to top**. Alias string representation: ``left-b2t``.
        )docstr")
      .value(
        "LeftT2B",
        LabelPosition::LeftT2B, R"docstr(
        Along the **left edge** of the bounding box, from **top
        to bottom**.
        )docstr")
      .value(
        "Right",
        LabelPosition::Right, R"docstr(
        Along the **right edge** of the bounding box, from **top
        to bottom**. Alias string representation: ``right-t2b``.
        )docstr")
      .value(
        "RightB2T",
        LabelPosition::RightB2T, R"docstr(
        Along the **right edge** of the bounding box, from **bottom
        to top**.
        )docstr");

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


VerticalAlignment VerticalAlignmentFromPyObject(const py::object &o) {
  if (py::isinstance<py::str>(o)) {
    const auto str = py::cast<std::string>(o);
    return VerticalAlignmentFromString(str);
  } else if (py::isinstance<VerticalAlignment>(o)) {
    return py::cast<VerticalAlignment>(o);
  } else {
    const std::string tp = py::cast<std::string>(
        o.attr("__class__").attr("__name__"));
    std::ostringstream str;
    str << "Cannot cast type `" << tp
        << "` to `viren2d.VerticalAlignment`!";
    throw std::invalid_argument(str.str());
  }
}


/// Convenience construction of a text style,
/// which accepts alignment definition as
/// either enum or string representation.
TextStyle CreateTextStyle(
    unsigned int font_size, const std::string &font_family,
    const Color &font_color, bool font_bold, bool font_italic,
    double spacing, const py::object &halign, const py::object &valign) {
  HorizontalAlignment horz_align = HorizontalAlignmentFromPyObject(halign);
  VerticalAlignment vert_align = VerticalAlignmentFromPyObject(valign);
  return TextStyle(
        font_size, font_family, font_color,
        font_bold, font_italic, spacing, horz_align, vert_align);
}


void RegisterTextStyle(py::module &m) {
  std::string doc = R"docstr(
      How text should be rendered.

      **Corresponding C++ API:** ``viren2d::TextStyle``.

      Example:
        >>> text_style = viren2d.TextStyle(
        >>>     family='monospace', size=18,
        >>>     color='navy-blue', bold=True,
        >>>     italic=False, line_spacing=1.1,
        >>>     halign='left', valign='top')
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
        halign: Horizontal alignment of multi-line text
          as :class:`~viren2d.HorizontalAlignment` enum. This parameter
          also accepts the corresponding string representation.
        valign: Vertical alignment of multi-line text
          as :class:`~viren2d.VerticalAlignment` enum. This parameter
          also accepts the corresponding string representation. Only affects
          alignment of **fixed-size** text boxes, see
          :meth:`~viren2d.Painter.draw_text_box`.
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
        py::arg("halign") = default_style.halign,
        py::arg("valign") = default_style.valign);

  text_style.def(
        "copy",
        [](const TextStyle &st) { return TextStyle(st); },
        "Returns a deep copy.")
      .def(
        "__repr__",
        [](const TextStyle &st) {
          return "<TextStyle" + st.ToString() + ">";
        })
      .def(
        "__str__",
        &TextStyle::ToString)
      .def(
        py::pickle(&TextStyleToTuple, &TextStyleFromTuple), R"docstr(
        :class:`~viren2d.TextStyle` instances can be pickled.
        )docstr")
      .def(
        py::self == py::self, R"docstr(
        Checks for equality.
        )docstr")
      .def(
        py::self != py::self, R"docstr(
        Checks for inequality.
        )docstr")
      .def(
        "is_valid",
        &TextStyle::IsValid, R"docstr(
        Returns ``True`` if the style allows rendering text.

          **Corresponding C++ API:** ``viren2d::TextStyle::IsValid``.
        )docstr")
      .def_readwrite(
        "size",
        &TextStyle::size, R"docstr(
        float: Font size in pixels.

          **Corresponding C++ API:** ``viren2d::TextStyle::size``.
        )docstr")
      .def_readwrite(
        "family",
        &TextStyle::family, R"docstr(
        str: Name of the font family.

          Most available fonts on the system should be supported.
          If you experience issues, try the generic CSS2 family names
          first, *e.g.* ``serif``, ``sans-serif``, or ``monospace``. Refer to the
          `Cairo documentation <https://www.cairographics.org/manual/cairo-text.html#cairo_select_font_face>`__
          for more details.

          **Corresponding C++ API:** ``viren2d::TextStyle::family``.
        )docstr")
      .def_readwrite(
        "bold",
        &TextStyle::bold, R"docstr(
        bool: If ``True``, the font weight will be bold.

          **Corresponding C++ API:** ``viren2d::TextStyle::bold``.
        )docstr")
      .def_readwrite(
        "italic",
        &TextStyle::italic, R"docstr(
        bool: If ``True``, the font slant will be italic.

          **Corresponding C++ API:** ``viren2d::TextStyle::italic``.
        )docstr")
      .def_readwrite(
        "line_spacing",
        &TextStyle::line_spacing, R"docstr(
        float: Scaling factor of the vertical distance between
          consecutive lines of text.

          **Corresponding C++ API:** ``viren2d::TextStyle::line_spacing``.
        )docstr")
      .def_readwrite(
        "color",
        &TextStyle::color, R"docstr(
        :class:`~viren2d.Color`: Color of the text glyphs.

          **Corresponding C++ API:** ``viren2d::TextStyle::color``.
        )docstr");


  text_style.def_property(
        "halign",
        [](const TextStyle &s) { return s.halign; },
        [](TextStyle &s, py::object o) {
            s.halign = HorizontalAlignmentFromPyObject(o);
        }, R"docstr(
        :class:`~viren2d.HorizontalAlignment`:  Horizontal alignment of
          multi-line text.

          In addition to the enumeration values, you can use
          the string representations (``left|west``,
          ``center|middle``, ``right|east``) to set this member:

          >>> style.halign = viren2d.HorizontalAlignment.Center
          >>> style.halign = 'center'

          **Corresponding C++ API:** ``viren2d::TextStyle::halign``.
        )docstr");


  text_style.def_property(
        "valign",
        [](const TextStyle &s) { return s.valign; },
        [](TextStyle &s, py::object o) {
            s.valign = VerticalAlignmentFromPyObject(o);
        }, R"docstr(
        :class:`~viren2d.VerticalAlignment`:  Vertical alignment of
          multi-line text. Will only affect the output of **fixed-size**
          text boxes, see :meth:`~viren2d.Painter.draw_text_box`.

          In addition to the enumeration values, you can use
          the string representations (``top|north``,
          ``center|middle``, ``bottom|south``) to set this member:

          >>> style.valign = viren2d.VerticalAlignment.Center
          >>> style.valign = 'center'

          **Corresponding C++ API:** ``viren2d::TextStyle::valign``.
        )docstr");
}

} // namespace bindings
} // namespace viren2d

