# Protein Folding Optimization using ABC Algorithm

This project implements protein folding optimization based on HP lattice model, using the Artificial Bee Colony (ABC) algorithm. The program simulates protein folding in both 2D and 3D space and visualizes the results using OpenGL.

## Features

- **ABC Algorithm Implementation**: Optimizes protein folding using employed bees, onlooker bees, and scout bees
- **2D/3D Folding Support**: Supports both 2D and 3D protein folding simulation
- **Interactive 3D Visualization**: Real-time visualization with mouse controls
- **Detailed Logging**: Comprehensive simulation logs saved to `data/` directory
- **Progress Tracking**: Real-time progress display with statistics

## Requirements

### System Dependencies

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt-get install freeglut3 freeglut3-dev
```

### Required Libraries

- **OpenGL**: Graphics rendering library
- **GLUT/FreeGLUT**: OpenGL Utility Toolkit for window management
- **GLU**: OpenGL Utility Library
- **C++11 or later**: Modern C++ standard

## Project Structure

```
.
├── Makefile             # Build configuration
├── README.md            # This file
├── bin/                 # Compiled executables (generated after make)
├── data/                # Simulation log files (auto-generated)
├── include/             # Header files
│   ├── abc_hp.h         # ABC algorithm header
│   ├── decode.h         # Protein sequence decoder header
│   ├── display.h        # OpenGL visualization header
│   ├── setting.h        # Configuration settings header
│   └── vec3.h           # 3D vector operations header
└── src/                 # Source files
    ├── abc_hp.cpp       # ABC algorithm implementation
    ├── decode.cpp       # Protein sequence decoder
    ├── display.cpp      # OpenGL visualization
    ├── main.cpp         # Main program and simulation logic
    └── setting.cpp      # Configuration settings implementation
```

## Compilation

### Using Make (Recommended)

```bash
# Clone or navigate to the project directory
cd ABC/

# Compile the project
make

# Clean build files (optional)
make clean
```

## Usage

### Running the Program

```bash
# Run with default parameters
./bin/main

# The program will:
# 1. Request you to set configuration parameters
# 2. Display configuration parameters
# 3. Generate initial population
# 4. Run ABC optimization for specified generations
# 5. Show results and statistics
# 6. Open 3D visualization window
# 7. Save detailed log to data/ directory
```

## Configuration

### Interactive Configuration System

When you run the program, an interactive configuration system will guide you through setting up the simulation parameters. The system provides real-time validation and error handling for all inputs.

The configuration process includes the following parameters:

### 1. Optimization Dimension Selection

**Interactive Controls:**
- Use **Left Arrow (←)** and **Right Arrow (→)** keys to select between 2D and 3D
- Press **Enter** to confirm your selection
- **Default:** 2D optimization

```plain text
? Select the dimension for the optimization: 2D / 3D
✔ Dimension for the optimization: 2D
```

**Options:**
- **2D Mode**: Faster computation, suitable for initial testing and quick results
- **3D Mode**: More realistic protein folding simulation with complete spatial freedom

### 2. Protein Sequence Input

**Input Format:** The system accepts a compact notation for protein sequences:

```plain text
? Enter the input array (default = H10H2(PH)3(P3)2HPH4(PH)3PH2):
✔ Input array: H10H2(PH)3(P3)2HPH4(PH)3PH2
```


**Supported Syntax:**
- **Single amino acids**: `H` (hydrophobic), `P` (polar)
- **Repetition notation**: `H10` = `HHHHHHHHHH` (10 hydrophobic amino acids)
- **Grouped sequences**: `(PH)3` = `PHPHPH` (repeat PH pattern 3 times)
- **Complex combinations**: `(P3)2` = `PPPPPP` (P repeated 3 times, then whole group repeated 2 times)
- **Whitespace**: Spaces are ignored and can be used for readability

**Validation Features:**
- **Syntax error detection**: Shows exact position of syntax errors with visual indicators
- **Minimum length requirement**: Sequences must be at least 4 amino acids long
- **Real-time parsing**: Input is decoded and validated immediately

**Default Sequence:** `H10H2(PH)3(P3)2HPH4(PH)3PH2`
- Decodes to: `HHHHHHHHHHHHPHPHPHPPPPPPHPHHHHPHPHPHPHH` (39 amino acids)

**Examples**

```
length   sequence
 20      (HP)2PH2PHP2HPH2P2HPH
 48      P2HP2H2P2H2P5H10P6H2P2H2P2HP2H5
 48      HPH2P2H4PH3P2H2P2HPH2PHPH2P2H2P3HP8H2
 48      PHPH2P2HPH3P2H2PH2P3H5P2HPH2(PH)2P4HP2(HP)2
 48      PH2P6H2P3H3PHP2HPH2(P2H)2P2H2P2H7P2H2
