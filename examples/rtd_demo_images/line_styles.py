import numpy as np
import viren2d

def demo_line_cap():
    # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 300
    canvas_height = 120
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

    # Style specifications:
    text_style = viren2d.TextStyle(
        font_family='xkcd', font_size=18, color='white')

    line_style = viren2d.LineStyle(
        line_width=29, color='navy-blue')

    y = 20
    line_style.line_cap = 'butt'
    painter.draw_line(
        (30, y), (canvas_width - 30, y), line_style)
    
    painter.draw_text(
        [str(line_style.line_cap)], (canvas_width / 2, y),
        'center', text_style)

    y = canvas_height / 2
    line_style.line_cap = 'round'
    painter.draw_line(
        (30, y), (canvas_width - 30, y), line_style)

    painter.draw_text(
        [str(line_style.line_cap)], (canvas_width / 2, y),
        'center', text_style)

    y = canvas_height - 20
    line_style.line_cap = 'square'
    painter.draw_line(
        (30, y), (canvas_width - 30, y), line_style)

    painter.draw_text(
        [str(line_style.line_cap)], (canvas_width / 2, y),
        'center', text_style)

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)


def demo_line_join():
    # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 400
    canvas_height = 240
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

    # Style specifications:
    text_style = viren2d.TextStyle(
        font_family='xkcd', font_size=18, color='black')

    line_style = viren2d.LineStyle(
        line_width=49, color='navy-blue')
    
    # Draw polygons with different line join styles:
    poly = [(40, 20), (120, 100), (40, 180)]
    line_style.line_join = 'miter'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.line_join)], (80, 210),
        'north', text_style)

    poly = [(p[0] + 120, p[1]) for p in poly]
    line_style.line_join = 'bevel'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.line_join)], (200, 210),
        'north', text_style)

    poly = [(p[0] + 120, p[1]) for p in poly]
    line_style.line_join = 'round'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.line_join)], (320, 210),
        'north', text_style)

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)

