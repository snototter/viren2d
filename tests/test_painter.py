import pytest
import viren2d
import pathlib
import pickle


def test_viren2d_version():
    here = pathlib.Path(__file__).parent.parent.resolve()
    expected_version = (here / "VERSION").read_text().strip()
    assert expected_version == viren2d.__version__
   

def test_painter_basics():
    p = viren2d.Painter()
    assert not p.is_valid()
    
    with pytest.raises(RuntimeError):
        p.get_canvas(False)

    with pytest.raises(RuntimeError):
        p.get_canvas_size()
    
    p.set_canvas_rgb(400, 300)
    assert p.is_valid()

    w, h = p.get_canvas_size()
    assert w == 400
    assert h == 300

#TODO test init filename
#TODO test init image


# test_draw_xxx should ensure that we identify
# breaking API changes (e.g. missed bindings; forgot
# to update parametrization, ...)
#
# Instead of using a fixture to set up the canvas,
# each test_draw_xxx should try to draw on an invalid
# canvas first. This is to ensure that calling 
# draw_xxx before setting up the canvas will raise
# a RuntimeError


def is_valid_line(line_style):
    return line_style.is_valid()
#        or (line_style == viren2d.LineStyle.Default)


def is_valid_line_or_fill(line_style, fill_color):
    return line_style.is_valid() or fill_color.is_valid()
#        or (line_style == viren2d.LineStyle.Default)


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
    p.set_canvas_rgb(400, 300)
    assert p.is_valid()

    style = viren2d.LineStyle()
    # Draw with explicit types
    p.draw_arc(viren2d.Vec2d(45, 77), 30, 20, 300, style, True, viren2d.rgba(1, 0, 1))

    # Draw with implicit conversions
    p.draw_arc((1, 2), 10, 30, 40, style, False, "midnight-blue!80")
    p.draw_arc(
        angle1=20, angle2=300, center=(1, 2), radius=10,
        fill_color='black!40', style=style, include_center=False)

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
    # Try drawing on invalid painter
    with pytest.raises(RuntimeError):
        p.draw_arrow((0, 0), (50, 50))
    # Prepare canvas
    p.set_canvas_rgb(400, 300)
    assert p.is_valid()

    # Draw with explicit types
    style = viren2d.ArrowStyle()
    p.draw_arrow(viren2d.Vec2d(0, 30), viren2d.Vec2d(70, 80), style)

    # Draw with implicit conversions
    p.draw_arrow((0, 30), (70, 80), style)
    p.draw_arrow(arrow_style=style, pt2=(10, 10), pt1=(50, 30))
#TODO  arrow_style_configurations():
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
    # Try drawing on invalid painter
    with pytest.raises(RuntimeError):
        p.draw_circle((10, 10), 5)
    # Prepare canvas
    p.set_canvas_rgb(400, 300)
    assert p.is_valid()
    
    # Draw with explicit types
    style = viren2d.LineStyle()
    p.draw_circle(viren2d.Vec2d(0, 30), 7, style, viren2d.Color("black"))

    # Draw with implicit conversions
    p.draw_circle((0, 30), 70, style, "blue!10")
    p.draw_circle(radius=70, center=(0, 30), fill_color="blue!10", style=style)
  
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
    # Try drawing on invalid painter
    with pytest.raises(RuntimeError):
        p.draw_ellipse(viren2d.Ellipse((150, 150), (100, 300)))
    # Prepare canvas
    p.set_canvas_rgb(400, 300)
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
    p.draw_ellipse(style=style,
        ellipse=viren2d.Ellipse(axes=(100, 300), center=(150, 180),
            rotation=90, angle_from=45, angle_to=-45,
            include_center=False))

    
#TODO    ### Sweep valid and invalid configurations
    # Collect ellipses
    # Try to draw with different style variations


def test_draw_grid():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    # Try drawing on invalid painter
    with pytest.raises(RuntimeError):
        p.draw_grid(20, 20, viren2d.LineStyle())
    # Prepare canvas
    p.set_canvas_rgb(400, 300)
    assert p.is_valid()
    
    # Draw with explicit types
    style = viren2d.LineStyle()
    p.draw_grid(20, 20)
    p.draw_grid(20, 20, style)
    p.draw_grid(20, 20, style, viren2d.Vec2d(), viren2d.Vec2d())

    # Draw with implicit conversions
    p.draw_grid(20, 20, style, (0, 0), (50, 50))
    p.draw_grid(20, 20, style=style)
  
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
    # Try drawing on invalid painter
    with pytest.raises(RuntimeError):
        p.draw_line((0, 0), (50, 50), viren2d.LineStyle())
    # Prepare canvas
    p.set_canvas_rgb(400, 300)
    assert p.is_valid()

    ### Draw with explicit types
    style = viren2d.LineStyle()
    p.draw_line(viren2d.Vec2d(0, 30), viren2d.Vec2d(70, 80), style)
    # Same as above from kwargs
    p.draw_line(pt2=viren2d.Vec2d(70, 80), pt1=viren2d.Vec2d(0, 30), style=style)
    p.draw_line(pt2=viren2d.Vec2d(y=80, x=70), pt1=viren2d.Vec2d(x=0, y=30), style=style)

    ### Draw with implicit conversions
    p.draw_line((0, 30), (70, 80), style)
    # Using kwargs
    p.draw_line(style=style, pt2=(10, 10), pt1=(50, 30))
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
  

def test_draw_rect():
    ### Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    # Try drawing on invalid painter
    with pytest.raises(RuntimeError):
        p.draw_rect(((0, 0), (30, 50)))
    # Prepare canvas
    p.set_canvas_rgb(400, 300)
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
    p.draw_rect(style=style, rect=(10, 20, 30, 50))
    # Like above, but includes additional fill
    p.draw_rect(rect=(10, 20, 30, 50), style=style, fill_color='green')

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

#TODO add tests for other draw_xxx functions    
