import numpy as np
import viren2d
from vito import imvis, imutils, pyutils as pu
import timeit


WIDTH, HEIGHT = 1024, 768
REPETITIONS = [10, 1000]#, 10000]


def _time_painter_init():
    print('----------------------------')
    print("Timings for painter's canvas")
    print('----------------------------')
    painter = viren2d.Painter()
    # Set up painter once, due to the memory allocation
    pu.tic()
    painter.set_canvas_rgb(WIDTH, HEIGHT)
    res = pu.ttoc()
    print(f'Initial {WIDTH}x{HEIGHT} canvas setup: {res:f} ms')
    for runs in REPETITIONS:
        print(f'* {runs} repetitions')
        res = timeit.timeit(
            lambda: painter.set_canvas_rgb(WIDTH, HEIGHT, 'azure'),
            number=runs)
        print(f'  * from color name:  {res/runs:f} ms')
        
        res = timeit.timeit(
            lambda: painter.set_canvas_rgb(WIDTH, HEIGHT, (0.3, 0.4, 0.5)),
            number=runs)
        print(f'  * from tuple:       {res/runs:f} ms')

        res = timeit.timeit(
            lambda: painter.set_canvas_rgb(WIDTH, HEIGHT, viren2d.Color(0.3, 0.4, 0.5)),
            number=runs)
        print(f'  * from Color:       {res/runs:f} ms')


def _time_primitives():
    print('----------------------------')
    print("Timings for primitives")
    print('----------------------------')
    painter = viren2d.Painter()
    # Set up painter once, due to the memory allocation
    pu.tic()
    painter.set_canvas_rgb(WIDTH, HEIGHT)
    res = pu.ttoc()
    print(f'Initial {WIDTH}x{HEIGHT} canvas setup: {res:f} ms')
    line_style = viren2d.LineStyle()

    print()
    print('Lines')
    print('~~~~~')
    for runs in REPETITIONS:
        print(f'* {runs} repetitions')
        line_style.width = 5
        res = timeit.timeit(
            lambda: painter.draw_line(
                (50, 50), (WIDTH-50, HEIGHT-50), line_style),
            number=runs)
        print(f'  * Odd width:  {res/runs:f} ms, {line_style}')

        line_style.width = 6
        res = timeit.timeit(
            lambda: painter.draw_line(
                (50, 50), (WIDTH-50, HEIGHT-50), line_style),
            number=runs)
        print(f'  * Even width: {res/runs:f} ms, {line_style}')

    print()
    print('Arcs')
    print('~~~~')
    for runs in REPETITIONS:
        print(f'* {runs} repetitions')
        line_style.width = 5
        res = timeit.timeit(
            lambda: painter.draw_arc(
                (WIDTH/2, HEIGHT/2), radius=200,
                angle_from=45, angle_to=330,
                line_style=line_style),
            number=runs)
        print(f'  * Odd line width:  {res/runs:f} ms')

        line_style.width = 6
        res = timeit.timeit(
            lambda: painter.draw_arc(
                (WIDTH/2, HEIGHT/2), radius=200,
                angle_from=45, angle_to=330,
                line_style=line_style),
            number=runs)
        print(f'  * Even line width: {res/runs:f} ms')

        res = timeit.timeit(
            lambda: painter.draw_arc(
                (WIDTH/2, HEIGHT/2), radius=200,
                angle_from=45, angle_to=330,
                line_style=line_style,
                fill_color=(0.2, 0.3, 0.4)),
            number=runs)
        print(f'  * Filled:          {res/runs:f} ms')

    print()
    print('Ellipses')
    print('~~~~~~~~')
    ellipse = viren2d.Ellipse(
        (WIDTH/2, HEIGHT/2), (200, 120),
        angle_from=45, angle_to=330)
    for runs in REPETITIONS:
        print(f'* {runs} repetitions')
        line_style.width = 5
        ellipse.rotation = 0
        res = timeit.timeit(
            lambda: painter.draw_ellipse(
                ellipse, line_style),
            number=runs)
        print(f'  * Odd line width:   {res/runs:f} ms')

        line_style.width = 6
        res = timeit.timeit(
            lambda: painter.draw_ellipse(
                ellipse, line_style),
            number=runs)
        print(f'  * Even line width:  {res/runs:f} ms')

        res = timeit.timeit(
            lambda: painter.draw_ellipse(
                ellipse, line_style, (0.4, 0.5, 0.6)),
            number=runs)
        print(f'  * Filled:           {res/runs:f} ms')

        ellipse.rotation = 42
        res = timeit.timeit(
            lambda: painter.draw_ellipse(
                ellipse, line_style, (0.4, 0.5, 0.6)),
            number=runs)
        print(f'  * Filled & rotated: {res/runs:f} ms')

    print()
    print('Rectangles')
    print('~~~~~~~~~~')
    rect = viren2d.Rect(
        (WIDTH/2, HEIGHT/2), (200, 120))
    line_style.width = 5
    for runs in REPETITIONS:
        print(f'* {runs} repetitions')
        rect.rotation = 0
        rect.radius = 0
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style),
            number=runs)
        print(f'  * Square/axis-aligned: {res/runs:f} ms')

        rect.rotation = 42
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style),
            number=runs)
        print(f'  * Square/rotate:       {res/runs:f} ms')

        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style, (0.4, 0.5, 0.6)),
            number=runs)
        print(f'  * Square/round/fill:   {res/runs:f} ms')

        rect.rotation = 0
        rect.radius = 0.3
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style),
            number=runs)
        print(f'  * Round/axis-aligned:  {res/runs:f} ms')

        rect.rotation = 0
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style, (0.4, 0.5, 0.6)),
            number=runs)
        print(f'  * Round/axis/fill:     {res/runs:f} ms')

        rect.rotation = 42
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style),
            number=runs)
        print(f'  * Round/rotate:        {res/runs:f} ms')


        rect.rotation = 42
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style, (0.4, 0.5, 0.6)),
            number=runs)
        print(f'  * Round/rotate/fill:   {res/runs:f} ms')


def compute_timings():
#    img = imutils.imread('ninja.jpg', mode='RGB')
    _time_painter_init()
    print()
    _time_primitives()
    #TODO time trajectory, bounding box, poses, etc


if __name__ == '__main__':
    compute_timings()

