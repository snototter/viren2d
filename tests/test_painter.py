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
    if line_style is None:
        return False
    return line_style.is_valid()


def is_valid_color(color):
    if color is None:
        return False
    return color.is_valid()


def is_valid_line_or_fill(line_style, fill_color):
    return is_valid_line(line_style) or is_valid_color(fill_color)


def color_configurations():
    colors = list()
    colors.append(viren2d.Color())
    colors.append(None)
    colors.append(viren2d.rgba(0.3, 0.1, 0.2))
    colors.append(viren2d.Color('blue!40'))
    return colors


def line_style_configurations():
    # Returns various valid/invalid line style configurations
    styles = list()
    style = viren2d.LineStyle()
    styles.append(style.copy())
    styles.append(viren2d.LineStyle.Invalid)
    styles.append(None)
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
    assert not p.draw_arc((0, 0), 15, 0, 10)
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    style = viren2d.LineStyle()
    # Draw with explicit types
    assert p.draw_arc(
        viren2d.Vec2d(45, 77), 30, 20, 300, style, True, viren2d.rgba(1, 0, 1))

    # Draw with implicit conversions
    assert p.draw_arc((1, 2), 10, 30, 40, style, False, "midnight-blue!80")
    assert p.draw_arc(
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
                                assert p.draw_arc(center, radius, angles[0], angles[1], style, inc_center, fill_color)
                            else:
                                assert not p.draw_arc(center, radius, angles[0], angles[1], style, inc_center, fill_color)
                            # Painter should always be kept in a valid state
                            assert p.is_valid()


def test_draw_arrow():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_arrow((0, 0), (50, 50))
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    # Draw with explicit types
    style = viren2d.ArrowStyle()
    assert p.draw_arrow(viren2d.Vec2d(0, 30), viren2d.Vec2d(70, 80), style)

    # Draw with implicit conversions
    assert p.draw_arrow((0, 30), (70, 80), style)
    assert p.draw_arrow(arrow_style=style, pt2=(10, 10), pt1=(50, 30))
    # Sweep valid and invalid configurations
    for pt1 in [(10, 2), (-100, -300), (0.2, 1000), (50, 900)]:
        for pt2 in [(-99, -32), (10, 30), (100, 0.2), (90, 5)]:
            for tip_len in [-2, 0.1, 1, 3, 100]:
                style.tip_length = tip_len
                for tip_angle in [-10, 1, 30, 90, 179, 180, 300]:
                    style.tip_angle = tip_angle
                    for fill in [True, False]:
                        style.tip_closed = fill
                        res = p.draw_arrow(pt1, pt2, style)
                        assert res == style.is_valid()
                        # Painter should always be kept in a valid state
                        assert p.is_valid()


def test_draw_circles():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_circle((10, 10), 5)
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()
    
    # Draw with explicit types
    style = viren2d.LineStyle()
    assert p.draw_circle(
        viren2d.Vec2d(0, 30), 7, style, viren2d.Color("black"))

    # Draw with implicit conversions
    assert p.draw_circle((0, 30), 70, style, "blue!10")
    assert p.draw_circle(
        radius=70, center=(0, 30), fill_color="blue!10", line_style=style)
  
    # Sweep valid and invalid configurations
    for center in [(10, 20), (0, 0), (-100, -30), (0.2, 3000)]:
        for radius in [-3, 0, 1, 5, 50, 170]:
            for style in line_style_configurations():
                for fill_color in color_configurations():
                    if radius > 0 and is_valid_line_or_fill(style, fill_color):
                        assert p.draw_circle(center, radius, style, fill_color)
                    else:
                        assert not p.draw_circle(center, radius, style, fill_color)
                    # Painter should always be kept in a valid state
                    assert p.is_valid()


def test_draw_ellipse():
    ### Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_ellipse(viren2d.Ellipse((150, 150), (300, 100)))
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    # Draw with explicit initialization
    style = viren2d.LineStyle()
    # Ellipse as center, size
    assert p.draw_ellipse(viren2d.Ellipse((150, 150), (300, 100)), style)
    # The above from kwargs
    assert p.draw_ellipse(viren2d.Ellipse(axes=(300, 100), center=(150, 150)), style)

    # Implicit ellipse as (cx, cy), (major, minor), rotation, angle_from, angle_to, include_center
    assert p.draw_ellipse(((150, 180), (300, 100), 90, 45, -45, False), style)
    # The above explicitly from kwargs
    assert p.draw_ellipse(line_style=style,
        ellipse=viren2d.Ellipse(axes=(300, 200), center=(150, 180),
            rotation=90, angle_from=45, angle_to=-45,
            include_center=False))


def test_draw_grid():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_grid(20, 20, viren2d.LineStyle())
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()
    
    # Draw with explicit types
    style = viren2d.LineStyle()
    assert p.draw_grid(20, 20)
    assert p.draw_grid(20, 20, style)
    assert p.draw_grid(20, 20, style, viren2d.Vec2d(), viren2d.Vec2d())

    # Draw with explicit types & named parameters
    style = viren2d.LineStyle()
    assert p.draw_grid(spacing_x=20, spacing_y=20)
    assert p.draw_grid(spacing_x=20, spacing_y=20, line_style=style)
    assert p.draw_grid(
        spacing_x=20, spacing_y=20, line_style=style,
        top_left=viren2d.Vec2d(), bottom_right=viren2d.Vec2d())

    # Draw with implicit conversions
    assert p.draw_grid(20, 20, style, (0, 0), (50, 50))
    # Draw with implicit conversions
    assert p.draw_grid(
        spacing_x=20, spacing_y=20, line_style=style,
        top_left=(0, 0), bottom_right=(50, 50))
    
    # Sweep valid and invalid configurations
    for tl in [(0, 0), (-10, 3), (100, 20)]:
      for br in [(0, 0), (-20, -70), (300, 30)]:
          for sx in [-2, 0, 10]:
            for sy in [-5, 0, 5]:
              for style in line_style_configurations():
                  if is_valid_line(style) and sx > 0 and sy > 0:
                      assert p.draw_grid(sx, sy, style, tl, br)
                  else:
                      assert not p.draw_grid(sx, sy, style, tl, br)
                  # Painter should always be kept in a valid state
                  assert p.is_valid()


def test_draw_line():
    ### Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_line((0, 0), (50, 50), viren2d.LineStyle())
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    ### Draw with explicit types
    style = viren2d.LineStyle()
    assert p.draw_line(viren2d.Vec2d(0, 30), viren2d.Vec2d(70, 80), style)
    # Same as above from kwargs
    assert p.draw_line(
        pt2=viren2d.Vec2d(70, 80), pt1=viren2d.Vec2d(0, 30), line_style=style)
    assert p.draw_line(
        pt2=viren2d.Vec2d(y=80, x=70), pt1=viren2d.Vec2d(x=0, y=30), line_style=style)

    ### Draw with implicit conversions
    assert p.draw_line((0, 30), (70, 80), style)
    # Using kwargs
    assert p.draw_line(line_style=style, pt2=(10, 10), pt1=(50, 30))
    assert p.draw_line(pt2=(10, 10), pt1=(50, 30))

    ### Sweep valid and invalid configurations
    for style in line_style_configurations():
        for pt1 in [(10, 2), (-100, -300), (0.2, 1000), (5000, 90000)]:
            for pt2 in [(-99, -32), (10, 30), (100000, 0.2), (90, 5)]:
                assert p.draw_line(pt1, pt2, style) == is_valid_line(style)
                # Painter should always be kept in a valid state
                assert p.is_valid()


def test_draw_marker():
    ### Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_marker((0, 0))
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    # Draw inside/on the border/outside of the canvas
    assert p.draw_marker((10, 10))
    assert p.draw_marker((0, 0))
    assert p.draw_marker((-100, 20))
    
    # Same with the default style
    style = viren2d.MarkerStyle()
    assert p.draw_marker((10, 10), style)
    assert p.draw_marker((0, 0), style)
    assert p.draw_marker((-100, 20), style)

    # Call draw_marker with kwargs and different parameter order
    assert p.draw_marker(position=(0, 0), marker_style=style)
    assert p.draw_marker(marker_style=style, position=(-100, 20))
    assert p.draw_marker(position=(0, 0))
    # Invalid keywords
    with pytest.raises(TypeError):
        p.draw_marker(pt=(0, 0), marker_style=style)
    with pytest.raises(TypeError):
        p.draw_marker(position=(0, 0), style=style)

    # Test draw markers
    markers = [
        ((900, 34), 'red'),
        ((300, 10), viren2d.Color.Invalid),
        ((-10, 34), 'carrot!30'),
        ((900, 34), 'blue!1'),
        ((770, 42), (-1, -1, -1)),
        ((100, 20), 'invalid')
    ]
    assert p.draw_markers(markers)
    assert p.draw_markers(markers, style)
    # Same with kwargs
    assert p.draw_markers(markers=markers)
    assert p.draw_markers(markers=markers, marker_style=style)
    assert p.draw_markers(marker_style=style, markers=markers)


def test_draw_polygon():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_polygon([(0, 0), (50, 50), (30, 20)])
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    assert p.draw_polygon([(0, 0), (50, 50), (30, 20)])

    # A polygon needs at least 3 points
    assert not p.draw_polygon([(0, 0), (50, 50)])

    for line_style in line_style_configurations():
        for fill_color in color_configurations():
            res = p.draw_polygon(
                    polygon=[(0, 0), (50, 50), (30, 20)],
                    line_style=line_style, fill_color=fill_color)
            assert res == is_valid_line_or_fill(line_style, fill_color)


def test_draw_rect():
    ### Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_rect(((0, 0), (30, 50)))
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    ### Draw with explicit initialization
    style = viren2d.LineStyle()
    # Rect as (cx, cx), (w, h)
    assert p.draw_rect(viren2d.Rect((10, 20), (30, 50)), style)
    
    # Rect as center, size
    assert p.draw_rect(viren2d.Rect((10, 20), (30, 50)), style)
    # The above from kwargs
    assert p.draw_rect(viren2d.Rect(center=(10, 20), size=(30, 50)), style)

    # Rect as center, size, angle, radius
    assert p.draw_rect(viren2d.Rect((10, 20), (30, 50), 30, 0), style)
    # The above from kwargs
    assert p.draw_rect(viren2d.Rect(center=(10, 20), size=(30, 50), rotation=30, radius=0), style)

    ### Draw with implicit conversions
    # Rect as tuple (cx, cy, w, h)
    assert p.draw_rect((10, 20, 30, 50), style)
    # Same as above, but with kwargs
    assert p.draw_rect(line_style=style, rect=(10, 20, 30, 50))
    # Like above, but includes additional fill
    assert p.draw_rect(rect=(10, 20, 30, 50), line_style=style, fill_color='green')

    # Rect from tuple (cx, cy, w, h, angle)
    assert p.draw_rect((10, 20, 30, 50, 78), style)
    # Rect from tuple (cx, cy, w, h, angle, radius)
    assert p.draw_rect((10, 20, 30, 50, 78, 5), style)
    # Rect from tuple (center, size)
    assert p.draw_rect(((10, 20), (30, 50)), style)
    # Rect from tuple (center, size, angle)
    assert p.draw_rect(((10, 20), (30, 50), 78), style)
    # Rect from tuple (center, size, angle, radius)
    assert p.draw_rect(((10, 20), (30, 50), 78, 5), style)
    
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
                    assert p.draw_rect(rect, style, fill_color)
                else:
                    assert not p.draw_rect(rect, style, fill_color)


def test_draw_text():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    res = p.draw_text(['test'], (10, 10))
    assert not res.is_valid()
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    res = p.draw_text(['test'], (10, 10))
    assert res.is_valid()
    res = p.draw_text(text=['test'], position=(10, 10))
    assert res.is_valid()

    text_style = viren2d.TextStyle()
    text_style.size = -3
    res = p.draw_text(
        text=['test'], position=(50, 100), anchor='north-west',
        text_style=text_style, padding=(77, 3), rotation=13)
    assert not res.is_valid()

    # With/without parameter names    
    text_style.size = 10
    res = p.draw_text(
        text=['test'], position=(50, 100), anchor='north-west',
        text_style=text_style, padding=(77, 3), rotation=13)
    assert res.is_valid()

    res = p.draw_text(
            ['test'], (50, 100), 'south', text_style, (77, 3), 13)
    assert res.is_valid()


def test_textbox_anchors():
    p = viren2d.Painter(height=300, width=400)
    assert p.is_valid()

    anchors = viren2d.Anchor.list_all()
    assert all([isinstance(a, viren2d.Anchor) for a in anchors])

    anchors += [str(a) for a in anchors]

    text_style = viren2d.TextStyle()
    assert text_style.is_valid()

    for anchor in anchors:
        res = p.draw_text(
                ['test'], (50, 100), anchor, text_style, (77, 3), 13)
        assert res.is_valid()


def test_draw_text_box():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    res = p.draw_text_box(['test'], (10, 10))
    assert not res.is_valid()
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    res = p.draw_text_box(['test'], (10, 10))
    assert res.is_valid()
    res = p.draw_text_box(text=['test'], position=(10, 10))
    assert res.is_valid()

    for border_style in [viren2d.LineStyle.Invalid, viren2d.LineStyle()]:
        for box_size in [(-1, -1), (20, 10), (200, 70)]:
            text_style = viren2d.TextStyle()
            text_style.size = -3
            res = p.draw_text_box(
                    text=['test'], position=(50, 100), anchor='north-west',
                    text_style=text_style, padding=(77, 3), rotation=90,
                    line_style=border_style, fill_color='blue', radius=0.1,
                    fixed_size=box_size)
            assert not res.is_valid()

            # With/without parameter names    
            text_style.size = 15
            res = p.draw_text_box(
                    text=['test'], position=(50, 100), anchor='north-west',
                    text_style=text_style, padding=(77, 3), rotation=90,
                    line_style=border_style, fill_color='blue', radius=0.1,
                    fixed_size=box_size)
            assert res.is_valid()

            res = p.draw_text_box(
                    ['test'], (50, 100), 'north-west', text_style, (77, 3),
                    90, border_style, 'blue', 0.1, box_size)
            assert res.is_valid()


def test_draw_trajectory():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_trajectory([(0, 0), (50, 50)])
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    assert p.draw_trajectory([(0, 0), (50, 50)])

    # Create dummy trajectory (which may partially be
    # outside the image boundaries)
    num_points = 100
    x = 600 * np.random.rand(num_points, 1) - 100
    y = 500 * np.random.rand(num_points, 1) - 100
    pts = [(x[i], y[i]) for i in range(num_points)]

    line_style = viren2d.LineStyle()
    
    ##### Plain trajectory (draws a single path)
    assert p.draw_trajectory(
        pts, line_style, viren2d.Color.Invalid)
    # Include all parameters explicitly
    assert p.draw_trajectory(
        pts, line_style, viren2d.Color.Invalid,
        True, 3, viren2d.fade_out_linear)
    # Use named arguments
    assert p.draw_trajectory(
        trajectory=pts, line_style=line_style,
        fade_out_color=viren2d.Color.Invalid)
    # Include all parameters:
    assert p.draw_trajectory(
        trajectory=pts, line_style=line_style,
        fade_out_color=viren2d.Color.Invalid,
        tail_first=False, smoothing_window=17,
        fading_factor=viren2d.fade_out_quadratic)

    # Fading out (draws multiple line segments)
    assert p.draw_trajectory(
        pts, line_style, viren2d.Color.White)
    # Include all parameters explicitly
    assert p.draw_trajectory(
        pts, line_style, viren2d.Color.Black,
        True, 3, viren2d.fade_out_linear)
    # Use named arguments
    assert p.draw_trajectory(
        trajectory=pts, line_style=line_style,
        fade_out_color=viren2d.Color.Red)
    # Include all parameters:
    assert p.draw_trajectory(
        trajectory=pts, line_style=line_style,
        fade_out_color=viren2d.Color.Blue,
        tail_first=False, smoothing_window=17,
        fading_factor=viren2d.fade_out_quadratic)
    
    ### Sweep valid and invalid configurations
    for line_style in line_style_configurations():
        res = p.draw_trajectory(
            trajectory=pts, line_style=line_style)
        assert res == is_valid_line(line_style)
        # Painter should always be kept in a valid state
        assert p.is_valid()


def test_draw_trajectories():
    num_points = 50
    trajectories = list()
    for color in ['blue', 'same', 'same!40', 'invalid']:
        x = 600 * np.random.rand(num_points, 1)
        y = 500 * np.random.rand(num_points, 1)
        pts = [(x[i], y[i]) for i in range(num_points)]
        trajectories.append((pts, color))

    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_trajectories(trajectories)
    # Prepare canvas
    p.set_canvas_rgb(height=300, width=400)
    assert p.is_valid()

    assert p.draw_trajectories(trajectories)
    assert p.draw_trajectories(trajectories=trajectories)

    line_style = viren2d.LineStyle(width=10)

    line_style.color = 'invalid'
    assert not p.draw_trajectories(trajectories, line_style=line_style)

    line_style.color = 'azure'
    # Include all parameters (not fading):
    assert p.draw_trajectories(
        trajectories=trajectories, line_style=line_style,
        fade_out_color=viren2d.Color.Invalid,
        tail_first=False, smoothing_window=17,
        fading_factor=viren2d.fade_out_quadratic)

    # Fading out (draws multiple line segments)
    assert p.draw_trajectories(
        trajectories, line_style, viren2d.Color.White)
    assert p.draw_trajectories(
        trajectories=trajectories, line_style=line_style,
        fade_out_color=viren2d.Color.Red)
    # Include all parameters:
    assert p.draw_trajectories(
        trajectories=trajectories, line_style=line_style,
        fade_out_color=viren2d.Color.Blue,
        tail_first=False, smoothing_window=17,
        fading_factor=viren2d.fade_out_quadratic)


def test_bounding_boxes_2d():
    box_style = viren2d.BoundingBox2DStyle(
        line_style=viren2d.LineStyle(),
        text_style=viren2d.TextStyle(),
        box_fill_color='same!20', text_fill_color='white!60',
        clip_label=True)
    rect = viren2d.Rect.from_ltwh(20, 42, 200, 400, radius=0.2)
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    assert not p.draw_bounding_box_2d(rect=rect)
    
    p = viren2d.Painter(1000, 1000, 'lightblue')
    assert p.is_valid()
    assert p.draw_bounding_box_2d(rect=rect)

    # All parameters:
    assert p.draw_bounding_box_2d(
        rect=rect, box_style=box_style,
        label_top=['Multi-Line Label', '(... at the top)'],
        label_bottom=['Bottom Edge'], label_left=['Left Edge'],
        left_t2b=True, label_right=[], right_t2b=False)
    
    assert p.draw_bounding_box_2d(
        rect=rect, box_style=box_style,
        label_top=['Multi-Line Label', '(... at the top)'],
        label_bottom=['Bottom Edge'], label_left=['Left Edge'],
        left_t2b=False, label_right=['Right Edge'], right_t2b=True)


def test_pinhole_xyz_axes():
    p = viren2d.Painter(1000, 1000, 'white')
    for dt in [np.int64, np.float32, np.float64]:
        assert p.draw_xyz_axes(
            np.eye(3, dtype=dt), np.eye(3, dtype=dt),
            np.zeros((3, 1), dtype=dt))

    K = np.array(
        [[523.2, 0.0, 341.0],
        [0.0, 523.2, 256.0],
        [0.0, 0.0, 1.0]], dtype=np.float32)
    R = np.array(
        [[ 0.99013141,  0.14006482, -0.00465153],
        [ 0.05439048, -0.41465762, -0.90835056],
        [-0.12915675,  0.89913342, -0.41818372]], dtype=np.float64)
    t = np.array([-51.8, 17.3, 82.5], dtype=np.float64)

    # Using default optional parameters:
    assert p.draw_xyz_axes(K, R, t)
    assert p.draw_xyz_axes(K=K, R=R, t=t)
    
    # Try implicit conversion for Fortran-style arrays
    assert p.draw_xyz_axes(
        K=np.asfarray(K), R=np.asfarray(R), t=np.asfarray(t))

    # Specify all parameters:
    assert p.draw_xyz_axes(
        K=K, R=R, t=t, origin=(0, 1, 2), lengths=(1e3, 1.5e3, 1e3),
        arrow_style=viren2d.ArrowStyle(), color_x='red',
        color_y=(0, 1, 0), color_z='navyblue')
    
    # Invalid vector conversion:
    with pytest.raises(TypeError):
        p.draw_xyz_axes(K=K, R=R, t=(1, 2))

    # Implicit conversion from sliced inputs:
    M = np.ones((5, 9))
    M[0:3, 0:3] = R
    M[0:3, 3] = t[:]
    assert p.draw_xyz_axes(K=K, R=M[:3, :3], t=M[:3, 3])

#TODO split painter interface tests into several test suites!
#TODO test clipping
#TODO test draw_gradients