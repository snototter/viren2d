# viren2d - Vision & Rendering 2D
This is a light-weight 2D rendering toolbox to easily (and aesthetically) visualize common computer vision results (read: *detections, trajectories, and the like*) in both __Python and C++__ environments.

This toolbox uses the [Cairo graphics library](https://www.cairographics.org/) under the hood.

**Why yet another toolbox?**  
I'm too tired of/too lazy to look up how to visualize some results within the software framework I currently have to work with.
Ideally, I wanted something that can be easily used both within C++ and Python settings - as I have to switch between these two regularly.

Previously, I experimented with a [similar toolbox (`vcp`)](https://github.com/snototter/vitocpp/) based on [OpenCV](https://github.com/opencv/opencv), but this has a few drawbacks (considering *ease-of-use*):  
*a)* OpenCV is a quite heavy dependency (plus, did you ever get to enjoy a version mismatch between (unofficial) Python bindings in your virtualenv and your system's OpenCV library?)  
*b)* it offers only limited drawing capabilities (it's an image processing library, thus not focusing on graphics pleasing to the eye), and  
*c)* my `vcp` framework grew over the years (and suffers from the usual *"I will clean this mess up later"* bad coding habbit).

So here comes **`viren2d`**, a toolbox with only a single purpose: easily create aesthetically pleasing visualizations.
* Lightweight: `viren2d` (TODO add python pkg size + cpp library size) uses `libcairo2` (567 KB TODO check - apt unzipped to 20mb (packages weren't isolated, though)
* Ease-of-use: TODO refer to examples/tutorial
* Maintainability (hopefully)

Note on efficiency: I preferred code readability over efficiency. Nevertheless, compared to my previous attempt using OpenCV, the switch to Cairo immediately paid off (despite using CPU-only in-memory image surface rendering, which is by far the slowest usage of Cairo).

Marginalia: I also wanted to brush up on my C++ knowledge, refresh my CMake foo, dive into pybind11, etc.

**How to pronounce viren2d?**  
"vi" as in "vision", "ren" as in "rendering": **"vi-ren-2d"**
This is by the way also how you pronounce the German word for viruses. I could have been affected by the aftermath of the pandemic when picking a name.



## Installation
**Supported platforms**:
So far, `viren2d` has been tested on: Ubuntu 18.04 & 20.04
todo python3, cairo, ninja, cmake should work on almost all platforms

let me know if you set it up on any other systems, so I can update the install/setup instructions accordingly: 

### Prerequisites
TODO add usage requirements (not dev versions)

install via apt: libcairo2-dev, ninja-build, cmake>3.12 (todo check or was it 3.15?)

### Direct Installation
Requires a python package manager which supports [pyproject.toml](https://peps.python.org/pep-0518/), for example: `pip >= 10.0`
 ```bash
 # Set up a virtualenv:
 python3 -m venv venv
 source venv/bin/activate
 python -m pip install -U pip

 > python -m pip install git+https://github.com/snototter/viren2d.git
 ```
### Build from Source
```bash
# Clone recursively to set up the external libraries

git clone --recursive https://github.com/snototter/viren2d.git
cd viren2d
TODO c++ vs python

python -m pip install .
```

Note, if you want to re-install it, you may have to delete the CMake cache first (as it points to a Ninja binary, which is located within /tmp).
```bash
rm -r build/temp.*
python -m pip install .
```

## Roadmap
* An initial version v1.0 will hopefully released in July/August 2022
* I would NOT recommend using this library/toolbox before we reach v1.0 (don't say I didn't warn you)


## Features
I'm planning to support visualization of the following primitives/objects:
* [ ] 2D bounding boxes / Object detection results
* [x] Arcs
* [ ] Arrows
* [x] Circles
* [ ] Ellipses
* [x] Lines
* [ ] Markers
* [ ] Polygons
* [x] Rectangles - from axis-aligned & box-shaped to rounded corners and rotated
* [ ] Text (simple)
* [ ] Trajectories / Tracking results (plain paths vs fading effect)
* [ ] Poses (e.g. human pose estimation results) - low priority feature
* [ ] Image overlay
* [ ] Pseudocoloring - depends on how I will integrate image support (external, lightweight library; if it supports different data types, pseudocoloring is on the feature list)
* [ ] Camera calibration-related: Ground plane, horizon, 3D bounding boxes (projected onto 2D image/canvas)

