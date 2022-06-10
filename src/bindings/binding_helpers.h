#ifndef __VIREN2D_BINDING_HELPERS_H__
#define __VIREN2D_BINDING_HELPERS_H__

#include <string>

#include <pybind11/pybind11.h>

#include <viren2d/colors.h>
#include <viren2d/primitives.h>


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)


namespace viren2d {
/** @brief Utilities to simplify module definition. */
namespace bindings {
//-------------------------------------------------  Common (Binding utils)

/**
 * Returns the fully qualified type name string: "module"."name",
 * optionally enclosed in pointy brackets.
 */
std::string FullyQualifiedType(const std::string &name, bool with_tags = false);


//-------------------------------------------------  Common (Color definition)
void RegisterColor(pybind11::module &m);
Color ColorFromTuple(pybind11::tuple tpl);
pybind11::tuple ColorToTuple(const Color &obj);


//-------------------------------------------------  Primitives (Ellipse)
void RegisterEllipse(pybind11::module &m);
//Ellipse EllipseFromTupleOrList(pybind11::object object);
//Ellipse EllipseFromTuple(pybind11::tuple tpl);
//pybind11::tuple EllipseToTuple(const Ellipse &obj);


//-------------------------------------------------  Primitives (Rect)
void RegisterRectangle(pybind11::module &m);
//Rect RectFromTuple(pybind11::tuple tpl);
//Rect RectFromTupleOrList(pybind11::object object);
//pybind11::tuple RectToTuple(const Rect &obj);


//-------------------------------------------------  Styles (LineStyle)
void RegisterLineCap(pybind11::module &m);
void RegisterLineJoin(pybind11::module &m);
void RegisterLineStyle(pybind11::module &m);

//-------------------------------------------------  Styles (ArrowStyle)
void RegisterArrowStyle(pybind11::module &m);

//-------------------------------------------------  Styles (TextStyle)
//TODOvoid RegisterHorizontalAlignment(pybind11::module &m);
void RegisterAnchors(pybind11::module &m);
void RegisterTextStyle(pybind11::module &m);

//-------------------------------------------------  Styles (BoundingBox2DStyle)
void RegisterBoundingBoxLabelPosition(pybind11::module &m);
void RegisterBoundingBox2DStyle(pybind11::module &m);

//-------------------------------------------------  Vectors
void RegisterVectors(pybind11::module &m);

//-------------------------------------------------  Painter
void RegisterImageBuffer(pybind11::module &m);
void RegisterPainter(pybind11::module &m);

} // namespace bindings
} // namespace viren2d

#endif // __VIREN2D_BINDING_HELPERS_H__

