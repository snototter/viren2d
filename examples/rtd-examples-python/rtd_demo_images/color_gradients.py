import numpy as np
import viren2d

def _demo_linear_gradients():
    height, width = 270, 400
    painter = viren2d.Painter(height, width, 'white!0')
    line_style = viren2d.LineStyle(2.0, 'silver!80')

    # 45° slanted gradient
    lin1 = viren2d.LinearColorGradient((0, 0), (width, width))
    offsets = np.linspace(0, 1, 20)
    for i in range(len(offsets)):
        lin1.add_color_stop(
            offsets[i], 'black' if (i % 2) == 0 else 'crimson')

    offset_x = 10
    vis = lin1.visualization(width - 2 * offset_x, 70)
    top = 10
    painter.draw_image(
        vis, position=(offset_x, top), anchor='top-left', clip_factor=0.1,
        line_style=line_style)
    top += 90

    # High-frequency vertical lines
    lin2 = viren2d.LinearColorGradient((0, 0), (width - 2 * offset_x, 0))
    offsets = np.linspace(0, 1, 41)
    for i in range(len(offsets)):
        lin2.add_color_stop(
            offsets[i], 'black' if (i % 2) == 0 else 'teal-green')
    
    vis = lin2.visualization(width - 2 * offset_x, 70)
    painter.draw_image(
        vis, position=(offset_x, top), anchor='top-left', clip_factor=0.1,
        line_style=line_style)
    top += 90

    # Basic horiztontal gradient
    lin3 = viren2d.LinearColorGradient((0, 0), (0, 70))
    lin3.add_color_stop(0.1, 'black')
    lin3.add_color_stop(0.5, 'azure')
    lin3.add_color_stop(0.9, 'black')

    vis = lin3.visualization(width - 2 * offset_x, 70)
    painter.draw_image(
        vis, position=(offset_x, top), anchor='top-left', clip_factor=0.1,
        line_style=line_style)

    return np.array(painter.canvas, copy=True)


def _demo_radial_gradients():
    height, width = 160, 340
    painter = viren2d.Painter(height, width, 'white!0')
    line_style = viren2d.LineStyle(2.0, 'silver!80')

    # Radial gradient + canvas clipping to create the impression of a 3D shape
    offset_x = 10
    radius = 70
    cy = 10 + radius
    cx = offset_x + radius
    rad1 = viren2d.RadialColorGradient(
        (cx + 30, cy + 30), 10, (cx + 30, cy + 30), radius + 10)
    rad1.add_color_stop(0, 'white')
    rad1.add_color_stop(1, (.6, .6, .6))
    painter.set_clip_circle((cx, cy), radius)
    painter.draw_gradient(rad1)
    painter.reset_clip()

    # Radial gradient which fades to alpha = 0
    cx = width - offset_x - radius
    rad2 = viren2d.RadialColorGradient((cx, cy), radius / 3, (cx, cy), radius)
    rad2.add_color_stop(0.1, 'crimson')
    rad2.add_color_stop(1, 'black!0')
    painter.set_clip_circle((cx, cy), radius)
    painter.draw_gradient(rad2)
    painter.reset_clip()
    painter.draw_circle((cx, cy), radius, line_style=line_style)

    return np.array(painter.canvas, copy=True)



def demo_color_gradients():
    return [
        ('linear', _demo_linear_gradients()),
        ('radial', _demo_radial_gradients())
    ]