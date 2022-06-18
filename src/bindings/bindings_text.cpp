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
  // FIXME(snototter) update once we text style properties are decided

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
  halign.value("Left", HorizontalAlignment::Left, "Horizontally left-aligned.")
      .value("Center", HorizontalAlignment::Center, "Horizontally centered.")
      .value("Right", HorizontalAlignment::Right, "Horizontally right-aligned.");

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
  valign.value("Top", VerticalAlignment::Top, "Vertically top-aligned.")
      .value("Center", VerticalAlignment::Center, "Vertically centered.")
      .value("Bottom", VerticalAlignment::Bottom, "Vertically bottom-aligned.");

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
  anchor.value("Center", TextAnchor::Center,
             "Aligns text **both** horizontally & vertically **centered**.")
      .value("Left", TextAnchor::Left,
             "Aligns text **horizontally left-aligned** & **vertically centered**.")
      .value("Right", TextAnchor::Right,
             "Aligns text **horizontally right-aligned** & **vertically centered**.")
      .value("Top", TextAnchor::Top,
             "Aligns text **horizontally centered** & **vertically top-aligned**.")
      .value("Bottom", TextAnchor::Bottom,
             "Aligns text **horizontally centered** & **vertically bottom-aligned**.")
      .value("TopLeft", TextAnchor::TopLeft,
             "Aligns text **horizontally left-** & **vertically top-aligned**.")
      .value("TopRight", TextAnchor::TopRight,
             "Aligns text **horizontally right-** & **vertically top-aligned**.")
      .value("BottomLeft", TextAnchor::BottomLeft,
             "Aligns text **horizontally left-** & **vertically bottom-aligned**.")
      .value("BottomRight", TextAnchor::BottomRight,
             "Aligns text **horizontally right-** & **vertically bottom-aligned**.");

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

  std::string doc = "Returns all :class:`~"
      + FullyQualifiedType("TextAnchor") + "` values.\n\n"
      "Added to the Python class because due to our bindings,\n"
      "we cannot easily provide an iterator to list all\n"
      "values.\n\n"
      "**Corresponding C++ API:** ``viren2d::ListTextAnchors``.";
  anchor.def_static("list_all", &ListTextAnchors, doc.c_str());


  py::enum_<BoundingBoxLabelPosition> bblp(m, "BoundingBoxLabelPosition",
             "Enum specifying where to place a bounding box label.");
  bblp.value("Top", BoundingBoxLabelPosition::Top,
             "At the **top** of the bounding box.")
      .value("Bottom", BoundingBoxLabelPosition::Bottom,
             "At the **bottom** of the bounding box.")
      .value("Left", BoundingBoxLabelPosition::Left,
             "Along the **left edge** of the bounding box, **from bottom to top**.")
      .value("LeftT2B", BoundingBoxLabelPosition::LeftT2B,
             "Along the **left edge** of the bounding box, from **top to bottom**.")
      .value("Right", BoundingBoxLabelPosition::Right,
             "Along the **right edge** of the bounding box, from **top to bottom**.")
      .value("RightB2T", BoundingBoxLabelPosition::RightB2T,
             "Along the **right edge** of the bounding box, from **bottom to top**.");

  bblp.def(
      "__str__", [](BoundingBoxLabelPosition lp) -> py::str {
          return py::str(BoundingBoxLabelPositionToString(lp));
      }, py::name("__str__"), py::is_method(m));

  bblp.def(
      "__repr__", [](BoundingBoxLabelPosition lp) -> py::str {
          std::ostringstream s;
          s << "<BoundingBoxLabelPosition."
            << BoundingBoxLabelPositionToString(lp) << '>';
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
      >>> style.font_family = 'monospace'
      >>> style.font_size = 10
      >>> style.color = 'navy-blue!80'
      >>> style.bold = True
      >>> style.alignment = 'left'
      >>> style.line_spacing = 1.1

      >>> # Alternatively, you would get the same style via:
      >>> style = viren2d.TextStyle(
      >>>     font_family='monospace', font_size=10,
      >>>     color='navy-blue!80', bold=True
      >>>     alignment=viren2d.HorizontalAlignment.Left,
      >>>     line_spacing=1.1)
      )docstr";
  py::class_<TextStyle>text_style(m, "TextStyle", doc.c_str());


  doc = "Creates a customized text style.\n\n"
        "Args:\n"
        "  size: Font size in pixels as ``float``.\n"
        "  family: Name of the font family, refer to the class\n"
        "    member :attr:`font_family` for details.\n"
        "  color: Text color as :class:`~" + FullyQualifiedType("Color") + "`.\n"
        "  bold: If ``True``, the font weight will be bold (type ``bool``).\n"
        "  italic: If ``True``, the font slant will be italic (type ``bool``).\n"
        "  line_spacing: Scaling factor of the vertical distance between\n"
        "    consecutive lines of text.\n"
        "  alignment: Horizontal alignment of multi-line text as :class:`~"
        + FullyQualifiedType("HorizontalAlignment") + "` enum.";
  TextStyle default_style;
  text_style.def(py::init<unsigned int, const std::string &,
                          const Color &, bool, bool, double,
                          HorizontalAlignment>(), doc.c_str(),
         py::arg("size") = default_style.size,
         py::arg("font_family") = default_style.family,
         py::arg("color") = default_style.color,
         py::arg("bold") = default_style.bold,
         py::arg("italic") = default_style.italic,
         py::arg("line_spacing") = default_style.line_spacing,
         py::arg("alignment") = default_style.alignment);

  text_style.def("copy", [](const TextStyle &st) { return TextStyle(st); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const TextStyle &st)
           { return FullyQualifiedType(st.ToString(), true); })
      .def("__str__", &TextStyle::ToString)
      .def(py::pickle(&TextStyleToTuple, &TextStyleFromTuple),
           ":class:`~viren2d.TextStyle` instances can be pickled.")
      .def(py::self == py::self, "Checks for equality.")
      .def(py::self != py::self, "Checks for inequality.")
      .def("is_valid", &TextStyle::IsValid,
          "Check if the style allows rendering text.")
      .def_readwrite("size", &TextStyle::size,
          "float: Font size in pixels.")
      .def_readwrite("family", &TextStyle::family, R"docstr(
          str: Name of the font family.

          Most available fonts on the system should be supported.
          If you experience issues, try the generic CSS2 family names
          first, *e.g.* ``serif``, ``sans-serif``, or ``monospace``. Refer to the
          `Cairo documentation <https://www.cairographics.org/manual/cairo-text.html#cairo_select_font_face>`__
          for more details.
          )docstr")
      .def_readwrite("bold", &TextStyle::bold,
          "bool: If ``True``, the font weight will be bold.")
      .def_readwrite("italic", &TextStyle::italic,
          "bool: If ``True``, the font slant will be italic.")
      .def_readwrite("line_spacing", &TextStyle::line_spacing,
          "float: Scaling factor of the vertical distance between\n"
          "  consecutive lines of text.");


  doc = ":class:`~" + FullyQualifiedType("Color")
      + "`: Color of the text glyphs.";
  text_style.def_readwrite("color", &TextStyle::color, doc.c_str());


  doc = ":class:`~" + FullyQualifiedType("HorizontalAlignment") + "`: "
        "Horizontal alignment of multi-line text.\n\n"
        "In addition to the enum values, you can use\n"
        "the string representations (``left|west``, ``center``,\n"
        "``right|east``) to set this member:\n\n"
        ">>> style.alignment = viren2d.HorizontalAlignment.Center\n"
        ">>> style.alignment = 'center'\n";
  text_style.def_property("alignment",
        [](TextStyle &s) { return s.alignment; },
        [](TextStyle &s, py::object o) {
            s.alignment = HorizontalAlignmentFromPyObject(o);
        }, doc.c_str());

//  // A TextStyle can be initialized from a given tuple.
//  py::implicitly_convertible<py::tuple, TextStyle>();
}

} // namespace bindings
} // namespace viren2d

