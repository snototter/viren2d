.. include:: ../symbols.rst

---------------
Know the Basics
---------------

**How to draw "stuff"?**
   :ref:`Initialize a painter's canvas<tutorial-painter-usage>` and invoke its
   ``draw_...`` methods.

**Which "stuff" can be drawn?**
   .. viren2d-drawing-summary::

**What about other visualizations?**
   Besides drawing, ``viren2d`` also provides:

   *  Colormaps to (pseudo-)colorize 2D image/depth data

..
   *  TODO Optical flow visualizations

   *  Pixelation to implement proper anonymization,
      see :meth:`~viren2d.ImageBuffer.pixelate`.
   

**Which coordinate system is used?**
   All coordinates assume an image coordinate system, where the origin
   ``(0, 0)`` is at the *top left* corner, the positive *x*-axis is
   pointing right and the positive *y*-axis is pointing down.


**How to specify angles?**
   All angle measurements should be provided in *degrees*. The rotation
   direction for positive angles is from the positive *x*-axis towards the
   positive *y*-axis.