```

### 3. Number of Generations

**Description:** Sets the maximum number of evolutionary iterations for the ABC algorithm.

```plain text
? Enter the number of generations (default=100):
✔ Generations: 100
```

**Parameters:**
- **Default:** 100 generations
- **Valid range:** Any positive integer
- **Recommendation:** 
  - Small sequences: 50-100 generations
  - Large sequences: 200-500 generations
  - Quick testing: 10-50 generations

### 4. Population Size

**Description:** Defines the number of candidate solutions (bees) in each generation.

```plain text
? Enter the population size (default = 1000):
✔ Population size: 1000
```

**Parameters:**
- **Default:** 1000 individuals
- **Valid range:** Any positive integer
- **Impact on performance:**
  - Larger populations: Better exploration, slower computation
  - Smaller populations: Faster computation, may miss optimal solutions

### 5. Scout Bee Trial Limit

**Description:** Maximum number of unsuccessful attempts before a food source (solution) is abandoned and replaced by a scout bee.

```plain text
? Enter the max trials for Scout Bee (default = 50):
✔ Max trials for Scout Bee: 50
```

**Parameters:**
- **Default:** 50 trials
- **Valid range:** Any positive integer
- **Algorithm behavior:**
  - Lower values: More exploration, faster diversification
  - Higher values: More exploitation, slower diversification
- **Recommendation:**
  - Exploration-focused: 20-40 trials
  - Exploitation-focused: 50-100 trials

### Input Validation and Error Handling

The configuration system includes comprehensive validation:

1. **Dimension Selection**: Only arrow keys and Enter are accepted
2. **Protein Sequence**: 
   - Syntax validation with error position indicators
   - Minimum length enforcement (≥ 4 amino acids)
   - Automatic decoding of compact notation
3. **Numeric Parameters**: 
   - Positive integer validation for all numeric inputs
   - Clear error messages with retry prompts
4. **Default Values**: Press Enter without input to use default values

### Example Configuration Session

```plain text
Please configure the simulation settings. If you want to use the default value, just press Enter.
✔ Dimension for the optimization: 3D
✔ Input array: H10H2(PH)3(P3)2HPH4(PH)3PH2  
✔ Generations: 100
✔ Population size: 1000
✔ Max trials for Scout Bee: 50
```

## 3D Visualization Controls

### Mouse Controls

- **Left Mouse + Drag**: Rotate the 3D model using trackball rotation
- **Right Mouse + Drag**: Zoom in/out (change viewing distance)
- **Mouse Wheel**: Zoom in/out (alternative method)

### Visualization Elements

- **Red/Blue Spheres**: Amino acids (H = hydrophobic, P = polar)
- **Solid Cylinders**: Covalent bonds between adjacent amino acids
- **Dashed Cylinders**: Hydrogen bonds between non-adjacent amino acids
- **Lighting**: Multiple light sources for better 3D visualization

### Navigation Tips

1. **Rotation**: Click and drag with left mouse button to rotate around the protein
2. **Zooming**: Use right mouse button drag or mouse wheel to zoom
3. **Reset View**: Close and restart the program to reset to default view

## Output Files

### Console Output

- Real-time progress with generation statistics
- Final results summary in a formatted box
- Execution time and performance metrics

### Log Files

Log files are automatically saved to `data/simulation_log_YYYYMMDD_HHMMSS.txt` with:

- Simulation parameters and configuration
- Generation-by-generation statistics
- New best solution discoveries
- Final results and optimal sequence
- Execution time and performance data

## Algorithm Details

### ABC Algorithm Components

1. **Employed Bees**: Explore solutions around current food sources
2. **Onlooker Bees**: Select promising solutions using roulette wheel selection
3. **Scout Bees**: Generate new random solutions when current ones stagnate

### Fitness Function

The fitness function counts the number of hydrogen bonds between non-adjacent hydrophobic amino acids that are spatially adjacent (distance = 1).

### Move Encoding

- **S (0)**: Straight (continue in current direction)
- **L (1)**: Left turn
- **R (-1)**: Right turn
- **U (2)**: Up (3D only)
- **D (-2)**: Down (3D only)

## Trouble Shooting

### Memory Issues

When using large population sizes (e.g., > 1,000) or long protein sequences, the program may consume significant amounts of memory and potentially cause out-of-memory errors. If you encounter memory-related issues, consider the following solutions:

#### 1. Reduce Parameters

- Decrease the population size
- Use shorter protein sequences for testing
- Reduce the number of generations

#### 2. Enable Swap Memory (Linux)

If you need to use large parameters, you can create additional swap space to extend available memory:

**Check current memory and swap usage:**

```bash
# Check memory usage
free -h

