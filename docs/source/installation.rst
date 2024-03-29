.. _installation:

============
Installation
============

-----------------
Operating Systems
-----------------

Currently, the main target operating systems are GNU/Linux distributions.
There are, however, no platform-specific components in ``viren2d`` and all
dependencies and build tools are available for Unix, Mac and Windows.
Please let me know if you've set it up on any other platform, so I can update
the installation instructions accordingly.


   .. important::
      **Windows:** There is no official CMake package configuration for Cairo and
      `the one included with <https://github.com/snototter/viren2d/blob/main/cmake/FindCairo.cmake>`__
      ``viren2d`` uses a *hard-coded* search path, *i.e.*
      ``${CMAKE_CURRENT_LIST_DIR}/libs/cairo-windows``. You will have to adjust
      this path or use your own `FindCairo.cmake`.
      
      If you find a simple setup/build workflow for Windows, please drop me a line
      or `consider creating a PR <https://github.com/snototter/viren2d/pulls>`__
      to update the installation instructions for future users.


   .. important::
      **Ubuntu:** If you run `Ubuntu<=18.04`, you need to install CMake via  
      `Kitware's APT repository <https://apt.kitware.com/>`__. The default
      software repository is stuck at CMake version *3.10*.

      From Ubuntu 20.04 on, everything works out-of-the-box. Note, however,
      that tests are only run on the LTS versions.


------------
Requirements
------------

Most internal dependencies are automatically set up by the library's
CMake-based build pipeline. Nevertheless, the basic build environment, Cairo
and Eigen3 must be set up manually:

* A C++ compiler supporting at least ``C++17``.
* `CMake \>= 3.15 <https://cmake.org/>`__ and a
  `compatible build tool <https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html>`__,
  like `Make <https://www.gnu.org/software/make/>`__, `Ninja <https://ninja-build.org/>`__,
  *etc.* For example, on GNU/Linux distributions, you'd simply need:

  .. code-block:: console

    # Debian & derivatives (like Ubuntu)
    sudo apt install cmake build-essentials ninja-build
    
    # On Fedora & openSUSE distributions, you need to install the same
    # packages via your package manager, i.e. `yum` or `zypper`.

* The `Cairo 2D graphics library <https://www.cairographics.org/download>`__.

  Currently, there are no ``viren2d`` packages - thus, you need the
  Cairo development packages (``-dev`` or ``-devel``) for your system. For
  example, on GNU/Linux distributions, you simply install Cairo via:

  .. code-block:: console

     # Debian & derivatives (like Ubuntu)
     sudo apt install libcairo2-dev

     # On Fedora & openSUSE distributions, the package name
     # is `cairo-devel`.

* The header-only `Eigen linear algebra library <https://eigen.tuxfamily.org/>`__.
  
  .. code-block:: console

     # Debian & derivatives
     sudo apt install libeigen3-dev

     # On Fedora & openSUSE distributions, the package name
     # is `eigen3-devel`.

* Additional requirements to build the optional Python bindings:

  * `Python \>= 3.6 <https://www.python.org/>`_, along with its development
    library, *i.e.* system package ``python3-dev`` on Linux/macOS.
    On Windows, please follow the setup instructions of
    `pybind11 <https://pybind11.readthedocs.io/en/stable/basics.html>`__.
  * Python's standard package installer
    `pip \>= 10.0.0 <https://pypi.org/project/pip/>`_.
  * The only runtime dependency is `NumPy \>= 1.7.0 <https://numpy.org/>`_.


------------
Python Setup
------------

If all requirements are installed, the simplest way to install ``viren2d`` is
to use the default package manager, ``pip``, which supports installation
directly from the github repository:

   .. code-block:: console
      :caption: Set up a virtual environment with up-to-date pip.

      python3 -m venv venv
      source venv/bin/activate
      python -m pip install -U pip
 
   .. code-block:: console
      :caption: Install ``viren2d``.

      python -m pip install git+https://github.com/snototter/viren2d.git


-----------
Python Demo
-----------

To verify the installation, you can optionally run the following copy-pastable
example. Note that this additionally requires `Pillow <https://pillow.readthedocs.io/en/stable/>`__ package, which can be installed via:

  .. code-block:: console

     python -m pip install Pillow

  .. literalinclude:: ../../examples/hello-world-python/hello_world.py
     :language: python
     :linenos:
     :caption: ``viren2d`` Hello World Example in Python.

  .. figure:: ./images/hello-world.jpg
     :width: 400
     :alt: Hello world example
     :align: center

     The resulting visualization.


---------
C++ Setup
---------

The recommended way of integrating ``viren2d`` in your application is via
`CMake's FetchContent module <https://cmake.org/cmake/help/latest/module/FetchContent.html>`__,
as shown below:

  .. literalinclude:: ../../examples/hello-world-cpp/CMakeLists.txt
     :language: cmake
     :linenos:
     :lines: 7-23
     :caption: Exemplary ``CMakeLists.txt`` to integrate ``viren2d`` via CMake's FetchContent module. 


--------
C++ Demo
--------

For the corresponding demo application, refer to the
`C++ hello world example <https://github.com/snototter/viren2d/tree/main/examples/hello-world-cpp>`__.
To build this standalone demo, only the C++ source file and the example's
``CMakeLists.txt`` are needed. *FetchContent* will download and set up all
dependencies as needed.
For example, if these two files are placed in a ``hello-world-example``
folder, a classical CMake build would look like:

  .. code-block:: console

     cd hello-world-example
     mkdir && cd build
     cmake ..
     cmake --build .
     ./hello-world
