# Computer Vision Results, but Nice-Looking
This is a light-weight toolbox to easily visualize common 2D computer vision results (read: *detections, trajectories, and the like*) in both __Python and C++__ environments.
Under the hood, `viren2d` uses the [Cairo graphics library](https://www.cairographics.org/), so visually unpleasing results will hopefully only be caused by questionable color/style choices :wink:.


## Roadmap
* An initial version v1.0 will hopefully released in September 2022 (this is a hobby/leisure time project)
* I would NOT recommend using this library/toolbox before we reach v1.0 (don't say I didn't warn you) - the API may change anytime (and I'm messing around in the main branch)


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
TODO doc  
install via apt: libcairo2-dev, ninja-build, cmake>3.14  
(don't plan on shipping binaries, thus everyone needs libcairo2-dev)


### Platforms known to work
So far, `viren2d` has been tested on:  
* Ubuntu 20.04  
  Works out of the box (TODO test with usage requirements instead of dev)
* Ubuntu 18.04  
  You need to install CMake via [Kitware's APT repositor](https://apt.kitware.com/), because the default Ubuntu repository is stuck at CMake 3.10.

There are, however, no platform-specific components in `viren2d` and all dependencies/build tools are available for Unix, Mac and Windows ([Cairo](https://www.cairographics.org/download/), [Ninja](https://ninja-build.org/), [CMake](https://cmake.org/), [python3](https://www.python.org/downloads/), any C++ compiler).  
Please let me know if you've set it up on any other platform, so I can update the install/setup instructions accordingly.  

**Watch out Windows users:**  
There's no official CMake configuration for Cairo and the one [included here](./cmake/FindCairo.cmake) uses a hard-coded path (i.e. `${CMAKE_CURRENT_LIST_DIR}/libs/cairo-windows`) which will not match your install location. Adjust this path, or use your own `FindCairo.cmake` and please drop me a line (or PR) to update/simplify installation for future users.


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
Clone this repository recursively to set up the external libraries. Then follow the standard CMake pipeline. In a nutshell:
```bash
git clone --recursive https://github.com/snototter/viren2d.git
cd viren2d


###########################################
# Install the Python library
###########################################
python -m pip install .


###########################################
# Build & install the C++ library locally
###########################################
mkdir build && cd build
cmake ..
cmake --build .
cmake --build . --target install
```

**Note for Python users:**  
If you want to re-install viren2d and run into a Ninja build error (Ninja binary not found in temporary directory), you should delete the CMake cache first:
```bash
rm -r build/temp.*
python -m pip install .
```


## Features
I'm planning to support visualization of the following primitives/objects:
* [ ] 2D bounding boxes / Object detection results
* [x] Arcs
* [x] Arrows (single-/double-headed, filled/open, solid/dashed)
* [x] Circles
* [ ] Ellipses
* [x] Lines
* [ ] Markers
* [ ] Polygons
* [x] Rectangles - from axis-aligned & box-shaped to rounded corners and rotated
* [ ] Textboxes (simple, i.e. with Cairo backend; text w/ and w/o colorized background)
* [ ] Trajectories / Tracking results (plain paths vs fading effect)
* [ ] Poses (e.g. human pose estimation results) - low priority feature
* [ ] Image overlay (overlay/blending; clipping, e.g. circle/ellipse)
* [ ] Pseudocoloring: similar to `vcp` and `vito`; but requires extension of the ImageBuffer class to support floating point buffers
* [ ] Camera calibration-related: Ground plane, horizon, 3D bounding boxes (projected onto 2D image/canvas)


# TODOs
* [ ] Use spdlog; set library-global debug level via interface function
* [ ] Add user-adjustable default styles (LineStyle, ArrowStyle, etc.)
* [ ] Nice-to-have: Add colormaps & pseudocoloring (low priority, v2.0?)
* [ ] Nice-to-have: Color space conversion
* [ ] Nice-to-have: Curves & curved arrows


