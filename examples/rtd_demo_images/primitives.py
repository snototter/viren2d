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
    
    fill_color = 'carrot'

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


def demo_arrows():
    # Set up empty canvas:
    painter = viren2d.Painter()
    painter.set_canvas_rgb(400, 200, color='white!0')

    # Style specification:
    arrow_style = viren2d.ArrowStyle(
        width=7, color='forest-green', cap=viren2d.LineCap.Round)

    painter.draw_arrow((10, 190), (100, 10), arrow_style)

    arrow_style.color = 'crimson'
    arrow_style.tip_closed = True
    arrow_style.tip_angle = 10
    painter.draw_arrow(pt2=(80, 190), pt1=(180, 10), arrow_style=arrow_style)

    arrow_style.color = 'navy-blue'
    arrow_style.tip_closed = False
    arrow_style.tip_angle = 30
    arrow_style.double_headed = True
    painter.draw_arrow((200, 30), (390, 30), arrow_style)

    arrow_style.dash_pattern = [20, 20]
    painter.draw_arrow((200, 100), (390, 100), arrow_style)

    arrow_style.tip_angle = 15
    arrow_style.tip_closed = True
    painter.draw_arrow((200, 170), (390, 170), arrow_style)

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)
