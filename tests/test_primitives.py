"""
Test construction/casts/serialization of the geometric primitives

Note that Color, Vec and Styles are tested in separate test suites
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
    rects = list()
    r = vi.Rect(3, 7, 0, 0)
    assert not r.is_valid()
    rects.append(r.copy())  
    
    r = vi.Rect(cx=0, cy=3, w=10, h=20, rotation=30, radius=4)
    rects.append(r.copy())

    # Pickle them
    for r in rects:
        data = pickle.dumps(r)
        restored = pickle.loads(data)
        assert r == restored

    # TODO once to/from json is supported, add similar test logic for json here


#TODO test ellipse

