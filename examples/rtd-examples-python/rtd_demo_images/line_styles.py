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
        family='xkcd', size=18, color='white')

    line_style = viren2d.LineStyle(width=29, color='azure!80')
    highlight_style = viren2d.LineStyle(width=3, color='salmon!80')

    y = 20
    line_style.cap = 'butt'
    painter.draw_line(
        (30, y), (canvas_width - 30, y), line_style)
    
    painter.draw_text(
        [str(line_style.cap)], (canvas_width / 2, y),
        'center', text_style)

    y = canvas_height / 2
    line_style.cap = 'round'
    painter.draw_line(
        (30, y), (canvas_width - 30, y), line_style)

    painter.draw_text(
        [str(line_style.cap)], (canvas_width / 2, y),
        'center', text_style)

    y = canvas_height - 20
    line_style.cap = 'square'
    painter.draw_line(
        (30, y), (canvas_width - 30, y), line_style)

    painter.draw_text(
        [str(line_style.cap)], (canvas_width / 2, y),
        'center', text_style)

    painter.draw_line((30, 0), (30, canvas_height), highlight_style)
    painter.draw_line(
        (canvas_width - 30, 0), (canvas_width - 30, canvas_height),
        highlight_style)
    
    return np.array(painter.canvas, copy=True)


def demo_line_join():
    # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 400
    canvas_height = 200
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!0')

    # Style specifications:
    text_style = viren2d.TextStyle(
        family='xkcd', size=18, color='white')

    line_style = viren2d.LineStyle(
        width=49, color='azure!80')
    
    # Draw polygons with different line join styles:
    poly = [(40, 20), (120, 100), (40, 180)]
    line_style.join = 'miter'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.join)], (80, 140),
        'center', text_style, rotation=-45)

    poly = [(p[0] + 120, p[1]) for p in poly]
    line_style.join = 'bevel'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.join)], (200, 140),
        'center', text_style, rotation=-45)

    poly = [(p[0] + 120, p[1]) for p in poly]
    line_style.join = 'round'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.join)], (320, 140),
        'center', text_style, rotation=-45)

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)
