.. _installation:

============
Installation
============

------------
Requirements
------------

Most internal dependencies are automatically set up by the library's
CMake-based build pipeline. Nevertheless, the basic build environment and Cairo
must be set up manually:

* A C++ compiler supporting at least ``C++14``.
* `CMake \>= 3.15 <https://cmake.org/>`__ and a
  `compatible build tool <https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html>`__,
  like `Make <https://www.gnu.org/software/make/>`__, `Ninja <https://ninja-build.org/>`__,
  *etc.* For example, on GNU/Linux distributions, you'd simply need:

  .. code-block:: console

    # Debian & derivatives (like Ubuntu)
    sudo apt install cmake build-essentials ninja-build
    
    # On Fedora & openSUSE distributions, you need to install the same
    # packages via your package manager, i.e. `yum` or `zypper`.
    
  .. note::
     If you run **Ubuntu 18.04**, the default software repository is stuck at
     CMake version `3.10`. To get a more recent version, install CMake via
     `Kitware's APT repository <https://apt.kitware.com/>`__.

* The `Cairo 2D graphics library <https://www.cairographics.org/download>`__.

  Currently, there are no ``viren2d`` packages - thus, you need the
  Cairo development packages (``-dev`` or ``-devel``) for your system. For
  example, on GNU/Linux distributions, you simply install Cairo via:

  .. code-block:: console

     # Debian & derivatives (like Ubuntu)
     sudo apt install libcairo2-dev

     # On Fedora & openSUSE distributions, the package name
     # is `cairo-devel`.

* Additional requirements to build the optional Python bindings:

  * `Python \>= 3.6 <https://www.python.org/>`_, along with its development
    library, *i.e.* system package ``python3-dev`` on Linux/macOS.
  * Currently, the Python bindings are only tested with
    `pip \>= 10.0.0 <https://pypi.org/project/pip/>`_.
  * The only runtime dependency is `NumPy \>= 1.7.0 <https://numpy.org/>`_.


------
Python
------

If all requirements are installed, the simplest way to install ``viren2d`` is
to use the default package manager, ``pip``, which supports installation
directly from the github repository:

   .. code-block:: console

      # Set up a virtual environment with up-to-date pip:
      python3 -m venv venv
      source venv/bin/activate
      python -m pip install -U pip
 
      # Install viren2d:
      python -m pip install git+https://github.com/snototter/viren2d.git

To verify the installation, you can optionally run the following copy-pastable
example. Note that this additionally requires the lightweight
`vito <https://pypi.org/project/vito/>`__ package, which can be installed via:

  .. code-block:: console

     python -m pip install vito

  .. literalinclude:: ../../examples/rtd_hello_world.py
     :language: python
     :linenos:
     :caption: Python Hello World Example for ``viren2d``

  .. figure:: ./images/hello_world.jpg
     :width: 400
     :alt: Hello world example
     :align: center

     The resulting *hello world* drawing.

---
C++
---

.. warning::
   TODO cpp documentation missing - populate with instructions from readme/point
   to the cmake examples (local vs fetch content)
   
   (DONE) requirement list checked with fresh/clean Ubuntu 20.04 and 22.04
