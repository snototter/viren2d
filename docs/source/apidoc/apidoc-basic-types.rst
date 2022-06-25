.. warning::
   Currently, the documentation for static class members is missing due to
   a potential `pybind11 bug <https://github.com/pybind/pybind11/issues/3815>`_.


-------------------------
Basic Shapes & Data Types
-------------------------

~~~~~
Color
~~~~~

.. autoclass:: viren2d.Color
   :autosummary:
   :autosummary-nosignatures:
   :members:
   :undoc-members:


~~~~~~~~~~~
Color Names
~~~~~~~~~~~

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


~~~~~~~~~
Vector 2D
~~~~~~~~~

.. autoclass:: viren2d.Vec2d
   :autosummary:
   :autosummary-nosignatures:
   :members:


~~~~~~~~~
Vector 3D
~~~~~~~~~

.. autoclass:: viren2d.Vec3d
   :autosummary:
   :autosummary-nosignatures:
   :members:


~~~~~~~
Ellipse
~~~~~~~

Exemplary ellipse configurations:

   .. image:: ../images/ellipses.png
      :width: 600
      :alt: Exemplary Ellipses
      :align: center

   The code listing to create this visualization is shown in the
   :ref:`tutorial section on ellipses<tutorial-draw-ellipses>`.


.. autoclass:: viren2d.Ellipse
   :autosummary:
   :autosummary-nosignatures:
   :members:


~~~~~~~~~
Rectangle
~~~~~~~~~

Exemplary rectangle configurations:

   .. image:: ../images/rectangles.png
      :width: 600
      :alt: Exemplary Rectangles
      :align: center

   The code listing to create this visualization is shown in the
   :ref:`tutorial section on rectangles<tutorial-draw-rects>`.


.. autoclass:: viren2d.Rect
   :autosummary:
   :autosummary-nosignatures:
   :members:


~~~~~~~~~~~~~~~~~~~~~
Primitives Utilitites
~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: viren2d.ellipse_from_endpoints

.. autofunction:: viren2d.rect_from_cwh

.. autofunction:: viren2d.rect_from_lrtb

.. autofunction:: viren2d.rect_from_ltwh


~~~~~~~~~~~
ImageBuffer
~~~~~~~~~~~

.. autoclass:: viren2d.ImageBuffer
   :autosummary:
   :autosummary-nosignatures:
   :members:

.. note:: 
   Since an :class:`~viren2d.ImageBuffer` uses standard row-major memory
   layout, it can be swiftly converted to native types of common image
   processing libraries. For details and example code, refer to the
   :ref:`tutorial on type conversion<tutorial-conversion>`.

