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
    p.draw_arc(angle1=20, angle2=300, center=(1, 2), radius=10, fill_color='black!40', line_style=style, include_center=False)

    # Sweep valid and invalid configurations (all should work without exceptions)
    # This includes drawing edge cases (outside image boundaries, degenerate geometries, ...)
    for center in [(50, 50), (-100, -100), (1000, 100), (30, 30)]:
        for radius in [0, 10, 50, -10]:
            for angles in [(-90, -100), (-80, -30), (-10, 10), (0, 300), (300, 360), (380, 500)]:
                for inc_center in [True, False]:
                    for fill_color in [viren2d.Color(), viren2d.Color("red"), viren2d.Color("blue!10")]:
                        for line_width in [0, 1, 20]:
                            style.line_width = line_width
                            if style.is_valid() or fill_color.is_valid():
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
    p.draw_circle(radius=70, center=(0, 30), fill_color="blue!10", line_style=style)
  
    # Sweep valid and invalid configurations
    for center in [(10, 20), (0, 0), (-100, -30), (0.2, 3000)]:
        for radius in [-3, 0, 1, 5, 50, 170]:
            for line_width in [-1, 0.5, 10]:
                style.line_width = line_width
                for fill_color in [viren2d.Color(), viren2d.Color("red"), viren2d.Color("blue!10")]:
                    if style.is_valid() or fill_color.is_valid():
                        p.draw_circle(center, radius, style, fill_color)
                    else:
                        with pytest.raises(ValueError):
                            p.draw_circle(center, radius, style, fill_color)
                    # Painter should always be kept in a valid state
                    assert p.is_valid()


def test_draw_grid():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    # Try drawing on invalid painter
    with pytest.raises(RuntimeError):
        p.draw_grid(20, 20)
    # Prepare canvas
    p.set_canvas_rgb(400, 300)
    assert p.is_valid()
    
    # Draw with explicit types
    style = viren2d.LineStyle()
    p.draw_grid(20, 20, viren2d.Vec2d(), viren2d.Vec2d(), style)

    # Draw with implicit conversions
    p.draw_grid(20, 20, (0, 0), (50, 50), style)
    p.draw_grid(20, 20, line_style=style)
  
    # Sweep valid and invalid configurations
    for tl in [(0, 0), (-10, 3), (100, 20), (5000, -70)]:
      for br in [(0, 0), (-20, -70), (300, 30), (5000, -70)]:
          for sx in [-5, 0, 1, 10, 1000]:
            for sy in [-5, 0, 1, 10, 1000]:
              for line_width in [-1, 0.5, 10]:
                  style.line_width = line_width
                  if style.is_valid() and sx > 0 and sy > 0:
                      p.draw_grid(sx, sy, tl, br, style)
                  else:
                      with pytest.raises(ValueError):
                          p.draw_grid(sx, sy, tl, br, style)
                  # Painter should always be kept in a valid state
                  assert p.is_valid()


def test_draw_line():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    # Try drawing on invalid painter
    with pytest.raises(RuntimeError):
        p.draw_line((0, 0), (50, 50))
    # Prepare canvas
    p.set_canvas_rgb(400, 300)
    assert p.is_valid()

    # Draw with explicit types
    style = viren2d.LineStyle()
    p.draw_line(viren2d.Vec2d(0, 30), viren2d.Vec2d(70, 80), style)

    # Draw with implicit conversions
    p.draw_line((0, 30), (70, 80), style)
    p.draw_line(line_style=style, pt2=(10, 10), pt1=(50, 30))

    # Sweep valid and invalid configurations
    for pt1 in [(10, 2), (-100, -300), (0.2, 1000), (50, 900)]:
        for pt2 in [(-99, -32), (10, 30), (100, 0.2), (90, 5)]:
            for lw in [-2, 0.1, 1, 3, 100]:
                style.line_width = lw
                for dp in [[], [10], [10, 20], [1, 2, 3]]:
                    style.dash_pattern = dp
                    for color in [viren2d.Color(), viren2d.Color("red"), viren2d.Color("blue!10")]:
                        style.color = color
                        if style.is_valid():
                            p.draw_line(pt1, pt2, style)
                        else:
                            with pytest.raises(ValueError):
                                p.draw_line(pt1, pt2, style)
                        # Painter should always be kept in a valid state
                        assert p.is_valid()
  

def test_draw_rect():
    # Try drawing on uninitialized canvas
    p = viren2d.Painter()
    assert not p.is_valid()
    # Try drawing on invalid painter
    with pytest.raises(RuntimeError):
        p.draw_rect((0, 0, 30, 50))
    # Prepare canvas
    p.set_canvas_rgb(400, 300)
    assert p.is_valid()

    # Now drawing should work
    style = viren2d.LineStyle()
    p.draw_rect(viren2d.Rect((10, 20, 30, 50)), style)

    # Draw with implicit conversions
    p.draw_rect((10, 20, 30, 50), style)
    p.draw_rect((10, 20, 30, 50, 78), style)
    p.draw_rect((10, 20, 30, 50, 78, 5), style)
    p.draw_rect(((10, 20), (30, 50)), style)
    p.draw_rect(((10, 20), (30, 50), 78), style)
    p.draw_rect(((10, 20), (30, 50), 78, 5), style)

    # Corner radius too big
    with pytest.raises(ValueError):
        p.draw_rect((10, 20, 30, 50, 78, 30), style)

    for center in [(-10, -100), (0, 0), (100, 300), (10000, 7000)]:
        for size in [(-5, -3), (0, 0), (0, 10), (4, 0), (10, 20), (300, 500), (40000, 50000)]:
            for angle in [-40, 0, 30, 380]:
                for radius in [-10, 0, 5, 100, 30000]:
                    rect = viren2d.Rect((center, size, angle, radius))
                    for lw in [-2, 0, 3, 50]:
                        style.line_width = lw
                        for fill_color in [viren2d.Color(), viren2d.Color("red"), viren2d.Color("blue!10")]:
                            if rect.is_valid() and (style.is_valid() or fill_color.is_valid()):
                                p.draw_rect(rect, style, fill_color)
                            else:
                                with pytest.raises(ValueError):
                                    p.draw_rect(rect, style, fill_color)

    # Sweep valid and invalid configurations
#TODO add tests for other draw_xxx functions    

    
