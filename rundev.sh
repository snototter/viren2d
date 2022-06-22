#!/bin/bash --
#
# Utility which runs all tests & examples to check
# for broken test cases, demos, ...
#
# Important:
# * This script must be run in the project's root folder
# * This script assumes that the environment is already set up
# * Be aware of demos that wait for user input -- should not be run unattended

# Abort on error
# keep track of the last executed command
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
# echo an error message before exiting
trap 'echo "\"${last_command}\" command filed with exit code $?."' EXIT

cd build
cmake ..
make clean
make -j
ctest -V
./demo-viren2d
cd ..

source venv/bin/activate
rm -rf build/temp*
pip install .
pytest tests/

cd examples
python render_rtd_images.py
python runtimes.py
python demo.py
cd ..

cd docs
make html
cd ..
