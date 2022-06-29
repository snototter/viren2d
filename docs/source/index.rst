.. viren2d documentation master file.

==============================================
Visualize Computer Vision Results Neatly in 2D
==============================================

.. include:: symbols.rst

|doc-status-badge| |license-badge|

   
.. warning::
   ``viren2d`` has not yet reached a stable `v1.0` release.
   
   An initial, stable/feature-complete version might be released in
   late autumn 2022 (this is only a hobby/leisure time project).
   I **do not recommend** using this toolbox before ``v1.0``.
   Until then, the API may and will change anytime, many features
   are still not properly tested, python bindings are
   incomplete, the documentation is a mess, *etc.*


``viren2d`` is my *Swiss Army knife* to visualize the results of common
computer vision tasks in 2D. It is designed to be swiftly integrated into
both **C++ and Python** environments.


**What is it good for?**
  This library is primarily designed to overlay *detections, trajectories,
  text, arrows* and more on image data. Additionally, it can be used as a
  simplistic drawing toolbox, if basic shapes are all you need.


**How to get started?**
   * Check the requirements and :ref:`install the library<installation>`.
   * Learn how to use the library by the examples in the :ref:`quickstart tutorial<quickstart>`.
   * Look up the :ref:`API documentation<api-documentation>`.
   * If you want to contribute, check the :ref:`development guide<dev-guide>`.


**What's under the hood?**
   The heavy lifting to ensure proper rendering of the visualizations is done
   via the `Cairo 2D graphics library <https://www.cairographics.org/>`_.

   Python bindings for the C++ library are realized via 
   `pybind11 <https://pybind11.readthedocs.io/>`__.


**How to pronounce viren2d?**
   "*vi-ren-2d*", with *vi* as in *vision* and *ren* as in *rendering*. This is
   by the way also how you pronounce the German word *Viren* (viruses). I might
   have been affected by the aftermath of the pandemic when picking a name...


**Which features are available?**
   Already implemented, documented & tested features in the following list are
   marked by |checked|. See also the up-to-date
   :ref:`list of implemented drawing functions<api-documentation>`.
   
   * |checked| Arcs
   * |checked| Arrows
   * |unchecked| Bounding Boxes (2D) - WIP (implemented, but not fully tested/documented)
   * |checked| Circles
   * |checked| Ellipses
   * |checked| Lines
   * |checked| Markers
   * |unchecked| Polygons - WIP (implemented, but not fully tested/documented)
   * |checked| Rectangles
   * |unchecked| Text (single-/multi-line) - WIP (implemented, but not fully tested/documented)
   * |unchecked| Text Boxes (single-/multi-line) - WIP (implemented, but not fully tested/documented)
   * |checked| Trajectories

**Which features are to come in the future?**

   * |unchecked| Anonymization
   * |unchecked| Bounding Boxes (3D)
   * |unchecked| Camera calibration-related: Ground plane, horizon, axes, ...
   * |unchecked| Image overlay
   * |unchecked| Pose estimation results
   * |unchecked| Pseudocoloring


.. toctree::
   :maxdepth: 2
   :hidden:

   installation
   quickstart
   api-documentation
   dev-guide
