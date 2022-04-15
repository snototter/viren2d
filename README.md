System packages:

TODO
TODO
TODO vivi/colors.hpp --> color & color maps, maybe colornames
TODO vivi/primitives.hpp --> point, line, rect, bounding box, ...
TODO vivi/drawing.hpp --> painter
TODO
TODO


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
```
