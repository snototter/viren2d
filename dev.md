# TODO
* readme - install/setup section
* readme - quickstart
* readme - make github banner with viren2d ;-)
* TODO test - via pytest
* separate example/tutorial doc

TODO rename to viren2d  



## Examples

Example code requires python packages `vito` and `numpy`
```python
from vito import imvis
import numpy as np
import viren2d

# Dashed 4-px wide line
line_style = viren2d.LineStyle(4, viren2d.colors.salmon(0.8), [20, 15])

p = viren2d.Painter()
p.set_canvas_rgb(800, 600, viren2d.colors.lavender())
p.draw_line((10, 200), (600, 10), line_style)

# Get SHARED image buffer (changed by subsequent draw_... calls)
img = np.array(p.get_canvas(), copy=False)
imvis.imshow(img)

line_style.color = viren2d.colors.midnight_blue()
line_style.line_cap = viren2d.LineStyle.Cap.Round
line_style.dash_pattern = [50, 10, 10, 10]
p.draw_line((0, 0), (400, 400), line_style)

# Don't need to re-retrieve the canvas because we requested the
# shared buffer. This is the default behavior unless you use
#   p.get_canvas(False)
imvis.imshow(img)

line_style.line_width = 2
line_style.dash_pattern = []
#p.draw_circle((300, 300), 40, line_style, viren2d.colors.light_blue(0.4))
p.draw_arc((300, 300), 40, 0, 200, line_style, viren2d.colors.light_blue(0.4))
imvis.imshow(img)

```



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




```
# install via apt: libcairo2-dev, ninja-build
# git clone --recursive https://github.com/snototter/viren2d.git
# currently, we also need libopencv-dev
# python -m pip install git+https://github.com/snototter/viren2d.git

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
TODO viren2d/colors.hpp --> color & color maps, maybe colornames
TODO viren2d/primitives.hpp --> point, line, rect, bounding box, ...
TODO viren2d/drawing.hpp --> painter
TODO
TODO


apt package for cpp stuff
https://earthly.dev/blog/creating-and-hosting-your-own-deb-packages-and-apt-repo/

python packaging (try build instead of setuptools)
https://packaging.python.org/en/latest/guides/publishing-package-distribution-releases-using-github-actions-ci-cd-workflows/








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
* examples/viren2d-demo.cpp
* include/viren2d/
* src/viren2d
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
import viren2d

# Dashed 4-px wide line
line_style = viren2d.LineStyle(4, viren2d.colors.salmon(0.8), [20, 15])

p = viren2d.Painter()
p.set_canvas_rgb(800, 600, viren2d.colors.lavender())
p.draw_line((10, 200), (600, 10), line_style)

# Get SHARED image buffer (changed by subsequent draw_... calls)
img = np.array(p.get_canvas(), copy=False)
imvis.imshow(img)

line_style.color = viren2d.colors.midnight_blue()
line_style.line_cap = viren2d.LineStyle.Cap.Round
line_style.dash_pattern = [50, 10, 10, 10]
p.draw_line((0, 0), (400, 400), line_style)

# Don't need to re-retrieve the canvas because we requested the
# shared buffer. This is the default behavior unless you use
#   p.get_canvas(False)
imvis.imshow(img)

line_style.line_width = 2
line_style.dash_pattern = []
#p.draw_circle((300, 300), 40, line_style, viren2d.colors.light_blue(0.4))
p.draw_arc((300, 300), 40, 0, 200, line_style, viren2d.colors.light_blue(0.4))
imvis.imshow(img)





c1 = viren2d.colors.rgba(1, 0, 0.5)
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

viren2d.Vec2d(1, 2, 3) # raises typeerror
v = viren2d.Vec2d(1, 2)
v.x
v.x = 3
str(v)
v.y
v.z # attribute error


v2 = viren2d.Vec2d(3, 4)
v.dot(v2)

v[0]
v[1]
v[2] # index error


import pickle
data = pickle.dumps(v)
vp = pickle.loads(data)
assert vp == v
assert v != v2

a = viren2d.Vec3d(1, 0, 0)
b = viren2d.Vec3d(0, 1, 0)
a.cross(b)


import viren2d
import pickle
ls = viren2d.LineStyle(3.9, viren2d.colors.RGBA(200, 0, 200))
viren2d.LineStyle(3.9, (1, 0, 1), [], viren2d.LineStyle.Cap.Round)

ls1 = viren2d.LineStyle(3.9, (1, 0, 1), [10, 30], viren2d.LineStyle.Cap.Round, viren2d.LineStyle.Join.Bevel)
data = pickle.dumps(ls1)
ls2 = pickle.loads(data)
ls1
assert ls1 == ls2
ls1.line_join = viren2d.LineStyle.Join.Round
assert ls1 != ls2


import viren2d
import pickle
r1 = viren2d.Rect(40, 20, 80, 40, 45, 10)
r2 = viren2d.Rect((40, 20, 80, 40, 45, 10))
assert r1 == r2
r1
data = pickle.dumps(r1)
r3 = pickle.loads(data)
assert r1 == r3
r1.cx = 50
assert r1 != r3


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



import viren2d
p = viren2d.Painter()
#p.set_canvas_rgb(400, 300, (1, 0, 1))
p.set_canvas_filename('examples/flamingo.jpg')
ls = viren2d.LineStyle(4.2, (1, 0, 1, .8), [20, 20], viren2d.LineStyle.Cap.Round)
fill = viren2d.Color(0, 1, 1, 0.6)
p.draw_rect((200, 150, 50, 90, 3.6, 10), ls, fill)
p.show()

p.draw_line((10, 10), (600, 100), viren2d.LineStyle(3.9, (0, 0, 0), [10, 30]))
p.draw_line((10, 200), (600, 10), (3.9, (0, 1, 1)))
p.draw_circle((100, 100), 30, (4, (0, 1, 1), [10, 10]), (0, 1, 1, 0.6))
p.show()


####################################################
# get canvas dev:

# Draw something
import viren2d
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


# 
import viren2d
import numpy as np

# 1 channel
img_np = np.zeros((480, 640), dtype=np.uint8)
buf = viren2d.ImageBuffer(img_np)
print(f'{buf}\n {buf.width} x {buf.height} x {buf.channels}, stride {buf.stride}, owns data: {buf.owns_data}')

# 3 channel
img_np = np.zeros((480, 640, 3), dtype=np.uint8)
buf = viren2d.ImageBuffer(img_np)
print(f'{buf}\n {buf.width} x {buf.height} x {buf.channels}, stride {buf.stride}, owns data: {buf.owns_data}')


# Set image canvas from Python :)
# Canvas from grayscale
import viren2d
import numpy as np
from vito import imvis
p = viren2d.Painter()
img_np = np.zeros((480, 640), dtype=np.uint8)
img_np[:] = 120
b = viren2d.ImageBuffer(img_np)
b
p.set_canvas_image(img_np)
p.draw_line((10, 200), (600, 10), (3.9, (0, 1, 1)))
canvas = np.array(p.get_canvas(), copy=False)
imvis.imshow(canvas)

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



