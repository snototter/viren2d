#include <stdexcept>
#include <sstream>
#include <cmath>
#include <tuple>

// public viren2d headers
#include <viren2d/collage.h>
#include <viren2d/drawing.h>

// private viren2d headers
#include <helpers/logging.h>


namespace viren2d {
namespace helpers {

/// Returns the position of the image anchor within the given cell.
inline Vec2d ComputeImageAnchor(
    const Vec2d &top_left, int cell_width, int cell_height,
    HorizontalAlignment halign, VerticalAlignment valign) {
  Vec2d pos(top_left);
  switch (halign) {
    case HorizontalAlignment::Left:
      break;

    case HorizontalAlignment::Center:
      pos.x() += cell_width / 2.0;
      break;

    case HorizontalAlignment::Right:
      pos.x() += cell_width;
      break;
  }

  switch(valign) {
    case VerticalAlignment::Top:
      break;

    case VerticalAlignment::Center:
      pos.y() += cell_height / 2.0;
      break;

    case VerticalAlignment::Bottom:
      pos.y() += cell_height;
      break;
  }

  return pos;
}


/// Computes the target/output image size.
inline std::tuple<Vec2i, Vec2d> ComputeImageSize(
    const ImageBuffer &img, const Vec2i &requested_size) {
  if (!img.IsValid()) {
    return std::make_tuple(Vec2i(0, 0), Vec2d(1.0, 1.0));
  }

  int dst_width = img.Width();
  int dst_height = img.Height();
  double scale_x = 1.0;
  double scale_y = 1.0;

  if (requested_size.width() > 0) {
    // A fixed width has been requested.
    dst_width = requested_size.width();
    scale_x = dst_width / static_cast<double>(img.Width());

    if (requested_size.height() > 0) {
      // Height is also fixed.
      dst_height = requested_size.height();
      scale_y = dst_height / static_cast<double>(img.Height());
    } else {
      // Compute output height using the same scaling factor.
      scale_y = scale_x;
      dst_height = static_cast<int>(scale_y * img.Height());
    }
  } else {
    // We only need to adjust the output size if a fixed height has been
    // requested. Otherwise, we just keep the default variable settings.
    if (requested_size.height() > 0) {
      dst_height = requested_size.height();
      scale_y = dst_height / static_cast<double>(img.Height());

      // Compute output width using the same scaling factor.
      scale_x = scale_y;
      dst_width = static_cast<int>(scale_x * img.Width());
    }
  }

  return std::make_tuple(
        Vec2i(dst_width, dst_height), Vec2d(scale_x, scale_y));
}
} // namespace helpers


ImageBuffer Collage(
    const std::vector<std::vector<ImageBuffer>> &images,
    const Vec2i &image_size,
    Anchor cell_alignment,
    const Color &fill_color,
    int output_channels,
    const Vec2i &spacing,
    const Vec2i &margin,
    double clip_factor) {
  SPDLOG_DEBUG(
      "Collage: {} rows, img_sz={:s}, bg={:s}, alignment={:s}, channels={}, "
      "spacing={:s}, margin={:s}, clip_factor={:.2f}",
      images.size(), image_size, fill_color, alignment, output_channels,
      spacing, margin, clip_factor);

  if ((output_channels < 3) || (output_channels > 4)) {
    std::ostringstream msg;
    msg << "Invalid number of output channels requested ("
        << output_channels << "), only 3 or 4 are supported!";
    throw std::invalid_argument(msg.str());
  }

  // First pass:
  // * Compute scaling factors for each image.
  // * Compute size of each row/column.
  // * Per-image information is stored in a flattened vector.
  std::vector<int> row_heights;
  std::vector<int> column_widths;
  std::vector<Vec2d> image_scales;
  for (std::size_t row = 0; row < images.size(); ++row) {
    int max_img_height = 0;
    int column_width = 0;
    for (std::size_t col = 0; col < images[row].size(); ++col) {
      if ((col > 0) && images[row][col].IsValid()) {
        column_width += spacing.x();
      }

      Vec2i size;
      Vec2d scale;
      std::tie(size, scale) = helpers::ComputeImageSize(
            images[row][col], image_size);

      image_scales.push_back(scale);

      column_width += size.width();

      // Adjust row and column height
      if (size.height() > max_img_height) {
        max_img_height = size.height();
      }

      if (col < column_widths.size()) {
        if (size.width() > column_widths[col]) {
          column_widths[col] = size.width();
        }
      } else {
        column_widths.push_back(size.width());
      }
    }

    row_heights.push_back(max_img_height);
  }

  // Second pass:
  // * Compute the position of the image anchor points within the cells.
  // * Compute the canvas size in the 2nd pass, because some images could
  //   have been 'invalid' to request skipping the corresponding cells.
  const auto halign = HorizontalAlignmentFromAnchor(cell_alignment);
  const auto valign = VerticalAlignmentFromAnchor(cell_alignment);
  int canvas_height = 0;
  std::vector<Vec2d> anchor_points;
  Vec2d top_left(margin);
  std::size_t idx_flat = 0;
  for (std::size_t row = 0; row < images.size(); ++row) {
    // The left-most cell at each row starts margin.x pixels from the
    // left border.
    top_left.x() = margin.x();
    for (std::size_t col = 0; col < images[row].size(); ++col) {
      anchor_points.push_back(helpers::ComputeImageAnchor(
          top_left, column_widths[col], row_heights[row],
          halign, valign));

      ++idx_flat;
      // If a column is empty, we ignore it. Thus, we need to skip the
      // spacing (or we'd end up with twice the spacing between columns).
      if (column_widths[col] > 0) {
        top_left.x() += column_widths[col] + spacing.width();
      }
    }

    // If a row is completely empty, we also ignore it.
    if ((row > 0) && (row_heights[row] > 0)) {
      canvas_height += spacing.height();
      top_left.y() += spacing.height();
    }
    canvas_height += row_heights[row];
    top_left.y() += row_heights[row];
  }

  // Compute the required canvas width.
  int canvas_width = 0;
  for (std::size_t col = 0; col < column_widths.size(); ++col) {
    if ((col > 0) && (column_widths[col] > 0)) {
      canvas_width += spacing.width();
    }
    canvas_width += column_widths[col];
  }

  // Sanity check.
  if ((canvas_width <= 0) || (canvas_height <= 0)) {
    std::ostringstream msg;
    msg << "Cannot create collage from given inputs, canvas size would be w="
        << canvas_width << ", h=" << canvas_height << '!';
    SPDLOG_WARN(msg.str());
    return ImageBuffer();
  }

  // Reuse the painter to draw the images.
  canvas_height += 2 * margin.height();
  canvas_width += 2 * margin.width();
  auto painter = CreatePainter();
  painter->SetCanvas(canvas_height, canvas_width, fill_color);

  idx_flat = 0;
  for (std::size_t row = 0; row < images.size(); ++row) {
    for (std::size_t col = 0; col < images[row].size(); ++col) {
      if (images[row][col].IsValid()) {
        painter->DrawImage(
              images[row][col], anchor_points[idx_flat], cell_alignment, 1.0,
              image_scales[idx_flat].x(), image_scales[idx_flat].y(), 0.0,
              clip_factor, LineStyle::Invalid);
      }
      ++idx_flat;
    }
  }

  return painter->GetCanvas(false).ToChannels(output_channels);
}

} // namespace viren2d
