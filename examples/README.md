# Usage Examples

**Note (Python):** The Python examples assume that you have a virtual environment set up
and already installed ``viren2d``, plus additional dependencies to display the
results. For example:
```bash
python3 -m venv my-demo-venv
source my-demo-venv/bin/activate
python -m pip install --upgrade pip

# Either install viren2d from a local path:
python -m pip install path/to/viren2d
# Or from github:
python -m pip install git+https://github.com/snototter/viren2d.git

# Install Pillow for displaying images
python -m pip install Pillow
```


## Hello World
The `hello-world-{cpp|python}` examples help to verify that `viren2d` has
been properly set up.
* **C++:** This demo is configured to fetch the library directly from github.
  Note that using `ninja` is optional, although highly recommended.
  ```bash
  cd examples/hello-world-cpp
  mkdir build && cd build
  
  # This exampleNote that the following will fetch viren2d directly from github.
  # Using `ninja` is optional, but recommended.
  cmake -GNinja ..
  cmake --build .
  
  ./hello-world
  ```
* **Python:** 
  ```bash
  python examples/hello-world-python/hello_world.py
  ```


## Demo
The `demo-{cpp|python}` examples are showcase applications with the intend to
demonstrate most of the available functionality.
* **C++:** TODO a) integrated into library build (option viren2d_BUILD_DEMO), b) standalone application
* **Python:** TODO not yet implemented; will maybe never come


## Read the Docs Examples
All Python scripts within `examples/rtd-examples-python` are used to create the
visualizations, cheat sheets, etc. for the documentation at
https://viren2d.readthedocs.io/  
These images can be generated via:
```bash
python examples/rtd-examples-python/render_rtd_images.py
```
Basic runtime measurements can be conducted via:
```bash
python examples/rtd-examples-python/runtimes.py
```

