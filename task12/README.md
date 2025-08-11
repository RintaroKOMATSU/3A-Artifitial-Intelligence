# Multi-Objective Optimization Simulator

A comprehensive multi-objective optimization simulator implementing MOABC (Multi-Objective Artificial Bee Colony) and MOGWO (Multi-Objective Grey Wolf Optimizer) algorithms for arbitrary objective functions. This simulator provides an interactive GUI for configuring optimization parameters, visualizing results, and comparing algorithm performance.

## Features

- **Dual Algorithm Support**: Implements both MOABC and MOGWO algorithms with real-time comparison capabilities
- **Interactive GUI**: Dark-themed user interface built with CustomTkinter for easy parameter configuration
- **Real-time Visualization**: Live plotting of Pareto frontiers with matplotlib integration
- **Function Management**: Load/save custom objective functions in DAT format
- **Performance Metrics**: Built-in calculation of Coverage Rate (CR) and Relative Non-dominated Individuals (RNI)
- **Theoretical Pareto Front**: Optional display of true Pareto optimal solutions for validation
- **Export Capabilities**: Save simulation results as high-quality PDF/PNG figures

## Requirements

The simulator is written in Python 3 and requires the following dependencies:

```bash
pip install numpy matplotlib tk customtkinter
```

## Installation and Usage

1. Ensure all dependencies are installed
2. Run the simulator:

   ```bash
   python3 MOsimulator.py
   ```

## File Structure

- `MOsimulator.py`: Main simulation program containing all algorithm implementations and GUI
- `README.md`: This documentation file
- `functions/`: Directory containing benchmark function definitions in DAT format
  - `p1*.dat` to `p6*.dat`: Correspond to the 6 benchmark functions used in the report

## User Interface Components

### Algorithm Settings

- **Algorithm Selection**: Choose between MOABC, MOGWO, or both algorithms simultaneously
- **Population Size**: Configure the number of individuals in the population (default: 100)
- **Maximum Generation**: Set the termination criterion (default: 1000)
- **External Archive Capacity**: Define the maximum size of the Pareto archive (default: 100)

### Function Configuration

- **Variable Count**: Specify the number of decision variables
- **Variable Bounds**: Set minimum and maximum values for each variable
- **Sub-functions**: Define auxiliary functions (g₁, g₂, etc.) for complex objective functions
- **Objective Functions**: Configure f₁ and f₂ expressions using mathematical notation

### True Pareto Front Settings

- **Enable/Disable**: Toggle display of theoretical Pareto optimal solutions
- **Parameter Range**: Define the range for generating true Pareto front
- **Mathematical Expressions**: Specify the analytical forms of objective functions

### Visualization Controls

- **Start/Pause**: Begin or pause the optimization simulation
- **Real-time Updates**: Watch the evolution of solutions in objective space
- **Legend**: Automatic legend generation for active algorithms

## Supported Mathematical Functions

The simulator supports a wide range of mathematical operations:

- Basic arithmetic: `+`, `-`, `*`, `/`, `**`
- Trigonometric: `sin`, `cos`, `tan`, `arcsin`, `arccos`, `arctan`
- Hyperbolic: `sinh`, `cosh`, `tanh`, `arcsinh`, `arccosh`, `arctanh`
- Exponential/Logarithmic: `exp`, `log`, `log10`, `log2`
- Other: `sqrt`, `abs`, `ceil`, `floor`, `round`, `sign`
- Constants: `pi`, `e`
- Aggregation functions: `SUM(start, end, expression)`, `PRODUCT(start, end, expression)`
- Conditional: `IF(condition, value_if_true, value_if_false)`

### Variable and Function References

- Variables: `x[1]`, `x[2]`, ..., `x[N]` (1-indexed)
- Sub-functions: `g[1]`, `g[2]`, ..., `g[M]` (1-indexed)
- Special variable: `N` (total number of variables)

## Menu Functions

### File Menu

- **Load Function Setting**: Import pre-configured functions from DAT files
- **Save Function Setting**: Export current function configuration
- **Save Figure**: Export visualization as PDF or PNG
- **Exit**: Close the application

### Simulation Menu

- **Start Simulation**: Begin optimization process
- **Pause Simulation**: Temporarily halt the simulation
- **Show CR**: Display Coverage Rate for active algorithms
- **Show RNI**: Calculate and display Relative Non-dominated Individuals (requires both algorithms)

## Algorithm Implementations

### MOABC (Multi-Objective Artificial Bee Colony)

- **Population Initialization**: Random initialization within variable bounds
- **Employed Bee Phase**: Solution improvement using neighborhood search
- **Onlooker Bee Phase**: Selection based on Pareto dominance
- **External Archive**: Maintains non-dominated solutions with crowding distance
- **Mutation Strategy**: Adaptive perturbation with probability Pm = 0.4

### MOGWO (Multi-Objective Grey Wolf Optimizer)

- **Social Hierarchy**: Alpha, beta, and delta wolves selection from external archive
- **Position Update**: Weighted combination of three leader positions
- **Adaptive Parameters**: Linearly decreasing exploration parameter
- **Crowding Distance**: Tournament selection for archive management
- **Diversity Maintenance**: Automatic pruning when archive exceeds capacity

## Performance Metrics

### Coverage Rate (CR)

Measures the portion of the true Pareto front covered by the algorithm's solutions:

```text
CR = (Number of covered segments) / (Total segments)
```

### Relative Non-dominated Individuals (RNI)

Compares the quality of solutions between two algorithms:

```text
RNI_A = (Non-dominated solutions of A) / (Total non-dominated solutions)
```

## Error Handling

The simulator includes comprehensive error checking for:

- Invalid mathematical expressions
- Variable index out of bounds
- Circular references in sub-functions
- Invalid parameter values
- File format validation

## Cross-Platform Compatibility

Developed on Windows with compatibility testing on:

- ECCS terminals (macOS)
- WSL (Windows Subsystem for Linux)
- Native Linux environments

**Note**: UI scaling may vary across different platforms and display configurations.

## Example Usage

1. **Load a benchmark function**: File → Load Function Setting → Select `p1_ZDT1.dat`
2. **Configure algorithms**: Check both MOABC and MOGWO in Algorithm Settings
3. **Adjust parameters**: Set Population Size to 50, Maximum Generation to 500
4. **Enable true Pareto front**: Check "Show true pareto front" for comparison
5. **Start simulation**: Click the circular start button (●)
6. **Monitor progress**: Watch real-time evolution in the visualization panel
7. **Analyze results**: Use Simulation menu to view CR and RNI metrics

## Technical Details

### External Archive Management

Both algorithms maintain an external archive of non-dominated solutions using:

- Pareto dominance sorting
- Crowding distance calculation for diversity preservation
- Automatic size management when capacity is exceeded

### Objective Function Evaluation

The Function class provides:

- Dynamic expression parsing and evaluation
- Support for complex mathematical operations
- Recursive sub-function evaluation
- Comprehensive error validation

### Visualization Features

- Real-time scatter plot updates
- Automatic axis scaling
- Color-coded algorithm differentiation
- Optional theoretical Pareto front overlay
- Export-quality figure generation
