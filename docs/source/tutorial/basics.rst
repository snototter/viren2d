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
   TODO this page needs to be extended with info on pseudocolorization;

   (maybe add anonymization also - via imagebuffer)?

   add "What about other visualizations?"
      pseudocoloring (can also be usable for label2img, segmentation results, etc.)
