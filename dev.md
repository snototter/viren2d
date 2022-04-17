# TODO
* readme - install/setup section
* readme - quickstart
* readme - make github banner with vivi ;-)
* separate example/tutorial doc

TODO rename to viren2d  
A light-weight 2D rendering toolbox for various computer vision tasks.

How to pronounce viren2d?  
"vi" as in "vision", "ren" as in "rendering": "vi-ren-2d"
This is by the way also how you pronounce the German word for viruses (Totally coincidentally, or maybe I'm still affected by the aftermath of the pandemic)

Why?  
I'm too tired of/too lazy to look up how to visualize some results (read: bounding boxes, trajectories, etc.) within the software framework I currently have to work with.
Ideally, I wanted something that can be easily used both within C++ and Python settings - as I have to switch between these two regularly.

Initially, I implemented a [similar toolbox](https://github.com/snototter/vitocpp/) based on OpenCV, but a) OpenCV is a quite heavy dependency (did you ever get to "enjoy" a version mismatch between (unofficial) Python bindings and your system's OpenCV library?); b) it offers only limited drawing capabilities (it's an image processing library, thus not focusing on "nice" graphics) and c) my framework grew over the years (and thus, suffering from the usual __"I will clean this mess up later"__ bad coding habbit).

So here comes `viren2d`, a toolbox with only a single purpose: easily create aesthetically pleasing visualizations.
* Lightweight: under the hood, viren2d uses libcairo2 (approximately 20 MB of disk space TODO check, vs X00 MB opencv?)
* Ease-of-use: 
* Maintainability (hopefully)

Note on efficiency: I preferred code readability over efficiency. Nevertheless, compared to my previous attempt using OpenCV, the switch to Cairo immediately paid off (despite using CPU-only in-memory image surface rendering, which is by far the slowest usage of Cairo).

## Installation
**Supported platforms**:
So far, `viren2d` has been tested on: Ubuntu 18.04 & 20.04
todo python3, cairo, ninja, cmake should work on almost all platforms

let me know if you set it up on any other systems, so I can update the install/setup instructions accordingly: 

### Prerequisites
install via apt: libcairo2-dev, ninja-build, cmake>3.12 (todo check or was it 3.15?)

### Direct Installation
  * **Install directly via pip>=10** TODO find correct version (must support pyproject.toml!!!)
    ```bash
    # ... set up a virtualenv ...
    python3 -m venv venv
    source venv/bin/activate
    python -m pip install -U pip
    
    > python -m pip install git+https://github.com/snototter/vivi.git
    ```
### Build from Source
```bash
# Clone recursively to set up the external libraries
git clone --recursive https://github.com/snototter/vivi.git
cd viren2d
TODO c++ vs python

python -m pip install .
```

Note, if you want to re-install it, you may have to delete the CMake cache first (as it points to a Ninja binary, which is located within /tmp).
```bash
rm -r build/temp.*
python -m pip install .
```


## Examples

Example code requires python packages `vito` and `numpy`
```python
from vito import imvis
import numpy as np
import vivi

# Dashed 4-px wide line
line_style = vivi.LineStyle(4, vivi.colors.salmon(0.8), [20, 15])

p = vivi.Painter()
p.set_canvas_rgb(800, 600, vivi.colors.lavender())
p.draw_line((10, 200), (600, 10), line_style)

# Get SHARED image buffer (changed by subsequent draw_... calls)
img = np.array(p.get_canvas(), copy=False)
imvis.imshow(img)

line_style.color = vivi.colors.midnight_blue()
line_style.line_cap = vivi.LineStyle.Cap.Round
line_style.dash_pattern = [50, 10, 10, 10]
p.draw_line((0, 0), (400, 400), line_style)

# Don't need to re-retrieve the canvas because we requested the
# shared buffer. This is the default behavior unless you use
#   p.get_canvas(False)
imvis.imshow(img)

line_style.line_width = 2
line_style.dash_pattern = []
p.draw_circle((300, 300), 40, line_style, vivi.colors.light_blue(0.4))
imvis.imshow(img)
```


```python
import vivi
# Draw something
p = vivi.Painter()
p.set_canvas_filename('examples/flamingo.jpg')
ls = vivi.LineStyle(4.2, (1, 0, 1, .8), [20, 20], vivi.LineStyle.Cap.Round)
fill = vivi.colors.Color(0, 1, 1, 0.6)
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




```
# install via apt: libcairo2-dev, ninja-build
# git clone --recursive https://github.com/snototter/vivi.git
# currently, we also need libopencv-dev
# python -m pip install git+https://github.com/snototter/vivi.git

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


nice-to-have / low prio:
json serialization of rect and other primitives


System packages:
cmake, python3, pip, venv
maybe eigen3 in the future

TODO
TODO
TODO vivi/colors.hpp --> color & color maps, maybe colornames
TODO vivi/primitives.hpp --> point, line, rect, bounding box, ...
TODO vivi/drawing.hpp --> painter
TODO
TODO



cmake
libcairo2-dev
nope##libeigen3-dev

for the examples (optional)
opencv

```cpp
/*
check rgb/bgr
https://stackoverflow.com/questions/66127550/python-cairo-save-figure-as-np-array-gives-weird-results

cpp style guide
https://google.github.io/styleguide/cppguide.html#Pointer_and_Reference_Expressions
r vs lvalue: ;-) https://stackoverflow.com/a/33829750/400948

pybind11 return numpy:
https://stackoverflow.com/questions/44659924/returning-numpy-arrays-via-pybind11

Image loading lightweight:
https://stackoverflow.com/a/40812978/400948
--> recommends https://github.com/nothings/stb


How to make a clean library with CMake
https://github.com/robotology/how-to-export-cpp-library/blob/master/CMakeLists.txt

CMake include other target
https://stackoverflow.com/a/61097014/400948


TODO repo structure
* examples/vivi-demo.cpp
* include/vivi/
* src/vivi
* tests / would be nice...


Cairo tutorials
https://www.cairographics.org/samples/
https://www.cairographics.org/tutorial/
https://www.cairographics.org/FAQ/#paint_from_a_surface
https://zetcode.com/gfx/cairo/cairobackends/

FindCairo.cmake taken from
https://github.com/preshing/CairoSample/blob/master/modules/FindCairo.cmake

opencv <--> eigen3
https://stackoverflow.com/questions/14783329/opencv-cvmat-and-eigenmatrix
*/
```

run
```
pip install -e .
pip install vito

python

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

from vito import imvis
import numpy as np
import vivi

# Dashed 4-px wide line
line_style = vivi.LineStyle(4, vivi.colors.salmon(0.8), [20, 15])

p = vivi.Painter()
p.set_canvas_rgb(800, 600, vivi.colors.lavender())
p.draw_line((10, 200), (600, 10), line_style)

# Get SHARED image buffer (changed by subsequent draw_... calls)
img = np.array(p.get_canvas(), copy=False)
imvis.imshow(img)

line_style.color = vivi.colors.midnight_blue()
line_style.line_cap = vivi.LineStyle.Cap.Round
line_style.dash_pattern = [50, 10, 10, 10]
p.draw_line((0, 0), (400, 400), line_style)

# Don't need to re-retrieve the canvas because we requested the
# shared buffer. This is the default behavior unless you use
#   p.get_canvas(False)
imvis.imshow(img)

line_style.line_width = 2
line_style.dash_pattern = []
p.draw_circle((300, 300), 40, line_style, vivi.colors.light_blue(0.4))
imvis.imshow(img)





c1 = vivi.colors.rgba(1, 0, 0.5)
assert c1 == c1.inverse().inverse()
c1.__getstate__()
c1.red -= 0.03
c1.as_RGBA()
c1.as_hex()

import pickle
data = pickle.dumps(c1)
data
c2 = pickle.loads(data)
c2
assert c1 == c2

# stuff below is broken (due to submodule colors)

vivi.Vec2d(1, 2, 3) # raises typeerror
v = vivi.Vec2d(1, 2)
v.x
v.x = 3
str(v)
v.y
v.z # attribute error


v2 = vivi.Vec2d(3, 4)
v.dot(v2)

v[0]
v[1]
v[2] # index error


import pickle
data = pickle.dumps(v)
vp = pickle.loads(data)
assert vp == v
assert v != v2

a = vivi.Vec3d(1, 0, 0)
b = vivi.Vec3d(0, 1, 0)
a.cross(b)


import vivi
import pickle
ls = vivi.LineStyle(3.9, vivi.colors.RGBA(200, 0, 200))
vivi.LineStyle(3.9, (1, 0, 1), [], vivi.LineStyle.Cap.Round)

ls1 = vivi.LineStyle(3.9, (1, 0, 1), [10, 30], vivi.LineStyle.Cap.Round, vivi.LineStyle.Join.Bevel)
data = pickle.dumps(ls1)
ls2 = pickle.loads(data)
ls1
assert ls1 == ls2
ls1.line_join = vivi.LineStyle.Join.Round
assert ls1 != ls2


import vivi
import pickle
r1 = vivi.Rect(40, 20, 80, 40, 45, 10)
r2 = vivi.Rect((40, 20, 80, 40, 45, 10))
assert r1 == r2
r1
data = pickle.dumps(r1)
r3 = pickle.loads(data)
assert r1 == r3
r1.cx = 50
assert r1 != r3


import vivi
from vito import pyutils as pu
pu.tic()
p = vivi.Painter()
p.set_canvas_rgb(400, 300, (1, 1, 1))
ls = vivi.LineStyle(4.2, (1, 0, 1, .8), [20, 20], vivi.LineStyle.Cap.Round)
fill = vivi.Color(0, 1, 1, 0.6)
p.draw_rect((200, 150, 50, 90, 3.6, 10), ls, fill)

#for i in range(100):
#    p.draw_rect((200, 150, 50, 90, i*3.6, 10), (4.2, (1, 0, 1, .2)), (0, 1, 1, 0.2))

pu.toc()
# 100 rects -> 15 to 16ms
# 1 rect -> 5 ms
p.show()



import vivi
p = vivi.Painter()
#p.set_canvas_rgb(400, 300, (1, 0, 1))
p.set_canvas_filename('examples/flamingo.jpg')
ls = vivi.LineStyle(4.2, (1, 0, 1, .8), [20, 20], vivi.LineStyle.Cap.Round)
fill = vivi.Color(0, 1, 1, 0.6)
p.draw_rect((200, 150, 50, 90, 3.6, 10), ls, fill)
p.show()

p.draw_line((10, 10), (600, 100), vivi.LineStyle(3.9, (0, 0, 0), [10, 30]))
p.draw_line((10, 200), (600, 10), (3.9, (0, 1, 1)))
p.draw_circle((100, 100), 30, (4, (0, 1, 1), [10, 10]), (0, 1, 1, 0.6))
p.show()


####################################################
# get canvas dev:

# Draw something
import vivi
p = vivi.Painter()
p.set_canvas_filename('examples/flamingo.jpg')
ls = vivi.LineStyle(4.2, (1, 0, 1, .8), [20, 20], vivi.LineStyle.Cap.Round)
fill = vivi.colors.Color(0, 1, 1, 0.6)
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


# 
import vivi
import numpy as np

# 1 channel
img_np = np.zeros((480, 640), dtype=np.uint8)
buf = vivi.ImageBuffer(img_np)
print(f'{buf}\n {buf.width} x {buf.height} x {buf.channels}, stride {buf.stride}, owns data: {buf.owns_data}')

# 3 channel
img_np = np.zeros((480, 640, 3), dtype=np.uint8)
buf = vivi.ImageBuffer(img_np)
print(f'{buf}\n {buf.width} x {buf.height} x {buf.channels}, stride {buf.stride}, owns data: {buf.owns_data}')


# Set image canvas from Python :)
# Canvas from grayscale
import vivi
import numpy as np
from vito import imvis
p = vivi.Painter()
img_np = np.zeros((480, 640), dtype=np.uint8)
img_np[:] = 120
b = vivi.ImageBuffer(img_np)
b
p.set_canvas_image(img_np)
p.draw_line((10, 200), (600, 10), (3.9, (0, 1, 1)))
canvas = np.array(p.get_canvas(), copy=False)
imvis.imshow(canvas)

# Canvas from RGB
import vivi
import numpy as np
from vito import imvis
p = vivi.Painter()
img_np = np.zeros((480, 640, 3), dtype=np.uint8)
img_np[:, :, 1] = 200
img_np[:, :, 2] = 200 # cyan
b = vivi.ImageBuffer(img_np)
p.set_canvas_image(b)
p.draw_line((10, 200), (600, 10), (3.9, (0, 0, 1), [20, 20]))
imvis.imshow(np.array(p.get_canvas(), copy=False))


# Canvas from RGBA
import vivi
import numpy as np
from vito import imvis
p = vivi.Painter()
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
ls = vivi.LineStyle(4.2, (1, 0, 1, .8), [20, 20], vivi.LineStyle.Cap.Round)
fill = vivi.Color(0, 1, 1, 0.6)
p.draw_rect((200, 150, 50, 90, 3.6, 10), ls, fill)

from vito import imvis
canvas = np.array(p.get_canvas(), copy=False)
imvis.imshow(canvas)
```



