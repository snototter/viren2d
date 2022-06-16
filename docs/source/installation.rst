.. _installation:

============
Installation
============

------------
Requirements
------------

TODO cmake min version should be bumped to 3.15
TODO you'll need python 3.x-dev!

* A C++ compiler supporting at least ``C++14``.
* `CMake \>= 3.14 <https://cmake.org/>`_ and a
  `compatible build tool <https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html>`_,
  like `Make <https://www.gnu.org/software/make/>`_, `Ninja <https://ninja-build.org/>`_,
  *etc.*
* The `Cairo 2D graphics library <https://www.cairographics.org/download>`_.
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

TODO populate with instructions from readme


---
C++
---

TODO populate with instructions from readme


