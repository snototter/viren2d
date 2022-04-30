import pytest
import viren2d
import pathlib


def test_vector_init():
    with pytest.raises(TypeError):
        viren2d.Vec2d(1, 2, 3) # raises typeerror
    
    
    
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

