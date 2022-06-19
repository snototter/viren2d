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

...........
C++ Library
...........

*  For each custom type (where applicable), add a c'tor using 
   ``std::initializer_list`` for less cluttered & more convenient use.

*  All drawing functions should shift the user-given coordinates by `0.5` if
   applicable, to support sharp lines. For details refer to the
   `corresponding Cairo FAQ <https://www.cairographics.org/FAQ/#sharp_lines>`__.

*  Each drawing function should call ``helpers::CheckCanvas`` and if possible,
   reuse the implemented sanity checks, *e.g.* ``helpers::CheckLineStyle``.
   Of course, implement additional sanity checks as needed.

*  Drawing functions won't be tested via mocking, because mocking the Cairo C
   interface would be a pain & I neither want to switch to
   `cairomm <https://github.com/freedesktop/cairomm>`__ nor write my own C++
   wrapper.

   There are, however, tests to ensure that we can identify code changes that
   would break the drawing API before releasing them into the wild, *e.g.*
   check ``tests/test_painter.py``. 
   Besides these interface stability tests, there is currently no plan for
   more detailed drawing functionality testing. Instead, focus on interface
   clarity and/or provide easily understandable/extendable demos.

*  Any non-drawing/non-visualization related functionality should be properly
   tested.

   The rule-of-thumb is to implement program-logic tests (input validation,
   algorithmic correctness, operator overloading madness, *etc.*) in the C++
   test suite. Usability tests (like the interface stability checks
   mentioned earlier) should be conducted via the python test suite.
   
*  TODO update/extend task template

   Task template for (almost) each new function:  
   
   .. code-block:: cpp

      //TODO [ ] add documentation
      //TODO [ ] add C++ test (tests/xxx_test.cpp)
      //TODO [ ] add Python bindings
      //TODO [ ] add Python test (tests/test_xxx.py)
      //TODO [ ] add C++ demo
      //TODO [ ] add Python demo
 

..............
Python Library
..............

TODO order these points:

*  When extending the :class:`~viren2d.Painter`, keep the alphabetic order of
   its ``draw_xxx`` bindings to aid maintainability.

*  TODO Design choice: consistent parameter names in ``draw_xxx``, *i.e.* if
   the method expect a :class:`~viren2d.LineStyle`, the parameter name should
   be ``line_style``.

   Benefits imho: 
   a) using the drawing interface becomes easier (I have an ``XyzStyle``, so the
   parameter will be ``xyz_style``)
   b) is less likely to break the interface if we add extensions, e.g. if we
   choose to support an additional label somewhere, we would need an additional
   style parameter of type :class:`~viren2d.TextStyle`). And then, we run into
   the hardest problem of CS, i.e. naming ;-)
   

*  All relevant interface methods, such as the ``draw_xxx`` variants of the
   :class:`~viren2d.Painter` should provide a minimal code example via their
   docstring.

   This code example should be *copy/pastable* to aid the users of this
   library. For example, refer to the docstring of
   :meth:`~viren2d.Painter.draw_line`.


*  TODO rework/update the following instructions!

   How to bind a new class X

    * Test initialization, pickling, comparison, etc.
    * Declare it py::implicitly_convertible if a simple/intuitive
      conversion exists
    * @deprecated Implement moddef::CreateX (init from py::tuple/list/whatever)
    * All this info does not hold for ImageBuffer - which exposes a
      buffer view (and we need to be able to convert to/from numpy
      arrays)
    * support implicit casts (e.g. from tuples) -- then you can also add ``py::pickle``
    * Implement __str__ & __repr__
    * nice-to-have: operator == and !=

**Intricacies, I wish I had know before:**

   *  Don't ever use python keywords as names of function arguments, or users
      can't re-order the inputs via *kwargs*, such as ``f(arg_x=foo, arg_a=1)``.

      Yes, this was "fun" (read: *a pain*) to debug.

      Refer to the Python documentation for a listing of the
      `language keywords <https://docs.python.org/3.8/reference/lexical_analysis.html#keywords>`__.

   *  Double-check the python bindings for typos, semantic errors due to
      copying/pasting, *etc.* 

      For example, due to the inherent Python design, it is perfectly legal to
      override existing attributes. A copy/paste error can easily lead to
      different Python class attributes modifying *the same* C++ class attribute.

      Debugging this is also not as much fun as it sounds.



~~~~~~~~~~~~~~~~~~~
Testing Environment
~~~~~~~~~~~~~~~~~~~

TODO list notes on testing (googletest, pytest)

*  C++ tests for program-logic
   googletest

   .. code-block:: console
      :caption: Manual C++ Testing Workflow

      # Recommendation: Enable color output for ctest/googletest
      # To enable this permanently, add this definition to your shell
      # configuration, e.g. ~/.bashrc
      export GTEST_COLOR=1

      # Build
      cd /path/to/viren2d/build
      cmake --build .

      ctest -j....TODO


*  Python tests for interface usage - to avoid/identify breaking
   API changes (parameter/variable naming, ordering, type conversions, etc.)
   pytest

   .. code-block:: console
      :caption: Manual Python Testing Workflow

      #TODO doc pytest
      pip install pytest
      pytest tests/test_*.py
  

~~~~~~~~~~~~~~
Library Layout
~~~~~~~~~~~~~~

.. _dev-library-layout:

Before diving into the layout of the code framework, note: to avoid name
clashes or having to use naming schemes which rely on underscores, the physical
C++ and Python libraries use different names:

*  The target name of the C++ library is ``viren2d++``, whereas the target name
   of the Python library is ``viren2d``.

*  Currently, I prefer to statically link the C++ library into the consuming
   application. The Python bindings, however, have to be dynamic libraries.

*  TODO double-check before release

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


...................
Other Functionality
...................

TODO design decisions for pseudocolor, etc.


~~~~~~~~~~~~
Nice-to-Have
~~~~~~~~~~~~

Some **functional features**, that I'd like to see at some time in the future:

*  Pseudocoloring (for any/most common input data types - not only uint8)

*  Support switching between the image surface and cairo's SVG surface.

   Note: I did some preliminary tests on this but didn't follow this route
   for now. Even with the basic SVG examples I wasn't able to correctly
   render image data (always ended up with empty SVGs). This will need some
   deeper digging...

*  Creating stereoglyphs

*  Smoothing of trajectories.
  
   Although this should be implemented in werkzeugkiste, maybe along with sketch
   filtering, like xkcd, *e.g.* 
   `via 1D interpolation <https://github.com/slayton/matlab-xkcdify>`__ ,
   or `as in mpl <https://github.com/JohannesBuchner/matplotlib-xkcdify>`__ - 
   though this will be quite some work due to spline fitting (*e.g.* via
   `ALGLIB <http://www.alglib.net/interpolation/spline3.php#header7>`__ or
   `NCAR/EOL bspline <https://github.com/NCAR/bspline>`__)


Some **workflow-related extensions**, I'd fancy:

*  Packaging and publishing on PyPI

*  Packaging with conda. cairo2 is already available via conda channels.
   Automate via `github actions <https://autobencoder.com/2020-08-24-conda-actions/>`__.

*  Automate the *rtd_example_image* generation via github workflows (upon each
   push, but before the RTD workflow starts building the docs)

*  Prepare github templates for PRs, issue reports, *etc.*


