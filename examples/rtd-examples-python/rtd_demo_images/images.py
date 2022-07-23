import viren2d
import numpy as np
from pathlib import Path


def demo_image_anchors():
    # Colorize peaks, and draw them at different reference points with
    # the available anchors
    painter = viren2d.Painter()
    canvas_width = 440
    canvas_height = 440
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

    line_style = viren2d.LineStyle(
        width=3, dash_pattern=[10, 10], color=(0.4, 0.4, 0.4))

    marker_style = viren2d.MarkerStyle(
        viren2d.Marker.RotatedReticle, size=14, thickness=3, filled=True,
        color='rose-red!90', bg_border=2, bg_color='ivory!80')
    
    # Draw the grid
    offset_x = 20
    offset_y = 20
    delta_x = 200
    delta_y = 200
    for i in range(3):
        x = offset_x + i * delta_x
        y = offset_y + i * delta_y
        painter.draw_line((5, y), (canvas_width - 5, y), line_style)
        painter.draw_line((x, 5), (x, canvas_height - 5), line_style)
    
    # Prepare the image to be overlaid
    peaks = viren2d.peaks()
    vis = viren2d.colorize_scaled(
        data=peaks, colormap='gouldian', low=-6.5, high=8, bins=8)
    scaling = 100 / peaks.width

    x = offset_x
    y = offset_y
    for anchor in viren2d.Anchor.list_all():
        pos = (x, y)
        if anchor != viren2d.Anchor.Center:
            painter.draw_marker(pos, marker_style)

        painter.draw_image(
            image=vis, position=pos, anchor=anchor,
            scale_x=scaling, scale_y=scaling, rotation=0,
            clip_factor=0.2, alpha=1.0)

        x += delta_x
        if x >= canvas_width:
            x = offset_x
            y += delta_y

    return np.array(painter.canvas, copy=True)


def demo_image_overlay():
    # FIXME change this to something actually useful for the rtd doc! --> could overlay optical flow...
    painter = viren2d.Painter()

    img_buf = viren2d.load_image_uint8(
        str(Path(__file__).absolute().parents[2] / 'data' / 'ninja.jpg'))

    painter.set_canvas_rgb(width=600, height=400, color='white')
    
    line_style = viren2d.LineStyle(width=8, color='navy-blue!80')

    scaling = painter.canvas.height / img_buf.width
    painter.draw_image(
        image=img_buf, position=(painter.canvas.width / 2, painter.canvas.height / 2),
        anchor='center', scale_x=scaling, scale_y=scaling, rotation=15,
        clip_factor=0.9,
        line_style=line_style)

    return np.array(painter.canvas, copy=True)
