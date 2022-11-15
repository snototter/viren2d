#ifndef __VIREN2D_COLLAGE_H__
#define __VIREN2D_COLLAGE_H__

#include <vector>

#include <viren2d/colors.h>
#include <viren2d/imagebuffer.h>
#include <viren2d/positioning.h>
#include <viren2d/primitives.h>


namespace viren2d {

/// Creates a collage.
///
/// Positions the given images in a grid and renders them onto
/// the output ImageBuffer. The jagged input parameter ``images``
/// defines their arrangement, for example:
///
///   >>> images = [[img1, img2]]
///   Results in 1 row, 2 columns.
///
///   >>> images = [[img1, img2, img3], [None, img4], [None, None, img5]]
///   Results in 3 rows, 3 columns:
///   ' img1  img2  img3 '
///   '       img4       '
///   '             img5 '
///
/// The maximum size of all images in a row/column define the
/// corresponding height/width.
/// This can be constrained by specifying a fixed ``size`` **for each
/// image**. Optionally, only one dimension can be fixed.
/// In this case, the other image dimension will be adjusted according
/// to its aspect ratio. For example:
///
///   >>> size = (-1, -1)
///   Each image will be placed as-is.
///
///   >>> size = (200, -1)
///   Each image will be 200 pixels wide.
///
///   >>> size = (-1, 400)
///   Each image will be 400 pixels tall.
///
/// Warning:
///   TODO Runtime measurements are still missing. Scaling should be
///   the bottleneck, conversion to uint8c4 will also contribute slightly.
///
/// Args:
///   images: A jagged vector of input images.
///   image_size: Optional fixed size of each image.
///   cell_alignment: Placement of each image within its corresponding cell.
///   fill_color: Background color. Must be a valid :class:`~viren2d.Color`.
///   output_channels: Number of output channels, must be either 3 or 4.
///   spacing: Distance between neighboring columns and rows.
///   margin: Distance between the collage boundary and the first/last
///     row and column.
///   clip_factor: If greater than 0, the corners of each image will be
///     clipped. In particular, :math:`0 < \text{clip} \leq 0.5` will
///     result in a rounded rectangle, where the corner radius will be
///     ``clip_factor`` times :math:`\min(\text{width}, \text{height})`.
///     If :math:`\text{clip} > 0.5`, the clip region will be an ellipse,
///     where major/minor axis length equal the width/height of the image.
ImageBuffer Collage(const std::vector<std::vector<ImageBuffer>> &images,
    const Vec2i &image_size = {-1, -1},
    Anchor cell_alignment = Anchor::TopLeft,
    const Color &fill_color = Color::White,
    int output_channels = 3,
    const Vec2i &spacing = {0, 0},
    const Vec2i &margin = {0, 0},
    double clip_factor = 0.0);

} // namespace viren2d

#endif // __VIREN2D_COLLAGE_H__
