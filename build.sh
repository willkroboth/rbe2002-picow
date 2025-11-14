# Exit script early if any commands fail
set -e

# Create build directory to hold compiling work
mkdir -p build
cd build

# Compile cmake project
cmake .. -DPICO_BOARD=pico_w
make
