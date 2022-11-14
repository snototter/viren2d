.. include:: ../symbols.rst
   
~~~~~~~~~~~~~~~~~~~
Scaled Colorization
~~~~~~~~~~~~~~~~~~~

Scaled colorization divides the data range into a discrete number of evenly
spaced bins which are then used to look up the corresponding color from a
color map. The following example uses the
`Gouldian color map <https://colorcet.com/>`__ to colorize the 
`peaks example data <https://www.mathworks.com/help/matlab/ref/peaks.html>`__:


|image-colorized-peaks|

Corresponding Python code:

.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/colormaps.py
   :language: python
   :emphasize-lines: 1, 15-16
   :lines: 77-103
   :linenos:
   :dedent: 4

