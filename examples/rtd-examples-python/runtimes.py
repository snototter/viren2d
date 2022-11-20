import numpy as np
import viren2d
from vito import imvis, imutils, pyutils as pu
import timeit


WIDTH, HEIGHT = 1024, 768
REPETITIONS = [10, 100]#, 1000]#, 10000]


def _time_imagebuffer():
    print('-----------------------------')
    print("Timings for ImageBuffer inits")
    print('-----------------------------')

    print('ImageBuffer share/copy numpy array')
    print('~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~')

    width, height = 1024, 768

    for runs in REPETITIONS:
        print(f'* {runs} repetitions')
        for dt in [np.uint8, np.int32, np.float32, np.float64]:
            for channels in [1, 3, 4]:
                data = (255 * np.random.rand(height, width, channels)).astype(dt)

                res = timeit.timeit(
                    lambda: viren2d.ImageBuffer(data, copy=False),
                    number=runs) * 1e3
                print(f' * Share buffer, {dt.__name__:7s} {WIDTH}x{HEIGHT}x{channels}: {res/runs:.3f} ms')

                res = timeit.timeit(
                    lambda: viren2d.ImageBuffer(data, copy=True),
                    number=runs) * 1e3
                print(f' * Copy buffer,  {dt.__name__:7s} {WIDTH}x{HEIGHT}x{channels}: {res/runs:.3f} ms')

    print()
    print('Init Canvas from ImageBuffer/numpy')
    print('~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~')

    painter = viren2d.Painter()
    # Set up painter once, due to the memory allocation
    pu.tic()
    painter.set_canvas_rgb(WIDTH, HEIGHT)
    res = pu.ttoc()
    print(f'Initial {WIDTH}x{HEIGHT} canvas setup: {res:f} ms')
    for runs in REPETITIONS:
        print(f'* {runs} repetitions')
        for channels in [3, 4]:
            data = (255 * np.random.rand(5, 15, channels)).astype(np.uint8)
            res = timeit.timeit(
                lambda: painter.set_canvas_image(viren2d.ImageBuffer(data, copy=False)),
                number=runs) * 1e3
            print(f' * Initialize canvas from shared buffer (explicit), {WIDTH}x{HEIGHT}x{channels}: {res/runs:.3f} ms')

            res = timeit.timeit(
                lambda: painter.set_canvas_image(data),
                number=runs)
            print(f' * Initialize canvas from numpy (implicit), {WIDTH}x{HEIGHT}x{channels}:         {res/runs:.3f} ms')


def _time_color_init():
    print('--------------------------------')
    print("Timings for color initialization")
    print('--------------------------------')

    for runs in REPETITIONS:
        for cstr in ['magenta', 'red!20', 'forest-green!80', '#00ff00', '#ff00aabb']:
            res = timeit.timeit(
                lambda: viren2d.Color(cstr),
                number=runs) * 1e3
            print(f'* From string:  {res/runs:.3f} ms - "{cstr}"')
            
            color = viren2d.Color(cstr)
            ctuple = color.to_rgba()
            res = timeit.timeit(
                lambda: viren2d.Color(ctuple),
                number=runs) * 1e3
            print(f'* From tuple:   {res/runs:.3f} ms - {ctuple}')


def _time_painter_init():
    print('----------------------------')
    print("Timings for painter's canvas")
    print('----------------------------')
    painter = viren2d.Painter()
    # Set up painter once, due to the memory allocation
    pu.tic()
    painter.set_canvas_rgb(WIDTH, HEIGHT)
    res = pu.ttoc()
    print(f'Initial {WIDTH}x{HEIGHT} canvas setup: {res:.3f} ms')
    for runs in REPETITIONS:
        print(f'* {runs} repetitions')
        res = timeit.timeit(
            lambda: painter.set_canvas_rgb(WIDTH, HEIGHT, 'azure'),
            number=runs) * 1e3
        print(f'  * from color name:  {res/runs:.3f} ms')
        
        res = timeit.timeit(
            lambda: painter.set_canvas_rgb(WIDTH, HEIGHT, (0.3, 0.4, 0.5)),
            number=runs) * 1e3
        print(f'  * from tuple:       {res/runs:.3f} ms')

        res = timeit.timeit(
            lambda: painter.set_canvas_rgb(WIDTH, HEIGHT, viren2d.Color(0.3, 0.4, 0.5)),
            number=runs) * 1e3
        print(f'  * from Color:       {res/runs:.3f} ms')


