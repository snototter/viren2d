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
  // FIXME(snototter) update once we text style properties are decided
  return py::make_tuple(obj.font_size, obj.font_family,
                        ColorToTuple(obj.font_color),
                        obj.font_bold, obj.font_italic);
}


TextStyle TextStyleFromTuple(py::tuple tpl) {
  // FIXME(snototter) update once we text style properties are decided

  // Convert empty tuple to pre-defined default style
  if(tpl.empty()) {
    return TextStyle();
  }

  if (tpl.size() > 5) {
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
    style.font_color = ColorFromTuple(tpl[2]);
  }

  if (tpl.size() > 3) {
    style.font_bold = tpl[3].cast<bool>();
  }

  if (tpl.size() > 4) {
    style.font_italic = tpl[4].cast<bool>();
  }

  return style;
}

void RegisterAnchors(py::module &m) {
  py::enum_<HorizontalAlignment>(m, "HorizontalAlignment",
             "Specifies horizontal alignment.")
      .value("Left", HorizontalAlignment::Left)
      .value("Center", HorizontalAlignment::Center)
      .value("Right", HorizontalAlignment::Right);

  std::string doc = "Parses a string into a :class:`"
      + FullyQualifiedType("HorizontalAlignment") + "`.";
  m.def("halign",
        static_cast<HorizontalAlignment (*)(const std::string &)>(&HorizontalAlignmentFromString),
        doc.c_str(),
        py::arg("alignment"));


  py::enum_<VerticalAlignment>(m, "VerticalAlignment",
             "Specifies vertical alignment.")
      .value("Top", VerticalAlignment::Top)
      .value("Center", VerticalAlignment::Center)
      .value("Bottom", VerticalAlignment::Bottom);

  doc = "Parses a string into a :class:`"
        + FullyQualifiedType("VerticalAlignment") + "`.";
  m.def("valign",
        static_cast<VerticalAlignment (*)(const std::string &)>(&VerticalAlignmentFromString),
        doc.c_str(),
        py::arg("alignment"));

  py::enum_<TextAnchor>(m, "TextAnchor",
             "TODO doc")
      .value("Center", TextAnchor::Center,
             "Aligns text BOTH horizontally & vertically CENTERED.")
      .value("Left", TextAnchor::Left,
             "Aligns text horizontally LEFT & vertically CENTERED.")
      .value("Right", TextAnchor::Right,
             "Aligns text horizontally RIGHT & vertically CENTERED.")
      .value("Top", TextAnchor::Top,
             "Aligns text horizontally CENTERED & vertically TOP-ALIGNED.")
      .value("Bottom", TextAnchor::Bottom,
             "Aligns text horizontally CENTERED & vertically BOTTOM-ALIGNED.")
      .value("TopLeft", TextAnchor::TopLeft,
             "Aligns text horizontally LEFT & vertically TOP-ALIGNED.")
      .value("TopRight", TextAnchor::TopRight,
             "Aligns text horizontally RIGHT & vertically TOP-ALIGNED.")
      .value("BottomLeft", TextAnchor::BottomLeft,
             "Aligns text horizontally LEFT & vertically BOTTOM-ALIGNED.")
      .value("BottomRight", TextAnchor::BottomRight,
             "Aligns text horizontally RIGHT & vertically BOTTOM-ALIGNED.");

  doc = "Parses a string into a :class:`"
        + FullyQualifiedType("TextAnchor") + "`.\n\n"
        "Valid string representations include standard\n"
        "position specifications (*e.g.* ``center``, ``top``, ``bottom-left``, *etc.*)\n"
        "and the 8 compass directions (*e.g.*, ``north``, ``south-west``).\n\n"
        "Before parsing, any whitespaces, dashes & underscores\n"
        "will be removed and the case will be ignored.";
  m.def("text_anchor",
        static_cast<TextAnchor (*)(const std::string &)>(&TextAnchorFromString),
        doc.c_str(), py::arg("anchor_string"));


  py::enum_<BoundingBoxLabelPosition>(m, "BoundingBoxLabelPosition",
             "Where to place a bounding box label.")
      .value("Top", BoundingBoxLabelPosition::Top,
             "At the top of the bounding box.")
      .value("Bottom", BoundingBoxLabelPosition::Bottom,
             "At the bottom of the bounding box.")
      .value("Left", BoundingBoxLabelPosition::Left,
             "Along the left edge of the bounding box, from\n"
             "bottom to top.")
      .value("LeftT2B", BoundingBoxLabelPosition::LeftT2B,
             "Along the left edge of the bounding box, from\n"
             "top to bottom.")
      .value("Right", BoundingBoxLabelPosition::Right,
             "Along the right edge of the bounding box, from\n"
             "top to bottom.")
      .value("RightB2T", BoundingBoxLabelPosition::RightB2T,
             "Along the right edge of the bounding box, from\n"
             "bottom to top.");

  m.def("label_position",
        static_cast<BoundingBoxLabelPosition (*)(const std::string &)>(&BoundingBoxLabelPositionFromString),
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
//  text_style.def(py::init<>(&moddef::LineStyleFromTuple), doc.c_str());

  //FIXME
  doc = "Customize your text style:\n"
        ":line_width:    float\n"
        ":color:         " + FullyQualifiedType("Color") + "\n"
        ":dash_pattern:  list[float]\n"
        ":line_cap:      " + FullyQualifiedType("LineCap") + "\n"
        ":line_join:     " + FullyQualifiedType("LineJoin");
//  line_style.def(py::init<double, Color, std::vector<double>,
//                          LineCap, LineJoin>(),
//           doc.c_str(),
//           py::arg("line_width"),
//           py::arg("color"),
//           py::arg("dash_pattern") = std::vector<double>(),
//           py::arg("line_cap") = LineCap::Butt,
//           py::arg("line_join") = LineJoin::Miter);

  text_style.def(py::init<>(), "Creates a default, library-wide preset text style.\n"
           "Note that this default style is NOT the same as the\n"
           "one you can set per painter. For that style,\n"
           "see `Painter.set_default_text_style()`.")
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
           "TODO doc.")
      .def_readwrite("font_family", &TextStyle::font_family,
           "TODO doc");

  //TODO add other fields
  doc = "Text color as " + FullyQualifiedType("Color") + ".";
  text_style.def_readwrite("font_color", &TextStyle::font_color,
           doc.c_str());

  // A TextStyle can be initialized from a given tuple.
  py::implicitly_convertible<py::tuple, TextStyle>();
}

} // namespace bindings
} // namespace viren2d

