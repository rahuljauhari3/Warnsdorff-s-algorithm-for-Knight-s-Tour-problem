#! /usr/bin/bash

# Test script for Warnsdorff's Knight's Tour Algorithm
# Usage: ./test.sh <board_size> [mode]
# Mode can be: single, multi, both (default: both)

if [ $# -lt 1 ]; then
    echo "Usage: ./test.sh <board_size> [mode]"
    echo "Mode can be: single, multi, both (default: both)"
    exit 1
fi

BOARD_SIZE=$1
MODE=${2:-both}

# Check if executable exists
if [ ! -f "./warnsdorff_algorithm" ]; then
    echo "Error: warnsdorff_algorithm executable not found. Please compile first with:"
    echo "gcc -pthread -o warnsdorff_algorithm warnsdorff_algorithm.c"
    exit 1
fi

#(time ./warnsdorff_algorithm 20 0 0) > outputs/0.txt 2>&1

rm -rf outputs
mkdir outputs
rm -f __resultNotSolved.txt

echo "Testing Knight's Tour on ${BOARD_SIZE}x${BOARD_SIZE} board in ${MODE} mode..."

for (( i=0; i<$BOARD_SIZE; i++ ))
do 
	for (( j=0; j<$BOARD_SIZE; j++ ))
	do
		(time ./warnsdorff_algorithm $BOARD_SIZE $i $j $MODE) > outputs/[$i][$j].txt 2>&1
	done
done



echo "\nAnalyzing results..."

for (( i=0; i<$BOARD_SIZE; i++ ))
do 
	for (( j=0; j<$BOARD_SIZE; j++ ))
	do
		if grep -q "No Possible Tour" outputs/[$i][$j].txt; then
			echo "No solution for (N, StartX, StartY) : ($BOARD_SIZE, $i, $j)"
			echo "No solution for (N, StartX, StartY) : ($BOARD_SIZE, $i, $j)" >> __resultNotSolved.txt
		fi
	done
done

# Summary
TOTAL_POSITIONS=$((BOARD_SIZE * BOARD_SIZE))
UNSOLVED_COUNT=$(wc -l < __resultNotSolved.txt 2>/dev/null || echo 0)
SOLVED_COUNT=$((TOTAL_POSITIONS - UNSOLVED_COUNT))

echo "\n=== Test Summary ==="
echo "Board size: ${BOARD_SIZE}x${BOARD_SIZE}"
echo "Mode: ${MODE}"
echo "Total positions tested: ${TOTAL_POSITIONS}"
echo "Positions with solutions: ${SOLVED_COUNT}"
echo "Positions without solutions: ${UNSOLVED_COUNT}"

if [ $UNSOLVED_COUNT -gt 0 ]; then
    echo "\nUnsolved positions saved to: __resultNotSolved.txt"
fi

