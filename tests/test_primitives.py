"""
Test construction/casts/serialization of the geometric primitives

Note that Color, Vec and Styles are tested in separate test suites

FIXME check how to get coverage reports: https://github.com/pypa/setuptools/issues/3025
"""
import pytest
import viren2d as vi
import pickle


def test_rect_creation():
    # Empty initialization is not allowed
    with pytest.raises(TypeError):
        r = vi.Rect()
    with pytest.raises(ValueError):
        r = vi.Rect(())
    with pytest.raises(ValueError):
        r = vi.Rect([])

    r = vi.Rect(0, 0, 0, 0)
    assert not r.is_valid()

    with pytest.raises(TypeError):
        r = vi.Rect(width=0, height=0, cx=0, cy=0)

    r = vi.Rect(0, 0, 1, 1)
    assert r.is_valid()

    # Test initialization with different c'tors
    def check_fixed_values(rect):
        assert rect.cx == pytest.approx(0)
        assert rect.cy == pytest.approx(3)
        assert rect.width == pytest.approx(10)
        assert rect.height == pytest.approx(20)
        assert rect.rotation == pytest.approx(30)
        assert rect.radius == pytest.approx(4)
        assert rect.is_valid()

    r = vi.Rect(0, 3, 10, 20, 30, 4)
    check_fixed_values(r)

    r = vi.Rect((0, 3, 10, 20, 30, 4))
    check_fixed_values(r)

    r = vi.Rect([0, 3, 10, 20, 30, 4])
    check_fixed_values(r)

    r = vi.Rect(cx=0, cy=3, w=10, h=20, rotation=30, radius=4)
    check_fixed_values(r)

    r = vi.Rect((0, 3), (10, 20), 30, 4)
    check_fixed_values(r)

    r = vi.Rect(center=(0, 3), size=(10, 20), radius=4, rotation=30)
    check_fixed_values(r)

    r2 = r.copy()
    assert r2 == r
    r2.cx += 3
    assert r2 != r


def test_rect_serialization():
    # Create dummy rects
    objs = list()
    o = vi.Rect(3, 7, 0, 0)
    assert not o.is_valid()
    objs.append(o.copy())  
    
    o = vi.Rect(cx=0, cy=3, w=10, h=20, rotation=30, radius=4)
    objs.append(o.copy())

    # Pickle them
    for o in objs:
        data = pickle.dumps(o)
        restored = pickle.loads(data)
        assert o == restored

    # TODO once to/from json is supported, add similar test logic for json here


def test_ellipse_creation():
    # Empty initialization is not allowed
    with pytest.raises(TypeError):
        o = vi.Ellipse()
    with pytest.raises(ValueError):
        o = vi.Ellipse(())
    with pytest.raises(ValueError):
        o = vi.Ellipse([])

    o = vi.Ellipse(0, 0, 0, 0)
    assert not o.is_valid()

    with pytest.raises(TypeError):
        vi.Ellipse(width=0, height=0, cx=0, cy=0)

    o = vi.Ellipse(0, 0, 1, 1)
    assert o.is_valid()

    # Test initialization with different c'tors
    def check_fixed_values(obj):
        assert obj.cx == pytest.approx(1)
        assert obj.cy == pytest.approx(10)
        assert obj.minor_axis == pytest.approx(20)
        assert obj.major_axis == pytest.approx(30)
        assert obj.rotation == pytest.approx(40)
        assert obj.angle_from == pytest.approx(50)
        assert obj.angle_to == pytest.approx(60)
        assert not obj.include_center
        assert obj.is_valid()

    o = vi.Ellipse(1, 10, 30, 20, 40, 50, 60, False)
    check_fixed_values(o)
    # tuple
    o = vi.Ellipse((1, 10, 30, 20, 40, 50, 60, False))
    check_fixed_values(o)
    # list
    o = vi.Ellipse([1, 10, 30, 20, 40, 50, 60, False])
    check_fixed_values(o)

    o = vi.Ellipse(cx=1, major_axis=30, minor_axis=20, cy=10, rotation=40,
                   angle_from=50, angle_to=60, include_center=False)
    check_fixed_values(o)

    o = vi.Ellipse((1, 10), (30, 20), 40, 50, 60, False)
    check_fixed_values(o)
    # init from (here implicitly cast) Vec2d ensures that major/minor axis
    # are chosen correctly, even if you mix them up:
    o = vi.Ellipse((1, 10), (20, 30), 40, 50, 60, False)
    check_fixed_values(o)

    o = vi.Ellipse(axes=(20, 30), rotation=40, center=(1, 10),
                   angle_to=60, angle_from=50, include_center=False)
    check_fixed_values(o)

    o2 = o.copy()
    assert o2 == o
    o2.cx += 3
    assert o2 != o


def test_ellipse_serialization():
    # Create dummy ellipses
    objs = list()
    o = vi.Ellipse(3, 7, 0, 0)
    assert not o.is_valid()
    objs.append(o.copy())  
    
    o = vi.Ellipse(cx=0, cy=3, major_axis=10, minor_axis=20, rotation=30)
    objs.append(o.copy())

    o = vi.Ellipse(cx=0, cy=3, major_axis=10, minor_axis=20, rotation=30,
                   angle_from=10, angle_to=300)
    objs.append(o.copy())

    # Pickle them
    for o in objs:
        data = pickle.dumps(o)
        restored = pickle.loads(data)
        assert o == restored

    # TODO once to/from json is supported, add similar test logic for json here

