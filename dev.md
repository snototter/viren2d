# TODO List

Tasks in preparation of first stable release:
* Task list for styles:
  * [ ] Implement ToDetailedString() for each style.  
    To be used in exceptions + exposed in Python API as `detailed_str`.
  * [ ] Finalize RTD documentation, i.e. add reference/cheat sheet for each style.
* Build pipeline:
  * [ ] Refactor CMakeLists - use default paths (as e.g. in `wzk`)
  * [ ] Adjust CMakeLists: fetch all dependencies (incl. spdlog and stb).
  * [ ] Remove spdlog and stb submodules
  * [ ] Include backup/fallback snapshots of the 3 major dependencies, i.e. spdlog, stb and pybind11 (not needed as long as they're actively maintained)
* Tests:
  * restructure!
* Examples:
  * restructure, see ``examples/hello-world-cpp`` (clear and concise)


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


# Utility Scrips
## Convert Colormaps
```python
from vito import colormaps
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib import cm


def cmap2viren(cname: str, cmap: list, per_row: int = 4):
    idx = 0
    cmap_str = ''
    for r, g, b in cmap:
        if idx % per_row == 0:
            cmap_str += '\n  '
        cmap_str += f'RGBColor({r:3d}, {g:3d}, {b:3d})'
        idx += 1
        if idx < len(cmap):
            cmap_str += ', '
    print(f"""
constexpr RGBColor kColorMap{cname}[] = {{{cmap_str}\n}};
constexpr std::size_t kBins{cname} = sizeof(kColorMap{cname}) / sizeof(kColorMap{cname}[0]);
    """);
    

def mpl2viren(cname: str, per_row: int = 4):
    try:
        mpl_map = plt.get_cmap(cname.lower())
    except:
        mpl_map = plt.get_cmap(cname)
    to_sample = np.linspace(1, mpl_map.N, num=256, endpoint=True) - 1
    cmap = [mpl_map(i) for i in to_sample.astype(np.int32)]
    cmap = [(int(255*r), int(255*g), int(255*b)) for r,g,b,_ in cmap]
    cmap2viren(cname, cmap, per_row)


# vito2vi
for cname in colormaps.colormap_names:
    cmap = Colormaps.by_name(cname, return_rgb=True)
    cmap2viren(cname, cmap)
    print()
    print()
    
# mpl2vi
cnames = ['Spring', 'Summer', 'Autumn', 'Winter', 'Bone', 'Plasma', 'Spectral', 'Purples', 'Blues', 'Oranges', 'Reds']
cname = 'nipy_Spectral'
mpl2viren(cname)


```

