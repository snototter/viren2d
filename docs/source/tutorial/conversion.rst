.. include:: ../symbols.rst

.. _tutorial-conversion:

--------------------------------
Integration with Other Libraries
--------------------------------

``viren2d`` was designed to have as little external dependencies as possible.
Thus, it uses its own :class:`~viren2d.ImageBuffer` class to encapsulate image
data. In real applications, however, you usually work with standard libraries,
such as `OpenCV <https://opencv.org/>`_ or `NumPy <https://numpy.org/>`_. The
following examples show how to pass image data between these libraries.


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
   provides an option to access its internal memory - refer to the documentation
   of its :meth:`~viren2d.Painter.get_canvas`.
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
almost a no-brainer thanks to the buffer protocol. You only need to decide
whether to copy the data or not:

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
*i.e.* a ``cv::Mat``, is straightforward:

.. code-block:: cpp
   :linenos:

   // Suppose we already have an ImageBuffer, e.g. via Painter::GetCanvas.
   viren2d::ImageBuffer img_buffer = ...
   
   // Create an OpenCV matrix header which reuses the memory (to
   // avoid additional memory allocation)
   cv::Mat cv_buffer(
       img_buffer.height, img_buffer.width,
       CV_MAKETYPE(CV_8U, img_buffer.channels),
       img_buffer.data, img_buffer.stride);


The only **caveat** is that OpenCV works with images in **BGR(A)**
format by design, whereas ``viren2d`` uses the **RGB(A)** format.
Thus, usually you'll want/have to swap the *red* and *blue* channels:

.. code-block:: cpp
   :linenos:

   // Request a copy of the canvas, because an ImageBuffer swaps
   // its channels **in-place**.
   viren2d::ImageBuffer img_buffer = painter->GetCanvas(true);

   // Convert color format to BGR(A)
   img_buffer.SwapChannels(0, 2);

   // Create an OpenCV matrix header which reuses the memory (to
   // avoid additional memory allocation)
   cv::Mat cv_buffer(
       img_buffer.height, img_buffer.width,
       CV_MAKETYPE(CV_8U, img_buffer.channels),
       img_buffer.data, img_buffer.stride);


~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OpenCV |right-arrow| viren2d
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Converting from a ``cv::Mat`` to a ``viren2d::ImageBuffer`` is similarly
straightforward:

.. code-block:: cpp
   :linenos:

   // Suppose we already have an OpenCV image
   cv::Mat img_cv = ...

   // Either create a shared ImageBuffer:
   viren2d::ImageBuffer img_buf;

   img_buf.CreateSharedBuffer(
       img_cv.data, img_cv.cols, img_cv.rows,
       img_cv.channels(), img_cv.step);

   // Or let the ImageBuffer copy the data:
   img_buf.CreateCopy(
       img_cv.data,img_cv.cols, img_cv.rows,
       img_cv.channels(), img_cv.step);
   
   // If the OpenCV image is in BGR(A) format, you
   // may want to convert it to RGB(A). Note, however,
   // that this operation changes the buffer in-place!
   img_buf.SwapChannels(0, 2);
