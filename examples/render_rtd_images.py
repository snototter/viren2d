"""This script renders all images included in the RTD documentation."""
import numpy as np
from pathlib import Path
from vito import imvis, imutils

from rtd_demo_images import *


def _process_result(
        img: np.ndarray,
        show_image: bool,
        title: str,
        save_image: bool,
        filename: str) -> None:
    if show_image:
        imvis.imshow(img, title, wait_ms=100)
    if save_image:
        output_folder = Path(__file__).absolute().parents[1] / 'docs' / 'source' / 'images'
        imutils.imsave(output_folder / filename, img)


def render_rtd_demos(show_images: bool, save_images: bool):
    # Arrows
    img = demo_arrows()
    _process_result(
        img, show_images, 'Arrows', save_images, 'arrows.png')

    # Color names
    img = demo_color_names()
    _process_result(
        img, show_images, 'Color Names', save_images, 'color-names.png')

    # Line cap
    img = demo_line_cap()
    _process_result(
        img, show_images, 'Line Cap', save_images, 'line-cap.png')

    # Line join
    img = demo_line_join()
    _process_result(
        img, show_images, 'Line Join', save_images, 'line-join.png')

    # Markers/Keypoints
    img = demo_markers()
    _process_result(
        img, show_images, 'Markers', save_images, 'markers.png')

    # Text anchors
    img = demo_text_anchors()
    _process_result(
        img, show_images, 'Text anchors', save_images, 'text-anchors.png')

    # Multi-line text, horizontal alignment
    img = demo_multiline_text()
    _process_result(
        img, show_images, 'Multi-line Text', save_images, 'text-multi-line.png')

    # Ellipse
    img = demo_ellipse()
    _process_result(
        img, show_images, 'Ellipse', save_images, 'ellipse.png')


if __name__ == '__main__':
    render_rtd_demos(True, True)

