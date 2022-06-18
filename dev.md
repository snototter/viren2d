# Development Guide
`viren2d++`, i.e. the C++ library:
* For each custom type (where applicable), add c'tor using
  initializer_list (for less cluttered/more convenient use)
* All drawing functions should shift the user-given coordinates
  by 0.5 if needed to support sharp lines. For details see:
  see https://www.cairographics.org/FAQ/#sharp_lines
* Each drawing function should call `CheckCanvas` and `CheckLineStyle`
  (or a more suitable sanity check function).
* Drawing functions won't be tested via mocking (because mocking
  the Cairo C interface would be a pain & I don't want to switch
  to cairomm or write my own C++ wrapper). There are, however,
  tests to ensure we identify code changes that would break the
  drawing API, see `tests/test_painter.py`.  
  Besides these interface tests, drawing functionality should
  have understandable demos.  
* All other functionality should be tested, ideally both in C++
  and Python.
* Task template for (almost) each new function:  
  ```cpp
  //TODO [ ] add documentation
  //TODO [ ] add C++ test (tests/xxx_test.cpp)
  //TODO [ ] add Python bindings
  //TODO [ ] add Python test (tests/test_xxx.py)
  //TODO [ ] add C++ demo
  //TODO [ ] add Python demo
  ```
 
`viren2d`, i.e. the Python bindings:
* Don't use python keywords as names of function arguments
 or you can't order the arguments via "f(arg_x=foo, arg_a=1)"
* Keep draw_xxx bindings in alphabetic order for maintainability
* How to bind a new class X:
 * Implement pickling::SerializeX
 * Implement pickling::DeserializeX
 * Implement __str__ & __repr__
 * nice-to-have: operator == and !=
 * Test initialization, pickling, comparison, etc.
 * Declare it py::implicitly_convertible if a simple/intuitive
   conversion exists
 * @deprecated Implement moddef::CreateX (init from py::tuple/list/whatever)
 * All this info does not hold for ImageBuffer - which exposes a
   buffer view (and we need to be able to convert to/from numpy
   arrays)
   

## Examples

TODO most examples are completely obsolete/outdated!

Example code requires python packages `vito` and `numpy`

TODO side-effects of shared buffer should be well documented (and/or return copy by default)
```python
import viren2d
# Draw something
p = viren2d.Painter()
p.set_canvas_filename('examples/flamingo.jpg')
ls = viren2d.LineStyle(4.2, (1, 0, 1, .8), [20, 20], viren2d.LineStyle.Cap.Round)
fill = viren2d.colors.Color(0, 1, 1, 0.6)
p.draw_rect((200, 150, 50, 90, 3.6, 10), ls, fill)

# How to get the visualization?
import numpy as np
from vito import imvis

# Copied memory (avoiding the additional copy from C++ buffer to numpy)
img_copied = np.array(p.get_canvas(True), copy=False)
imvis.imshow(img_copied)

# Shared memory - changing img_np will modify the canvas memory!
img_shared = np.array(p.get_canvas(False), copy=False)
imvis.imshow(img_shared)
# Demonstrating shared memory side effects:
img_shared[:, :, 0] = 0
img_shared = np.array(p.get_canvas(False), copy=False)
imvis.imshow(img_shared)
imvis.imshow(img_copied)
```


TODO set up/build/run:
```
# install via apt: libcairo2-dev, ninja-build
# git clone --recursive https://github.com/snototter/viren2d.git
# python -m pip install git+https://github.com/snototter/viren2d.git
# optional: libopencv-dev

cd build
make -j && ./demo2d

#vs

python -m pip install .
# if this fails with /tmp/..../ninja not found:
rm -r build/temp.linux-x86_64-cpython-38/
python -m pip install .
## or don't use ninja:
#export CMAKE_GENERATOR="Unix Makefiles"
#python -m pip install .
```

python packaging (try build instead of setuptools)
https://packaging.python.org/en/latest/guides/publishing-package-distribution-releases-using-github-actions-ci-cd-workflows/


