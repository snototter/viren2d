System packages:

TODO
TODO
TODO vivi/colors.hpp --> color & color maps, maybe colornames
TODO vivi/primitives.hpp --> point, line, rect, bounding box, ...
TODO vivi/drawing.hpp --> painter
TODO
TODO

export CMAKE_GENERATOR="Unix Makefiles"
pip install .
or rm -r build/temp...


cmake
libcairo2-dev
nope##libcairomm-1.0-dev
nope##libeigen3-dev

for the examples:
opencv

structure:::
src/drawing/text.cpp

```cpp
/*
check rgb/bgr
https://stackoverflow.com/questions/66127550/python-cairo-save-figure-as-np-array-gives-weird-results
opencv memory layout:
https://docs.opencv.org/4.x/db/da5/tutorial_how_to_scan_images.html

cpp style guide
https://google.github.io/styleguide/cppguide.html#Pointer_and_Reference_Expressions
r vs lvalue: ;-) https://stackoverflow.com/a/33829750/400948

pybind11 return numpy:
https://stackoverflow.com/questions/44659924/returning-numpy-arrays-via-pybind11

Image loading lightweight:
https://stackoverflow.com/a/40812978/400948
--> recommends https://github.com/nothings/stb

Logging:
https://github.com/gabime/spdlog

Clean library with CMake
https://github.com/robotology/how-to-export-cpp-library/blob/master/CMakeLists.txt

CMake set up header only library:
https://stackoverflow.com/questions/28629084/how-to-get-a-header-only-library-portably-from-version-control-using-cmake

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

FindCairo.cmake
https://github.com/preshing/CairoSample/blob/master/modules/FindCairo.cmake

opencv <--> eigen3
https://stackoverflow.com/questions/14783329/opencv-cvmat-and-eigenmatrix
*/
```


TODO git submodule add ... libs/stb
https://github.com/nothings/stb


run
```
pip install -e .

python

import vivi
c1 = vivi.rgba(1, 0, 0.5)
c1 == c1.inverse().inverse()
c1.__getstate__()
c1.red += 0.001
c1.as_RGBA()
c1.as_hex()

import pickle
data = pickle.dumps(c1)
data
c2 = pickle.loads(data)
c2


vivi.Vec2d(1, 2, 3)
v = vivi.Vec2d(1, 2)
v.x
v.x = 3
str(v)
v.y
v.z


v2 = vivi.Vec2d(3, 4)
v.dot(v2)

v[0]
v[1]
v[2]

import pickle
data = pickle.dumps(v)
vp = pickle.loads(data)
vp == v
v == v2

a = vivi.Vec3d(1, 0, 0)
b = vivi.Vec3d(0, 1, 0)
a.cross(b)



import vivi
import pickle
ls = vivi.LineStyle(3.9, vivi.RGBA(200, 0, 200))
vivi.LineStyle(3.9, (1, 0, 1))
ls = vivi.LineStyle(3.9, (1, 0, 1), [10, 30])
data = pickle.dumps(ls)
ls
des = pickle.loads(data)
des == ls


import vivi
import pickle
r1 = vivi.Rect(40, 20, 80, 40, 45, 10)
r2 = vivi.Rect((40, 20, 80, 40, 45, 10))
r1 == r2
r1
data = pickle.dumps(r1)
r3 = pickle.loads(data)
r1 == r3
r1.cx = 50
r1 == r3


import vivi
from vito import pyutils as pu
pu.tic()
p = vivi.Painter()
p.set_canvas_rgb(400, 300, (1, 1, 1))
#for i in range(100):
#    p.draw_rect((200, 150, 50, 90, i*3.6, 10), (4.2, (1, 0, 1, .2)), (0, 1, 1, 0.2))
p.draw_rect((200, 150, 50, 90, 3.6, 10), (4.2, (1, 0, 1, .2)), (0, 1, 1, 0.2))

pu.toc()
# 100 rects -> 15 to 16ms
# 1 rect -> 5 ms
p.show()



import vivi
p = vivi.Painter()
p.set_canvas_rgb(400, 300, (1, 0, 1))
p.draw_line((10, 10), (600, 100), vivi.LineStyle(3.9, (0, 0, 0), [10, 30]))
p.draw_line((10, 200), (600, 10), (3.9, (0, 1, 1)))
p.show()
```



