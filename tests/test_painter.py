"""
These tests should ensure that we identify breaking API changes.

Goal is to detect typos, missing bindings, etc.

Instead of using a fixture to set up the canvas, each test_draw_xxx should try
to draw on an invalid canvas first. This is to ensure that calling draw_xxx
before setting up the canvas will raise a RuntimeError
"""

import pytest
import viren2d
import pathlib
import numpy as np


def test_viren2d_version():
    here = pathlib.Path(__file__).parent.parent.resolve()
    expected_version = (here / 'VERSION').read_text().strip()
    assert expected_version == viren2d.__version__
   

def test_painter_basics():
    p = viren2d.Painter()
    assert not p.is_valid()
    
    with pytest.raises(RuntimeError):
        p.get_canvas(False)

    with pytest.raises(RuntimeError):
        p.canvas
    
    assert p.width == 0
    assert p.height == 0

    p.set_canvas_rgb(width=800, height=600)
    assert p.is_valid()
    assert p.canvas.width == 800
    assert p.canvas.height == 600
    assert p.canvas.channels == 4  # the canvas should always be RGBA!
    assert p.canvas.itemsize == 1  # and of type uint8
    assert p.canvas.dtype == np.dtype('uint8')
    assert p.width == 800
    assert p.height == 600

    p.set_canvas_rgb(300, 400)  # parameter order is (H, W)
    assert p.is_valid()

    assert p.canvas.width == 400
    assert p.canvas.height == 300
    assert p.canvas.channels == 4  # the canvas should always be RGBA!
    assert p.width == 400
    assert p.height == 300

    w, h = p.get_canvas_size()
    assert w == 400
    assert h == 300

    img_np = np.ones((800, 700, 3))
    p = viren2d.Painter(img_np)
    assert p.is_valid()
    assert p.width == 700
    assert p.height == 800


def is_valid_line(line_style):
    return line_style.is_valid()


def is_valid_line_or_fill(line_style, fill_color):
    return line_style.is_valid() or fill_color.is_valid()


def color_configurations():
    colors = list()
    colors.append(viren2d.Color())
    colors.append(viren2d.rgba(0.3, 0.1, 0.2))
    colors.append(viren2d.Color('blue!40'))
    return colors


def line_style_configurations():
    # Returns various valid/invalid line style configurations
    styles = list()
    style = viren2d.LineStyle()
    styles.append(style.copy())
    styles.append(viren2d.LineStyle.Invalid)
    for lw in [-2, 0, 0.1, 1, 6]:
        style.width = lw
        for color in color_configurations():
            style.color = color
            for dp in [[], [10], [10, 20], [1, 2, 3]]:
                style.dash_pattern = dp

                styles.append(style.copy())
                # Ensure that the list contains an exact copy
                assert styles[-1] == style
                if len(styles) < 2:
                    continue
                # Ensure that we're not accidentally using references
                assert styles[-1] != styles[-2]
    return styles



def test_draw_arc():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    # Try drawing on invalid painter
    with pytest.raises(RuntimeError):
        p.draw_arc((0, 0), 15, 0, 10)
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    style = viren2d.LineStyle()
    # Draw with explicit types
    p.draw_arc(viren2d.Vec2d(45, 77), 30, 20, 300, style, True, viren2d.rgba(1, 0, 1))

    # Draw with implicit conversions
    p.draw_arc((1, 2), 10, 30, 40, style, False, "midnight-blue!80")
    p.draw_arc(
        angle_from=20, angle_to=300, center=(1, 2), radius=10,
        fill_color='black!40', line_style=style, include_center=False)

    # Sweep valid and invalid configurations
    for center in [(50, 50), (-10, 0.1)]:
        for radius in [-10, 0, 10]:
            for angles in [(-90, -100), (-80, -30), (-10, 10), (0, 300)]:
                for inc_center in [True, False]:
                    for style in line_style_configurations():
                        for fill_color in color_configurations():
                            if radius > 0 and is_valid_line_or_fill(style, fill_color):
                                p.draw_arc(center, radius, angles[0], angles[1], style, inc_center, fill_color)
                            else:
                                with pytest.raises(ValueError):
                                    p.draw_arc(center, radius, angles[0], angles[1], style, inc_center, fill_color)
                            # Painter should always be kept in a valid state
                            assert p.is_valid()


