// STL
#include <string>
#include <exception>
#include <tuple>

// Non-STL external
#include <werkzeugkiste/geometry/camera.h>
#include <werkzeugkiste/geometry/projection.h>
namespace wgu = werkzeugkiste::geometry;


#include <helpers/drawing_helpers.h>

namespace viren2d {
namespace helpers {
Line2d DrawHorizonLineImpl(
    cairo_surface_t *surface, cairo_t *context,
    const Matrix3x3d &K, const Matrix3x3d &R, const Vec3d &t,
    const LineStyle &line_style, const Vec2i &img_size) {
  // Surface/context check is done in `DrawLine`
  const Line2d horizon = werkzeugkiste::geometry::GetProjectionOfHorizon(
        K, R, t, img_size);
  DrawLine(surface, context, horizon.From(), horizon.To(), line_style);
  return horizon;
}


bool DrawXYZAxes(
    cairo_surface_t *surface, cairo_t *context,
    const Matrix3x3d &K, const Matrix3x3d &R, const Vec3d &t,
    const Vec3d &origin, const Vec3d &lengths, const ArrowStyle &style,
    const Color &color_x, const Color &color_y, const Color &color_z,
    const Vec2i &img_size, Vec2d &img_origin, Vec2d &img_endpoint_x,
    Vec2d &img_endpoint_y, Vec2d &img_endpoint_z) {
  // Surface/context check is done in `DrawArrow`

  //FIXME just return on invalid inputs (in each DrawXXX call)?
  if (!style.IsValid()) {
    std::string s(
          "Cannot draw x/y/z axes with an invalid arrow style: ");
    s += style.ToDetailedString();
    s += '!';
    throw std::invalid_argument(s);
  }

  wgu::Plane image_plane = wgu::ImagePlaneInWorldCoordinateSystem(R, t);


  //FIXME implement once Line3d is integrated into werkzeugkiste
  // Vec4d image_plane = wgu::ImagePlaneInWorldCoordinateSystem(R, t);
  // const bool is_origin_in_front = wgu::IsPointInFrontOfPlane(origin, image_plane);
  // if(is_origin_in_front) {

  const Matrix3x4d P = wgu::ProjectionMatrixFromKRt(K, R, t);
  Vec3d tip_x = origin + Vec3d(lengths.x(), 0.0, 0.0);
  Vec3d tip_y = origin + Vec3d(0.0, lengths.y(), 0.0);
  Vec3d tip_z = origin + Vec3d(0.0, 0.0, lengths.z());

  const bool is_origin_in_front = image_plane.IsPointInFrontOfPlane(origin);
  if (is_origin_in_front) {
    for (const auto &p : {std::make_pair('x', &tip_x),
         std::make_pair('y', &tip_y), std::make_pair('z', &tip_z)}) {
      if (!image_plane.IsPointInFrontOfPlane(*p.second)) {
//        SPDLOG_INFO(
//              "`DrawXYZAxes`: Adjusting end point of {} axis as it would "
//              "project behind the image plane.", p.first);
        // FIXME implement, once wgu has line3d
//        const auto axis = Line3d(origin, *p.second);
//        Vec3d intersection;
//        if (IntersectionLineSegmentPlane(axis, image_plane, &intersection))
//        {
//          // Replace the invalid end point by the midpoint from origin to the image plane
//          const auto visible_axis = Line3d(origin, intersection);
//          *p.second = visible_axis.MidPoint();
//        }
//        else
//          LOG_FAILURE("Couldn't compute a valid intersection point between " << p.second << " axis and image plane (maybe numerical issue?)");
      }
    }
  }

  // Project points into the image:
  std::tie(
        img_origin, img_endpoint_x, img_endpoint_y,
        img_endpoint_z) = wgu::ProjectToVecs(
        P, origin, tip_x, tip_y, tip_z);

  // Check if any point is visible:
  bool is_any_visible = is_origin_in_front;
  for (const auto &p : {img_endpoint_x, img_endpoint_y, img_endpoint_z}) {
    is_any_visible |= wgu::IsPointInsideImage(p, img_size);
  }

  bool success = true;
  ArrowStyle axis_style(style);
  if (color_x.IsValid()) {
    axis_style.color = color_x;
    success &= DrawArrow(
          surface, context, img_origin, img_endpoint_x, axis_style);
  }

  if (color_y.IsValid()) {
    axis_style.color = color_y;
    success &= DrawArrow(
          surface, context, img_origin, img_endpoint_y, axis_style);
  }

  if (color_z.IsValid()) {
    axis_style.color = color_z;
    success &= DrawArrow(
          surface, context, img_origin, img_endpoint_z, axis_style);
  }
  
  return is_any_visible && success;
}


} // namespace helpers
} // namespace viren2d
