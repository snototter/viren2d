.. include:: ../symbols.rst

-----------------
Overview & Basics
-----------------

**How to draw "stuff"?**
   The :class:`~viren2d.Painter` class supports drawing onto existing images
   or a plain canvas. After :ref:`setting up the canvas<tutorial-painter-usage>`,
   use the ``Painter.draw_...`` methods.


**Which "stuff" can be drawn?**
   .. viren2d-drawing-summary::


**What about visualizations besides drawing?**
   Besides drawing, ``viren2d`` also provides various
   :ref:`color maps and colorization techniques<tutorial-colorization>`
   to inspect and represent data, such as depth measurements, topographic
   data, gradient or flow fields, *etc.*

   Additionally, the :class:`~viren2d.ImageBuffer` provides some basic image
   processing functionalities, *e.g.* to :meth:`~viren2d.ImageBuffer.blend`
   images, :meth:`~viren2d.ImageBuffer.pixelate` regions, or perform
   some general color space conversions. Note, however, that these
   functionalities are only provided for user convenience and are not optimized
   towards high-speed, *i.e.* ``viren2d`` does not use
   `OpenMP <https://www.openmp.org/>`__,
   `oneTBB <https://github.com/oneapi-src/oneTBB>`__,
   `AVX <https://en.wikipedia.org/wiki/Advanced_Vector_Extensions>`__,
   `Neon <https://developer.arm.com/Architectures/Neon>`__
   or any other multi-threading/multi-processing extensions.


**Which coordinate system is used?**
   All coordinates assume an image coordinate system, where the origin
   ``(0, 0)`` is at the *top left* corner, the positive :math:`x` axis is
   pointing right and the positive :math:`y` axis is pointing down.


**How to specify angles?**
   Unless stated otherwise in the corresponding API documentation, all angle
   measurements are assumed to be in *degrees*. The rotation direction
   for positive angles is from the positive :math:`x` axis towards
   the positive :math:`y` axis.