```cpp
r vs lvalue: ;-) https://stackoverflow.com/a/33829750/400948


Image loading lightweight: https://stackoverflow.com/a/40812978/400948
--> that's why I switched to https://github.com/nothings/stb

How to make a clean library with CMake
https://github.com/robotology/how-to-export-cpp-library/blob/master/CMakeLists.txt

CMake include other target
https://stackoverflow.com/a/61097014/400948


Cairo tutorials
https://zetcode.com/gfx/cairo/cairobackends/

TODO
FindCairo.cmake taken from
https://github.com/preshing/CairoSample/blob/master/modules/FindCairo.cmake

opencv <--> eigen3
https://stackoverflow.com/questions/14783329/opencv-cvmat-and-eigenmatrix
```

run
```
TODO timing drawing calls would be nice (not really accurate due to conversion overhead, but still worth having a (better than) ballpark figure)

import viren2d
from vito import pyutils as pu
pu.tic()
p = viren2d.Painter()
p.set_canvas_rgb(400, 300, (1, 1, 1))
ls = viren2d.LineStyle(4.2, (1, 0, 1, .8), [20, 20], viren2d.LineStyle.Cap.Round)
fill = viren2d.Color(0, 1, 1, 0.6)
p.draw_rect((200, 150, 50, 90, 3.6, 10), ls, fill)

#for i in range(100):
#    p.draw_rect((200, 150, 50, 90, i*3.6, 10), (4.2, (1, 0, 1, .2)), (0, 1, 1, 0.2))

pu.toc()
# 100 rects -> 15 to 16ms
# 1 rect -> 5 ms
p.show()




####################################################
# get canvas dev:

# Canvas from RGB
import viren2d
import numpy as np
from vito import imvis
p = viren2d.Painter()
img_np = np.zeros((480, 640, 3), dtype=np.uint8)
img_np[:, :, 1] = 200
img_np[:, :, 2] = 200 # cyan
b = viren2d.ImageBuffer(img_np)
p.set_canvas_image(b)
p.draw_line((10, 200), (600, 10), (3.9, (0, 0, 1), [20, 20]))
imvis.imshow(np.array(p.get_canvas(), copy=False))


# Canvas from RGBA
import viren2d
import numpy as np
from vito import imvis
p = viren2d.Painter()
img_np = np.zeros((480, 640, 4), dtype=np.uint8)
img_np[:, :, 1] = 200
img_np[:, :, 2] = 200 # cyan
img_np[:, :, 3] = 128 # 50% transparency
# Draw something
p.set_canvas_image(img_np)
p.draw_line((10, 200), (600, 10), (3.9, (0, 0, 1), [20, 20]))
imvis.imshow(np.array(p.get_canvas(), copy=False)) # Show transparent canvas
imvis.imshow(np.array(p.get_canvas().to_rgb(), copy=False)) # Drop transparency


img_np = np.zeros((480, 640, 4), dtype=np.uint8)
img_np[:, :, 3] = 255
img_np[:, :, 2] = 200
p.set_canvas_image(img_np)
ls = viren2d.LineStyle(4.2, (1, 0, 1, .8), [20, 20], viren2d.LineStyle.Cap.Round)
fill = viren2d.Color(0, 1, 1, 0.6)
p.draw_rect((200, 150, 50, 90, 3.6, 10), ls, fill)

from vito import imvis
canvas = np.array(p.get_canvas(), copy=False)
imvis.imshow(canvas)
```

# TODO Add to Tests

```python
from vito import imvis
import numpy as np
import viren2d

viren2d.__version__

# Get a list of premixed/known colors
viren2d.color_names()

# AttributeError: read-only!
viren2d.Color.black = 3
```


!!!!!!!!!!!!!
TODO compare library sizes (apt)
opencv

apt-cache --no-all-versions show libopencv* | awk '
function human(x) {
        if (x<1000) {return x} else {x/=1024}
        s="kMGTEPZY";
        while (x>=1000 && length(s)>1)
            {x/=1024; s=substr(s,2)}
        return int(x+0.5) substr(s,1,1)
    }
$1 == "Package:" { p = $2 } $1 == "Installed-Size:"    { print p, $2, human($2)}'


$1 == "Package:" { p = $2 } $1 == "Size:"    { print p, $2, human($2)}'

https://unix.stackexchange.com/a/44087


# Testing
```
pip install pytest
pytest tests/test_*.py
open bug? https://github.com/pytorch/pytorch/issues/50481
```


