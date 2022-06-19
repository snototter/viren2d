"""
Test construction/casts/serialization of the geometric primitives

Note that Color, Vec and Styles are tested in separate test suites

TODO check how to get coverage reports: https://github.com/pypa/setuptools/issues/3025
"""
from typing import Union
import pytest
import viren2d as vi
import pickle
import math


def test_rect_creation():
    # Empty initialization is not allowed
    with pytest.raises(TypeError):
        r = vi.Rect()
    
    with pytest.raises(TypeError):
        r = vi.Rect(0, 0, 0, 0)

    with pytest.raises(TypeError):
        r = vi.Rect(width=0, height=0, cx=0, cy=0)

    r = vi.Rect((0, 0), (0, 0))
    assert not r.is_valid()

    r = vi.Rect((0, 0), (1, 1))
    assert r.is_valid()

    r = vi.Rect([0, 0], [1, 1])
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

    r = vi.Rect((0, 3), (10, 20), 30, 4)
    check_fixed_values(r)

    
    r = vi.Rect([0, 3], [10, 20], 30, 4)
    check_fixed_values(r)
    
    # the convenience wrapper 'cwh' (for the sake of completeness, since
    # we also provide - separately tested - lrtb/ltwh inits)
    r = vi.Rect.from_cwh(0, 3, 10, 20, 30, 4)
    check_fixed_values(r)

    r = vi.Rect.from_cwh(cx=0, cy=3, width=10, height=20, rotation=30, radius=4)
    check_fixed_values(r)
    r = vi.Rect.from_cwh(width=10, cx=0, cy=3, height=20, rotation=30, radius=4)
    check_fixed_values(r)

    r = vi.Rect((0, 3), (10, 20), 30, 4)
    check_fixed_values(r)

    r = vi.Rect(center=(0, 3), size=(10, 20), radius=4, rotation=30)
    check_fixed_values(r)

    r2 = r.copy()
    assert r2 == r
    r2.cx += 3
    assert r2 != r


def _expect_rect(obj: vi.Rect, expected: Union[list, tuple]):
    assert obj.cx == pytest.approx(expected[0])
    assert obj.cy == pytest.approx(expected[1])
    assert obj.width == pytest.approx(expected[2])
    assert obj.height == pytest.approx(expected[3])
    assert obj.rotation == pytest.approx(expected[4])
    assert obj.radius == pytest.approx(expected[5])
    if len(expected) > 6:
        assert obj.is_valid() == expected[6]
    else:
        assert obj.is_valid()


def test_rect_convenience_creation():
    ############# L, T, W, H
    r = vi.Rect.from_ltwh(3, 5, 30, 10)
    _expect_rect(r, (18, 10, 30, 10, 0, 0))

    # relative radius
    r = vi.Rect.from_ltwh(3, 5, 30, 10, 0.4)
    _expect_rect(r, (18, 10, 30, 10, 0, 0.4))

    # absolute radius
    r = vi.Rect.from_ltwh(3, 5, 30, 10, 10)
    _expect_rect(r, (18, 10, 30, 10, 0, 10, False))
    r = vi.Rect.from_ltwh(3, 5, 30, 10, 5)
    _expect_rect(r, (18, 10, 30, 10, 0, 5, True))

    # named parameters
    r = vi.Rect.from_ltwh(left=3, top=5, width=30, height=10, radius=0.3)
    _expect_rect(r, (18, 10, 30, 10, 0, 0.3))
    r = vi.Rect.from_ltwh(width=30, height=10, left=3, top=5, radius=0.3)
    _expect_rect(r, (18, 10, 30, 10, 0, 0.3))

    ############# L, R, T, B
    r = vi.Rect.from_lrtb(3, 33, 5, 10)
    _expect_rect(r, (18, 7.5, 30, 5, 0, 0))

    # relative radius
    r = vi.Rect.from_lrtb(3, 33, 5, 10, 0.4)
    _expect_rect(r, (18, 7.5, 30, 5, 0, 0.4))

    # absolute radius
    r = vi.Rect.from_lrtb(3, 33, 5, 10, 10)
    _expect_rect(r, (18, 7.5, 30, 5, 0, 10, False))
    r = vi.Rect.from_lrtb(3, 33, 5, 10, 2)
    _expect_rect(r, (18, 7.5, 30, 5, 0, 2, True))

    # named parameters
    r = vi.Rect.from_lrtb(left=3, right=33, top=5, bottom=10, radius=0.4)
    _expect_rect(r, (18, 7.5, 30, 5, 0, 0.4))
    r = vi.Rect.from_lrtb(left=3, bottom=10, radius=0.4, right=33, top=5)
    _expect_rect(r, (18, 7.5, 30, 5, 0, 0.4))