def test_draw_arrow():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    with pytest.raises(RuntimeError):
        p.draw_arrow((0, 0), (50, 50))
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    # Draw with explicit types
    style = viren2d.ArrowStyle()
    p.draw_arrow(viren2d.Vec2d(0, 30), viren2d.Vec2d(70, 80), style)

    # Draw with implicit conversions
    p.draw_arrow((0, 30), (70, 80), style)
    p.draw_arrow(arrow_style=style, pt2=(10, 10), pt1=(50, 30))
    # Sweep valid and invalid configurations
    for pt1 in [(10, 2), (-100, -300), (0.2, 1000), (50, 900)]:
        for pt2 in [(-99, -32), (10, 30), (100, 0.2), (90, 5)]:
            for tip_len in [-2, 0.1, 1, 3, 100]:
                style.tip_length = tip_len
                for tip_angle in [-10, 1, 30, 90, 179, 180, 300]:
                    style.tip_angle = tip_angle
                    for fill in [True, False]:
                        style.tip_closed = fill
                        if style.is_valid():
                            p.draw_arrow(pt1, pt2, style)
                        else:
                            with pytest.raises(ValueError):
                                p.draw_arrow(pt1, pt2, style)
                        # Painter should always be kept in a valid state
                        assert p.is_valid()


def test_draw_circles():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    with pytest.raises(RuntimeError):
        p.draw_circle((10, 10), 5)
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()
    
    # Draw with explicit types
    style = viren2d.LineStyle()
    p.draw_circle(viren2d.Vec2d(0, 30), 7, style, viren2d.Color("black"))

    # Draw with implicit conversions
    p.draw_circle((0, 30), 70, style, "blue!10")
    p.draw_circle(
        radius=70, center=(0, 30), fill_color="blue!10", line_style=style)
  
    # Sweep valid and invalid configurations
    for center in [(10, 20), (0, 0), (-100, -30), (0.2, 3000)]:
        for radius in [-3, 0, 1, 5, 50, 170]:
            for style in line_style_configurations():
                for fill_color in color_configurations():
                    if radius > 0 and is_valid_line_or_fill(style, fill_color):
                        p.draw_circle(center, radius, style, fill_color)
                    else:
                        with pytest.raises(ValueError):
                            p.draw_circle(center, radius, style, fill_color)
                    # Painter should always be kept in a valid state
                    assert p.is_valid()


def test_draw_ellipse():
    ### Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    with pytest.raises(RuntimeError):
        p.draw_ellipse(viren2d.Ellipse((150, 150), (300, 100)))
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()


    # Draw with explicit initialization
    style = viren2d.LineStyle()
    # Ellipse as center, size
    p.draw_ellipse(viren2d.Ellipse((150, 150), (300, 100)), style)
    # The above from kwargs
    p.draw_ellipse(viren2d.Ellipse(axes=(300, 100), center=(150, 150)), style)

    # Implicit ellipse as (cx, cy), (major, minor), rotation, angle_from, angle_to, include_center
    p.draw_ellipse(((150, 180), (300, 100), 90, 45, -45, False), style)
    # The above explicitly from kwargs
    p.draw_ellipse(line_style=style,
        ellipse=viren2d.Ellipse(axes=(300, 200), center=(150, 180),
            rotation=90, angle_from=45, angle_to=-45,
            include_center=False))


def test_draw_grid():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    with pytest.raises(RuntimeError):
        p.draw_grid(20, 20, viren2d.LineStyle())
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()
    
    # Draw with explicit types
    style = viren2d.LineStyle()
    p.draw_grid(20, 20)
    p.draw_grid(20, 20, style)
    p.draw_grid(20, 20, style, viren2d.Vec2d(), viren2d.Vec2d())

    # Draw with explicit types & named parameters
    style = viren2d.LineStyle()
    p.draw_grid(spacing_x=20, spacing_y=20)
    p.draw_grid(spacing_x=20, spacing_y=20, line_style=style)
    p.draw_grid(
        spacing_x=20, spacing_y=20, line_style=style,
        top_left=viren2d.Vec2d(), bottom_right=viren2d.Vec2d())

    # Draw with implicit conversions
    p.draw_grid(20, 20, style, (0, 0), (50, 50))
    # Draw with implicit conversions
    p.draw_grid(
        spacing_x=20, spacing_y=20, line_style=style,
        top_left=(0, 0), bottom_right=(50, 50))
    
    # Sweep valid and invalid configurations
    for tl in [(0, 0), (-10, 3), (100, 20)]:
      for br in [(0, 0), (-20, -70), (300, 30)]:
          for sx in [-2, 0, 10]:
            for sy in [-5, 0, 5]:
              for style in line_style_configurations():
                  if is_valid_line(style) and sx > 0 and sy > 0:
                      p.draw_grid(sx, sy, style, tl, br)
                  else:
                      with pytest.raises(ValueError):
                          p.draw_grid(sx, sy, style, tl, br)
                  # Painter should always be kept in a valid state
                  assert p.is_valid()


