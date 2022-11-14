.. include:: ../symbols.rst
   
-------------------
Handling Image Data
-------------------

.. autosummary::
   :nosignatures:

   viren2d.ImageBuffer
   viren2d.color_pop
   viren2d.convert_gray2rgb
   viren2d.convert_hsv2rgb
   viren2d.convert_rgb2gray
   viren2d.convert_rgb2hsv
   viren2d.load_image_uint8
   viren2d.save_image_uint8


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


~~~~~~~~~~~~~~~~~~~~~~~
Color Space Conversions
~~~~~~~~~~~~~~~~~~~~~~~

.. autofunction:: viren2d.convert_gray2rgb

.. autofunction:: viren2d.convert_hsv2rgb

.. autofunction:: viren2d.convert_rgb2gray

.. autofunction:: viren2d.convert_rgb2hsv

.. autofunction:: viren2d.color_pop


~~~~~~~~~
Image I/O
~~~~~~~~~

The image I/O functions are quite limited in comparison to specialized image
processing libraries, *i.e.* only loading/saving 8-bit images is supported.

.. autofunction:: viren2d.load_image_uint8

.. autofunction:: viren2d.save_image_uint8
