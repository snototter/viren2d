.. warning::
   Currently, the documentation for static class members is missing due to
   a potential `pybind11 bug <https://github.com/pybind/pybind11/issues/3815>`_.


------------
Basic Shapes
------------

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


~~~~~~~~~~~~~~~~
Shape Utilitites
~~~~~~~~~~~~~~~~

.. autofunction:: viren2d.ellipse_from_endpoints

.. autofunction:: viren2d.rect_from_cwh

.. autofunction:: viren2d.rect_from_lrtb

.. autofunction:: viren2d.rect_from_ltwh
