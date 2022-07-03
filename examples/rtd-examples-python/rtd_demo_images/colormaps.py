import numpy as np
import viren2d


def demo_colormaps():
    width = 400
    height = 200
    painter = viren2d.Painter(width=width, height=height, color='white!0')

    peaks = viren2d.peaks()

    vis1 = viren2d.colorize(
        data=peaks, colormap='turbo', low=-6.5, high=8, bins=256)
    vis2 = viren2d.colorize(
        data=peaks, colormap='turbo', low=-6.5, high=8, bins=8)

    scaling = 0.47 * width / peaks.width

    line_style = viren2d.LineStyle(width=8, color='navy-blue!80')

    painter.draw_image(
        image=vis1, position=(6, height / 2), anchor='left',
        scale_x=scaling, scale_y=scaling, rotation=0, clip_factor=0.1,
        line_style=line_style)
    painter.draw_image(
        image=vis2, position=(width - 6, height / 2), anchor='right',
        scale_x=scaling, scale_y=scaling, rotation=0, clip_factor=0.1,
        line_style=line_style)

    return np.array(painter.canvas, copy=True)
