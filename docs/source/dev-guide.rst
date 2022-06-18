=================
Development Guide
=================

.. _dev-guide:

If you plan on extending ``viren2d``, please consider the following style
guide, design decisions and recommendations. This ensures that your new feature
will be swiftly integrated into the library.

~~~~~~~~~~~~
Coding Style
~~~~~~~~~~~~

The implementation should (mostly) follow the Google coding style guides
`for C++ <https://google.github.io/styleguide/cppguide.html>`__ and
`for Python <https://google.github.io/styleguide/pyguide.html>`__.

General design decision: The Python bindings should use **snake case**
(except for class/type names), whereas the C++ interface should use **camel case**.

Since the documentation is build using `sphinx <https://www.sphinx-doc.org/>`__,
the *reStructuredText* (reST) markup should be used. Check the 
`reStructuredText Primer <https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html>`_
if you need a refresher on reST syntax.


~~~~~~~~~~~~~
Documentation
~~~~~~~~~~~~~

If you're about to extend ``viren2d``, you should already be familiar with
its :ref:`API documentation<api-documentation>` here on
`RTD <https://readthedocs.org/>`_. So the most important advice for documenting
new features is: **Be consistent!**

The interface documentation should focus on **clarity** and **readability**.

Don't forget to update the :ref:`API documentation<api-documentation>`
once the new feature is finished.

Additionally, it might be useful to include a short example in the
:ref:`quickstart tutorial<quickstart>`.



~~~~~~~~~~~~~~
Implementation
~~~~~~~~~~~~~~

The following C++/Python guidelines summarize the most important implementation
tasks (besides implementing the actual functionality).
Please also familiarize yourself with the
:ref:`library layout<dev-library-layout>` explained below.

TODO add guide from dev.md


~~~~~
Tests
~~~~~

TODO list notes on testing

*  C++ tests for logic

*  Python tests for interface usage - to avoid/identify breaking
   API changes (parameter/variable naming, ordering, type conversions, etc.)

.. code-block:: console
   :caption: Testing the C++ library

   # Recommendation: Enable color output for ctest/googletest
   # To enable this permanently, add this definition to your shell
   # configuration, e.g. ~/.bashrc
   export GTEST_COLOR=1

   # Build
   cd /path/to/viren2d/build
   cmake --build .

   ctest -j....TODO


.. code-block:: console
   :caption: Testing the Python bindings

   #TODO doc pytest
   pip install pytest
   pytest tests/test_*.py
   
   

~~~~~~~~
Examples
~~~~~~~~

TODO huge learning benefit vs huge maintenance burden (unless the interface is properly tested and stable!)

python examples preferred
cpp examples nice-to-have


~~~~~~~~~~~~~~
Library Layout
~~~~~~~~~~~~~~

.. _dev-library-layout:

The following subsections provide a hands-on introduction on the library
layout with supplementary explanations on some design choices.


.....................
Drawing Functionality
.....................

To familiarize yourself with the library layout, let's pick a drawing method of
the :class:`~viren2d.Painter`, *e.g.* :meth:`~viren2d.Painter.draw_line`.

First, look up the corresponding **Python binding** in
``src/bindings/bindings_painter.cpp``. In our example,
:meth:`~viren2d.Painter.draw_line` is
`defined here <https://github.com/snototter/viren2d/blob/b2d3757ac450a17c26623988afbb2eb9e29215d7/src/bindings/bindings_painter.cpp#L395>`__.

You'll note that ``viren2d`` uses an additional ``PainterWrapper`` class
between the Python and C++ interface, *i.e.* the ``PainterWrapper::DrawLine``
`defined here <https://github.com/snototter/viren2d/blob/b2d3757ac450a17c26623988afbb2eb9e29215d7/src/bindings/bindings_painter.cpp#L123>`__.
This is due to a design choice: I prefer clean public interfaces, which
requires pure virtual methods.

While this *can* be handled by `pybind11 <https://pybind11.readthedocs.io/>`__
directly, it would require trampoline classes which need additional pybind11
macros & dependencies in the otherwise (rather) puristic C++ interface. The
current wrapper-based solution is much cleaner in my opinion.

