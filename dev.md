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
TODO link to list of keywords: https://docs.python.org/3.8/reference/lexical_analysis.html#keywords
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
   


## Installation
TODO add to dev documentation:

* how to set up sphinx (requirements in docs/)

### Dependencies
TODO add to rtd

* [pybind11](https://github.com/pybind/pybind11) TODO doc; header only; submodule
* [spdlog](https://github.com/gabime/spdlog)  
  TODO doc; submodule; several variants (packaged; header only; installed) - recommended: installed static lib; TODO doc install  
  https://stackoverflow.com/a/29882995/400948  
  ensure that you are on branch v1.x!
  ```bash
  cd libs/spdlog
  mkdir build && cd build
  cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
  cmake --build .
  # Install location defaults to /usr/local
  # If you change the CMAKE_INSTALL_PREFIX, ensure that viren2d's CMakeLists.txt will
  # find the installed spdlog library (i.e. it needs to locate the spdlogConfig* files
  # which will be installed at /usr/local/lib/cmake/spdlog/)
  sudo cmake --install .
  ```
* [stb](https://github.com/nothings/stb) TODO doc; header only; submodule

### Platforms known to work
TODO add to rtd

So far, `viren2d` has been tested on:  
* Ubuntu 20.04  
  Works out of the box
* Ubuntu 18.04  
  You need to install CMake via [Kitware's APT repository](https://apt.kitware.com/), because the default Ubuntu repository is stuck at v3.10.

There are, however, no platform-specific components in `viren2d` and all dependencies/build tools are available for Unix, Mac and Windows ([Cairo](https://www.cairographics.org/download/), [Ninja](https://ninja-build.org/), [CMake](https://cmake.org/), [python3](https://www.python.org/downloads/), any C++ compiler).  
Please let me know if you've set it up on any other platform, so I can update the install/setup instructions accordingly.  

**Watch out Windows users:**  
There's no official CMake configuration for Cairo and the one [included here](./cmake/FindCairo.cmake) uses a hard-coded search path (i.e. `${CMAKE_CURRENT_LIST_DIR}/libs/cairo-windows`) which will not match your install location. Adjust this path or use your own `FindCairo.cmake` (and please drop me a line or [create a PR](https://github.com/snototter/viren2d/pulls) to update/simplify the installation instructions for future users).

TODO this will be deprecated once we prepare for conda release



### Build from Source
Clone this repository recursively to set up the external libraries.
```bash
git clone --recursive https://github.com/snototter/viren2d.git
cd viren2d
```

To build and install the Python library (assuming you already activated your virtual environment), simply run
```bash
python -m pip install .
```
(This will automatically build any C++ dependencies)

To build and install the C++ library instead, follow the standard CMake pipeline:
```bash
mkdir build && cd build
# Configure
cmake ..

# Build
cmake --build .

# Install (Note that viren2d sets a local CMAKE_INSTALL_PREFIX)
cmake --build . --target install
```

**Note for Python users:**  
If you want to re-install `viren2d` and run into a Ninja build error (Ninja binary not found in temporary directory), you should delete the build cache first:
```bash
rm -r build/temp.*
python -m pip install .
```


# TODOs
* [ ] Use spdlog; set library-global debug level via interface function
  * doc cmake options for cpp
  * if installed via pip:
    ```bash
    CMAKE_ARGS="-Dviren2d_LOG_LEVEL=trace" python -m pip install .
    ```
  * in python, you have to call viren2d.set_log_level('debug')!




## Examples

TODO set up/build/run:
```
# install via apt: libcairo2-dev, ninja-build
# git clone --recursive https://github.com/snototter/viren2d.git
# python -m pip install git+https://github.com/snototter/viren2d.git
# optional: libopencv-dev

cd build
make -j && ./demo2d

#vs python

python -m pip install .
# if this fails with /tmp/..../ninja not found:
rm -r build/temp.linux-x86_64-cpython-38/
python -m pip install .
## or don't use ninja (much slower):
#export CMAKE_GENERATOR="Unix Makefiles"
#python -m pip install .
```

python packaging (try build instead of setuptools)
https://packaging.python.org/en/latest/guides/publishing-package-distribution-releases-using-github-actions-ci-cd-workflows/


```cpp
r vs lvalue: ;-) https://stackoverflow.com/a/33829750/400948

Image loading lightweight: https://stackoverflow.com/a/40812978/400948
--> that's why I switched to https://github.com/nothings/stb

TODO
FindCairo.cmake taken from
https://github.com/preshing/CairoSample/blob/master/modules/FindCairo.cmake

opencv <--> eigen3
https://stackoverflow.com/questions/14783329/opencv-cvmat-and-eigenmatrix
```



```
# compare library sizes (apt)
# see https://unix.stackexchange.com/a/44087

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
```