def _time_primitives():
    print('----------------------------')
    print("Timings for primitives")
    print('----------------------------')
    painter = viren2d.Painter()
    # Set up painter once, due to the memory allocation
    pu.tic()
    painter.set_canvas_rgb(WIDTH, HEIGHT)
    res = pu.ttoc()
    print(f'Initial {WIDTH}x{HEIGHT} canvas setup: {res:.3f} ms')
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
            number=runs) * 1e3
        print(f'  * Odd width:   {res/runs:.3f} ms, {line_style}')

        line_style.width = 6
        res = timeit.timeit(
            lambda: painter.draw_line(
                (50, 50), (WIDTH-50, HEIGHT-50), line_style),
            number=runs) * 1e3
        print(f'  * Even width:  {res/runs:.3f} ms, {line_style}')

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
            number=runs) * 1e3
        print(f'  * Odd line width:    {res/runs:.3f} ms')

        line_style.width = 6
        res = timeit.timeit(
            lambda: painter.draw_arc(
                (WIDTH/2, HEIGHT/2), radius=200,
                angle_from=45, angle_to=330,
                line_style=line_style),
            number=runs) * 1e3
        print(f'  * Even line width:   {res/runs:.3f} ms')

        res = timeit.timeit(
            lambda: painter.draw_arc(
                (WIDTH/2, HEIGHT/2), radius=200,
                angle_from=45, angle_to=330,
                line_style=line_style,
                fill_color=(0.2, 0.3, 0.4)),
            number=runs) * 1e3
        print(f'  * Filled:            {res/runs:.3f} ms')

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
            number=runs) * 1e3
        print(f'  * Odd line width:   {res/runs:.3f} ms')

        line_style.width = 6
        res = timeit.timeit(
            lambda: painter.draw_ellipse(
                ellipse, line_style),
            number=runs) * 1e3
        print(f'  * Even line width:  {res/runs:.3f} ms')

        res = timeit.timeit(
            lambda: painter.draw_ellipse(
                ellipse, line_style, (0.4, 0.5, 0.6)),
            number=runs) * 1e3
        print(f'  * Filled:           {res/runs:.3f} ms')

        ellipse.rotation = 42
        res = timeit.timeit(
            lambda: painter.draw_ellipse(
                ellipse, line_style, (0.4, 0.5, 0.6)),
            number=runs) * 1e3
        print(f'  * Filled & rotated: {res/runs:.3f} ms')

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
            number=runs) * 1e3
        print(f'  * Square/axis-aligned: {res/runs:.3f} ms')

        rect.rotation = 42
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style),
            number=runs) * 1e3
        print(f'  * Square/rotate:       {res/runs:.3f} ms')

        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style, (0.4, 0.5, 0.6)),
            number=runs) * 1e3
        print(f'  * Square/round/fill:   {res/runs:.3f} ms')

        rect.rotation = 0
        rect.radius = 0.3
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style),
            number=runs) * 1e3
        print(f'  * Round/axis-aligned:  {res/runs:.3f} ms')

        rect.rotation = 0
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style, (0.4, 0.5, 0.6)),
            number=runs) * 1e3
        print(f'  * Round/axis/fill:     {res/runs:.3f} ms')

        rect.rotation = 42
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style),
            number=runs) * 1e3
        print(f'  * Round/rotate:        {res/runs:.3f} ms')


        rect.rotation = 42
        res = timeit.timeit(
            lambda: painter.draw_rect(
                rect, line_style, (0.4, 0.5, 0.6)),
            number=runs) * 1e3
        print(f'  * Round/rotate/fill:   {res/runs:.3f} ms')


