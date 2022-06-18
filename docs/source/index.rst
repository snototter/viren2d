.. viren2d documentation master file.

.. include:: symbols.rst
   
.. warning::
   ``viren2d`` has not yet reached a stable `v1.0` release. Currently,
   it is highly experimental and quite incomplete!
   
   An initial stable/almost-complete version might be released around
   September/October 2022 (this is only a hobby/leisure time project).
   I **do not recommend** using this toolbox before v1.0 - don't say I
   didn't warn you! Until then, the API may (and will) change anytime,
   most features are still not properly tested, python bindings are
   incomplete, the documentation is a mess, *etc.*


==============================================
Visualize Computer Vision Results Neatly in 2D
==============================================

``viren2d`` - *Visualize results not too disgraceful* - is my *Swiss Army
knife* to visualize the results of common 2D computer vision tasks in both
**C++ and Python** environments.


**What can you use it for?**
  You can use it to easily overlay *detections, trajectories, text,* and more
  on your image data. To get an idea of its capabilities, check the exemplary
  visualizations in the :ref:`quickstart tutorial<quickstart>`\ .


**What's under the hood?**
   To ensure proper rendering of your visualizations, ``viren2d`` relies on the
   `Cairo graphics library <https://www.cairographics.org/>`_.


**How do you pronounce viren2d?**
   "*vi-ren-2d*", with *vi* as in *vision* and *ren* as in *rendering*. This is
   by the way also how you pronounce the German word *Viren* (viruses). I might
   have been affected by the aftermath of the pandemic when picking a name...


**How do I get started?**
   * :ref:`Install the library<installation>`.
   * Browse the :ref:`quickstart tutorial<quickstart>`.
   * Check the :ref:`API documentation<api-documentation>`.
   * If you want to contribute, check the :ref:`development guide<dev-guide>`.


**Which features are available?**
   Already implemented, documented & tested features are marked by |checked|
   
   * |unchecked| Arcs
   * |unchecked| Arrows
   * |unchecked| Circles
   * |unchecked| Ellipses
   * |unchecked| Lines
   * |unchecked| Markers
   * |unchecked| Object detection results: 2D bounding boxes
   * |unchecked| Object detection results: 3D bounding boxes - low priority feature (requires handling of camera calibration)
   * |unchecked| Polygons
   * |unchecked| Rectangles
   * |unchecked| Text (single-/multi-line)
   * |unchecked| Text boxes (single-/multi-line)
   * |unchecked| Trajectories
   * |unchecked| Pose estimation results - low priority feature
   * |unchecked| Image overlay - low priority feature
   * |unchecked| Pseudocoloring - low priority feature
   * |unchecked| Camera calibration-related: Ground plane, horizon, axes - low priority features


.. toctree::
   :maxdepth: 2
   :hidden:

   installation
   quickstart
   api-documentation
   dev-guide

