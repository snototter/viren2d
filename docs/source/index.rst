.. viren2d documentation master file.

==============================================
Visualize Computer Vision Results Neatly in 2D
==============================================

.. include:: symbols.rst

|doc-status-badge| |license-badge|

   
.. warning::
   ``viren2d`` has not yet reached a stable `v1.0` release.
   
   A stable, mostly feature-complete version might be released in
   early 2023 (this is only a hobby project).
   I **do not recommend** using this toolbox before ``v1.0``.
   Until then, the API may and will change anytime, many features
   are still not properly tested, python bindings & documentation
   might be incomplete, *etc.*


`viren2d <https://github.com/snototter/viren2d>`__ is my *Swiss Army knife*
to visualize the results of common computer vision tasks in 2D. It is designed
to be swiftly integrated into both **C++ and Python** environments.


**What is it good for?**
   This library is designed to overlay *detections, trajectories, hand & body
   poses* and more on image data. Additionally, it provides several
   colorization options to represent data by false colors, useful to inspect
   or highlight structure and features in your data more easily.
   Finally, it can be used as a simplistic drawing toolbox, if basic shapes are
   all you need.


**How to get started?**
   * Check the :ref:`requirements and install the library<installation>`.
   * Learn how to use the library by the examples in the :ref:`quickstart tutorial<quickstart>`.
   * Look up the :ref:`API documentation<api-documentation>`.
   * If you want to contribute, check the :ref:`development guide<dev-guide>`.


**What's under the hood?**
   The heavy lifting to ensure proper rendering of the visualizations is done
   via the `Cairo 2D graphics library <https://www.cairographics.org/>`_.

   Linear algebra needed to support 3D |right-arrow| 2D projections is handled
   via `Eigen <https://eigen.tuxfamily.org/>`__.

   Python bindings are realized via `pybind11 <https://pybind11.readthedocs.io/>`__.


**How to pronounce viren2d?**
   "*vi-ren-2d*", with *vi* as in *vision* and *ren* as in *rendering*. This is
   by the way also how you pronounce the German word *Viren* (viruses). I might
   have been affected by the aftermath of the pandemic when picking a name...


**Which features are available?**
   Already implemented, documented & tested features in the following list are
   marked by |checked|. See also the up-to-date
   :ref:`list of drawing functions<api-documentation-drawing-summary>`.
   
   * |checked| Drawing Arcs
   * |checked| Drawing Arrows
   * |checked| Drawing Bounding Boxes (2D)
   * |checked| Drawing Circles
   * |unchecked| Drawing Coordinate System Axes - WIP (implemented, but not tested/documented)
   * |checked| Drawing Ellipses
   * |checked| Drawing Lines
   * |checked| Drawing Markers
   * |checked| Drawing Polygons
   * |checked| Drawing Rectangles
   * |checked| Drawing Text (single-/multi-line)
   * |checked| Drawing Text Boxes (single-/multi-line)
   * |checked| Drawing Trajectories
   * |unchecked| Image Overlay - WIP (implemented, but not tested/documented)
   * |checked| Label Image Colorization
   * |checked| Optical Flow Colorization
   * |checked| Pixelation
   * |unchecked| Relief Shading - WIP (implemented, but not tested)
   * |checked| Scaled Colorization (Pseudo/False Colors)

**Which features will be added in the future?**

   * |unchecked| Drawing Bounding Boxes (3D)
   * |unchecked| Camera calibration-related: Ground plane, horizon, axes, ...   
   * |unchecked| Drawing Body/Hand Poses


.. toctree::
   :maxdepth: 2
   :hidden:

   installation
   quickstart
   api-documentation
   dev-guide
