import pytest
import viren2d
import pathlib

def test_viren2d_version():
    here = pathlib.Path(__file__).parent.parent.resolve()
    expected_version = (here / "VERSION").read_text().strip()
    assert expected_version == viren2d.__version__
   

def test_readonly_attributes():
    # Convenience color definitions inside Color are read-only:
    with pytest.raises(AttributeError):
        viren2d.Color.black = 3
    
    with pytest.raises(AttributeError):
        viren2d.Color.white = viren2d.Color.black


def check_color(color, r, g, b, a):
    assert color.red   == pytest.approx(r)
    assert color.green == pytest.approx(g)
    assert color.blue  == pytest.approx(b)
    assert color.alpha == pytest.approx(a)


def test_color_init():
    c = viren2d.Color()
    assert not c.is_valid()

    c = viren2d.RGBa(255, 0, 255)
    assert c.is_valid()
    assert c == viren2d.Color("magenta")
    assert c == "magenta"

    c = viren2d.Color('taupe', 0.2)
    assert c == 'taupe!20'

    # Get a list of premixed/known colors
    c = viren2d.Color('!black!30')
    assert c == 'white!30'
    
    # We should be able to create each named color:
    for cname in viren2d.color_names():
        c = viren2d.Color(cname)

    # Manually create a color from r,g,b,a
    color = viren2d.Color(0.3, 0.2, 0.5, 0.7);
    assert color.is_valid()
    check_color(color, 0.3, 0.2, 0.5, 0.7)
    # Copy c'tor
    c2 = viren2d.Color(color)
    assert color == c2
    c2 = viren2d.Color(0.3, 0.2, 0.5, 0.8)
    assert color != c2
    c2.alpha = 0.7
    assert color == c2




# pip install pytest pytest-cov
# pytest tests/test_colors.py
# open bug? https://github.com/pytorch/pytorch/issues/50481

