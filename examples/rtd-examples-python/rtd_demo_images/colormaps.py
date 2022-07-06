import numpy as np
import viren2d
import os


def demo_relief_shading():
    # Prepare the lookup table for the color bar
    row = np.array([[i] * 2 for i in range(256)]).astype(np.uint8).flatten()
    colorbar_data = np.repeat(row.reshape((1,-1)), 25, axis=0)
    
    width = 600
    height = 300
    painter = viren2d.Painter(width=width, height=height, color='white!0')

    text_style = viren2d.TextStyle(family='xkcd', size=21)

    border_style = viren2d.LineStyle(width=3, color='ivory')

    moon = viren2d.load_image_uint8(
        os.path.join(os.path.abspath(os.path.dirname(__file__)), '..', '..', 'data', 'lunar-farside.jpg'))
    
    cmap_names = ['Relief', 'Relief Low Contrast', 'Terrain']

    column_width = (width - (len(cmap_names) * 10)) / len(cmap_names)
    scale = column_width / moon.width

    x = column_width / 2 + 5
    for cmap_name in cmap_names:
        # Show the color bar on top of the shaded relief:
        colorbar = viren2d.colorize(
            data=colorbar_data, colormap=cmap_name, low=0, high=255)

        painter.draw_image(
            colorbar, position=(x, 15), anchor='center',
            clip_factor=0.5, scale_x=column_width/colorbar.width)

        painter.draw_text(
            [cmap_name], position=(x, 15), anchor='center',
            text_style=text_style)

        # Colorize the farside of the moon
        vis = viren2d.colorize(
            data=moon, colormap=cmap_name, low=0, high=255)

        relief = viren2d.relief_shading(moon, vis)

        painter.draw_image(
            relief, position=(x, 105), anchor='top', clip_factor=1,
            scale_x=scale, scale_y=scale)

        # Overlay the raw inputs
        painter.draw_image(
            moon, position=(x - 15, 35), anchor='top-right',
            clip_factor=0.1, scale_x=scale / 3, scale_y=scale / 3,
            line_style=border_style)
        
        painter.draw_image(
            vis, position=(x + 15, 35), anchor='top-left',
            clip_factor=0.1, scale_x=scale / 3, scale_y=scale / 3,
            line_style=border_style)

        # Draw a marker that looks kind of an element-wise multiplication operator:
        pos_multiplier = (x, 35 + moon.width * scale / 6)
        painter.draw_marker(
            pt=pos_multiplier,
            marker_style=viren2d.MarkerStyle('.', size=23, color='ivory'))
        painter.draw_marker(
            pt=pos_multiplier,
            marker_style=viren2d.MarkerStyle('o', size=18, color='black', thickness=2))
        painter.draw_marker(
            pt=pos_multiplier,
            marker_style=viren2d.MarkerStyle('x', size=12, color='black', thickness=2))

        x += (column_width + 10)

    return np.array(painter.canvas, copy=True)


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
