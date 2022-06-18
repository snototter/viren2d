import numpy as np
import viren2d


def demo_ellipse():
    # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 600
    canvas_height = 220
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

    # Style specifications:
    text_style = viren2d.TextStyle(family='xkcd', size=23, line_spacing=0.8)
    text_style.alignment = 'center'

    line_style = viren2d.LineStyle(
        width=3, color='midnight-blue')
    
    fill_color = 'carrot!80'

    # Ellipse top-left:
    ellipse = viren2d.Ellipse(
        center=(100, 60), axes=(180, 50), rotation=0)
    painter.draw_ellipse(ellipse, line_style, fill_color)
    
    painter.draw_text(['0°'], ellipse.center, 'center', text_style)
    
    # Ellipse top:
    ellipse.cx = 300
    ellipse.rotation = 30
    painter.draw_ellipse(ellipse, line_style, fill_color)

    painter.draw_text(
        ['30°'], ellipse.center, 'center', text_style,
        rotation=ellipse.rotation)

    # Ellipse top-right:
    ellipse.cx = 500
    ellipse.angle_from = 45
    ellipse.angle_to = 315
    painter.draw_ellipse(ellipse, line_style)

    painter.draw_text(
        ['30°'], ellipse.center, 'right', text_style,
        padding=(10, 0), rotation=ellipse.rotation)

    # Ellipse bottom-left:
    line_style.dash_pattern = [20, 10]
    ellipse.center = (100, 175)
    painter.draw_ellipse(ellipse, line_style, fill_color)

    # Ellipse bottom:
    ellipse.cx = 300
    painter.draw_ellipse(ellipse, viren2d.LineStyle.Invalid, fill_color)

    # Ellipse bottom-right:
    ellipse.include_center = False
    ellipse.cx = 500
    painter.draw_ellipse(ellipse, viren2d.LineStyle.Invalid, fill_color)

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)

