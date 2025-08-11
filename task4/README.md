# Knight's Tour Puzzle Solver

This repository contains implementations of various search algorithms to solve the Knight's Tour puzzle problem, where numbered pieces on a board must be rearranged to reach a goal configuration by moving like a chess knight.

## Project Structure

Each directory (1, 2, 3) corresponds to different problem configurations and implementations:
- **Directory 1**: Problem (1) - 3x3 board implementations
- **Directory 2**: Problem (2) - 4x4 board implementations  
- **Directory 3**: Problem (3) - 4x4 board with enhanced A* search

All files in each directory can be compiled simultaneously using:
```bash
$ make
```

## Algorithm Implementations

### 1. Directory 1 - 3x3 Board Basic Algorithms

#### Files:
- `KnightGame_A*1.cpp`: A* search implementation
- `KnightGame_depth1.cpp`: Depth-limited search with iterative deepening
- `KnightGame_width1.cpp`: Breadth-first search implementation

#### Initial State (3x3):
```
8|2|4
-----
9|5|6
-----
7|3|1
```

#### Goal State (3x3):
```
1|2|3
-----
4|5|6
-----
7|8|9
```

### 2. Directory 2 - 4x4 Board Standard Algorithms

#### Files:
- `KnightGame_A*2.cpp`: A* search for 4x4 board
- `KnightGame_depth2.cpp`: Depth-limited search for 4x4 board
- `KnightGame_width2.cpp`: Breadth-first search for 4x4 board

#### Initial State (4x4):
```
10| 1|12| 4
-----------
 5|13| 7| 9
-----------
15| 3| 2| 6
-----------
11|14| 8|16
```

#### Goal State (4x4):
```
 1| 2| 3| 4
-----------
 5| 6| 7| 8
-----------
 9|10|11|12
-----------
13|14|15|16
```

### 3. Directory 3 - Advanced A* Search with Multi-Piece Movement

#### Key Features:
- **Enhanced A* Algorithm**: Implements sophisticated heuristic functions
- **Multi-Piece Strategy**: Allows switching between different numbered pieces
- **Configurable Parameters**: Adjustable minimum movement requirements
- **Performance Optimization**: Uses priority queue and hash-based duplicate detection

#### Files:
- `KnightGame_A*3.cpp`: Advanced A* implementation with multiple heuristic functions
- `KnightGame_depth3.cpp`: Enhanced depth-limited search
- `KnightGame_width3.cpp`: Enhanced breadth-first search

#### Configuration Options in `KnightGame_A*3.cpp`:

##### Minimum Movement Length Configuration:
```cpp
#define MIN_NUM_LENGTH 1
```
This parameter controls the minimum number of consecutive moves required for the same piece before switching to another piece (parameter L in the report).

##### Heuristic Function Selection:
```cpp
void cal_f() {
    //cal_f1();    // Misplaced tiles heuristic
    //cal_f2();    // Misplaced tiles + Manhattan distance  
    cal_f3();     // Misplaced tiles + Manhattan distance % 3
    //cal_f4();    // Combined heuristic (f3 + Manhattan distance)
}
```

#### Heuristic Functions:

1. **f₁(n) - Misplaced Tiles**: Counts the number of tiles not in their goal positions
2. **f₂(n) - Manhattan Distance**: Adds Manhattan distance for each misplaced tile
3. **f₃(n) - Modulated Manhattan**: Uses Manhattan distance modulo 3 to avoid overestimation
4. **f₄(n) - Combined Heuristic**: Combines f₃ with full Manhattan distance

#### Algorithm Features:

##### Knight Movement Pattern:
The algorithm implements the standard chess knight movement pattern with 8 possible moves:
```cpp
char move[][2] = {{1,2}, {2,1}, {-2,1}, {1,-2}, {-1,2}, {2,-1}, {-1,-2}, {-2,-1}};
```

##### State Representation:
- **Board State**: 4x4 array representing piece positions
- **Active Piece**: Currently selected piece for movement
- **Movement Counter**: Tracks consecutive moves of the same piece
- **Parent Pointer**: Maintains solution path for reconstruction

##### Optimization Techniques:
- **Priority Queue**: Efficiently selects nodes with lowest f-value
- **Hash Set**: Prevents revisiting identical board states
- **Performance Monitoring**: Tracks number of generated child nodes and execution time

#### Problem Variants:

The implementation can solve two different initial configurations by commenting/uncommenting the appropriate initial state:

**Problem 3 (Default)**:
```cpp
std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cell_start = {{
    {16, 12, 8, 4},
    {15, 11, 7, 3},
    {14, 10, 6, 2},
    {13, 9, 5, 1}
}};
```

**Problem 2 (Alternative)**:
```cpp
// std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> cell_start = {{
//     {10, 1, 12, 4},
//     {5, 13, 7, 9},
//     {15, 3, 2, 6},
//     {11, 14, 8, 16}
// }};
```

## Compilation and Execution

Each directory contains a Makefile with the following targets:
- `make`: Compile all source files
- `make clean`: Remove all compiled binaries and temporary files
- `make tmpclean`: Remove only temporary files

### Compiler Settings:
- **Compiler**: g++
- **Optimization**: -O2 flag for performance optimization

## Output Information

The programs provide detailed execution information including:
- **Step-by-step board states** during search process
- **Solution path** from initial to goal state
- **Number of child nodes generated** (measure of computational complexity)
- **Solution length** (number of moves required)
- **Execution time** (for performance analysis)

## Technical Implementation Details

### Memory Management:
- Uses `std::shared_ptr` for automatic memory management of board states
- Implements proper parent-child relationships for solution path reconstruction

### Data Structures:
- **Priority Queue**: For A* algorithm node selection
- **Deque**: For breadth-first and depth-first search implementations
- **Unordered Set**: For efficient duplicate state detection using custom hash function

### Performance Features:
- **Custom Hash Function**: Efficiently hashes board states for duplicate detection
- **Progress Monitoring**: Displays progress every 100,000 generated nodes
- **Time Measurement**: High-resolution timing for performance analysis
