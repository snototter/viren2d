~~~~~~~~~~~~~~~~~~~
Scaled Colorization
~~~~~~~~~~~~~~~~~~~

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

