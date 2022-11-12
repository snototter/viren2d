#ifndef __VIREN2D_BINDING_HELPERS_H__
#define __VIREN2D_BINDING_HELPERS_H__

#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <viren2d/colors.h>
#include <viren2d/colormaps.h>
#include <viren2d/primitives.h>
#include <viren2d/imagebuffer.h>


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


namespace viren2d {
/// Utilities to simplify the module definition.
namespace bindings {
//-------------------------------------------------  Common (Binding utils)

/**
 * Returns the fully qualified type name string: "module"."name",
 * optionally enclosed in pointy brackets.
 */
std::string FullyQualifiedType(const std::string &name, bool with_tags = false);


//-------------------------------------------------  Common (Color definition)
void RegisterColor(pybind11::module &m);
Color ColorFromTuple(const pybind11::tuple &tpl);
pybind11::tuple ColorToTuple(const Color &obj);


//-------------------------------------------------  Primitives
void RegisterEllipse(pybind11::module &m);
void RegisterRectangle(pybind11::module &m);
void RegisterLine2d(pybind11::module &m);


//------------------------------------------------- ImageBuffer
ImageBuffer CreateImageBufferUint8C4(const pybind11::array &buf);


//-------------------------------------------------  Styles (MarkerStyle & LineStyle)
// Enums must be registered before using them in the
// class definitions.
void RegisterLineCap(pybind11::module &m);
void RegisterLineJoin(pybind11::module &m);
void RegisterMarker(pybind11::module &m);

void RegisterMarkerStyle(pybind11::module &m);
void RegisterLineStyle(pybind11::module &m);

//-------------------------------------------------  Styles (ArrowStyle)
void RegisterArrowStyle(pybind11::module &m);

//-------------------------------------------------  Styles (TextStyle)

void RegisterAnchors(pybind11::module &m);
void RegisterTextStyle(pybind11::module &m);

//-------------------------------------------------  Styles (BoundingBox2DStyle)
void RegisterBoundingBox2DStyle(pybind11::module &m);

//-------------------------------------------------  Vectors
void RegisterVectors(pybind11::module &m);

//-------------------------------------------------  Painter
std::string PathStringFromPyObject(const pybind11::object &path);
void RegisterImageBuffer(pybind11::module &m);
void RegisterPainter(pybind11::module &m);


//-------------------------------------------------  Color gradients
void RegisterColorGradients(pybind11::module &m);

//-------------------------------------------------  Colormaps
void RegisterColormaps(pybind11::module &m);
void RegisterColorMapEnum(pybind11::module &m);
ColorMap ColorMapFromPyObject(const pybind11::object &o);

//-------------------------------------------------  Optical Flow
void RegisterOpticalFlowUtils(pybind11::module &m);

} // namespace bindings
} // namespace viren2d

#endif // __VIREN2D_BINDING_HELPERS_H__

