~~~~~~~~~~~~~~~
Camera Geometry
~~~~~~~~~~~~~~~

.. warning::
   TODO doc


To inspect the results of a pinhole camera calibration, ``viren2d`` provides
the following utilities:

* Overlay coordinate system axes via :func:`~viren2d.Painter.draw_xyz_axes`.

* Estimate and overlay the line of horizon via :func:`~viren2d.Painter.draw_horizon_line`.


.. image:: ../images/pinhole-camera.png
   :width: 600
   :alt: Pinhole camera visualizations
   :align: center

Corresponding Python example code:

.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/pinhole.py
   :language: python
   :emphasize-lines: 24-26, 34-35
   :lines: 6-
   :linenos:
