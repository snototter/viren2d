.. _tutorial-painter-usage:

-------
Drawing
-------

The :py:class:`~viren2d.Painter` class allows you to draw either onto an
existing image or onto an empty canvas. Simply set it up via:

.. literalinclude:: ./code-snippets/painter-setup.py
   :language: python
   :linenos:
   :caption: Python API - How to set up the :py:class:`~viren2d.Painter`.


.. literalinclude:: ./code-snippets/painter-setup.cpp
   :language: cpp
   :linenos:
   :caption: C++ API - How to set up the ``viren2d::Painter``.


Once you've set up the painter, you can start drawing:

   .. toctree::
      :maxdepth: 1

      draw-primitives
      draw-markers
      draw-image
      draw-tracking-by-detection
      draw-poses
      draw-3d-results
      draw-3d-calibration
      draw-text


Finally, you simply retrieve the visualization result via 
:meth:`~viren2d.Painter.get_canvas`. This will yield an
:class:`~viren2d.ImageBuffer`, which can easily be :ref:`used with
common image processing libraries <tutorial-conversion>`, such as NumPy or
OpenCV. The following examples show one option to retrieve your first
visualization from the :class:`~viren2d.Painter`:


.. literalinclude:: ./code-snippets/painter-retrieve.py
   :language: python
   :linenos:
   :caption: Python API - How to retrieve the visualization from a :py:class:`~viren2d.Painter`.


.. literalinclude:: ./code-snippets/painter-retrieve.cpp
   :language: cpp
   :linenos:
   :caption: C++ API - How to retrieve the visualization from a ``viren2d::Painter``.


.. tip::
   You can retrieve either a **deep copy** or a **shared view** on the
   :class:`~viren2d.Painter`'s canvas.
   If you can ensure that the :class:`~viren2d.Painter` instance is not
   destroyed and you don't concurrently change the canvas, you should prefer
   the shared view because it avoids unnecessary memory allocation.

   Refer to :meth:`~viren2d.Painter.get_canvas` for details.


.. note::
   As demonstrated by the example code above, the ``viren2d`` **Python
   API is simply a wrapper of the C++ API**. Except for the naming conventions
   (Snake case in Python *vs.* camel case in C++), both APIs are highly
   similar.

   Thus, the rest of this tutorial omits the C++ examples. The corresponding
   C++ counterpart will instead be mentioned in the corresponding
   Python :ref:`API documentation<api-documentation>`.
