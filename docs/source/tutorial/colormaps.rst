.. _tutorial-colormaps:

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

.. important::
   Note that **not every color map is perceptually uniform**. Always check the
   documentation of the corresponding :class:`~viren2d.ColorMap` enum value
   when selecting a suitable false color schema.
   
   Be also aware that **choosing a color map is more intricate than most people
   anticipate**. For more details, I highly recommend
   `Peter Kovesi's excellent article <https://arxiv.org/abs/1509.03700>`__
   about the caveats with color maps and how to avoid them.


..........
Sequential
..........

Color maps where the lightness and/or saturation increases/decreases linearly
over the color map range. Useful to represent data which has an intrinsic
ordering.

.. image:: ../images/colormaps-cheat-sheet-sequential.png
   :width: 480
   :alt: Sequential color maps
   :align: center


...........
Categorical
...........

Color palettes with distinct colors suitable to visualize categorical data.

.. image:: ../images/colormaps-cheat-sheet-categorical.png
   :width: 480
   :alt: Categorical color maps
   :align: center


......
Cyclic
......

Cyclic color maps begin and end at the same color. They are useful to represent
data that wraps around, *e.g.* phase angles or orientation values.

.. image:: ../images/colormaps-cheat-sheet-cyclic.png
   :width: 480
   :alt: Cyclic color maps
   :align: center


.............................
Color Vision Deficiency (CVD)
.............................

Color maps suitable for viewers with color vision deficiency. These are
designed such that people who are color blind should share a common perceptual
interpretation of data with people who have normal color vision.

.. image:: ../images/colormaps-cheat-sheet-color-blind.png
   :width: 480
   :alt: Color maps suitable for the color-blind
   :align: center


.........
Diverging
.........

Color maps suitable to represent values above or below a defined
reference value. Note that diverging color maps usually have a small
perceptual flat spot at the center.

.. image:: ../images/colormaps-cheat-sheet-diverging.png
   :width: 480
   :alt: Categorical color maps
   :align: center


.......
Rainbow
.......

Rainbow color maps are widely used but too often misused. They require special
care as they can **easily lead to perception of features that are not present**
in the data and also suffer from **blind spots** (where the viewer cannot
distinguish actually present features).

These issues can be seen for example at the *jet* color gradient below, which
shows banding effects. For further details, refer to the summary
provided on `Peter Kovesi's website <https://colorcet.com/>`__.

.. image:: ../images/colormaps-cheat-sheet-rainbow.png
   :width: 480
   :alt: Rainbow color maps
   :align: center


...............
Special Purpose
...............

Miscellaneous color maps, often designed for a particular purpose, such as
representing thermographic (``thermal``), topographic (``earth``, ``ocean``,
*etc.*), or multi-spectral (``spectral``) data.

.. image:: ../images/colormaps-cheat-sheet-specialized.png
   :width: 480
   :alt: Specialized color maps
   :align: center


~~~~~~~~~~~~~~~~~~~~
Colorization Example
~~~~~~~~~~~~~~~~~~~~

The following example uses the `"gouldian" color map <https://colorcet.com/>`__
to colorize example data computed by the well-known
`peaks function <https://www.mathworks.com/help/matlab/ref/peaks.html>`__,
by varying the number of discretization bins:


.. image:: ../images/colormap-peaks.png
   :width: 500
   :alt: Exemplary colorization
   :align: center


Corresponding Python code:

.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/colormaps.py
   :language: python
   :emphasize-lines: 1, 13-14
   :lines: 78-97
   :linenos:
   :dedent: 4


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
