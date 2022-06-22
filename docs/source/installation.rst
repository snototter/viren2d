.. _installation:

============
Installation
============

------------
Requirements
------------

* A C++ compiler supporting at least ``C++14``.
* `CMake \>= 3.15 <https://cmake.org/>`__ and a
  `compatible build tool <https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html>`__,
  like `Make <https://www.gnu.org/software/make/>`__, `Ninja <https://ninja-build.org/>`__,
  *etc.*
* The `Cairo 2D graphics library <https://www.cairographics.org/download>`__.

  Currently, there are no ``viren2d`` packages - thus, you need the
  Cairo development packages (``-dev`` or ``-devel``) for your system. For
  example, on GNU/Linux distributions, you simply install Cairo via:

  .. code-block:: console

     # Debian & derivatives (like Ubuntu)
     sudo apt install libcairo2-dev

     # Fedora
     sudo yum install cairo-devel

     # openSUSE
     zypper install cairo-devel

* Additional requirements to build the optional Python bindings:

  * `Python \>= 3.6 <https://www.python.org/>`_, along with its development
    library, *i.e.* ``python3-dev`` on Linux/macOS.
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


---
C++
---

.. warning::
   TODO cpp documentation missing - populate with instructions from readme
   
   TODO how to setup/install spdlog is missing (we should also change the overall
   spdlog handling in viren2d, i.e. disable logging if not found & issue a cmake
   warning instead)

   TODO need to verify the requirement list with a fresh install

.. warning::
   Ubuntu 18.04 caveat: You need to install CMake via
   `Kitware's APT repository <https://apt.kitware.com/>`__, because the default
   Ubuntu repository is stuck at version `3.10`.
