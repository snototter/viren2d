----------
Color Maps
----------

Choose any supported :class:`~viren2d.ColorMap` to colorize 2D input data via
:meth:`~viren2d.colorize` or the :class:`~viren2d.Colorizer` (for repeatedly
colorizing inputs with similar characteristics, *e.g.* when displaying a ToF
camera stream).

~~~~~~~~~~~~~~~~~~~~
Available Color Maps
~~~~~~~~~~~~~~~~~~~~

TODO organize/relayout Color Map Cheat Sheet:

   .. image:: ../images/colormaps-cheat-sheet.png
      :width: 520
      :alt: Available color maps
      :align: center


~~~~~~~~~~~~~~~~~~~~
Colorization Example
~~~~~~~~~~~~~~~~~~~~

   

~~~~~~~~~~~~~~
Relief Shading
~~~~~~~~~~~~~~

*Multiplicative relief shading* can be performed via
:func:`~viren2d.relief_shading`. This technique can notably enhance the
perception of shape induced by the shading:

   .. image:: ../images/relief-shading.png
      :width: 600
      :alt: Topological visualizations
      :align: center
   
   The underlying data of this visualization shows the lunar farside as
   captured by the *Lunar Reconnaissance Orbiter* in 2011, provided by
   `NASA/GSFC/Arizona State University <https://photojournal.jpl.nasa.gov/catalog/PIA14021>`__.
   Corresponding Python code:

   .. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/colormaps.py
      :language: python
      :emphasize-lines: 21, 24
      :lines: 22-73
      :linenos:
      :dedent: 4
   


.. warning::
   TODO add example; group color map cheat sheet by sequential, cyclic, categorical, diverging, ...;
   link to kovesi's article and maybe some papers, too!
