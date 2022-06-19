.. viren2d documentation master file.

==============================================
Visualize Computer Vision Results Neatly in 2D
==============================================

|doc-status-badge| |license-badge|

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
   
   * |checked| Arcs
   * |checked| Arrows
   * |checked| Circles
   * |checked| Ellipses
   * |checked| Lines
   * |checked| Markers
   * |unchecked| Object detection results: 2D bounding boxes - WIP: untested
   * |unchecked| Object detection results: 3D bounding boxes (requires handling of camera calibration)
   * |unchecked| Polygons - WIP: untested
   * |checked| Rectangles
   * |unchecked| Text (single-/multi-line) - WIP: untested
   * |unchecked| Text boxes (single-/multi-line) - WIP: untested
   * |unchecked| Trajectories - WIP: untested
   * |unchecked| Camera calibration-related: Ground plane, horizon, axes - low priority features
   * |unchecked| Pose estimation results
   * |unchecked| Image overlay - low priority feature
   * |unchecked| Pseudocoloring - low priority feature


.. toctree::
   :maxdepth: 2
   :hidden:

   installation
   quickstart
   api-documentation
   dev-guide


.. |doc-status-badge| image:: https://readthedocs.org/projects/viren2d/badge/?version=latest&style=flat
    :alt: Documentation Status
    :target: https://viren2d.readthedocs.io
..    :scale: 100%

.. |license-badge| image:: https://img.shields.io/badge/license-MIT-blue.svg
    :alt: License MIT
    :target: https://github.com/snototter/viren2d/blob/main/LICENSE?raw=true
..    :scale: 100%

