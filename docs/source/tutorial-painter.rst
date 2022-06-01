.. _tutorial-painter-usage:

Tutorial: The Painter
---------------------

TODO basic workflow: 

1. You need to initialize a :py:class:`viren2d.Painter` before you can draw anything.
2. Draw... TODO ref to subsections
3. Retrieve the visualization (*i.e.* the painter's canvas).

.. literalinclude:: ./code-snippets/python/painter-usage.py
   :language: python
   :caption: Python API - How to use the Painter


.. literalinclude:: ./code-snippets/python/painter-usage.py
   :language: cpp
   :caption: C++ API - How to use the Painter TODO


.. tip::
   Using the shared view on the canvas avoids memory allocation...


Draw Basic Geometric Primitives
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TODO arc, circle....

.. function:: Draw arcs
   :noindex:
   
   TODO display image, include code to generate the example



.. code-block:: python
   :caption: Python API
   :emphasize-lines: 3, 6, 7

   painter.draw_arc(...)
   painter.draw_arc(...)
   painter.draw_arc(...)
   painter.draw_arc(...)
   painter.draw_arc(...)

   painter.draw_circle(...)
   painter.draw_circle(...)
   painter.draw_circle(...)
   painter.draw_circle(...)


.. code-block:: cpp
   :caption: C++ API
   :emphasize-lines: 2

   auto line_style = viren2d::LineStyle();
   painter->DrawArc(...);
   painter->DrawCircle(...);



Draw 2D Bounding Boxes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TODO

.. important::
   Use rounded corners...



