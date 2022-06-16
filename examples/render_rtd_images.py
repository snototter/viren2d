"""This script renders all images included in the RTD documentation."""
import numpy as np
from pathlib import Path
from vito import imvis, imutils

from rtd_demo_images.markers import demo_markers


def render_rtd_demos(show_images: bool, save_images: bool):
    output_folder = Path(__file__).absolute().parents[1] / 'docs' / 'source' / 'images'
    # Markers/Keypoints
    img = demo_markers()
    if show_images:
        imvis.imshow(img, 'Marker Demo', wait_ms=100)
    imutils.imsave(output_folder / 'markers.png', img)


if __name__ == '__main__':
    render_rtd_demos(False, True)

