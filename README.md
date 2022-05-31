# Computer Vision Results, but Nice-Looking
This is a light-weight toolbox to easily visualize common 2D computer vision results (read: *detections, trajectories, and the like*) in both __Python and C++__ environments.
Under the hood, `viren2d` uses the [Cairo graphics library](https://www.cairographics.org/), so visually unpleasing results will hopefully only be caused by questionable color/style choices :wink:.


## Roadmap
* An initial version v1.0 will hopefully released in September 2022 (this is a hobby/leisure time project)
* I would NOT recommend using this library/toolbox before we reach v1.0 (don't say I didn't warn you) - the API may change anytime (and I'm messing around in the main branch)

I'm planning to support visualization of the following primitives/objects:
* [x] Arcs
* [x] Arrows (single-/double-headed, filled/open, solid/dashed)
* [x] Circles
* [x] Ellipses
* [x] Lines
* [ ] Markers
* [ ] Object detection results: 2D bounding boxes
* [ ] Object detection results: 3D bounding boxes - low priority feature (requires handling of camera calibration)
* [ ] Polygons
* [x] Rectangles - from axis-aligned & box-shaped to rounded corners and rotated
* [x] Text (single-/multi-line) - only latin fonts are supported
* [x] Text boxes (single-/multi-line) - only latin fonts are supported
* [ ] Tracking results: Trajectories (plain paths vs fading effect)
* [ ] Pose estimation results: needs a simple/easy-to-use interface s.t. users can define the pose graph themselves (too many different skeleton models to support them explicitly in viren2d) - low priority feature
* [ ] Image overlay (overlay/blending; optionally clipping, e.g. via circle/ellipse) - low priority feature
* [ ] Pseudocoloring: similar to `vcp` and `vito`; but requires extension of the ImageBuffer class to support floating point buffers
* [ ] Camera calibration-related: Ground plane, horizon - low priority features
* [ ] Nice-to-have: Curves & curved arrows - low priority


## Description
**Why yet another toolbox?**  
Frequently switching between different tasks and frameworks, looking up how to visualize results became really tiring.
Ideally, I wanted something that can be easily used both within C++ and Python environments as I have to switch between these two regularly.

Previously, I experimented with a [similar toolbox (`vcp`)](https://github.com/snototter/vitocpp/) based on [OpenCV](https://github.com/opencv/opencv), but this has a few drawbacks (considering *ease-of-use*):  
*a)* OpenCV is a quite heavy dependency - plus, did you ever get to enjoy a version mismatch between the [(unofficial) Python bindings](https://pypi.org/project/opencv-python/) in your virtual environment and your system's OpenCV library?  
*b)* OpenCV is an image processing library, thus it offers only limited drawing capabilities, and  
*c)* my `vcp` framework grew over the years (and suffers from the usual *"I will clean this mess up later"* quick-and-dirty extensions).

**So here comes `viren2d`**, a toolbox with only a single purpose: easily & quickly create nice-looking visualizations.

**How to pronounce viren2d?**  
**"vi-ren-2d"**, with **vi** as in *vision*, plus **ren** as in *rendering*  
This is by the way also how you pronounce the German word *Viren* (viruses). I could have been affected by the aftermath of the pandemic when picking a name...

Marginalia: Another reason to work on this library was that I wanted to brush up on my C++ knowledge, refresh my CMake foo, dive deeper into pybind11, etc.

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

