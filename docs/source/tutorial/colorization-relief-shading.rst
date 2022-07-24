~~~~~~~~~~~~~~
Relief Shading
~~~~~~~~~~~~~~

*Multiplicative relief shading* is provided via
:func:`~viren2d.relief_shading`. This technique can notably enhance the
perception of shape induced by the shading:

.. image:: ../images/relief-shading.png
   :width: 600
   :alt: Exemplary relief shading
   :align: center


This example uses relief shading with different color maps to show
the topographic structure of (a small part of) the lunar farside, which
was captured by the *Lunar Reconnaissance Orbiter* in 2011 and published by
`NASA/GSFC/Arizona State University <https://photojournal.jpl.nasa.gov/catalog/PIA14021>`__.
The smaller images below each main visualization show the inputs to
:func:`~viren2d.relief_shading`, *i.e.* the relief and its false color
representation.
Note how the two isoluminant relief color maps are not useful on their own, but
induce a proper shape perception after shading.

Python code for this visualization example:

.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/colormaps.py
   :language: python
   :emphasize-lines: 24-25, 27
   :lines: 18-69
   :linenos:
   :dedent: 4
