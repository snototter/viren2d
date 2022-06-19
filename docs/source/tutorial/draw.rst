.. _tutorial-painter-usage:

---------------------
Tutorial: The Painter
---------------------

The :py:class:`viren2d.Painter` class allows you to draw either onto an
existing image or onto an empty canvas. Simply set it up via:

.. literalinclude:: ./code-snippets/painter-setup.py
   :language: python
   :linenos:
   :caption: Python API - How to set up the :py:class:`viren2d.Painter`.


.. literalinclude:: ./code-snippets/painter-setup.cpp
   :language: cpp
   :linenos:
   :caption: C++ API - How to set up the ``viren2d::Painter``.


Once you've set up the painter, you can start drawing:

   .. toctree::
      :maxdepth: 1

      draw-primitives
      draw-markers
      draw-text
      draw-surveillance
      draw-poses
      draw-3d-results
      draw-3d-calibration
    


Finally, you simply retrieve the visualization result via 
:meth:`viren2d.Painter.get_canvas`. This will yield a
:class:`viren2d.ImageBuffer`, which can easily be :ref:`used with
common image processing libraries <tutorial-conversion>`, such as NumPy or
OpenCV. The following examples show one option to retrieve your first
visualization from the :class:`~viren2d.Painter`:


.. literalinclude:: ./code-snippets/painter-retrieve.py
   :language: python
   :linenos:
   :caption: Python API - How to retrieve the visualization from a :py:class:`viren2d.Painter`.


.. literalinclude:: ./code-snippets/painter-retrieve.cpp
   :language: cpp
   :linenos:
   :caption: C++ API - How to retrieve the visualization from a ``viren2d::Painter``.


.. tip::
   You can retrieve either a **deep copy** or a **shared view** on the
   :class:`~viren2d.Painter`'s canvas.
   If you can ensure that the :class:`~viren2d.Painter` instance is not
   destroyed (and you don't immediately continue drawing), you should prefer
   the shared view because it avoids unnecessary memory allocation.

   Refer to :meth:`viren2d.Painter.get_canvas` for details.


.. note::
   As you've already seen in the code examples above, the ``viren2d`` **Python
   API is simply a wrapper of the C++ API**. Except for the naming conventions
   (Snake case in Python *vs.* camel case in C++), there is almost no
   difference.
   Thus, I will omit the (mostly redundant) C++ code examples to avoid
   unnecessarily cluttering this tutorial.
   You will definitely be able to deduce the corresponding C++ functionality
   from the Python examples alone - I put my trust in you!

   In cases where the **C++ counterpart is not easily deducible**, it is
   explicitly documented in the Python :ref:`API documentation<api-documentation>`.

