import viren2d
import numpy as np
from rtd_demo_images.constants import VIREN2D_DATA_PATH


def _calibrated_example():
    """Example image (`ninja`) and its calibration (estimated via PnP)."""
    img = viren2d.load_image_uint8(VIREN2D_DATA_PATH / 'ninja.jpg')
    K = np.array(
        [[523.17808219,   0.0,        341.0],
         [  0.0,        523.17808219, 256.0],
         [  0.0,          0.0,          1.0]], dtype=np.float64)
    R = np.array(
        [[ 0.99013141,  0.14006482, -0.00465153],
         [ 0.05439048, -0.41465762, -0.90835056],
         [-0.12915675,  0.89913342, -0.41818372]], dtype=np.float64)
    t = np.array([-51.84341161, 17.32680283, 82.51435241], dtype=np.float64)
    return img, K, R, t


def _overlay_axes(painter, K, R, t):
    """Draws the coordinate system axes."""
    arrow_style = viren2d.ArrowStyle(
        width=7, tip_length=35, tip_angle=25)
    # Instead of (0, 0, 0), we draw the axis origin at (12, 12, 0) to avoid
    # clipping the z-axis.
    # The size of a checkerboard grid cell is 24x24 millimeters - the length
    # of each axis will be 2 grid cells.
    painter.draw_xyz_axes(
        K=K, R=R, t=t, origin=(12, 12, 0),
        lengths=(48, 48, 48), arrow_style=arrow_style)


def _overlay_horizon(painter, K, R, t):
    """Draws the approximated line of horizon."""
    line_style = viren2d.LineStyle(
        width=7, color=viren2d.axis_color('z'), dash_pattern=[40, 50],
        cap='round')
    horizon = painter.draw_horizon_line(K=K, R=R, t=t, line_style=line_style)
    if horizon.is_valid():
        # Add a label
        text_style = viren2d.TextStyle(family='xkcd', size=29, color='#1a1c1d')
        line_style.width = 3
        line_style.dash_pattern = []
        painter.draw_text_box(
            ['Line of Horizon'],
            position=horizon.point_at_offset(0.8) + (0, 10),
            anchor='top', text_style=text_style, padding=(10, 6),
            line_style=line_style, fill_color='#c0bab1c0')


def demo_pinhole():
    """Visualizations for calibrated pinhole cameras"""
    img, K, R, t = _calibrated_example()
    painter1 = viren2d.Painter(img)

    # Visualize origin/world reference frame:  #TODO WIP - needs 3d line clipping
    _overlay_axes(painter1, K, R, t)

    # Visualize line of horizon
    _overlay_horizon(painter1, K, R, t)

    # TODO Visualize ground plane

    # Create the collage:
    canvas_width = 600
    canvas_height = 255
    padding = 5
    scale = (canvas_width / 2 - 2 * padding) / img.shape[1]
    painter_comb = viren2d.Painter(
        canvas_height, canvas_width, 'white!0')
    
    text_style = viren2d.TextStyle(family='xkcd', size=21, color='#c0bab1')
    line_style = viren2d.LineStyle(width=2, color=text_style.color)
    bg_color = '#1a1c1dc8'
    
    painter_comb.draw_image(
        painter1.canvas,  position=(padding, padding), anchor='top-left',
        scale_x=scale, scale_y=scale, clip_factor=0.1)
    painter_comb.draw_text_box(
        ['Axes & Horizon (TBD)'],  # TODO update text once finished
        position=(canvas_width / 4, canvas_height - 2 * padding),
        anchor='bottom', fill_color=bg_color, text_style=text_style,
        line_style=line_style, padding=(10, 6))

    # TODO Replace by ground plane visualization
    dimmed = (0.2 * np.array(img)).astype(np.uint8)
    painter_comb.draw_image(
        dimmed,  position=(canvas_width - padding, padding),
        anchor='top-right', scale_x=scale, scale_y=scale, clip_factor=0.1)
    painter_comb.draw_text_box(
        ['Ground plane (TBD)'],  # TODO update text once finished
        position=(3 * canvas_width / 4, canvas_height - 2 * padding),
        anchor='bottom', fill_color=bg_color, text_style=text_style,
        line_style=line_style, padding=(10, 6))

    return np.array(painter_comb.canvas, copy=True)
