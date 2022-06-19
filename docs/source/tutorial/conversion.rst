.. include:: ../symbols.rst

.. _tutorial-conversion:

--------------------------------
Integration with Other Libraries
--------------------------------

``viren2d`` was designed to have as little external dependencies as possible.
Thus, it uses its own :class:`~viren2d.ImageBuffer` class to encapsulate image
data. In real applications, however, you usually work with standard libraries,
such as `OpenCV <https://opencv.org/>`_ or `NumPy <https://numpy.org/>`_.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~
viren2d |right-arrow| NumPy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The :class:`~viren2d.ImageBuffer` exposes a buffer view (following the
`Python buffer protocol <https://docs.python.org/3/c-api/buffer.html>`_) and
thus, provides fast direct access to its raw internal data representation:

.. code-block:: python
   :linenos:

   import numpy as np
   import viren2d

   # Get an ImageBuffer, e.g. as the result of previous drawing steps:
   img_buf = painter.get_canvas()
   
   # Create a shared NumPy array (no memory allocation)
   img_np = np.array(img_buf, copy=False)

   # Create the NumPy array as a copy of the ImageBuffer
   img_np = np.array(img_buf, copy=True)


.. note::
   When using *shared* views on the memory, you should obviously pay attention
   to the objects' lifetimes - or meet *segfault, ye olde friend...*

   For the conversion example above, note that the :class:`~viren2d.Painter`
   provides an option to access its internal memory (refer to the documentation
   of its :meth:`~viren2d.Painter.get_canvas`).
   Thus, you can easily get a :class:`numpy.ndarray` which shares the memory
   with the painter's *canvas* (and you should be careful if you then either
   modify the array values or keep on drawing...)

   **TL;DR:** You should know about scopes & garbage collection when using
   shared memory views, **and** it's always a good idea to read the docs of any
   API you're using.



~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NumPy |right-arrow| viren2d
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Conversion from :class:`numpy.ndarray` to :class:`~viren2d.ImageBuffer` is again
almost a no-brainer thanks to the buffer protocol (you only need to decide
whether to copy the data or not):

.. code-block:: python
   :linenos:

   import numpy as np
   import viren2d

   img_np = np.ones((600, 800, 3), dtype=np.uint8)

   # Create a shared buffer (no memory allocation)
   img_buf = viren2d.ImageBuffer(img_np, copy=False)

   # Create a deeply copied buffer
   img_buf = viren2d.ImageBuffer(img_np, copy=True)


~~~~~~~~~~~~~~~~~~~~~~~~~~~~
viren2d |right-arrow| OpenCV
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Since :class:`~viren2d.ImageBuffer` uses the row-major storage order,
converting it to an
`n-dimensional OpenCV array <https://docs.opencv.org/3.4/d3/d63/classcv_1_1Mat.html>`__,
*i.e.* a ``cv::Mat``, is straightforward.
The only caveat is that OpenCV by-design works with images in **BGR(A)**
format, whereas ``viren2d`` works natively with **RGB(A)**.

.. code-block:: cpp
   :linenos:

   // Request a copy of the canvas, because of the (in-place) RGB/BGR conversion
   viren2d::ImageBuffer img_buffer = painter->GetCanvas(true);

   // Convert color format into OpenCV's BGR(A)
   img_buffer.RGB2BGR();

   // Create an OpenCV matrix header which reuses the img_buffer memory (to
   // avoid an additional memory allocation)
   cv::Mat cv_buffer(
       img_buffer.height, img_buffer.width,
       CV_MAKETYPE(CV_8U, img_buffer.channels),
       img_buffer.data, img_buffer.stride);


~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OpenCV |right-arrow| viren2d
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. warning::
   TODO doc, conversion is similar to the
   `deprecated mat2cairo conv <https://github.com/snototter/viren2d/commit/89bc71c048f6350e363534fdb8b723aff2f8bf0b#diff-7f1c583bd0c61d1204326f4062407685f1103ea367013072a2ca8a6814ccd0ba>`__
