---------------
Color Gradients
---------------

Color gradients are smooth blendings of different colors, specified by *color
stops* along a *control vector*.

.. autoclass:: viren2d.ColorGradient
   :autosummary:

~~~~~~~~~~~~~~~
Linear Gradient
~~~~~~~~~~~~~~~

.. autoclass:: viren2d.LinearColorGradient
   :autosummary:
   :autosummary-nosignatures:
   :members: __init__, add_color_stop, add_intensity_stop, is_valid

~~~~~~~~~~~~~~~
Radial Gradient
~~~~~~~~~~~~~~~

.. autoclass:: viren2d.RadialColorGradient
   :autosummary:
   :autosummary-nosignatures:
   :members: __init__, add_color_stop, add_intensity_stop, is_valid

~~~~~~~~~~~~~~~~~~~
Gradient Utilitites
~~~~~~~~~~~~~~~~~~~

.. autofunction:: viren2d.color_gradient_mask

.. autofunction:: viren2d.color_gradient_visualization
