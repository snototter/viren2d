.. include:: ../symbols.rst

.. _tutorial-conversion:

--------------
Image Handling
--------------

``viren2d`` was designed to have as little external dependencies as possible.
Thus, it uses its own :class:`~viren2d.ImageBuffer` class to encapsulate image
data. In real applications, however, you usually work with standard libraries,
such as `OpenCV <https://opencv.org/>`_ or `NumPy <https://numpy.org/>`_. The
following examples show how to pass image data between these libraries.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~
viren2d |right-arrow| NumPy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The :class:`~viren2d.ImageBuffer` exposes a buffer view following the
`Python buffer protocol <https://docs.python.org/3/c-api/buffer.html>`_, and
thus, provides fast direct access to its raw internal data representation.
This means that conversion to a :class:`numpy.ndarray` is as simple as:

.. code-block:: python
   :linenos:

   import numpy as np
   import viren2d

   # Get an ImageBuffer, e.g. as the result of previous drawing steps:
   painter = viren2d.Painter(height=600, width=800, color='azure')
   img_buf = painter.get_canvas()
   
   # Create a NumPy array which shares the allocated memory with `img_buf`:
   img_np = np.array(img_buf, copy=False)

   # Create a NumPy array as a copy of the ImageBuffer's memory:
   img_np = np.array(img_buf, copy=True)


~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NumPy |right-arrow| viren2d
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Conversion from :class:`numpy.ndarray` to :class:`~viren2d.ImageBuffer` is easy
thanks to the buffer protocol. You only need to decide whether to copy the data
or not:

.. code-block:: python
   :linenos:

   import numpy as np
   import viren2d

   img_np = np.ones((600, 800, 3), dtype=np.uint8)

   # Create a shared buffer (no memory allocation):
   img_buf = viren2d.ImageBuffer(img_np, copy=False)

   # Create a copy:
   img_buf = viren2d.ImageBuffer(img_np, copy=True)


The only **caveat** is that converting NumPy array views (*e.g.* slices,
non-contiguous buffers, *etc.*) to :class:`~viren2d.ImageBuffer` **will
always result in a deeply copied buffer** (as ``viren2d`` requires contiguous
memory layouts):

.. code-block:: python
   :linenos:

   import numpy as np
   import viren2d

   # Create a non-contiguous view, e.g. by flipping the color channels:
   img_np_bgr = np.ones((600, 800, 3), dtype=np.uint8)
   img_np_rgb = img_np_bgr[:, :, ::-1]

   # Convert this view to an ImageBuffer:
   img_buf = viren2d.ImageBuffer(img_np_rgb, copy=True)

   # Non-contiguous views will *always* result in a copy!
   # If the `copy` flag contradicts this, viren2d will log a warning
   # message and ignore the `copy` request:
   img_buf = viren2d.ImageBuffer(img_np_rgb, copy=False)

   # Note that this warning can be suppressed:
   img_buf = viren2d.ImageBuffer(img_np_rgb, copy=False, disable_warnings=True)



~~~~~~~~~~~~~~~~~~~~~~~~~~~~
viren2d |right-arrow| OpenCV
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Since both :class:`~viren2d.ImageBuffer` and OpenCV's ``cv::Mat`` use standard
C-style memory layout, *i.e.* row-major storage order, converting between these
classes is straightforward.

The only **caveat** is that OpenCV works with images in **BGR(A)**
format by design, whereas ``viren2d`` uses the **RGB(A)** format.
Thus, usually you'll need to swap the *red* and *blue* channels:

.. code-block:: cpp
   :linenos:

   // Request a copy of the canvas, because an ImageBuffer swaps
   // its channels **in-place**.
   viren2d::ImageBuffer img_buffer = painter->GetCanvas(true);

   // Convert color format to BGR(A)
   img_buffer.SwapChannels(0, 2);

   // Create an OpenCV matrix header which reuses the memory (to
   // avoid additional memory allocation).
   // Note that the painter will always return its visualization
   // as ImageBuffer of type `ImageBufferType::UInt8`, which
   // corresponds to OpenCV's `CV_8U`, as the underlying type
   // for both is `uint8_t`.
   cv::Mat cv_buffer(
       img_buffer.Height(), img_buffer.Width(),
       CV_MAKETYPE(CV_8U, img_buffer.Channels()),
       img_buffer.MutableData(), img_buffer.RowStride());


~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OpenCV |right-arrow| viren2d
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Converting from a ``cv::Mat`` to a ``viren2d::ImageBuffer`` is similarly
straightforward:

.. code-block:: cpp
   :linenos:

   // Suppose we already have an OpenCV image of type `CV_32F`
   cv::Mat img_cv = ...

   // Either create a shared ImageBuffer:
   viren2d::ImageBuffer img_buf;

   img_buf.CreateSharedBuffer(
       img_cv.data, img_cv.rows, img_cv.cols,
       img_cv.channels(), img_cv.step,
       viren2d::ImageBufferType::Float);

   // Or let the ImageBuffer copy the data:
   img_buf.CreateCopy(
       img_cv.data,img_cv.rows, img_cv.cols,
       img_cv.channels(), img_cv.step,
       viren2d::ImageBufferType::Float);
   
   // If the OpenCV image is in BGR(A) format, you
   // may want to convert it to RGB(A). Note, however,
   // that this operation changes the buffer in-place!
   img_buf.SwapChannels(0, 2);