def test_draw_line():
    ### Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    with pytest.raises(RuntimeError):
        p.draw_line((0, 0), (50, 50), viren2d.LineStyle())
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    ### Draw with explicit types
    style = viren2d.LineStyle()
    p.draw_line(viren2d.Vec2d(0, 30), viren2d.Vec2d(70, 80), style)
    # Same as above from kwargs
    p.draw_line(pt2=viren2d.Vec2d(70, 80), pt1=viren2d.Vec2d(0, 30), line_style=style)
    p.draw_line(pt2=viren2d.Vec2d(y=80, x=70), pt1=viren2d.Vec2d(x=0, y=30), line_style=style)

    ### Draw with implicit conversions
    p.draw_line((0, 30), (70, 80), style)
    # Using kwargs
    p.draw_line(line_style=style, pt2=(10, 10), pt1=(50, 30))
    p.draw_line(pt2=(10, 10), pt1=(50, 30))

    ### Sweep valid and invalid configurations
    for style in line_style_configurations():
        for pt1 in [(10, 2), (-100, -300), (0.2, 1000), (5000, 90000)]:
            for pt2 in [(-99, -32), (10, 30), (100000, 0.2), (90, 5)]:
                if is_valid_line(style):
                    p.draw_line(pt1, pt2, style)
                else:
                    with pytest.raises(ValueError):
                        p.draw_line(pt1, pt2, style)
                # Painter should always be kept in a valid state
                assert p.is_valid()


def test_draw_marker():
    ### Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    with pytest.raises(RuntimeError):
        p.draw_marker((0, 0))
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    # Draw inside/on the border/outside of the canvas
    p.draw_marker((10, 10))
    p.draw_marker((0, 0))
    p.draw_marker((-100, 20))
    
    # Same with the default style
    style = viren2d.MarkerStyle()
    p.draw_marker((10, 10), style)
    p.draw_marker((0, 0), style)
    p.draw_marker((-100, 20), style)

    # Call draw_marker with kwargs and different parameter order
    p.draw_marker(pt=(0, 0), marker_style=style)
    p.draw_marker(marker_style=style, pt=(-100, 20))
    p.draw_marker(pt=(0, 0))
    with pytest.raises(TypeError): # Typo
        p.draw_marker(position=(0, 0), marker_style=style)
    with pytest.raises(TypeError): # Typo
        p.draw_marker(pt=(0, 0), style=style)

    # Test draw markers
    markers = [
        ((900, 34), 'red'),
        ((300, 10), viren2d.Color.Invalid),
        ((-10, 34), 'carrot!30'),
        ((900, 34), 'blue!1'),
        ((770, 42), (-1, -1, -1)),
        ((100, 20), 'invalid')
    ]
    p.draw_markers(markers)
    p.draw_markers(markers, style)
    # Same with kwargs
    p.draw_markers(markers=markers)
    p.draw_markers(markers=markers, marker_style=style)
    p.draw_markers(marker_style=style, markers=markers)
  

def test_draw_rect():
    ### Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    with pytest.raises(RuntimeError):
        p.draw_rect(((0, 0), (30, 50)))
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()


    ### Draw with explicit initialization
    style = viren2d.LineStyle()
    # Rect as (cx, cx), (w, h)
    p.draw_rect(viren2d.Rect((10, 20), (30, 50)), style)
    
    # Rect as center, size
    p.draw_rect(viren2d.Rect((10, 20), (30, 50)), style)
    # The above from kwargs
    p.draw_rect(viren2d.Rect(center=(10, 20), size=(30, 50)), style)

    # Rect as center, size, angle, radius
    p.draw_rect(viren2d.Rect((10, 20), (30, 50), 30, 0), style)
    # The above from kwargs
    p.draw_rect(viren2d.Rect(center=(10, 20), size=(30, 50), rotation=30, radius=0), style)


    ### Draw with implicit conversions
    # Rect as tuple (cx, cy, w, h)
    p.draw_rect((10, 20, 30, 50), style)
    # Same as above, but with kwargs
    p.draw_rect(line_style=style, rect=(10, 20, 30, 50))
    # Like above, but includes additional fill
    p.draw_rect(rect=(10, 20, 30, 50), line_style=style, fill_color='green')

    # Rect from tuple (cx, cy, w, h, angle)
    p.draw_rect((10, 20, 30, 50, 78), style)
    # Rect from tuple (cx, cy, w, h, angle, radius)
    p.draw_rect((10, 20, 30, 50, 78, 5), style)
    # Rect from tuple (center, size)
    p.draw_rect(((10, 20), (30, 50)), style)
    # Rect from tuple (center, size, angle)
    p.draw_rect(((10, 20), (30, 50), 78), style)
    # Rect from tuple (center, size, angle, radius)
    p.draw_rect(((10, 20), (30, 50), 78, 5), style)

    
    ### Sweep valid and invalid configurations
    # Collect rectangles
    rects = list()
    for center in [(-10, -100), (0, 0), (100, 300)]:
        for size in [(-5, -3), (0, 10), (10, 20), (300, 500)]:
            for angle in [-40, 0, 30]:
                for radius in [-10, 0, 5, 100]:
                    rects.append(viren2d.Rect((center, size, angle, radius)))
    # Try to draw with different style variations
    for rect in rects:
        for style in line_style_configurations():
            for fill_color in color_configurations():
                if rect.is_valid() and is_valid_line_or_fill(style, fill_color):
                    p.draw_rect(rect, style, fill_color)
                else:
                    with pytest.raises(ValueError):
                        p.draw_rect(rect, style, fill_color)


