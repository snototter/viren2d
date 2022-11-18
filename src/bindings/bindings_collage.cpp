#include <sstream>
#include <stdexcept>

#include <pybind11/stl.h>

#include <bindings/binding_helpers.h>
#include <viren2d/collage.h>

#include <helpers/logging.h>

namespace py = pybind11;

namespace viren2d {
namespace bindings {
namespace helpers {

/// Extracts the images from an iterable (list or tuple).
/// Images can be numpy.ndarray, ImageBuffer, or None (which is used
/// to skip a cell of the collage grid).
template <typename Container>
std::vector<ImageBuffer> ExtractImageBuffers(
    const Container &img_container,
    std::size_t row_idx) {
  std::vector<ImageBuffer> images;
  for (py::handle img : img_container) {
    if (img.is(py::none())) {
      images.push_back(ImageBuffer());
    } else if (py::isinstance<ImageBuffer>(img)) {
      images.push_back(img.cast<ImageBuffer>());
    } else if (py::isinstance<py::array>(img)) {
      images.push_back(CastToImageBufferUInt8C4(img.cast<py::array>()));
    } else {
      const std::string tp = py::cast<std::string>(
        img.attr("__class__").attr("__name__"));
      std::ostringstream msg;
      msg << "Image input to `collage` at row "
          << row_idx << ", column " << images.size()
          << " must be a `numpy.ndarray` or `viren2d.ImageBuffer`, but got `"
          << tp << "`!";
      SPDLOG_ERROR(msg.str());
      throw std::invalid_argument(msg.str());
    }
  }
  return images;
}


/// Extracts the rows of the collage input
template <typename Container>
std::vector<std::vector<ImageBuffer>> ExtractImageBufferRows(
    const Container rows) {
  std::vector<std::vector<ImageBuffer>> images;
  for (py::handle row : rows) {
    if (py::isinstance<py::list>(row)) {
      images.push_back(
          ExtractImageBuffers<py::list>(
              row.cast<py::list>(),
              images.size()));
    } else if (py::isinstance<py::tuple>(row)) {
      images.push_back(
          ExtractImageBuffers<py::tuple>(
              row.cast<py::tuple>(), images.size()));
    } else {
      const std::string tp = py::cast<std::string>(
        row.attr("__class__").attr("__name__"));
      std::ostringstream msg;
      msg << "Input to `collage` at row " << images.size()
          << " must be a `list` or `tuple`, but got `"
          << tp << "`!";
      SPDLOG_ERROR(msg.str());
      throw std::invalid_argument(msg.str());
    }
  }
  return images;
}

} // namespace helpers


/*
import numpy as np
import viren2d
from vito import imvis, pyutils
red = np.zeros((50, 100, 4), dtype=np.uint8)
red[:, :, 0] = 255
red[:, :, 3] = 180
green = np.zeros((20, 30, 4), dtype=np.uint8)
green[:, :, 1] = 255
green[:, :, 3] = 255
blue = np.zeros((200, 170, 3), dtype=np.uint8)
blue[:, :, 2] = 255
pyutils.tic('clg'); c = viren2d.collage([[red, red, red, green], [None, blue], [None, None, None, red]], img_size=(200, -1), clip_factor=0.0, margin=(10, 5), spacing=(20, 20)); pyutils.toc('clg')
imvis.imshow(np.array(c), wait_ms=-1)

c = viren2d.collage([[red, green], [None, blue], [None, None, None, red]], img_size=(200, -1), clip_factor=0.1, margin=(10, 5), spacing=(20, 20))
c = viren2d.collage([[None, None], [None], [None, None, None]])


x = viren2d.load_image_uint8('demo.png', 4)
pyutils.tic('clg'); y = viren2d.collage([[x],[x]], img_size=(-1, -1), clip_factor=0.2, margin=(10, 5)); pyutils.toc('clg');
imvis.imshow(np.array(y), wait_ms=-1)

viren2d.collage([[np.ones((30, 30, 3)), np.zeros((50, 70, 3))]])
viren2d.collage([[np.ones((30, 30), dtype=np.uint8), np.zeros((50, 70), dtype=np.uint8)]])
viren2d.collage([[np.ones((3, 3)), np.zeros((5, 7))]])
viren2d.collage(['a'])
viren2d.collage([('a',), [1, 2]])
*/

/// Wrapper to support passing both lists and tuples (and to avoid deep copies
/// whenever possible (e.g. if the image data is already provided as u8c4, we
/// can reuse it for cairo as-is).
ImageBuffer CollageWrapper(
    const py::object py_images,
    const Vec2i &image_size,
    const py::object py_anchor,
    const Color &fill_color,
    int output_channels,
    const Vec2i &spacing,
    const Vec2i &margin,
    double clip_factor) {
  Anchor anchor = AnchorFromPyObject(py_anchor);

  std::vector<std::vector<ImageBuffer>> images;
  if (py::isinstance<py::list>(py_images)) {
    images = helpers::ExtractImageBufferRows<py::list>(
          py_images.cast<py::list>());
  } else if (py::isinstance<py::tuple>(py_images)) {
    images = helpers::ExtractImageBufferRows<py::tuple>(
          py_images.cast<py::tuple>());
  } else {
    const std::string tp = py::cast<std::string>(
        py_images.attr("__class__").attr("__name__"));
    std::string msg("Input to `collage` must be a container (list/tuple), but got ");
    msg += tp;
    msg += '!';
    SPDLOG_ERROR(msg);
    throw std::invalid_argument(msg);
  }

  return Collage(
      images, image_size, anchor, fill_color, output_channels,
      spacing, margin, clip_factor);
}


void RegisterCollage(py::module &m) {
  m.def("collage",
        &CollageWrapper, R"docstr(
        Creates a collage.

        Positions the given images in a grid and renders them onto
        an output :class:`~viren2d.ImageBuffer`. The jagged input
        parameter ``images`` defines their arrangement, for example:

          >>> images = [[img1, img2]]
          Results in 1 row, 2 columns.

          >>> images = [[img1], [img2]]
          Results in 2 rows, 1 column.

          >>> images = [[img1, img2, img3], [None, img4], [None, None, img5]]
          Results in 3 rows, 3 columns:
          ' img1  img2  img3 '
          '       img4       '
          '             img5 '

        The maximum size of all images in a row/column define the
        corresponding height/width.
        Optionally, the row/column size can be constrained by specifying
        a fixed ``size`` **for each image**, by either providing both, height
        and width, or only one fixed dimension. In the latter case, the
        other image dimension will be adjusted according to its aspect ratio.
        For example:

          >>> size = (-1, -1)
          Each image will be rendered at the original resolution.

          >>> size = (200, -1)
          Each image will be 200 pixels wide.

          >>> size = (-1, 400)
          Each image will be 400 pixels tall.

        Warning:
          TODO Runtime measurements are still missing. Scaling should be
          the bottleneck, conversion to uint8c4 will also contribute slightly.

        Example:
          >>> vis = viren2d.collage(
          >>>     [[img1, img2, img3], [None, img4], [img5]],
          >>>     size=(-1, -1), anchor='center', fill_color='white',
          >>>     channels=3, spacing=(5, 5), margin=(0, 0), clip_factor=0.0)

        Args:
          images: A jagged :class:`list` (or :class:`tuple`) of input images.
            Each image can either be an :class:`~viren2d.ImageBuffer`, a
            :class:`numpy.ndarray`, or :class:`None` (to skip a cell).
            Images will be converted to :class:`numpy.uint8` before rendering.
            For this, images of type :class:`numpy.float32` and :class:`numpy.float64`
            will be multiplied by `255`, whereas all other types are currently just
            cast/truncated.
          size: Optional fixed size of each image.
          anchor: Placement of each image within its corresponding cell.
          fill_color: Background color. Must be a valid :class:`~viren2d.Color`.
          channels: Number of output channels, must be either 3 or 4.
          spacing: Distance between neighboring columns and rows as
            :class:`viren2d.Vec2i`.
          margin: Distance between the collage boundary and the first/last
            row and column as :class:`viren2d.Vec2i`.
          clip_factor: If greater than 0, the corners of each image will be
            clipped. In particular, :math:`0 < \text{clip} \leq 0.5` will
            result in a rounded rectangle, where the corner radius will be
            ``clip_factor`` times :math:`\min(\text{width}, \text{height})`.
            If :math:`\text{clip} > 0.5`, the clip region will be an ellipse,
            where major/minor axis length equal the width/height of the image.

        Returns:
          A 3- or 4-channel :class:`~viren2d.ImageBuffer`. If there was an
          error, the output will be invalid (check via
          :meth:`~viren2d.ImageBuffer.is_valid`) and a corresponding error
          message will be logged.
        )docstr",
        py::arg("images"),
        py::arg("size") = Vec2i{-1, -1},
        py::arg("anchor") = Anchor::TopLeft,
        py::arg("fill_color") = Color::White,
        py::arg("channels") = 3,
        py::arg("spacing") = Vec2i{0, 0},
        py::arg("margin") = Vec2i{0, 0},
        py::arg("clip_factor") = 0.0);
}
} // namespace bindings
} // namespace viren2d

