-------------------
Handling Image Data
-------------------

~~~~~~~~~~~
ImageBuffer
~~~~~~~~~~~

.. note:: 
   Since an :class:`~viren2d.ImageBuffer` uses standard row-major memory
   layout, it can be swiftly converted to native types of common image
   processing libraries. For details and example code, refer to the
   :ref:`tutorial on type conversion<tutorial-conversion>`.

.. autoclass:: viren2d.ImageBuffer
   :autosummary:
   :autosummary-nosignatures:
   :members:


~~~~~~~~~
Image I/O
~~~~~~~~~

The image I/O functions are quite limited in comparison to specialized image
processing libraries, *i.e.* only loading/saving 8-bit images is supported.

.. autofunction:: viren2d.load_image_uint8

.. autofunction:: viren2d.save_image_uint8
