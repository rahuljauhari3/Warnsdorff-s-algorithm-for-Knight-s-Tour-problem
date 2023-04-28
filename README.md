# Knight's Tour Problem

This program solves the knight's tour problem on an `N x N` chessboard. The problem consists of finding a sequence of moves of a knight on a chessboard such that the knight visits every square exactly once.

The program is implemented in C and uses pthreads for parallel execution. The program uses a heuristic algorithm to find a solution to the problem.

## How to Use

To compile the program, run the following command in your terminal:

```bash
gcc -pthread -o knightstour knightstour.c
```

To run the program, execute the following command:

```bash
./knightstour <N> <x> <y>
```

where:
- `N` is the size of the chessboard (`N x N`). This argument is required.
- `x` and `y` are the starting position of the knight on the chessboard. If these arguments are not provided, the starting position is chosen randomly.

For example, to solve the knight's tour problem on a 10x10 chessboard with the starting position at (0, 0), run the following command:

```bash
./knightstour 10 0 0
```

## How it Works

The program uses a heuristic algorithm to find a solution to the problem. The algorithm works by choosing the next square to move to based on the number of unvisited squares that are reachable from that square. The square with the smallest number of reachable unvisited squares is chosen.

The program uses pthreads for parallel execution. It creates four threads, each of which tries to find a solution to the problem starting from a different orientation. The program uses a turnstile synchronization design pattern to create a thread barrier. This ensures that at least every thread is doing some work.

Once a solution is found, the program uses a mutex lock to ensure that only one thread sets the solved_status as 1 and outputs the path. The program then outputs the path starting from the initial position, including the orientation of the knight. The program also outputs which thread found the solution.
