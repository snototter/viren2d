#include <pybind11/pybind11.h>

#include <viren2d/viren2d.h>

#include <bindings/binding_helpers.h>

#include <bindings/werkzeugkiste-bindings/vector.h>
#include <bindings/werkzeugkiste-bindings/line2d.h>


//------------------------------------------------- Module definition
PYBIND11_MODULE(viren2d_PYMODULE_NAME, m) {
  m.doc() = R"doc(
    Visualize Computer Vision Results Neatly in 2D
    ----------------------------------------------

    This toolbox simplifies visualization of common 2D computer
    vision results, such as detections, trajectories, and the like.

    Example:

    >>> # Create a painter for visualization
    >>> painter = viren2d.Painter()
    >>> painter.set_canvas_rgb(300, 400, 'white')

    >>> # Set up style specifications
    >>> line_style = viren2d.LineStyle(...)
    >>> text_style = viren2d.TextStyle(...)

    >>> # Draw what you fancy
    >>> painter.draw_bounding_box(...)
    >>> painter.draw_arrow(...)

    >>> # Retrieve the visualization as numpy array
    >>> img_np = np.array(painter.get_canvas())
    )doc";

  // Pay attention to the order of registration!
  // Ensure that enums and other classes the
  // "to-be-registered" one uses/depends upon
  // are registered first.

  //------------------------------------------------- Color
  // Color maps are needed by color, thus we need to register the
  // enum before to avoid `ImportError` upon loading the module:
  viren2d::bindings::RegisterColorMapEnum(m);
  viren2d::bindings::RegisterColor(m);

  //------------------------------------------------- Primitives
  // Bind
  werkzeugkiste::bindings::RegisterVector<double, 2>(m);
  werkzeugkiste::bindings::RegisterVector<double, 3>(m);
  werkzeugkiste::bindings::RegisterVector<int32_t, 2>(m);
  werkzeugkiste::bindings::RegisterVector<int32_t, 3>(m);

  werkzeugkiste::bindings::RegisterLine2d(m);

  viren2d::bindings::RegisterEllipse(m);
  viren2d::bindings::RegisterRectangle(m);

  //------------------------------------------------- Drawing - Styles
  viren2d::bindings::RegisterLineCap(m);
  viren2d::bindings::RegisterLineJoin(m);
  viren2d::bindings::RegisterMarker(m);

  viren2d::bindings::RegisterMarkerStyle(m);
  viren2d::bindings::RegisterLineStyle(m);
  viren2d::bindings::RegisterArrowStyle(m);

  viren2d::bindings::RegisterAnchors(m);
  viren2d::bindings::RegisterTextStyle(m);

  viren2d::bindings::RegisterBoundingBox2DStyle(m);

  //------------------------------------------------- Drawing - ImageBuffer
  viren2d::bindings::RegisterImageBuffer(m);

  //------------------------------------------------- Drawing - Painter
  viren2d::bindings::RegisterPainter(m);

  //------------------------------------------------- Visualization - Collage
  // Requires painter & ImageBuffer to be defined previously
  viren2d::bindings::RegisterCollage(m);

  //------------------------------------------------- Visualization - Colormaps
  viren2d::bindings::RegisterColormaps(m);

  //------------------------------------------------- Visualization - Color gradients
  viren2d::bindings::RegisterColorGradients(m);

  //------------------------------------------------- Visualization - Optical Flow
  viren2d::bindings::RegisterOpticalFlowUtils(m);

//  //TODO(snototter) copy python documentation to cpp

//  //------------------------------------------------- Logging
  //TODO **Corresponding C++ API:** ``SetLogLevel``; explain usage (compile with flag & setloglevel in code!!
  //TODO remove library init when switching to spdlog v2?
//  m.def("set_log_level", static_cast<bool (*)(const std::string &)>(&viren2d::SetLogLevel),
//        "TODO", pybind11::arg("level"));
//  viren2d::SetLogLevel(viren2d::LogLevel::Trace);

#ifdef viren2d_VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(viren2d_VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
