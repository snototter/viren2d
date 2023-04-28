.. include:: ../symbols.rst

------------
Colorization
------------

.. autosummary::
   :nosignatures:

   viren2d.ColorMap
   viren2d.get_colormap
   viren2d.set_custom_colormap
   viren2d.colorize_labels
   viren2d.colorize_scaled
   viren2d.colorize_scalars
   viren2d.StreamColorizer
   viren2d.LimitsMode
   viren2d.relief_shading
   viren2d.peaks


~~~~~~~~~~
Color Maps
~~~~~~~~~~

Refer to the :ref:`color map tutorial<tutorial-colormaps>`
for additional notes, exemplary code and visualizations of all available
color maps.

.. autoclass:: viren2d.ColorMap
   :members: list_all


~~~~~~~~~~~~~~~~~
Custom Color Maps
~~~~~~~~~~~~~~~~~

Use :func:`~viren2d.set_custom_colormap` to register a custom color map.
Additionally, :func:`~viren2d.get_colormap` can be used to retrieve the colors
of a predefined map, which can then be adjusted to your application needs.


.. autofunction:: viren2d.get_colormap


.. autofunction:: viren2d.set_custom_colormap


~~~~~~~~~~~~~~~~~~~~~~~~~~~
Label/Category Colorization
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: viren2d.colorize_labels


~~~~~~~~~~~~~~~~~~~
Scaled Colorization
~~~~~~~~~~~~~~~~~~~

.. autofunction:: viren2d.colorize_scaled

.. autofunction:: viren2d.colorize_scalars


.. autoclass:: viren2d.StreamColorizer
   :autosummary:
   :autosummary-nosignatures:
   :members:


.. autoclass:: viren2d.LimitsMode


~~~~~~~~~~~~~~
Relief Shading
~~~~~~~~~~~~~~

.. autofunction:: viren2d.relief_shading   


~~~~~~~~~~~~
Example Data
~~~~~~~~~~~~

.. autofunction:: viren2d.peaks   


