import pytest
import viren2d
import pathlib

def test_version():
    here = pathlib.Path(__file__).parent.parent.resolve()
    expected_version = (here / "VERSION").read_text().strip()
    assert expected_version == viren2d.__version__
   

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

    #viren2d.color_names()

    # Convenience color definitions inside Color are read-only:
    with pytest.raises(AttributeError):
        viren2d.Color.black = 3
    
    with pytest.raises(AttributeError):
        viren2d.Color.white = viren2d.Color.black


# pip install pytest pytest-cov
# pytest tests/test_colors.py
# open bug? https://github.com/pytorch/pytorch/issues/50481