def _time_surveillance():
    print('-----------------------------------------')
    print('Timings for trajectories & bounding boxes')
    print('-----------------------------------------')
    painter = viren2d.Painter()
    # Set up painter once, due to the memory allocation
    pu.tic()
    painter.set_canvas_rgb(WIDTH, HEIGHT)
    res = pu.ttoc()
    print(f'Initial {WIDTH}x{HEIGHT} canvas setup: {res:.3f} ms')

    print()
    print('Trajectory')
    print('~~~~~~~~~~')

    for num_points in [10, 20, 30, 40, 50, 80, 100]:
        print(f'* {num_points} points')
        pu.tic()
        x = WIDTH * np.random.rand(num_points, 1)
        y = HEIGHT * np.random.rand(num_points, 1)
        pts = [(x[i], y[i]) for i in range(num_points)]
        res = pu.ttoc()
        print(f'* Random trajectory creation ({num_points:d} points): {res:.3f} ms')

        line_style = viren2d.LineStyle()
        
        for runs in REPETITIONS:
            print(f'  * {runs} repetitions')
            line_style.width = 5
            res = timeit.timeit(
                lambda: painter.draw_trajectory(
                    pts, line_style, viren2d.Color.Invalid),
                number=runs) * 1e3
            print(f'    * {len(pts)} points, odd line width, no gradient:  {res/runs:.3f} ms')

            line_style.width = 6
            res = timeit.timeit(
                lambda: painter.draw_trajectory(
                    pts, line_style, viren2d.Color.Invalid),
                number=runs) * 1e3
            print(f'    * {len(pts)} points, even line width, no gradient: {res/runs:.3f} ms')

            line_style.width = 5
            res = timeit.timeit(
                lambda: painter.draw_trajectory(
                    pts, line_style, (0.4, 0.4, 0.4, 0.4),
                    fading_factor=viren2d.fade_out_linear),
                number=runs) * 1e3
            print(f'    * {len(pts)} points, linear color gradient:        {res/runs:.3f} ms')

            res = timeit.timeit(
                lambda: painter.draw_trajectory(
                    pts, line_style, (0.4, 0.4, 0.4, 0.4),
                    fading_factor=viren2d.fade_out_quadratic),
                number=runs) * 1e3
            print(f'    * {len(pts)} points, quadratic color gradient:     {res/runs:.3f} ms')

            res = timeit.timeit(
                lambda: painter.draw_trajectory(
                    pts, line_style, (0.4, 0.4, 0.4, 0.4),
                    fading_factor=lambda v: v*v),
                number=runs) * 1e3
            print(f'    * {len(pts)} points, color gradient python lambda: {res/runs:.3f} ms')

    print()
    print('Bounding Box')
    print('~~~~~~~~~~~~')
    bbox_style = viren2d.BoundingBox2DStyle()
    rect = viren2d.Rect(
        (WIDTH/2, HEIGHT/2), (200, 120))
    line_style.width = 5
    for runs in REPETITIONS:
        print(f'* {runs} repetitions')
        bbox_style.box_fill_color = viren2d.Color.Invalid
        bbox_style.text_fill_color = viren2d.Color.Invalid
        rect.rotation = 0
        rect.radius = 0
        res = timeit.timeit(
            lambda: painter.draw_bounding_box_2d(
                rect, bbox_style, label_top=['label']),
            number=runs) * 1e3
        print(f'  * Square/axis-aligned box, single-line label:  {res/runs:.3f} ms')

        res = timeit.timeit(
            lambda: painter.draw_bounding_box_2d(
                rect, bbox_style, label_bottom=['this', 'is my', 'label']),
            number=runs) * 1e3
        print(f'  * Square/axis-aligned box, multi-line label:   {res/runs:.3f} ms')

        rect.rotation = 42
        res = timeit.timeit(
            lambda: painter.draw_bounding_box_2d(
                rect, bbox_style, label_top=['this', 'is another', 'label']),
            number=runs) * 1e3
        print(f'  * Square/rotated box, multi-line label:        {res/runs:.3f} ms')

        rect.radius = 0.2
        res = timeit.timeit(
            lambda: painter.draw_bounding_box_2d(
                rect, bbox_style, label_bottom=['this', 'is my', 'label']),
            number=runs) * 1e3
        print(f'  * Rounded/rotated box, multi-line label:       {res/runs:.3f} ms')

        bbox_style.box_fill_color = viren2d.Color.Same
        res = timeit.timeit(
            lambda: painter.draw_bounding_box_2d(
                rect, bbox_style, label_top=['this', 'is another', 'label']),
            number=runs) * 1e3
        print(f'  * Square/rotated/filled box, multi-line label: {res/runs:.3f} ms')

        bbox_style.box_fill_color = viren2d.Color.Same
        bbox_style.text_fill_color = (0.2, 0.3, 0.4, 0.1)
        res = timeit.timeit(
            lambda: painter.draw_bounding_box_2d(
                rect, bbox_style, label_top=['this', 'is another', 'label']),
            number=runs) * 1e3
        print(f'  * Square/rotated/filled box, multi-line label: {res/runs:.3f} ms (with explicit text fill color)')


def _time_collage():
    print('----------------------------')
    print("Timings for collage")
    print('----------------------------')

    width, height = 1280, 960
    
    for runs in [3, 5]:
        print(f'* {runs} repetitions')
        for dt in [np.uint8, np.float32]:
            for channels in [1, 3, 4]:
                scale = 1 if dt in [np.float32, np.float64] else 255
                data1 = (scale * np.random.rand(height, width, channels)).astype(dt)
                data2 = (scale * np.random.rand(height, width, channels)).astype(dt)
                data3 = (scale * np.random.rand(height, width, channels)).astype(dt)

                images = [[data1, None, data2], [data3, data1, None, data2], [data3, data1]]
                res = timeit.timeit(
                    lambda: viren2d.collage(
                        images, size=(-1, -1), anchor='center', fill_color='white',
                        channels=3, spacing=(20, 20), margin=(7, 7), clip_factor=0),
                    number=runs) * 1e3
                print(f'  * {dt.__name__} {width}x{height}x{channels}, full res:   {res/runs:.3f} ms')

                res = timeit.timeit(
                    lambda: viren2d.collage(
                        images, size=(400, -1), anchor='center', fill_color='white',
                        channels=3, spacing=(20, 20), margin=(7, 7), clip_factor=0),
                    number=runs) * 1e3
                print(f'  * {dt.__name__} {width}x{height}x{channels}, max width=400:   {res/runs:.3f} ms')

                res = timeit.timeit(
                    lambda: viren2d.collage(
                        images, size=(200, 200), anchor='center', fill_color='white',
                        channels=3, spacing=(20, 20), margin=(7, 7), clip_factor=0),
                    number=runs) * 1e3
                print(f'  * {dt.__name__} {width}x{height}x{channels}, fixed 200x200:   {res/runs:.3f} ms')
    

def compute_timings():
    _time_color_init()
    print()
    _time_imagebuffer()
    print()
    _time_painter_init()
    print()
    _time_primitives()
    print()
    _time_surveillance()
    print()
    _time_collage()
    #TODO bounding box, poses, etc


if __name__ == '__main__':
    compute_timings()

