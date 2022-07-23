"""This script renders all images included in the RTD documentation."""
import numpy as np
from vito import imvis, imutils

from rtd_demo_images import *


def _process_result(
        img: np.ndarray,
        show_image: bool,
        title: str,  # currently ignored
        save_image: bool,
        filename: str) -> None:
    if img is None:
        return
    if show_image:
        imvis.imshow(img, wait_ms=-1) # ignore the title
    if save_image:
        output_folder = VIREN2D_ROOT_PATH / 'docs' / 'source' / 'images'
        imutils.imsave(output_folder / filename, img)


def render_rtd_demos(show_images: bool, save_images: bool):
    """
    Code of demos is sufficiently concise & will be shown
    on RTD.
    """
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

    # Text anchors
    img = demo_text_anchors()
    _process_result(
        img, show_images, 'Text anchors', save_images, 'text-anchors.png')

    # Multi-line text, horizontal & vertical alignment
    img = demo_multiline_text()
    _process_result(
        img, show_images, 'Multi-line Text', save_images, 'text-multi-line.png')

    # Ellipse
    img = demo_ellipses()
    _process_result(
        img, show_images, 'Ellipse', save_images, 'ellipses.png')

    # Rectangle
    img = demo_rectangles()
    _process_result(
        img, show_images, 'Rectangles', save_images, 'rectangles.png')
    
    # Trajectory & Bounding Boxes
    img = demo_tracking_by_detection()
    _process_result(
        img, show_images, 'tracking-by-detection', save_images, 'tracking-by-detection.png')
    
    # Image overlay
    img = demo_image_overlay()
    _process_result(
        img, show_images, 'image-overlay', save_images, 'image-overlay.png')
    
    # Image anchors
    img = demo_image_anchors()
    _process_result(
        img, show_images, 'image-anchors', save_images, 'image-anchors.png')
    
    # Color map demo - Peaks
    img = demo_colormaps()
    _process_result(
        img, show_images, 'colormaps', save_images, 'colormap-peaks.png')
    
     # Color map demo - label visualizations
    img = demo_colorize_labels()
    _process_result(
        img, show_images, 'colorize-labels', save_images, 'colorize-labels.png')

    # Color map demo - Relief shading
    img = demo_relief_shading()
    _process_result(
        img, show_images, 'relief-shading', save_images, 'relief-shading.png')
    
    # Optical flow
    img = demo_optical_flow()
    _process_result(
        img, show_images, 'Optical Flow', save_images, 'optical-flow.png')

    # Pinhole camera calibration results
    img = demo_pinhole()
    _process_result(
        img, show_images, 'Pinhole Camera', save_images, 'pinhole-camera.png')


def render_rtd_cheatsheets(show_images: bool, save_images: bool):
    """
    Cheat sheets provide a visual reference for the most important
    settings of a style.
    """
    # LineStyle
    img = cheat_sheet_linestyle()
    _process_result(
        img, show_images, 'Line Style Cheat Sheet', save_images, 'line-style-cheat-sheet.png')

    # LineStyle
    img = cheat_sheet_arrowstyle()
    _process_result(
        img, show_images, 'Arrow Style Cheat Sheet', save_images, 'arrow-style-cheat-sheet.png')

    # Colormaps
    sheets = cheat_sheet_colormaps()
    for cat, img in sheets:
        _process_result(
            img, show_images, f'Colormaps Cheat Sheet {cat}', save_images, f'colormaps-cheat-sheet-{cat}.png')
    
    # Markers/Keypoints
    img = cheat_sheet_markers()
    _process_result(
        img, show_images, 'Marker Cheat Sheet', save_images, 'marker-cheat-sheet.png')


if __name__ == '__main__':
    render_rtd_demos(True, True)
    render_rtd_cheatsheets(True, True)
    # img = demo_pinhole()
    # _process_result(
    #     img, True, 'Pinhole Camera', True, 'pinhole-camera.png')
