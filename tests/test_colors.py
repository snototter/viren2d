import pytest
import viren2d
import pickle
 

def test_readonly_attributes():
    # Convenience color definitions inside Color are read-only:
    with pytest.raises(AttributeError):
        viren2d.Color.Black = 3
    
    with pytest.raises(AttributeError):
        viren2d.Color.White = viren2d.Color.Black


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

    c = viren2d.Color("magenta").with_alpha(0.3)
    assert c == "magenta!30"

    c = viren2d.Color.Yellow.with_alpha(0.78)
    assert c == "yellow!78"

    # From color specification (inverted)
    c = viren2d.Color("-white!30")
    assert c == "black!30"

    # Copy
    c2 = c.copy()
    assert c2 == c
    c2.alpha = 0.9
    assert c2 != c

    c = viren2d.Color('taupe', 0.2)
    assert c == 'taupe!20'

    # Get a list of premixed/known colors
    c = viren2d.Color('!black!30')
    assert c == 'white!30'
    
    # Manually create a color from r,g,b,a
    color = viren2d.Color(0.3, 0.2, 0.5, 0.7)
    assert color.is_valid()
    check_color(color, 0.3, 0.2, 0.5, 0.7)

    c2 = color.copy()
    assert color == c2
    c2 = viren2d.Color(0.3, 0.2, 0.5, 0.8)
    assert color != c2
    c2.alpha = 0.7
    assert color == c2


def test_saturation_casts():
    # Automatic saturation casts in c'tors
    c = viren2d.Color(255.0, 1000.0, 73000.0, 200.0)
    assert c.is_valid()
    assert c == viren2d.Color.White

    c = viren2d.Color(-1.0, -0.001, 0.5, 0.99)
    assert c.is_valid()
    assert c == viren2d.Color(0, 0, 0.5, 0.99)

    # ... should also work from the convenience functions:
    c = viren2d.RGBa(300.0, -1.0, 1234.0, -3.0)
    assert c.is_valid()
    assert c == "magenta!0"

    c = viren2d.RGBa(-3.0, 1000.0, -0.0001, 0.3)
    assert c.is_valid()
    assert c == "green!30"


def test_shades_of_gray():
    # Is color a shade of gray?
    c = viren2d.Color("black")
    assert c.is_valid()
    
    assert c.is_shade_of_gray()
    c.red = 0.02
    assert c.is_shade_of_gray()
    c.red = 0.03
    assert not c.is_shade_of_gray()
    assert c.is_shade_of_gray(0.05)

    c.red = c.green = c.blue = 0.77
    assert c.is_shade_of_gray()
    c.green = 0.9
    assert not c.is_shade_of_gray()


def test_webcodes():
    # Test initialization via webcodes/hex codes
    color = viren2d.Color("#000000", 0.1)
    assert color == "black!10"
    assert color.as_hex() == "#00000019"

    color = viren2d.Color("#fFfFfF", 0.3)
    assert color == 'white!30'
    assert color.as_hex() == "#ffffff4c"

    # Invalid inputs
    with pytest.raises(ValueError):
        viren2d.Color("abcd")
    with pytest.raises(ValueError):
        viren2d.Color("#abc")
    with pytest.raises(ValueError):
        viren2d.Color("#abcdefgha")

    # Hardcoded values:
    color = viren2d.Color("#0f5A12")
    assert color == viren2d.RGBa(15, 90, 18)

    color = viren2d.Color("#5500ba")
    assert color == viren2d.RGBa(85, 0, 186)

    color = viren2d.Color("#ea8435")
    assert color == viren2d.RGBa(234, 132, 53)

    color = viren2d.rgba(1, 0, 0.5)
    assert color.as_rgba() == (1, 0, 0.5, 1.0)
    assert color.as_RGBa() == (255, 0, 127, 1.0)
    assert color.as_hex() == '#ff007fff'

    color = viren2d.Color()
    assert not color.is_valid()
    assert color.as_hex() == '#????????'


