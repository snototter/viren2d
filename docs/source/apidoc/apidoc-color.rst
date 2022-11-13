.. include:: ../symbols.rst

.. note::
   Currently, the documentation for static class members is missing due to
   a potential `pybind11 bug <https://github.com/pybind/pybind11/issues/3815>`_.

   This affects the predefined colors, *e.g.* ``Color.Magenta``, but also the
   *special* members, *i.e.* ``Color.Invalid`` and ``Color.Same``.
   Usage of these special members is documented in the docstrings of the drawing
   functions (of the :class:`~viren2d.Painter`), wherever they are supported.

---------------
Defining Colors
---------------

~~~~~~~~~~~
Color Class
~~~~~~~~~~~

Within ``viren2d``, colors are handled via the :class:`~viren2d.Color` class
in **rgba** format, where :math:`r, g, b, a \in [0, 1]`.

.. autoclass:: viren2d.Color
   :autosummary:
   :autosummary-nosignatures:
   :members:
   :undoc-members:


~~~~~~~~~~~
Color Names
~~~~~~~~~~~

There are several predefined colors which can be initialized via their name string:

.. autofunction:: viren2d.color_names


**Color Name Cheat Sheet:**

.. figure:: ../images/color-names.png
   :width: 750
   :alt: Color names
   :align: center


~~~~~~~~~~~~~~~~
Color Utilitites
~~~~~~~~~~~~~~~~

.. autofunction:: viren2d.axis_color

.. autofunction:: viren2d.color_from_object_category

.. autofunction:: viren2d.color_from_object_id

.. autofunction:: viren2d.fade_out_linear

.. autofunction:: viren2d.fade_out_logarithmic

.. autofunction:: viren2d.fade_out_quadratic

.. autofunction:: viren2d.object_category_names

.. autofunction:: viren2d.rgba

.. autofunction:: viren2d.RGBa

