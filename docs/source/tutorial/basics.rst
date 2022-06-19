.. include:: ../symbols.rst

---------------
Know the Basics
---------------

**How do I draw "stuff"?**
   You need to :ref:`use a painter<tutorial-painter-usage>` and invoke its
   ``draw_...`` methods.


**What about the Coordinate System?**
   All coordinates assume an image coordinate system, where the origin
   ``(0, 0)`` is at the *top left* corner, the positive *x*-axis is
   pointing right and the positive *y*-axis is pointing down.


**How do I specify angles?**
   All angle measurements should be provided in *degrees*. The rotation
   direction for positive angles is from the positive *x*-axis towards the
   positive *y*-axis.


.. warning::
   TODO This overview page should be extended with info on pseudocolorization;
   anonymization (once ImageBuffer supports pixelization)

   |right-arrow| add "What about other visualizations?"
      Then, refer to pseudocoloring (label2img, segmentation results, etc.)

