# vivi
A visualization toolbox for (my) common computer vision tasks.

You already worked on [vitocpp](https://github.com/snototter/vitocpp), why yet another toolbox?  
Because vitocpp relies on OpenCV which is a) a quite heavy dependency and b) is for image processing (thus, offers only limited graphics capabilities).
In a nutshell, I wanted to have "something nice" for quickly visualizing my day-to-day results.
Marginalia: I also wanted to brush up on my C++ knowledge, refresh my CMake foo, dive into pybind11, etc.


## Roadmap
* An initial version v1.0 will hopefully released in July/August 2022
* I would NOT recommend using this library/toolbox before we reach v1.0 (don't say I didn't warn you)


## Features
I'm planning to support visualization of the following primitives/objects:
* [ ] 2D bounding boxes / Object detection results
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

