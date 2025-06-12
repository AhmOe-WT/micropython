#!/bin/bash
#Build firmware0.bin and firmware1.bin for a given board.
# Usage: ./build_firmware.sh [BOARD]

set -e

BOARD=${1:-Tiger_Plus}

#Build the mpy-cross compiler if it doesn't exist
if [ ! -f mpy-cross/mpy-cross ]; then
	echo "Building mpy-cross..."
	make -C mpy-cross
fi

# Update submodules and build firmware for the selected board
make -C ports/stm32 BOARD=$BOARD submodules
make -C ports/stm32 BOARD=$BOARD

BUILD_DIR=ports/stm32/build-$BOARD

echo "firmware0.bin and firmware1.bin are located in $BUILD_DIR"
