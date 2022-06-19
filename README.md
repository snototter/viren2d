# Computer Vision Results, but Nice-Looking
[![RTD Status](https://readthedocs.org/projects/viren2d/badge/?version=latest&style=flat)](https://viren2d.readthedocs.io)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/snototter/viren2d/blob/main/LICENSE?raw=true)

This is a [Cairo-based](https://www.cairographics.org/) visualization toolbox for common 2D computer vision results (read: *detections, trajectories, and the like*) in both __Python and C++__ environments.

The up-to-date documentation [lives on readthedocs](https://viren2d.readthedocs.io/).


## Installation
TODO add to documentation:

* how to set up sphinx (requirements in docs/)

### Dependencies
TODO add to rtd

* [pybind11](https://github.com/pybind/pybind11) TODO doc; header only; submodule
* [spdlog](https://github.com/gabime/spdlog)  
  TODO doc; submodule; several variants (packaged; header only; installed) - recommended: installed static lib; TODO doc install  
  https://stackoverflow.com/a/29882995/400948  
  ensure that you are on branch v1.x!
  ```bash
  cd libs/spdlog
  mkdir build && cd build
  cmake -DCMAKE_POSITION_INDEPENDENT_CODE=ON ..
  cmake --build .
  # Install location defaults to /usr/local
  # If you change the CMAKE_INSTALL_PREFIX, ensure that viren2d's CMakeLists.txt will
  # find the installed spdlog library (i.e. it needs to locate the spdlogConfig* files
  # which will be installed at /usr/local/lib/cmake/spdlog/)
  sudo cmake --install .
  ```
* [stb](https://github.com/nothings/stb) TODO doc; header only; submodule

### Platforms known to work
TODO add to rtd

So far, `viren2d` has been tested on:  
* Ubuntu 20.04  
  Works out of the box
* Ubuntu 18.04  
  You need to install CMake via [Kitware's APT repository](https://apt.kitware.com/), because the default Ubuntu repository is stuck at v3.10.

There are, however, no platform-specific components in `viren2d` and all dependencies/build tools are available for Unix, Mac and Windows ([Cairo](https://www.cairographics.org/download/), [Ninja](https://ninja-build.org/), [CMake](https://cmake.org/), [python3](https://www.python.org/downloads/), any C++ compiler).  
Please let me know if you've set it up on any other platform, so I can update the install/setup instructions accordingly.  

**Watch out Windows users:**  
There's no official CMake configuration for Cairo and the one [included here](./cmake/FindCairo.cmake) uses a hard-coded search path (i.e. `${CMAKE_CURRENT_LIST_DIR}/libs/cairo-windows`) which will not match your install location. Adjust this path or use your own `FindCairo.cmake` (and please drop me a line or [create a PR](https://github.com/snototter/viren2d/pulls) to update/simplify the installation instructions for future users).

TODO this will be deprecated once we prepare for conda release



### Build from Source
Clone this repository recursively to set up the external libraries.
```bash
git clone --recursive https://github.com/snototter/viren2d.git
cd viren2d
```

To build and install the Python library (assuming you already activated your virtual environment), simply run
```bash
python -m pip install .
```
(This will automatically build any C++ dependencies)

To build and install the C++ library instead, follow the standard CMake pipeline:
```bash
mkdir build && cd build
# Configure
cmake ..

# Build
cmake --build .

# Install (Note that viren2d sets a local CMAKE_INSTALL_PREFIX)
cmake --build . --target install
```

**Note for Python users:**  
If you want to re-install `viren2d` and run into a Ninja build error (Ninja binary not found in temporary directory), you should delete the build cache first:
```bash
rm -r build/temp.*
python -m pip install .
```


# TODOs
* [ ] Use spdlog; set library-global debug level via interface function
  * doc cmake options for cpp
  * if installed via pip:
    ```bash
    CMAKE_ARGS="-Dviren2d_LOG_LEVEL=trace" python -m pip install .
    ```
  * in python, you have to call viren2d.set_log_level('debug')!

