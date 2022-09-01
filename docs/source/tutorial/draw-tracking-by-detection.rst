.. _tutorial-tracking-by-detection:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Bounding Boxes & Trajectories
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Tracking-by-detection results can be visualized via
:meth:`~viren2d.Painter.draw_bounding_box_2d` and
:meth:`~viren2d.Painter.draw_trajectory`. For example:

.. figure:: ../images/tracking-by-detection.png
   :width: 682
   :alt: Tracking-by-Detection Example
   :align: center

This visualization has been created with the following Python code:

.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/tracking_by_detection.py
   :language: python
   :lines: 8-72
   :emphasize-lines: 10, 21-23, 31, 42-45
   :linenos:
   :dedent: 4
