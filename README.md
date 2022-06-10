# Computer Vision Results, but Nice-Looking
[![RTD Status](https://readthedocs.org/projects/viren2d/badge/?version=latest&style=flat)](https://viren2d.readthedocs.io)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/snototter/viren2d/blob/main/LICENSE?raw=true)

This is a [Cairo-based](https://www.cairographics.org/) visualization toolbox for common 2D computer vision results (read: *detections, trajectories, and the like*) in both __Python and C++__ environments.

## Roadmap
* An initial version v1.0 will be released around September/October 2022
* I would NOT recommend using this library/toolbox before we reach v1.0 (don't say I didn't warn you) - the API may change anytime (and I'm messing around in the main branch)

I'm planning to support visualization of the following primitives/objects:
* [x] Arcs
* [x] Arrows (single-/double-headed, filled/open, solid/dashed)
* [x] Circles
* [x] Ellipses
* [x] Lines
* [ ] Markers
* [x] Object detection results: 2D bounding boxes
* [ ] Object detection results: 3D bounding boxes - low priority feature (requires handling of camera calibration)
* [x] Polygons
* [x] Rectangles - from axis-aligned & box-shaped to rounded corners and rotated
* [x] Text (single-/multi-line) - only latin fonts are supported
* [x] Text boxes (single-/multi-line) - only latin fonts are supported
* [x] Tracking results: Trajectories
* [ ] Pose estimation results: needs a simple/easy-to-use interface s.t. users can define the pose graph themselves (too many different skeleton models to support them explicitly in viren2d) - low priority feature
* [ ] Image overlay (overlay/blending; optionally clipping, e.g. via circle/ellipse) - low priority feature
* [ ] Pseudocoloring: similar to `vcp` and `vito`; but requires extension of the ImageBuffer class to support floating point buffers
* [ ] Camera calibration-related: Ground plane, horizon - low priority features
* [ ] Nice-to-have: Curves & curved arrows - low priority


## Installation
### Prerequisites
* A C++ compiler supporting at least C++14
* [CMake](https://cmake.org/) >= 3.14 and a [compatible build tool](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html), like [Make](https://www.gnu.org/software/make/), [Ninja](https://ninja-build.org/), etc.
* The [Cairo](https://www.cairographics.org/download) 2D graphics library.  
  Currently, there is no plan to ship `viren2d` binaries. Thus, you need the Cairo development packages (`-dev` or `-devel`) for your system. For example, on GNU/Linux distributions, you simply install Cairo via:
  ```bash
  # Debian & derivatives (like Ubuntu)
  sudo apt install libcairo2-dev

  # Fedora
  sudo yum install cairo-devel

  # openSUSE
  zypper install cairo-devel
  ```
* If you want to build the Python bindings, you also need:
  * [Python](https://www.python.org/) >= 3.6
  * [pip](https://pypi.org/project/pip/) >= 10.0.0
  * Highly recommended: [virtualenv](https://pypi.org/project/virtualenv/)


TODO
documentation:
pip install sphinx sphinx-rtd-theme

### Dependencies
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
So far, `viren2d` has been tested on:  
* Ubuntu 20.04  
  Works out of the box
* Ubuntu 18.04  
  You need to install CMake via [Kitware's APT repository](https://apt.kitware.com/), because the default Ubuntu repository is stuck at v3.10.

There are, however, no platform-specific components in `viren2d` and all dependencies/build tools are available for Unix, Mac and Windows ([Cairo](https://www.cairographics.org/download/), [Ninja](https://ninja-build.org/), [CMake](https://cmake.org/), [python3](https://www.python.org/downloads/), any C++ compiler).  
Please let me know if you've set it up on any other platform, so I can update the install/setup instructions accordingly.  

**Watch out Windows users:**  
There's no official CMake configuration for Cairo and the one [included here](./cmake/FindCairo.cmake) uses a hard-coded search path (i.e. `${CMAKE_CURRENT_LIST_DIR}/libs/cairo-windows`) which will not match your install location. Adjust this path or use your own `FindCairo.cmake` (and please drop me a line or [create a PR](https://github.com/snototter/viren2d/pulls) to update/simplify the installation instructions for future users).


### Direct Installation
Requires a python package manager which supports [pyproject.toml](https://peps.python.org/pep-0518/), for example: `pip >= 10.0`
 ```bash
 # Set up a virtual environment with up-to-date pip:
 python3 -m venv venv
 source venv/bin/activate
 python -m pip install -U pip
 
 # Install viren2d
 python -m pip install git+https://github.com/snototter/viren2d.git
 ```


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

