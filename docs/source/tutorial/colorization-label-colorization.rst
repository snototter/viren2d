.. include:: ../symbols.rst

~~~~~~~~~~~~~~~~~~
Label Colorization
~~~~~~~~~~~~~~~~~~

Label images, such as the results of semantic, panoptic or instance
segmentation, should not be *scale colorized* as this would result
in numerically nearby labels being assigned to the same color. Instead,
use :func:`~viren2d.colorize_labels` which will properly alternate between
the colors of the chosen color map:

|image-label-colorization|

This visualization shows class- and instance-level annotations from
a crop of the `Cityscapes (Zurich) training dataset <https://www.cityscapes-dataset.com/>`__.
Corresponding colorization code:


.. literalinclude:: ../../../examples/rtd-examples-python/rtd_demo_images/colormaps.py
   :language: python
   :emphasize-lines: 13-14, 27-28
   :lines: 112-148
   :linenos:
   :dedent: 8

