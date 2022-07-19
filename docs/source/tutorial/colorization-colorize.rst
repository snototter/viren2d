~~~~~~~~~~~~~~~~~~~~~~~
Colorization Techniques
~~~~~~~~~~~~~~~~~~~~~~~

...................
Scaled Colorization
...................

Scaled colorization divides the data range into a discrete number of evenly
spaced bins which are then used to look up the corresponding color from a
color map. The following example uses the
`Gouldian color map <https://colorcet.com/>`__ to colorize the 
`peaks example data <https://www.mathworks.com/help/matlab/ref/peaks.html>`__:


.. image:: ../images/colormap-peaks.png
   :width: 500
   :alt: Exemplary colorization
   :align: center


Corresponding Python code:

.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/colormaps.py
   :language: python
   :emphasize-lines: 1, 13-14
   :lines: 78-104
   :linenos:
   :dedent: 4


..................
Label Colorization
..................

Label images, such as the results of semantic, panoptic or instance segementation, should
not be colorized by dividing the input data range into a discrete number of
bins. Instead, use :func:`~viren2d.colorize_labels` which will properly
alternate between the colors:


.. image:: ../images/colorize-labels.png
   :width: 500
   :alt: Exemplary label colorization
   :align: center


This visualization shows class- and instance-level annotations from
a crop of the `Cityscapes training dataset <https://www.cityscapes-dataset.com/>`__.
Corresponding colorization code:


.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/colormaps.py
   :language: python
   :emphasize-lines: 13-14, 27-28
   :lines: 115-151
   :linenos:
   :dedent: 8

