.. include:: ../symbols.rst

---------------
Color Gradients
---------------

Color gradients are smooth blendings of different colors, specified by *color
stops* along a *control vector*. An application example is shown in the
:ref:`optical flow tutorial<tutorial-optical-flow-blend>`.

.. autosummary::
   :nosignatures:

   viren2d.ColorGradient
   viren2d.LinearColorGradient
   viren2d.RadialColorGradient


~~~~~~~~~~~~~~~~~~~
Gradient Base Class
~~~~~~~~~~~~~~~~~~~

.. autoclass:: viren2d.ColorGradient
   :autosummary:

~~~~~~~~~~~~~~~
Linear Gradient
~~~~~~~~~~~~~~~

.. autoclass:: viren2d.LinearColorGradient
   :autosummary:
   :autosummary-nosignatures:
   :members: __init__, add_color_stop, add_intensity_stop, is_valid, mask, visualization

~~~~~~~~~~~~~~~
Radial Gradient
~~~~~~~~~~~~~~~

.. autoclass:: viren2d.RadialColorGradient
   :autosummary:
   :autosummary-nosignatures:
   :members: __init__, add_color_stop, add_intensity_stop, is_valid, mask, visualization
