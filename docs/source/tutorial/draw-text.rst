.. _tutorial-text:

~~~~~~~~~~~~~~~~~
Text & Text Boxes
~~~~~~~~~~~~~~~~~

Text can be rendered via :meth:`~viren2d.Painter.draw_text` or
:meth:`~viren2d.Painter.draw_text_box`.


.. _tutorial-text-anchors:

................
Text Positioning
................

To neatly position text, you simply need a **reference position** (*i.e. x* and
*y* coordinate) and a :class:`~viren2d.Anchor`. The following figure shows
text boxes, along with the corresponding anchor and reference positions,
which are highlighted via :func:`~viren2d.Painter.draw_marker`:

   .. image:: ../images/text-anchors.png
      :width: 440
      :alt: Text anchors
      :align: center

This visualization has been created with the following Python code:

   .. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/positioning.py
      :language: python
      :linenos:
      :lines: 35-51
      :emphasize-lines: 10
      :dedent: 4


.. _tutorial-text-multi-line:

...............
Multi-Line Text
...............

The following figure shows the different (horizontal) alignment options of
multi-line text boxes. The displayed text lines are an excerpt from
`Revelation, a Visual Poem <https://vimeo.com/57370112>`__.

   .. image:: ../images/text-multi-line.png
      :width: 750
      :alt: Multi-line Text Alignment
      :align: center

This visualization has been created with the following Python code:

   .. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/positioning.py
      :language: python
      :lines: 67-97
      :linenos:
      :emphasize-lines: 15, 21, 27
      :dedent: 4

