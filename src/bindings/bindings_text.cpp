#include <sstream>
#include <stdexcept>

#include <pybind11/operators.h>

#include <viren2d/styles.h>

#include <bindings/binding_helpers.h>
namespace py = pybind11;

namespace viren2d {
namespace bindings {
//-------------------------------------------------  TextStyle
py::tuple TextStyleToTuple(const TextStyle &obj) {
  return py::make_tuple(obj.font_size, obj.font_family,
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
  style.font_size = tpl[0].cast<int>();

  if (tpl.size() > 1) {
    style.font_family = tpl[1].cast<std::string>();
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
  py::enum_<HorizontalAlignment>(m, "HorizontalAlignment",
             "Specifies the horizontal alignment.")
      .value("Left", HorizontalAlignment::Left, "Horizontally left-aligned.")
      .value("Center", HorizontalAlignment::Center, "Horizontally centered.")
      .value("Right", HorizontalAlignment::Right, "Horizontally right-aligned.");

  std::string doc = "Parses a string into a :class:`"
      + FullyQualifiedType("HorizontalAlignment") + "`.";
  m.def("halign", HorizontalAlignmentFromString,
        doc.c_str(), py::arg("align"));


  py::enum_<VerticalAlignment>(m, "VerticalAlignment",
             "Specifies vertical alignment.")
      .value("Top", VerticalAlignment::Top, "Vertically top-aligned.")
      .value("Center", VerticalAlignment::Center, "Vertically centered.")
      .value("Bottom", VerticalAlignment::Bottom, "Vertically bottom-aligned.");

  doc = "Parses a string into a :class:`"
        + FullyQualifiedType("VerticalAlignment") + "`.";
  m.def("valign", VerticalAlignmentFromString,
        doc.c_str(), py::arg("align"));

  py::enum_<TextAnchor>(m, "TextAnchor",
             "TODO doc")
      .value("Center", TextAnchor::Center,
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

  doc = "Parses a string into a :class:`"
        + FullyQualifiedType("TextAnchor") + "`.\n\n"
        "Valid string representations include standard\n"
        "position specifications (*e.g.* ``center``, ``top``, ``right``, ``bottom-left``, *etc.*)\n"
        "and the 8 compass directions (*e.g.*, ``north``, ``south-west``).\n\n"
        "Before parsing, the input will be converted to lowercase and any\n"
        "whitespaces, dashes & underscores will be removed.";
  m.def("text_anchor", TextAnchorFromString,
        doc.c_str(), py::arg("anchor"));


  py::enum_<BoundingBoxLabelPosition>(m, "BoundingBoxLabelPosition",
             "Where to place a bounding box label.")
      .value("Top", BoundingBoxLabelPosition::Top,
             "At the top of the bounding box.")
      .value("Bottom", BoundingBoxLabelPosition::Bottom,
             "At the bottom of the bounding box.")
      .value("Left", BoundingBoxLabelPosition::Left,
             "Along the left edge of the bounding box, from bottom to top.")
      .value("LeftT2B", BoundingBoxLabelPosition::LeftT2B,
             "Along the left edge of the bounding box, from top to bottom.")
      .value("Right", BoundingBoxLabelPosition::Right,
             "Along the right edge of the bounding box, from top to bottom.")
      .value("RightB2T", BoundingBoxLabelPosition::RightB2T,
             "Along the right edge of the bounding box, from bottom to top.");

  m.def("label_position", BoundingBoxLabelPositionFromString,
        "TODO doc",
        py::arg("position_string"));
}


void RegisterTextStyle(py::module &m) {
  std::string doc = "How text should be rendered.";
  py::class_<TextStyle>text_style(m, "TextStyle", doc.c_str());

  //FIXME
  doc = "Initialize from `tuple`:\n"
         "Get the default line style via: ()\n"
         "Only width & color:  (line_width, color)"
         "Full configuration:  (line_width, color, dash_pattern,\n"
         "                      line_cap, line_join)\n"
         "With data types:\n"
         ":line_width:    float\n"
         ":color:         " + FullyQualifiedType("Color") + "\n"
         ":dash_pattern:  list[float]\n"
         ":line_cap:      " + FullyQualifiedType("LineCap") + "\n"
         ":line_join:     " + FullyQualifiedType("LineJoin");
  text_style.def(py::init<>(&TextStyleFromTuple), "TODO doc");

  //FIXME
  doc = "Customize your text style:\n"
        ":line_width:    float\n"
        ":color:         " + FullyQualifiedType("Color") + "\n"
        ":dash_pattern:  list[float]\n"
        ":line_cap:      " + FullyQualifiedType("LineCap") + "\n"
        ":line_join:     " + FullyQualifiedType("LineJoin");
  text_style.def(py::init<unsigned int, const std::string &,
                          const Color &, bool, bool, double,
                          HorizontalAlignment>(),
                 "TODO doc",
                 py::arg("font_size"),
                 py::arg("font_family"),
                 py::arg("color") = Color::Black,
                 py::arg("bold") = false,
                 py::arg("italic") = false,
                 py::arg("line_spacing") = 1.2,
                 py::arg("alignment") = HorizontalAlignment::Left);
//  line_style.def(py::init<double, Color, std::vector<double>,
//                          LineCap, LineJoin>(),
//           doc.c_str(),
//           py::arg("line_width"),
//           py::arg("color"),
//           py::arg("dash_pattern") = std::vector<double>(),
//           py::arg("line_cap") = LineCap::Butt,
//           py::arg("line_join") = LineJoin::Miter);

  text_style.def(py::init<>(), "Creates a default, library-wide preset text style.")
      .def("copy", [](const TextStyle &st) { return TextStyle(st); },
           "Returns a deep copy.")
      .def("__repr__",
           [](const TextStyle &st)
           { return FullyQualifiedType(st.ToString(), true); })
      .def("__str__", &TextStyle::ToString)
      .def(py::pickle(&TextStyleToTuple,
                      &TextStyleFromTuple))
      .def(py::self == py::self)
      .def(py::self != py::self)
      .def("is_valid", &TextStyle::IsValid,
           "Check if the style allows rendering text.")
      .def_readwrite("font_size", &TextStyle::font_size,
           "Font size in pixels.")
      .def_readwrite("font_family", &TextStyle::font_family,
           "Font family TODO see cairo doc: https://www.cairographics.org/manual/cairo-text.html#cairo-select-font-face")
      .def_readwrite("bold", &TextStyle::bold,
                     "TODO")
      .def_readwrite("italic", &TextStyle::italic,
                     "TODO")
      .def_readwrite("line_spacing", &TextStyle::line_spacing,
                     "TODO");

  doc = "Text color as :class:`" + FullyQualifiedType("Color") + "`.";
  text_style.def_readwrite("color", &TextStyle::color,
                           doc.c_str());

  doc = "Text alignment as :class:`" + FullyQualifiedType("HorizontalAlignment") + "`.";
  text_style.def_readwrite("alignment", &TextStyle::alignment,
                           doc.c_str());

  // A TextStyle can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, TextStyle>();
}

} // namespace bindings
} // namespace viren2d

