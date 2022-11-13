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
   `Peter Kovesi's excellent article <https://doi.org/10.48550/arXiv.1509.03700>`__
   about the caveats with color maps and how to avoid them.


..........
Sequential
..........

Sequential (*aka* linear) color maps linearly increase/decrease the lightness
and/or saturation over the color map range. They are the most generic color
maps and should be used to represent data which has an intrinsic ordering.

.. viren2d-color-map-table::
   :sequential:

...........
Categorical
...........

Categorical or quantitative maps are color palettes with distinct colors,
suitable to visualize categorical data.

.. viren2d-color-map-table::
   :categorical:

......
Cyclic
......

Cyclic color maps begin and end at the same color. They are useful to represent
data that wraps around, *e.g.* phase angles or orientation values.

.. viren2d-color-map-table::
   :cyclic:

.......................
Color Vision Deficiency
.......................

These color maps are suitable for viewers with color vision deficiency (CVD).
The colors are chosen such that people who are color blind should share a
common perceptual interpretation of data with people who have normal color
vision.

.. viren2d-color-map-table::
   :color-blind:

.........
Diverging
.........

Diverging color maps are suitable to represent values above or below a defined
reference value. Note that these maps usually have a small perceptual flat spot
at the center.

.. viren2d-color-map-table::
   :diverging:

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

.. viren2d-color-map-table::
   :rainbow:

...............
Special Purpose
...............

These color maps are often designed for a particular purpose, such as
representing thermographic (``thermal``), topographic (``earth``, ``ocean``,
*etc.*), or multi-spectral (``spectral``) data.

.. viren2d-color-map-table::
   :specialized:

...................
User-Defined Colors
...................

If you need other than the provided color maps, you can define your custom
color maps via :func:`~viren2d.set_custom_colormap`. These can then be used
analogously to the predefined color maps via the enumeration values
:attr:`ColorMap.Custom1`, :attr:`ColorMap.Custom2`, *etc.* or their
string representation.
For example:

.. code-block:: python
   :linenos:

   # Exemplary categorical data for visualization
   import numpy as np
   labels = np.array(
       [[0, 1, 2], [-3, -4, -6], [20000, 20001, 20003]],
       dtype=np.int32)

   # Register a custom color map. This is a usage example and by
   # no means a useful color map!
   viren2d.set_custom_colormap(
      'custom1', ['#800000', (0.94, 0.13, 0.15), 'rose-red'])

   # Apply the custom map for label colorization:
   vis = viren2d.colorize_labels(labels=labels, colormap='custom1')
