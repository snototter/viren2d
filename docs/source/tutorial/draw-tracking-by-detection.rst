.. include:: ../symbols.rst

.. _tutorial-tracking-by-detection:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Bounding Boxes & Trajectories
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Tracking-by-detection results can be visualized via
:meth:`~viren2d.Painter.draw_bounding_box_2d` and
:meth:`~viren2d.Painter.draw_trajectory`. For example:

|image-tracking-by-detection|

This visualization has been created with the following Python code:

.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/tracking_by_detection.py
   :language: python
   :lines: 8-63
   :emphasize-lines: 10-12, 17-18, 47-50
   :linenos:
   :dedent: 4
