.. _tutorial-conversion:

.. include:: ../symbols.rst

--------------------------------
Integration with Other Libraries
--------------------------------

``viren2d`` was designed to have as little external dependencies as possible.
Thus, it uses its own :class:`~viren2d.ImageBuffer` class to encapsulate image
data. In real applications, however, you usually work with standard libraries,
such as `OpenCV <https://opencv.org/>`_ or `NumPy <https://numpy.org/>`_.

TODO

~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NumPy |right-arrow| viren2d
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Conversion from NumPy arrays to :class:`~viren2d.ImageBuffer` leverages the
standard `Python buffer protocol <https://docs.python.org/3/c-api/buffer.html>`_.

NumPy>=1.7.0 is a runtime dependency due to our use of pybind11.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~
viren2d |right-arrow| NumPy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The :class:`~viren2d.ImageBuffer` exposes a buffer view (following the
`Python buffer protocol <https://docs.python.org/3/c-api/buffer.html>`_) and
thus, provides fast direct access to its raw internal data representation.


~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OpenCV |right-arrow| viren2d
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TODO similar to deprecated mat2cairo and cairo2mat code
https://github.com/snototter/viren2d/commit/89bc71c048f6350e363534fdb8b723aff2f8bf0b#diff-7f1c583bd0c61d1204326f4062407685f1103ea367013072a2ca8a6814ccd0ba


~~~~~~~~~~~~~~~~~~~~~~~~~~~~
viren2d |right-arrow| OpenCV
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TODO mention **Caveat:** OpenCV stores images in BGR(A) format, whereas viren2d works natively with RGB(A)
TODO get_canvas doc, add: canvas will always be in RGBA format (4-layers!)

.. code-block::
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

