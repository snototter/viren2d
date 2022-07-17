.. include:: ../symbols.rst

-----------------
Overview & Basics
-----------------

**How to draw "stuff"?**
   Simply :ref:`initialize a painter's canvas<tutorial-painter-usage>` and use
   its ``draw_...`` methods.


**Which "stuff" can be drawn?**
   .. viren2d-drawing-summary::


**What about visualizations besides drawing?**
   Besides drawing, ``viren2d`` also provides various color maps for *scaled
   colorization* to inspect and represent data, see :func:`~viren2d.colorize`
   and :class:`~viren2d.Colorizer`.

   Suitable color maps can also be used to visualize *optical flow*
   via :func:`~viren2d.colorize_optical_flow` or discrete label images, *e.g.*
   segmentation results, via :func:`~viren2d.colorize_labels`.

   The :class:`~viren2d.ImageBuffer` provides some basic image processing
   functionalities, suitable to :meth:`~viren2d.ImageBuffer.blend` images,
   :meth:`~viren2d.ImageBuffer.pixelate` some :meth:`~viren2d.ImageBuffer.roi`,
   *etc.* Note, however, that these functionalities are mostly provided for
   user convenience and are not optimized towards high-speed, *i.e.*
   ``viren2d`` does not use `OpenMP <https://www.openmp.org/>`__,
   `oneTBB <https://github.com/oneapi-src/oneTBB>`__,
   `AVX <https://en.wikipedia.org/wiki/Advanced_Vector_Extensions>`__,
   `Neon <https://developer.arm.com/Architectures/Neon>`__
   or any other multi-threading/multi-processing extensions.


**Which coordinate system is used?**
   All coordinates assume an image coordinate system, where the origin
   ``(0, 0)`` is at the *top left* corner, the positive *x*-axis is
   pointing right and the positive *y*-axis is pointing down.


**How to specify angles?**
   All angle measurements should be provided in *degrees*. The rotation
   direction for positive angles is from the positive *x*-axis towards the
   positive *y*-axis.
