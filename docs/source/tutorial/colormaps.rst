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

.. warning::
   TODO add short note to each category

..........
Sequential
..........

.. image:: ../images/colormaps-cheat-sheet-sequential.png
   :width: 480
   :alt: Sequential color maps
   :align: center

...........
Categorical
...........

.. image:: ../images/colormaps-cheat-sheet-categorical.png
   :width: 480
   :alt: Categorical color maps
   :align: center

......
Cyclic
......

.. image:: ../images/colormaps-cheat-sheet-cyclic.png
   :width: 480
   :alt: Cyclic color maps
   :align: center

...........
Color-blind
...........

.. image:: ../images/colormaps-cheat-sheet-color-blind.png
   :width: 480
   :alt: Color maps suitable for the color-blind
   :align: center

...........
Rainbow
...........

.. image:: ../images/colormaps-cheat-sheet-rainbow.png
   :width: 480
   :alt: Rainbow color maps
   :align: center

...........
Specialized
...........

.. image:: ../images/colormaps-cheat-sheet-specialized.png
   :width: 480
   :alt: Specialized color maps
   :align: center


~~~~~~~~~~~~~~~~~~~~
Colorization Example
~~~~~~~~~~~~~~~~~~~~

.. image:: ../images/colormap-peaks.png
   :width: 500
   :alt: Exemplary colorization
   :align: center

.. warning::
   TODO description + code

~~~~~~~~~~~~~~
Relief Shading
~~~~~~~~~~~~~~

*Multiplicative relief shading* can be performed via
:func:`~viren2d.relief_shading`. This technique can notably enhance the
perception of shape induced by the shading:

.. image:: ../images/relief-shading.png
   :width: 600
   :alt: Exemplary relief shading
   :align: center

This visualization uses relief shading with different color maps to
show the topographical structure of (a small part of) the lunar farside. The
underlying topographical data was captured by the *Lunar Reconnaissance
Orbiter* in 2011 and published by
`NASA/GSFC/Arizona State University <https://photojournal.jpl.nasa.gov/catalog/PIA14021>`__.
The smaller images below each main visualization show the inputs to
:func:`~viren2d.relief_shading`, *i.e.* the relief and its colorization.
Python code for this visualization example:

.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/colormaps.py
   :language: python
   :emphasize-lines: 23-24, 26
   :lines: 20-72
   :linenos:
   :dedent: 4
