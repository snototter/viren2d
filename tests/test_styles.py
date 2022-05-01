import pytest
import viren2d
import pickle


def test_line_style():
    # Default initialization should yield a valid style
    style = viren2d.LineStyle()
    assert style.is_valid()

    style.line_width = 0
    assert not style.is_valid()

    style.line_width = 0.4
    assert style.is_valid()

    style.color = viren2d.Color()
    assert not style.color.is_valid()
    assert not style.is_valid()

    # Comparison
    style = viren2d.LineStyle(2.0, viren2d.Color("magenta"), [],
                              viren2d.LineCap.Round, viren2d.LineJoin.Bevel)
    
    style2 = viren2d.LineStyle(2.0, "magenta", [],
                              viren2d.LineCap.Round, viren2d.LineJoin.Bevel)
    assert style == style2
    style2.line_cap = viren2d.LineCap.Butt
    assert style != style2

    tpl = (2.0, "magenta", [], viren2d.LineCap.Round, viren2d.LineJoin.Bevel)
    assert style == tpl


def test_arrow_style():
    # Default initialization should yield a valid style
    style = viren2d.ArrowStyle()
    assert style.is_valid()

    # Mess around with the base line style
    style.line_width = 0
    assert not style.is_valid()
    style.line_width = 0.4
    assert style.is_valid()

    style.color = viren2d.Color()
    assert not style.color.is_valid()
    assert not style.is_valid()
    style.color = viren2d.Color("orchid!50")
    assert style.is_valid()

    # Mess around with arrow-specific style
    style.tip_angle = 0
    assert not style.is_valid()
    style.tip_angle = 15
    assert style.is_valid()

    style.tip_length = 0
    assert not style.is_valid()
    style.tip_length = 3
    assert style.is_valid()
    

def test_arrow_tip_length():
    # Default initialization should yield a valid style
    style = viren2d.ArrowStyle()
    style.tip_length = 0.2
    assert style.tip_length_for_shaft(200.0) == pytest.approx(40.0)

    style.tip_length = 0.9
    assert style.tip_length_for_shaft(200.0) == pytest.approx(180.0)
    
    style.tip_length = 0.1
    assert style.tip_length_for_shaft(viren2d.Vec2d(90, 100), viren2d.Vec2d(110, 100)) == pytest.approx(2.0)

    style.tip_length = 90
    assert style.tip_length_for_shaft(200.0) == pytest.approx(90.0)
    style.tip_length = 2
    assert style.tip_length_for_shaft(30.0) == pytest.approx(2.0)


def test_line_operators():
    # Compare 2 LineStyle objects
    line_style1 = viren2d.LineStyle()
    line_style2 = viren2d.LineStyle()
    assert line_style1 == line_style2

    line_style2.dash_pattern = [10, 20]
    assert line_style1 != line_style2

    # Compare 2 ArrowStyle objects
    arrow_style1 = viren2d.ArrowStyle()
    arrow_style2 = viren2d.ArrowStyle()
    assert arrow_style1 == arrow_style2

    arrow_style1.tip_length = 30
    assert arrow_style1 != arrow_style2
    arrow_style1.tip_length = arrow_style2.tip_length
    assert arrow_style1 == arrow_style2

    arrow_style2.tip_angle = 23;
    assert arrow_style1 != arrow_style2
    arrow_style2.tip_angle = arrow_style1.tip_angle;
    assert arrow_style1 == arrow_style2

    # ArrowStyle should differ if we change its
    # base LineStyle properties:
    arrow_style2.dash_pattern = [5, 7]
    assert arrow_style1 != arrow_style2
    arrow_style2.dash_pattern = []
    assert arrow_style1 == arrow_style2


def test_pickling():
    # Serialize line style
    ls = viren2d.LineStyle(2.0, "orchid!30", [],
                           viren2d.LineCap.Round, viren2d.LineJoin.Bevel)
    data = pickle.dumps(ls)
    restored = pickle.loads(data)
    assert ls == restored

    # Serialize arrow style
    arr = viren2d.ArrowStyle(tip_length=42, tip_angle=20, tip_closed=True)
    data = pickle.dumps(arr)
    restored = pickle.loads(data)
    assert arr == restored

