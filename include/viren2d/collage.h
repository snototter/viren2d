#ifndef __VIREN2D_COLLAGE_H__
#define __VIREN2D_COLLAGE_H__

#include <vector>

#include <viren2d/colors.h>
#include <viren2d/imagebuffer.h>
#include <viren2d/positioning.h>
#include <viren2d/primitives.h>


namespace viren2d {

ImageBuffer Collage(
    const std::vector<std::vector<ImageBuffer>> &images,
    const Vec2i &image_size = {-1, -1},
    HorizontalAlignment halign = HorizontalAlignment::Left,
    VerticalAlignment valign = VerticalAlignment::Top,
    const Color &bg_color = Color::White,
    const Vec2i &spacing = {0, 0},
    const Vec2i &margin = {0, 0});

} // namespace viren2d

#endif // __VIREN2D_COLLAGE_H__