def test_color_names():
    # We should be able to create each named color:
    for cname in viren2d.color_names():
        c = viren2d.Color(cname)
    
    assert viren2d.Color(' - w h-it_e\t') == 'white'
    assert viren2d.Color('black \n') == 'black'
    assert viren2d.Color('navyblue') == viren2d.Color('navy-blue') == viren2d.Color('__navy_blue__')
    assert viren2d.Color('Forest Green !40') == viren2d.Color('forest-green', 0.4)


def test_complementary_colors():
    color = viren2d.Color("black")
    assert color.inverse() == viren2d.Color.White

    color = viren2d.rgba(1, 0, 0.5)
    assert color == color.inverse().inverse()

    assert viren2d.Color.White.inverse() ==\
           viren2d.Color.Black

    assert viren2d.Color.Red.inverse() ==\
           viren2d.Color.Cyan

    assert viren2d.Color.Green.inverse().with_alpha(0.4) ==\
           viren2d.Color.Magenta.with_alpha(0.4)

    assert viren2d.Color.Blue.inverse() ==\
           viren2d.Color.Yellow

    # Special handling for shades of gray
    for v in range(0, 128):
      # Dark gray --> white
      color = viren2d.RGBa(v, v, v).inverse()
      assert color == viren2d.Color.White
      # Light gray --> Black
      color = viren2d.RGBa(v + 128, v + 128, v + 128).inverse()
      assert color == viren2d.Color.Black


def test_operators():
    # Equality
    assert viren2d.Color.Black != viren2d.Color.Red
    assert viren2d.Color.Black == viren2d.Color.Black
    assert viren2d.Color.Cyan == viren2d.Color("cyan")
    assert viren2d.Color.Cyan != viren2d.Color("cyan!99")
    assert viren2d.Color.Cyan != viren2d.Color("midnight-blue")
    assert viren2d.Color.Magenta == viren2d.Color("magenta")
    assert viren2d.Color.Yellow == viren2d.Color("#ffff00")

    assert viren2d.Color.Black != viren2d.Color("black!60")
    assert viren2d.Color.Red != viren2d.Color("red!90")
    assert viren2d.Color.Red.with_alpha(0.9) == viren2d.Color("red!90")

    # Scalar multiplication
    color = 0.5 * viren2d.Color.Cyan
    assert color == viren2d.Color(0.0, 0.5, 0.5, 0.5)
    # Values should be clamped
    color.alpha = 0.7
    color *= 3
    assert color == viren2d.Color(0.0, 1.0, 1.0, 1.0)

    color = viren2d.Color.White * 0.5
    assert color == viren2d.Color(0.5, 0.5, 0.5, 0.5)
    color.alpha = 0.7
    cp = color.copy()
    color *= 1.5
    assert color == viren2d.Color(0.75, 0.75, 0.75, 1.0)
    assert (cp * 1.5) == color

    # Division by scalar (only rhs)
    color = viren2d.Color.Magenta / 2.0
    assert color == viren2d.Color(0.5, 0.0, 0.5, 0.5)
    cp = color.copy()
    color /= 5
    assert color == viren2d.Color(0.1, 0.0, 0.1, 0.1)
    assert color == (viren2d.Color.Magenta / 10.0)

    # Addition
    print(f'{color} + self = {color + color} vs 2*sefl = {2*color}')
    add = color + color
    assert (2 * color) == add
    cp.green = 0.3
    add += cp
    assert add == viren2d.rgba(0.7, 0.3, 0.7, 0.7)
    # but the operand should not have changed
    assert color == viren2d.Color.Magenta / 10.0
    # Values should saturate
    add += viren2d.Color.White
    assert add == "white"

    # Subtract
    add -= 2 * viren2d.Color.Magenta
    assert add == "GREEN!0"

    add = cp - color
    assert add == viren2d.rgba(0.4, 0.3, 0.4, 0.4)


def test_pickling():
    color = viren2d.Color("orchid!99")
    data = pickle.dumps(color)
    restored = pickle.loads(data)
    assert color == restored
    assert restored == "orchid!99"

    color.red = -3
    color.alpha = 5
    data = pickle.dumps(color)
    restored = pickle.loads(data)
    assert color == restored
