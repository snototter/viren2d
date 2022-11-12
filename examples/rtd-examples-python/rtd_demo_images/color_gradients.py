import numpy as np
import viren2d


def demo_color_gradients():
    height, width = 525, 600
    painter = viren2d.Painter(height, width, 'white!0')

    rtd_background_color = viren2d.RGBa(26, 28, 29)
    rtd_text_color = viren2d.RGBa(192, 186, 177)
    line_style = viren2d.LineStyle(2.0, 'silver!80')
    text_style = viren2d.TextStyle(
        family='xkcd', size=18, color=rtd_text_color)

    def _place_label(y, txt):
        painter.draw_text_box(
            [txt], position=(0, y), anchor='topleft',
            text_style=text_style, line_style=viren2d.LineStyle.Invalid,
            fill_color=rtd_background_color, padding=(10, 5), radius=0.3)

    _place_label(5, 'Linear Color Gradients:')
    offset_x = 15
    top = 50

    # 45° slanted gradient
    lin1 = viren2d.LinearColorGradient((0, 0), (width, width))
    offsets = np.linspace(0, 1, 20)
    for i in range(len(offsets)):
        lin1.add_color_stop(
            offsets[i], 'black' if (i % 2) == 0 else 'crimson')

    vis = viren2d.color_gradient_visualization(lin1, width - 2 * offset_x, 70)
    painter.draw_image(
        vis, position=(offset_x, top), anchor='top-left', clip_factor=0.1,
        line_style=line_style)
    top += 90

    # High-frequency vertical lines
    lin2 = viren2d.LinearColorGradient((offset_x, 0), (width - offset_x, 0))
    offsets = np.linspace(0, 1, 41)
    for i in range(len(offsets)):
        lin2.add_color_stop(
            offsets[i], 'black' if (i % 2) == 0 else 'teal-green')

    vis = viren2d.color_gradient_visualization(lin2, width - 2 * offset_x, 70)
    painter.draw_image(
        vis, position=(offset_x, top), anchor='top-left', clip_factor=0.1,
        line_style=line_style)
    top += 90
    
    # Basic horiztontal gradient
    lin3 = viren2d.LinearColorGradient((0, 0), (0, 70))
    lin3.add_color_stop(0.1, 'black')
    lin3.add_color_stop(0.5, 'azure')
    lin3.add_color_stop(0.9, 'black')
    
    vis = viren2d.color_gradient_visualization(lin3, width - 2 * offset_x, 70)
    painter.draw_image(
        vis, position=(offset_x, top), anchor='top-left', clip_factor=0.1,
        line_style=line_style)
    top += 100

    # Radial gradient + clipping to create the impression of a 3D shape
    _place_label(top, 'Radial Color Gradients:')
    top += 45
    radius = 70
    cy = top + radius
    cx = width / 3
    rad1 = viren2d.RadialColorGradient(
        (cx + 30, cy + 30), 10, (cx + 30, cy + 30), radius + 10)
    rad1.add_color_stop(0, 'white')
    rad1.add_color_stop(1, (.6, .6, .6))
    painter.set_clip_circle((cx, cy), radius)
    painter.draw_gradient(rad1)
    painter.reset_clip()

    # Radial gradient which also fades to alpha = 0
    cx = 2 * width / 3
    rad2 = viren2d.RadialColorGradient((cx, cy), radius / 3, (cx, cy), radius)
    rad2.add_color_stop(0.1, 'crimson')
    rad2.add_color_stop(1, 'black!0')
    painter.set_clip_circle((cx, cy), radius)
    painter.draw_gradient(rad2)
    painter.reset_clip()
    painter.draw_circle((cx, cy), radius, line_style=line_style)

    return np.array(painter.canvas, copy=True)
