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

    line_style = viren2d.LineStyle(
        width=29, color='navy-blue!80')

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
        family='xkcd', size=18, color=(0.3, 0.3, 0.3))

    line_style = viren2d.LineStyle(
        width=49, color='navy-blue!80')
    
    # Draw polygons with different line join styles:
    poly = [(40, 20), (120, 100), (40, 180)]
    line_style.join = 'miter'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.join)], (80, 210),
        'north', text_style)

    poly = [(p[0] + 120, p[1]) for p in poly]
    line_style.join = 'bevel'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.join)], (200, 210),
        'north', text_style)

    poly = [(p[0] + 120, p[1]) for p in poly]
    line_style.join = 'round'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.join)], (320, 210),
        'north', text_style)

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)


def cheat_sheet_linestyle():
     # Set up empty canvas:
    painter = viren2d.Painter()
    canvas_width = 600
    canvas_height = 600
    painter.set_canvas_rgb(
        width=canvas_width, height=canvas_height, color='white!100')#FIXME

   # Style specifications:
    text_style = viren2d.TextStyle(
        family='xkcd', size=18, color=(0.3, 0.3, 0.3))

    # Draw titles:
    painter.draw_text(
        ['Line Cap:'], (canvas_width / 4, 5),
        'north', text_style)
    
    painter.draw_text(
        ['Line Join:'], (3 * canvas_width / 4, 5),
        'north', text_style)
    
    painter.draw_text(
        ['Dash Patterns:'], (300, 300),
        'north', text_style)

    ### Different LineCap settings
    line_style = viren2d.LineStyle(
        width=29, color='navy-blue!80')

    y = 60
    line_style.cap = 'butt'
    painter.draw_line(
        (40, y), (canvas_width / 2 - 40, y), line_style)
    
    text_style.color = 'white'
    painter.draw_text(
        [str(line_style.cap)], (canvas_width / 4, y),
        'center', text_style)

    y = 100
    line_style.cap = 'round'
    painter.draw_line(
        (40, y), (canvas_width / 2 - 40, y), line_style)

    painter.draw_text(
        [str(line_style.cap)], (canvas_width / 4, y),
        'center', text_style)

    y = 140
    line_style.cap = 'square'
    painter.draw_line(
        (40, y), (canvas_width / 2 - 40, y), line_style)

    painter.draw_text(
        [str(line_style.cap)], (canvas_width / 4, y),
        'center', text_style)

    ### Different LineJoin settings
    line_style.cap = 'round'

    y = 60
    poly = [
        (canvas_width / 2 + 40, y),
        (3 * canvas_width / 4, y),
        (canvas_width - 70, y + 60)]
    line_style.join = 'miter'
    painter.draw_polygon(poly, line_style)

    painter.draw_text(
        [str(line_style.join)], (5.5 * canvas_width / 8, 60),
        'center', text_style)


    line_style.join = 'round'
    # poly = [p + (0, 50) for p in poly]
    # painter.draw_polygon(poly, line_style)

    # painter.draw_text(
    #     [str(line_style.join)], (poly[0] + poly[1]) / 2,
    #     'center', text_style, rotation=-40)

    # Return the visualization as a NumPy buffer (let NumPy take care of
    # the memory copy):
    shared_canvas = painter.get_canvas(copy=False)
    return np.array(shared_canvas, copy=True)