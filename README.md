# Knight's Tour Problem

This program solves the knight's tour problem on an `N x N` chessboard. The problem consists of finding a sequence of moves of a knight on a chessboard such that the knight visits every square exactly once.

The program is implemented in C and uses pthreads for parallel execution. The program uses a heuristic algorithm to find a solution to the problem.

## How to Use

To compile the program, run the following command in your terminal:

```bash
gcc -pthread -o warnsdorff_algorithm warnsdorff_algorithm.c
```

To run the program, execute the following command:

```bash
./warnsdorff_algorithm <N> <x> <y> [mode]
```

where:
- `N` is the size of the chessboard (`N x N`). This argument is required.
- `x` and `y` are the starting position of the knight on the chessboard. These arguments are required.
- `mode` is optional and can be one of:
  - `single` or `s` - Run in single-threaded mode only
  - `multi` or `m` - Run in multi-threaded mode only  
  - `both` (default) - Run both single-threaded and multi-threaded modes for performance comparison

For example, to solve the knight's tour problem on a 10x10 chessboard with the starting position at (0, 0), run the following command:

```bash
./warnsdorff_algorithm 10 0 0
```

To run only in single-threaded mode:

```bash
./warnsdorff_algorithm 10 0 0 single
```

To run only in multi-threaded mode:

```bash
./warnsdorff_algorithm 10 0 0 multi
```

## How it Works

The program uses a heuristic algorithm to find a solution to the problem. The algorithm works by choosing the next square to move to based on the number of unvisited squares that are reachable from that square. The square with the smallest number of reachable unvisited squares is chosen.

The program supports both single-threaded and multi-threaded execution modes:

**Multi-threaded Mode:**
- Creates four threads, each trying to find a solution starting from a different orientation
- Uses improved barrier synchronization with pthread condition variables for better thread coordination
- Implements thread-safe throughput tracking and early termination checks
- Uses mutex locks to ensure only one thread sets the solution status and outputs the path

**Single-threaded Mode:**
- Runs sequentially through all four orientations until a solution is found
- Provides baseline performance for comparison with multi-threaded execution

**Performance Timing:**
- Both modes include precise timing measurements using `gettimeofday()`
- Outputs start/end timestamps and total execution duration in milliseconds
- Enables direct performance comparison between single and multi-threaded approaches

## Performance Comparison

The following graph shows a performance comparison between multi-threaded and single-threaded implementations of the Knight's Tour algorithm:

![Multi threaded and Single threaded](Multi%20threaded%20%20and%20Single%20threaded.png)

As shown in the graph, the multi-threaded approach consistently demonstrates significantly lower latency compared to the single-threaded approach across all tested chessboard sizes. The performance advantage of multi-threading is evident from the start and generally widens as the chessboard size increases.

## Performance Testing

The program now includes built-in timing capabilities that allow you to measure and compare performance between single-threaded and multi-threaded modes. When running in "both" mode, the program will:

1. Execute the single-threaded version and measure its execution time
2. Execute the multi-threaded version and measure its execution time  
3. Display timing results for both modes for easy comparison

This makes it easy to benchmark the performance improvements of the multi-threaded implementation on different chessboard sizes and starting positions.