def test_rect_serialization():
    # Create dummy rects
    objs = list()
    o = vi.Rect((3, 7), (0, 0))
    assert not o.is_valid()
    objs.append(o.copy())  
    
    o = vi.Rect(center=(0, 3), size=(10, 20), rotation=30, radius=4)
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
    
    o = vi.Ellipse((0, 0), (0, 0))
    assert not o.is_valid()

    with pytest.raises(TypeError):
        vi.Ellipse(width=0, height=0, center=(0, 0))

    with pytest.raises(TypeError):
        vi.Ellipse(center=(0, 0), size=(10, 20))

    with pytest.raises(TypeError):  # typo "axis"
        vi.Ellipse(center=(0, 0), axis=(20, 10))

    o = vi.Ellipse((0, 0), (1, 1))
    assert o.is_valid()

    o = vi.Ellipse((0, 0), (3, 1))
    assert o.is_valid()

    # mess up minor/major
    o = vi.Ellipse((0, 0), (1, 3))
    assert not o.is_valid()

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

    o = vi.Ellipse((1, 10), (30, 20), 40, 50, 60, False)
    check_fixed_values(o)
    o = vi.Ellipse([1, 10], [30, 20], 40, 50, 60, False)
    check_fixed_values(o)

    # named arguments, different orders
    o = vi.Ellipse(
        axes=(30, 20), center=(1, 10), rotation=40,
        angle_from=50, angle_to=60, include_center=False)
    check_fixed_values(o)

    o = vi.Ellipse(rotation=40, include_center=False, center=(1, 10),
                   angle_to=60, angle_from=50, axes=(30, 20))
    check_fixed_values(o)

    # Deep copy
    o2 = o.copy()
    assert o2 == o
    o2.cx += 3
    assert o2 != o


def _expect_ellipse(obj: vi.Ellipse, expected: Union[list, tuple]):
    assert obj.cx == pytest.approx(expected[0])
    assert obj.cy == pytest.approx(expected[1])
    assert obj.major_axis == pytest.approx(expected[2])
    assert obj.minor_axis == pytest.approx(expected[3])
    assert obj.rotation == pytest.approx(expected[4])
    assert obj.angle_from == pytest.approx(expected[5])
    assert obj.angle_to == pytest.approx(expected[6])
    assert obj.include_center == expected[7]
    assert obj.is_valid()


def test_ellipse_convenience_creation():
    # Horizontal
    o = vi.Ellipse.from_endpoints((10, 100), (100, 100), 5)
    _expect_ellipse(o, (55, 100, 90, 5, 0, 0, 360, True))
    
    # Vertical
    o = vi.Ellipse.from_endpoints((10, 0), (10, 300), 150)
    _expect_ellipse(o, (10, 150, 300, 150, 90, 0, 360, True))

    # Vertical + angle specs
    o = vi.Ellipse.from_endpoints((10, 0), (10, 300), 150, 10, 20, False)
    _expect_ellipse(o, (10, 150, 300, 150, 90, 10, 20, False))

    # Rotated ellipse
    o = vi.Ellipse.from_endpoints((10, 0), (110, 100), 15)
    _expect_ellipse(o, (60, 50, math.sqrt(2e4), 15, 45, 0, 360, True))
    # Swap end points --> changes rotation
    o = vi.Ellipse.from_endpoints((110, 100), (10, 0), 15, 10, 64)
    _expect_ellipse(o, (60, 50, math.sqrt(2e4), 15, -135, 10, 64, True))


def test_ellipse_serialization():
    # Create dummy ellipses
    objs = list()
    o = vi.Ellipse((3, 7), (0, 0))
    assert not o.is_valid()
    objs.append(o.copy())  
    
    o = vi.Ellipse(center=(0, 3), axes=(20, 10), rotation=30)
    assert o.major_axis == 20
    assert o.minor_axis == 10
    objs.append(o.copy())

    o = vi.Ellipse(
        center=(0, 3), axes=(60, 10), rotation=75,
        angle_from=10, angle_to=300)
    objs.append(o.copy())

    # Pickle them
    for o in objs:
        data = pickle.dumps(o)
        restored = pickle.loads(data)
        assert o == restored

    # TODO once to/from json is supported, add similar test logic for json here

