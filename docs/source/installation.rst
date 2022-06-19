.. _installation:

============
Installation
============

------------
Requirements
------------

TODO you'll need python 3.x-dev packages!

TODO how to install spdlog is missing --> change in viren2d (disable logging if not found & issue a cmake warning)

TODO Note that NumPy>=1.7.0 is a runtime dependency (by pybind11).


* A C++ compiler supporting at least ``C++14``.
* `CMake \>= 3.15 <https://cmake.org/>`__ and a
  `compatible build tool <https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html>`__,
  like `Make <https://www.gnu.org/software/make/>`__, `Ninja <https://ninja-build.org/>`__,
  *etc.*
* The `Cairo 2D graphics library <https://www.cairographics.org/download>`__.

  Currently, there is no plan to ship ``viren2d`` binaries. Thus, you need the
  Cairo development packages (``-dev`` or ``-devel``) for your system. For
  example, on GNU/Linux distributions, you simply install Cairo via:

  .. code-block:: console

     # Debian & derivatives (like Ubuntu)
     sudo apt install libcairo2-dev

     # Fedora
     sudo yum install cairo-devel

     # openSUSE
     zypper install cairo-devel

* To build the Python bindings, you also need
  `Python \>= 3.6 <https://www.python.org/>`_ with
  `pip \>= 10.0.0 <https://pypi.org/project/pip/>`_.


------
Python
------

The simplest & recommended way to install ``viren2d`` is to use the default
package manager, ``pip``:

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

TODO populate with instructions from readme


