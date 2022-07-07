import numpy as np
import viren2d


def demo_ellipses():
    # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 600
    canvas_height = 220
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color=viren2d.RGBa(26, 28, 29))

    # Style specifications:
    text_style = viren2d.TextStyle(
        family='xkcd', size=23, line_spacing=0.8, alignment='center')

    line_style = viren2d.LineStyle(
        width=5, color='forest-green')
    
    fill_color = 'ivory!80'

    # Top-left ellipse:
    ellipse = viren2d.Ellipse(
        center=(100, 60), axes=(180, 50), rotation=0)
    painter.draw_ellipse(ellipse, line_style, fill_color)
    
    painter.draw_text(['0°'], ellipse.center, 'center', text_style)
    
    # Top ellipse:
    ellipse.cx = 300
    ellipse.rotation = 30
    painter.draw_ellipse(ellipse, line_style, fill_color)

    painter.draw_text(
        ['30°'], ellipse.center, 'center', text_style,
        rotation=ellipse.rotation)

    # Top-right ellipse:
    ellipse.cx = 500
    ellipse.angle_from = 45
    ellipse.angle_to = 315
    painter.draw_ellipse(ellipse, line_style)
    text_style.color = 'ivory'

    painter.draw_text(
        ['30°'], ellipse.center, 'right', text_style,
        padding=(10, 0), rotation=ellipse.rotation)

    # Bottom-left ellipse:
    line_style.dash_pattern = [20, 10]
    ellipse.center = (100, 175)
    painter.draw_ellipse(ellipse, line_style, fill_color)

    # Bottom ellipse:
    ellipse.cx = 300
    painter.draw_ellipse(ellipse, viren2d.LineStyle.Invalid, fill_color)

    # Bottom-right ellipse:
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
    painter.set_canvas_rgb(height=200, width=400, color='white!0')

    # Style specification:
    arrow_style = viren2d.ArrowStyle(
        width=7, color='forest-green', cap=viren2d.LineCap.Round)

    painter.draw_arrow((10, 190), (100, 10), arrow_style)

    arrow_style.color = 'rose-red'
    arrow_style.tip_closed = True
    arrow_style.tip_angle = 10
    painter.draw_arrow(pt2=(80, 190), pt1=(180, 10), arrow_style=arrow_style)

    arrow_style.color = 'azure'
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


def demo_rectangles():
    # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 600
    canvas_height = 220
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color=viren2d.RGBa(26, 28, 29))

    # Style specification:
    line_style = viren2d.LineStyle(
        width=5, color='forest-green')

    fill_color = 'ivory!80'

    # Square rectangles (left):
    rect = viren2d.Rect(center=(40, 110), size=(60, 180))
    painter.draw_rect(rect, line_style)

    rect.cx += 80
    painter.draw_rect(rect, line_style, fill_color)

    # Rounded rectangles (center):
    rect.cx = 250
    rect.radius = 0.3
    painter.draw_rect(rect, line_style)

    rect.cx += 80
    painter.draw_rect(rect, line_style, fill_color)

    # Dashed & rotated rectangles (right):
    line_style.dash_pattern = [20, 10]
    rect.cx = 460
    rect.rotation = 10
    painter.draw_rect(rect, line_style)

    rect.cx += 80
    painter.draw_rect(rect, line_style, fill_color)

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)
