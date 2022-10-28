------------
Colorization
------------

~~~~~~~~~~
Color Maps
~~~~~~~~~~

Refer to the :ref:`color map tutorial<tutorial-colormaps>`
for additional notes, exemplary code and visualizations of all implemented
color maps.

.. autoclass:: viren2d.ColorMap


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


