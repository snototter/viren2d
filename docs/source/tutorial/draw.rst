.. _tutorial-painter-usage:

Tutorial: The Painter
---------------------

TODO basic workflow: 

1. You need to initialize a :py:class:`viren2d.Painter` before you can draw anything.
2. Once you have a painter, you can start drawing:

   .. toctree::
      :maxdepth: 1

      draw-color-styles
      draw-primitives
      draw-text

3. Retrieve the visualization (*i.e.* the painter's canvas).

.. literalinclude:: ./code-snippets/python/painter-usage.py
   :language: python
   :caption: Python API - How to use the Painter


.. literalinclude:: ./code-snippets/python/painter-usage.py
   :language: cpp
   :caption: C++ API - How to use the Painter TODO


.. tip::
   Using the shared view on the canvas avoids memory allocation...



