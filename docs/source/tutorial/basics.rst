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

   *  Scaled colorization with various color maps,
      see :func:`~viren2d.colorize` and :class:`~viren2d.Colorizer`.

   *  Pixelation, *e.g.* to implement anonymization,
      see :meth:`~viren2d.ImageBuffer.pixelate`.

..
   *  TODO Optical flow visualizations
   
   

**Which coordinate system is used?**
   All coordinates assume an image coordinate system, where the origin
   ``(0, 0)`` is at the *top left* corner, the positive *x*-axis is
   pointing right and the positive *y*-axis is pointing down.


**How to specify angles?**
   All angle measurements should be provided in *degrees*. The rotation
   direction for positive angles is from the positive *x*-axis towards the
   positive *y*-axis.
