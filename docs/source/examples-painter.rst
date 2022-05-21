Drawing with the Painter
========================

You need a painter to draw anything...

.. py:class:: Painter
   :module: viren2d
   :noindex:


.. cpp:class::Painter
   :noindex:

.. tip::
   Using the shared view on the canvas avoids memory allocation...


Draw Arcs & Circles
-------------------

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



Draw Rectangles
-------------------

TODO

.. important::
   Use rounded corners...



