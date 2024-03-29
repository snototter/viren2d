# Task List
* [ ] Collect all TODOs in this central document


## Color class
* [ ] Extend with `to_lab`, `from_lab`
* [ ] Extend with `from_hsv`


## Color gradient
* [ ] Add tests


## ImageBuffer class
* [ ] Color space conversion to Lab
* [ ] Python bindings `CastToImageBufferUInt8C4` - reconsider scaling factors for (u)int16 and (u)int32.


## Library functionalities
* [ ] Test collage, benchmark runtime
* [ ] Refactor StreamColorizer: set lower/upper, compute only the other from input data range.
* [ ] pybind11: Check if there are any downsides to passing `const py::object &` (they're `py::handle` and thus, references anyhow).
* [ ] Color::FromObjectID Python bindings - support negative integers.

## Style definitions
* [ ] Implement ToDetailedString() for each style.  
  To be used in exceptions + exposed in Python API as `detailed_str`.
* [ ] Finalize RTD documentation, *i.e.* add reference/cheat sheet for each style.


## Testing
* [ ] Automate testing via CI hooks
* [ ] Investigate code coverage (how to combine C++ and Python coverage?)
* [ ] Restructure the complete test suite!
* [ ] Test `positioning.h` contents


## Installation & Building
* [ ] Eigen include/typedef causes compile error in drawing.h
* [ ] Remove spdlog and stb submodules, replace by CMake fetch
* [ ] Maybe fetch Eigen also(?)
* [ ] Include backup/fallback snapshots of the 3 major dependencies, i.e. spdlog, stb and pybind11 (not needed as long as they're actively maintained)
* [ ] Properly setup the `install` target of the C++ library.
* [ ] Refactor CMakeLists - use default paths (as e.g. in `wzk`)


## Demos
* [ ] Restructure all examples, see ``examples/hello-world-cpp`` (which is clear and concise)
* [ ] Clean up all demos


## Documentation
* [ ] Investigate git hooks to automate image creation for RTD docs.
* [ ] Add collage to landing page.
* [ ] Add type information to `returns` in docstrings.
* [ ] Document how to set up sphinx (requirements in docs/)
* Summary of helpful Sphinx tweaks: https://brendanhasz.github.io/2019/01/05/sphinx.html