def test_draw_text():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    with pytest.raises(RuntimeError):
        p.draw_text(['test'], (10, 10))
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    p.draw_text(['test'], (10, 10))
    p.draw_text(text=['test'], position=(10, 10))

    text_style = viren2d.TextStyle()
    text_style.size = -3
    with pytest.raises(ValueError):
        p.draw_text(
            text=['test'], position=(50, 100), anchor='north-west',
            text_style=text_style, padding=(77, 3), rotation=13)

    # With/without parameter names    
    text_style.size = 10
    p.draw_text(
        text=['test'], position=(50, 100), anchor='north-west',
        text_style=text_style, padding=(77, 3), rotation=13)

    p.draw_text(
            ['test'], (50, 100), 'south', text_style, (77, 3), 13)


def test_draw_text_box():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    with pytest.raises(RuntimeError):
        p.draw_text_box(['test'], (10, 10))
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    p.draw_text_box(['test'], (10, 10))
    p.draw_text_box(text=['test'], position=(10, 10))

    for border_style in [viren2d.LineStyle.Invalid, viren2d.LineStyle()]:
        for box_size in [(-1, -1), (20, 10), (200, 70)]:
            text_style = viren2d.TextStyle()
            text_style.size = -3
            with pytest.raises(ValueError):
                p.draw_text_box(
                    text=['test'], position=(50, 100), anchor='north-west',
                    text_style=text_style, padding=(77, 3), rotation=90,
                    line_style=border_style, fill_color='blue', radius=0.1,
                    fixed_size=box_size)

            # With/without parameter names    
            text_style.size = 15
            p.draw_text_box(
                    text=['test'], position=(50, 100), anchor='north-west',
                    text_style=text_style, padding=(77, 3), rotation=90,
                    line_style=border_style, fill_color='blue', radius=0.1,
                    fixed_size=box_size)

            p.draw_text_box(
                    ['test'], (50, 100), 'north-west', text_style, (77, 3),
                    90, border_style, 'blue', 0.1, box_size)


def test_draw_trajectory():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    with pytest.raises(RuntimeError):
        p.draw_trajectory([(0, 0), (50, 50)])
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    # Create dummy trajectory (which may partially be
    # outside the image boundaries)
    num_points = 100
    x = 600 * np.random.rand(num_points, 1) - 100
    y = 500 * np.random.rand(num_points, 1) - 100
    pts = [(x[i], y[i]) for i in range(num_points)]

    line_style = viren2d.LineStyle()
    
    ##### Plain trajectory (draws a single path)
    p.draw_trajectory(
        pts, line_style, viren2d.Color.Invalid)
    # Include all parameters explicitly
    p.draw_trajectory(
        pts, line_style, viren2d.Color.Invalid,
        True, 3, viren2d.fade_out_linear)
    # Use named arguments
    p.draw_trajectory(
        trajectory=pts, line_style=line_style,
        fade_out_color=viren2d.Color.Invalid)
    # Include all parameters:
    p.draw_trajectory(
        trajectory=pts, line_style=line_style,
        fade_out_color=viren2d.Color.Invalid,
        tail_first=False, smoothing_window=17,
        fading_factor=viren2d.fade_out_quadratic)

    # Fading out (draws multiple line segments)
    p.draw_trajectory(
        pts, line_style, viren2d.Color.White)
    # Include all parameters explicitly
    p.draw_trajectory(
        pts, line_style, viren2d.Color.Black,
        True, 3, viren2d.fade_out_linear)
    # Use named arguments
    p.draw_trajectory(
        trajectory=pts, line_style=line_style,
        fade_out_color=viren2d.Color.Red)
    # Include all parameters:
    p.draw_trajectory(
        trajectory=pts, line_style=line_style,
        fade_out_color=viren2d.Color.Blue,
        tail_first=False, smoothing_window=17,
        fading_factor=viren2d.fade_out_quadratic)
    
    ### Sweep valid and invalid configurations
    for line_style in line_style_configurations():
        if is_valid_line(line_style):
            p.draw_trajectory(
                trajectory=pts, line_style=line_style)
        else:
            with pytest.raises(ValueError):
                p.draw_trajectory(
                    trajectory=pts, line_style=line_style)
            # Painter should always be kept in a valid state
            assert p.is_valid()