# Check current swap
swapon --show
```

**Create a swap file (example: 4GB):**

```bash
# Create a 4GB swap file
sudo fallocate -l 4G /swapfile

# Set correct permissions
sudo chmod 600 /swapfile

# Set up the swap space
sudo mkswap /swapfile

# Enable the swap file
sudo swapon /swapfile

# Verify the swap is active
swapon --show
```

**Make swap permanent (optional):**
```bash
# Add to /etc/fstab to make swap persistent across reboots
echo '/swapfile none swap sw 0 0' | sudo tee -a /etc/fstab
```

**Remove swap when no longer needed:**
```bash
# Disable swap
sudo swapoff /swapfile

# Remove the swap file
sudo rm /swapfile

# Remove from /etc/fstab if added
sudo sed -i '/\/swapfile/d' /etc/fstab
```

#### 3. Memory Usage Guidelines

**Estimated memory usage per individual in population:**
- 2D folding: ~1-2 KB per amino acid
- 3D folding: ~2-4 KB per amino acid

**Example calculations:**
- 50 amino acids, 1000 population, 3D: ~200 MB
- 100 amino acids, 5000 population, 3D: ~2 GB
- 200 amino acids, 10000 population, 3D: ~16 GB

**Recommended limits without additional swap:**
- Systems with 8GB RAM: Population ≤ 5,000
- Systems with 16GB RAM: Population ≤ 10,000
- Systems with 32GB RAM: Population ≤ 20,000

## Example Output

```plain text
╔══════════════════════════════════════════════════════════╗
║              PROTEIN FOLDING OPTIMIZATION                ║
║                 Using ABC Algorithm                      ║
╚══════════════════════════════════════════════════════════╝

=== Parameters ===
Input Sequence: 
   HHHHHHHHHHHHPHPHPHPPPPPPHPHHHHPHPHPHPHH
Input Length: 39
Population Size: 1000 
Generations: 100
Trial Limit: 50
Optimization Mode: 3D

=== Evolution Process ===

Generating initial population ...
Initial population generated
Initial best fitness: 12 hydrogen bonds
Log file: data/simulation_log_20250811_175935.txt

Generation   10/100 │ [███▶                          ]  10.0% │ Best: 22 │ Avg: 13.3 │ Conv:   1/1000
Generation   20/100 │ [██████▶                       ]  20.0% │ Best: 23 │ Avg: 16.3 │ Conv:   3/1000
Generation   30/100 │ [█████████▶                    ]  30.0% │ Best: 23 │ Avg: 17.9 │ Conv:  10/1000
Generation   40/100 │ [████████████▶                 ]  40.0% │ Best: 25 │ Avg: 18.9 │ Conv:   4/1000
Generation   50/100 │ [███████████████▶              ]  50.0% │ Best: 25 │ Avg: 19.7 │ Conv:   7/1000
Generation   60/100 │ [██████████████████▶           ]  60.0% │ Best: 26 │ Avg: 20.1 │ Conv:   1/1000
Generation   70/100 │ [█████████████████████▶        ]  70.0% │ Best: 27 │ Avg: 20.2 │ Conv:   1/1000
Generation   80/100 │ [████████████████████████▶     ]  80.0% │ Best: 27 │ Avg: 20.6 │ Conv:   1/1000
Generation   90/100 │ [███████████████████████████▶  ]  90.0% │ Best: 27 │ Avg: 20.9 │ Conv:   3/1000
Generation  100/100 │ [██████████████████████████████] 100.0% │ Best: 27 │ Avg: 21.0 │ Conv:   7/1000
  Detailed Statistics: Max=27.0 Min=1.0 Mean=21.10 StdDev=3.98 Convergence=7/1000

Simulation finished

╔══════════════════════════════════════════════════════════╗
║                    SIMULATION RESULTS                    ║
╠══════════════════════════════════════════════════════════╣
║ Configuration:                                           ║
║   Input Sequence:                                        ║
║     HHHHHHHHHHHHPHPHPHPPPPPPHPHHHHPHPHPHPHH              ║
║   Input Length: 39                                       ║
║   Population Size: 1000                                  ║
║   Max Generations: 100                                   ║
║   Trial Limit: 50                                        ║
║   Optimization Mode: 3D                                  ║
╠══════════════════════════════════════════════════════════╣
║ Results:                                                 ║
║   Maximum Hydrogen Bonds: 27                             ║
║   Execution Time: 25.727 sec                             ║
╚══════════════════════════════════════════════════════════╝

Simulation log has been written to: data/simulation_log_20250811_175935.txt

Opening 3D visualization...
```
