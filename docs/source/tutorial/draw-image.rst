~~~~~~~~~~~~~~
Image Overlays
~~~~~~~~~~~~~~

Images can be superimposed onto the canvas via :meth:`~viren2d.Painter.draw_image`.
If only a *region of interest* should be overlaid, this can be selected via
:meth:`~viren2d.ImageBuffer.roi`.


To position an image, you simply need a **reference position** (*i.e.*
:math:`x` and :math:`y` coordinate) and choose a proper :class:`~viren2d.Anchor`. The following
visualization uses the `"gouldian" color map <https://colorcet.com/>`__ to
colorize example data computed by the well-known
`peaks function <https://www.mathworks.com/help/matlab/ref/peaks.html>`__:


.. image:: ../images/image-anchors.png
   :width: 440
   :alt: Image anchors
   :align: center


.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/images.py
   :language: python
   :linenos:
   :lines: 33-54
   :emphasize-lines: 14-17
   :dedent: 4