The ``PainterWrapper`` simply forwards each call to the public C++ ``Painter``
interface, see ``include/viren2d/drawing.h``. Our ``DrawLine`` method is
`defined here <https://github.com/snototter/viren2d/blob/b2d3757ac450a17c26623988afbb2eb9e29215d7/include/viren2d/drawing.h#L215>`__.

Another design choice is that the public C++ interface should provide sane
default values for optional parameters. To avoid the hazzle of potentially
defining different defaults in the implementing subclasses, I use protected
pure virtual ``DrawXXXImpl`` (*implementation*) methods. For example, the
``DrawLineImpl`` is
`declared here <https://github.com/snototter/viren2d/blob/b2d3757ac450a17c26623988afbb2eb9e29215d7/include/viren2d/drawing.h#L344>`__.

This abstract interface is implemented by the ``ImagePainter`` within
``src/drawing.cpp``. Its main tasks are handling the internal
`Cairo <https://www.cairographics.org/>`__ context (*i.e.* resource
allocation and cleaning up) and to export the visualization upon user request.

To aid maintainability, the actual drawing functionalities are implemented
as seperate helpers. These helpers are declared in the header file
``src/helpers/drawing_helpers.h``.
Thus, the ``ImagePainter``'s ``DrawXXXImpl`` methods (*e.g.* for
`our line example <https://github.com/snototter/viren2d/blob/b2d3757ac450a17c26623988afbb2eb9e29215d7/src/drawing.cpp#L180>`__)
only need to invoke the appropriate helper.

As another design choice, ``viren2d`` refrains from using external language
bindings of Cairo and instead directly uses its C library. The major reason
is that I had to learn about Cairo anyhow and I was too lazy to look up the
corresponding idioms in `cairomm <https://github.com/freedesktop/cairomm>`__.
Now, those familiar with using any C library will understand, why encapsulating
(read *hiding*) the complexity of the required wrapping code is another good
reason to outsource these drawing helpers.

**TL;DR**, the actual drawing part of this :meth:`~viren2d.Painter.draw_line`
walkthrough is
`implemented here <https://github.com/snototter/viren2d/blob/b2d3757ac450a17c26623988afbb2eb9e29215d7/src/helpers/drawing_helpers_primitives.cpp#L399>`__.


**Recommended Readings:**

In case you need to familiarize yourself with Cairo, I can recommend:

*  The primary sources of official
   `Cairo documentation <https://www.cairographics.org/documentation/>`__.

   In particular, I'd start with the
   `Cairo tutorial <https://www.cairographics.org/tutorial/>`__, which
   provides a neat introduction of its drawing model.

   Then, progress to the
   `example code snippets <https://www.cairographics.org/samples/>`__,
   read the (short but important) `FAQ <https://www.cairographics.org/FAQ/>`__,
   and then consider the official tips in the
   `Cairo cookbook <https://www.cairographics.org/cookbook/>`__.

   Finally, frequently consult the
   `Cairo API manual <https://www.cairographics.org/manual/>`__
   when using the library.

*  Another helpful ressource are the drawing routines implmented by
   `Cairou (formerly Cairocks) <https://github.com/cubicool/cairou>`_.


.................................
Other Visualization Functionality
.................................

TODO design decisions for pseudocolor, etc.


~~~~~~~~~~~~
Nice-to-Have
~~~~~~~~~~~~

Some functional features, that I'd like to see at some time in the future:

*  Pseudocoloring (for any/most common input data types - not only uint8)

*  Support switching between the image surface and cairo's SVG surface.

   Note: I did some preliminary tests on this but didn't follow this route,
   because even with the basic SVG examples I wasn't able to correctly
   render image data. This will need some deeper digging than I can currently
   effort.

*  Creating stereoglyphs


Some workflow-related extensions, I'd fancy:

*  Packaging and publishing on PyPI

*  Packaging with conda (cairo2 is already available via conda channels)

*  Automate the *rtd_example_image* generation via github workflows (upon each
   push, but before the RTD workflow starts building the docs)

