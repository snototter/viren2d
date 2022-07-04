----------
Color Maps
----------

Choose any supported :class:`~viren2d.ColorMap` to colorize 2D input data via
:meth:`~viren2d.colorize` or the :class:`~viren2d.Colorizer` (for repeatedly
colorizing inputs with similar characteristics, *e.g.* when displaying a ToF
camera stream).

**Color Map Cheat Sheet:**

   .. image:: ../images/colormaps-cheat-sheet.png
      :width: 520
      :alt: Available color maps
      :align: center

**Color Map Example:**

.. comment
   This visualization has been created with the following Python code:

   .. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/markers.py
      :language: python
      :emphasize-lines: 3-4, 35, 41
      :lines: 11-62
      :linenos:
      :dedent: 4


.. warning::
   TODO add example; group color map cheat sheet by sequential, cyclic, categorical, and rainbow;
   link to kovesi's article
