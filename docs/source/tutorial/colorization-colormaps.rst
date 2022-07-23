.. _tutorial-colormaps:

~~~~~~~~~~
Color Maps
~~~~~~~~~~

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

Sequential (*aka* linear) color maps linearly increase/decrease the lightness
and/or saturation over the color map range. They are the most generic color
maps and should be used to represent data which has an intrinsic ordering.

.. image:: ../images/colormaps-cheat-sheet-sequential.png
   :width: 480
   :alt: Sequential color maps
   :align: center


...........
Categorical
...........

Categorical or quantitative maps are color palettes with distinct colors,
suitable to visualize categorical data.

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


.......................
Color Vision Deficiency
.......................

These color maps are suitable for viewers with color vision deficiency (CVD).
The colors are chosen such that people who are color blind should share a
common perceptual interpretation of data with people who have normal color
vision.

.. image:: ../images/colormaps-cheat-sheet-color-blind.png
   :width: 480
   :alt: Color maps suitable for the color-blind
   :align: center


.........
Diverging
.........

Diverging color maps are suitable to represent values above or below a defined
reference value. Note that these maps usually have a small perceptual flat spot
at the center.

.. image:: ../images/colormaps-cheat-sheet-diverging.png
   :width: 480
   :alt: Categorical color maps
   :align: center


.......
Rainbow
.......

Rainbow color maps are widely used but also too often misused. They require
special care as they can **easily lead to perception of features that are not
present** in the data and also suffer from **blind spots** (where the viewer
cannot distinguish actually present features).

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

These color maps are often designed for a particular purpose, such as
representing thermographic (``thermal``), topographic (``earth``, ``ocean``,
*etc.*), or multi-spectral (``spectral``) data.

.. image:: ../images/colormaps-cheat-sheet-specialized.png
   :width: 480
   :alt: Specialized color maps
   :align: center


...................
User-Defined Colors
...................

If you need other than the provided color maps, you can define your custom
color maps via :func:`~viren2d.set_custom_colormap`. These can then be used
analogously to the predefined color maps via the enumeration values
:attr:`ColorMap.Custom1`, :attr:`ColorMap.Custom2`, *etc.*
