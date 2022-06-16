Know the Basics
---------------

**How do I draw "stuff"?**
   You need to :ref:`use a painter<tutorial-painter-usage>` and invoke its
   ``draw_...`` methods.

.. note::
   TODO needs to be extended with pseudocolorization; what about anonymization (imagebuffer)?
   TODO add warning at colors - most common mistake is to specify a RGB tuple (which results in white due to the saturation cast!)

**What about the Coordinate System?**
   All coordinates assume an image coordinate system, where the origin
   ``(0, 0)`` is at the *top left* corner, the positive *x*-axis is
   pointing right and the positive *y*-axis is pointing down.


**How do I specify angles?**
   All angle measurements should be provided in *degrees*. The rotation
   direction for positive angles is from the positive *x*-axis towards the
   positive *y*-axis.


