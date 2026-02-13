# Generic Formula Solver

A sophisticated genetic algorithm-based solver that discovers mathematical formulas from data. This tool uses evolutionary computing to find closed-form expressions that match input-output relationships.

## Features

- **Genetic Algorithm**: Evolutionary approach with mutation, crossover, and selection
- **Multi-threading**: Parallel evolution for faster convergence
- **Configurable Operators**: Enable/disable specific mathematical operations
- **Multiple Fitness Functions**: Basic, enhanced, and ultra-precision modes
- **Progress Tracking**: Real-time visualization of evolution progress
- **Pre-configured Profiles**: Optimized settings for different problem types
- **Formula Constraints**: Control complexity, depth, and size of solutions
- **Export Formats**: Human-readable, JSON, and CSV outputs

## Supported Operations

- **Arithmetic**: Addition, subtraction, multiplication, division, power, modulo
- **Trigonometric**: sin, cos, tan, asin, acos, atan
- **Hyperbolic**: sinh, cosh, tanh
- **Exponential/Logarithmic**: exp, log, log10, log2
- **Root Functions**: sqrt, cbrt
- **Rounding**: floor, ceil, round
- **Other**: abs, sign, sigmoid, gamma, reciprocal
- **Constants**: π (pi), e (Euler's number)

## Installation

### Prerequisites
- C++17 compatible compiler
- CMake 3.10 or higher

### Build

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j
```

### Run Tests

```bash
cmake -DBUILD_TESTING=ON ..
cmake --build . -j
ctest
```

## Usage

### Basic Usage

```bash
./solver input.csv results.csv <variable_names> [options]
```

**Arguments:**
- `input.csv` - CSV file with input values (one row per data point)
- `results.csv` - CSV file with expected output values (one value per row)
- `variable_names` - Space-separated names for variables (e.g., `x` or `mass velocity`)

**Example:**
```bash
./solver input.csv results.csv x --time 60 --threads 4
```

### Command-Line Options

```
--config <file>                   Load configuration from file
--fitness <basic|enhanced|ultra>  Fitness function (default: enhanced)
--target <0..1>                   Target fitness threshold (default: 0.999999)
--time <seconds>                  Time limit in seconds
--threads <N>                     Number of worker threads (default: CPU-1)
--no-cli                          Disable interactive mode (batch mode)
--format <human|json|csv>         Output format (default: human)
--output <file>                   Write output to file (default: stdout)
-h, --help                        Show help message
```

### Configuration Files

Use pre-configured settings for different problem types:

```bash
# For simple polynomials and algebraic formulas
./solver input.csv results.csv x --config configs/simple-algebraic.conf

# For physics and engineering problems
./solver input.csv results.csv mass velocity --config configs/physics.conf

# For number theory problems (primes, sequences, etc.)
./solver input.csv results.csv n --config configs/number-theory.conf

# General purpose (default settings)
./solver input.csv results.csv x --config configs/default.conf
```

See [configs/README.md](configs/README.md) for details on creating custom configurations.

## Examples

### Simple Algebraic Formula (x²)
```bash
cd build
./solver ../test/resources/input-one-to-ten.csv \
         ../test/resources/square-results.csv x \
         --time 10 --threads 4

# Output: (x)^2
```

### Circle Area (πr²)
```bash
./solver ../test/resources/input-one-to-ten.csv \
         ../test/resources/circle-results.csv r \
         --config ../configs/simple-algebraic.conf

# Output: (π*(r)^2)
```

### Kinetic Energy (½mv²)
```bash
./solver ../test/resources/kinetic-energy-input.csv \
         ../test/resources/kinetic-energy-results.csv mass velocity \
         --config ../configs/physics.conf

# Output: (0.5*(mass*(velocity)^2))
```

### Gravitational Force
```bash
./solver ../test/resources/gravity-input.csv \
         ../test/resources/gravity-results.csv m1 m2 d \
         --config ../configs/physics.conf

# Output: ((6.674e-11*(m1*m2))/(d)^2)
```

### Prime Number Search (Research)
```bash
./solver ../test/resources/input-one-to-thousand.csv \
         ../test/resources/primes-results.csv n \
         --config ../configs/number-theory.conf \
         --time 300 --threads 8

# Note: As expected, no perfect closed-form formula exists for primes.
# The solver achieves ~93% accuracy with approximations.
```

## Configuration System

The solver supports extensive runtime configuration through `.conf` files:

### Available Configurations

1. **default.conf** - Balanced general-purpose settings
2. **simple-algebraic.conf** - Optimized for polynomials and basic arithmetic
3. **physics.conf** - Tuned for physics and engineering formulas
4. **number-theory.conf** - Aggressive search for number patterns

### Configuration Options

Create custom `.conf` files with these options:

```ini
# Population settings
populationSize = 1000
hallOfFameSize = 3

# Operator control
enableAddition = true
enableTrigonometric = false
enableModulo = true

# Formula constraints
maxFormulaDepth = 10
maxFormulaNodes = 40
minConstantValue = -1000
maxConstantValue = 1000

# Fitness weights
accuracyWeight = 0.7
complexityWeight = 0.1

# Stopping criteria
maxIterations = 200000
stopOnPerfectMatch = true

# Progress reporting
progressReportInterval = 1000
verboseLogging = false
```

See [configs/README.md](configs/README.md) for complete documentation.

## Output Formats

### Human-Readable (default)
Shows formulas with fitness scores and C++ code for integration.

### JSON
```bash
./solver input.csv results.csv x --format json --output results.json
```

```json
{
  "metrics": { "duration_seconds": 5.23, "threads": 4 },
  "result": { "formula": "(x)^2", "score": 1.0 }
}
```

### CSV
```bash
./solver input.csv results.csv x --format csv --output results.csv
```

```csv
formula,score,duration_seconds
"(x)^2",1.0,5.23
```

## Algorithm Details

### Genetic Algorithm Workflow

1. **Initialization**: Generate random population + heuristic seeds
2. **Selection**: Choose best formulas for reproduction
3. **Mutation**: Apply 24 different mutation operators
   - Simple changes: flip operands, increment constants
   - Structural: add/remove operations, swap functions
   - Adaptive: context-aware targeted mutations
4. **Crossover**: Combine two formulas to create offspring
5. **Evaluation**: Compute fitness using multi-criteria function
6. **Survival**: Keep top performers, discard weak solutions
7. **Repeat** until target reached or time limit

### Fitness Evaluation

**Basic Fitness**: Simple accuracy-based scoring

**Enhanced Fitness** (default):
- 70% Accuracy
- 10% Simplicity (prefers shorter formulas)
- 10% Range consistency
- 10% Monotonicity

**Ultra Precision**: For near-perfect solutions requiring fine-tuning

### Stagnation Handling

- **Diversity injection**: Introduces random formulas when stuck
- **Adaptive mutation**: Uses creative operators during stagnation
- **Progressive intervention**: Escalates strategies if progress stops

## Performance Tips

### For Simple Problems
- Use `--config configs/simple-algebraic.conf`
- Disable unused operators to speed up search
- Set `maxFormulaDepth = 5` to avoid overcomplexity

### For Complex Problems
- Increase `populationSize` to 2000+
- Use more threads: `--threads 8`
- Set longer time limits: `--time 300`
- Enable all operators for maximum search space

### For Number Theory
- Use `--config configs/number-theory.conf`
- Enable modulo operations
- Allow high complexity: `maxFormulaDepth = 12`
- Be patient - these problems are fundamentally hard!

## Limitations

### What the Solver Can Find
- Closed-form algebraic expressions
- Physics formulas with known patterns
- Polynomial relationships
- Combinations of elementary functions

### What the Solver Cannot Find
- **Prime number formulas**: No closed-form exists (proven mathematically)
- Truly random relationships
- Formulas requiring domain knowledge not encoded in operators
- Solutions requiring operations outside the operator set

### Regarding the "Holy Grail" of Prime Formulas

The goal of finding a simple formula for prime numbers is a well-known open problem in mathematics. Current consensus:

1. **No simple closed-form formula exists** for the nth prime
2. **Prime number theorem** gives asymptotic approximations
3. **Sieve methods** are the practical approach for generation
4. **This is why RSA encryption works** - if easy primality formulas existed, cryptography would break

This solver can find approximation formulas that achieve ~90-95% accuracy on prime sequences, demonstrating the genetic algorithm's power, but cannot find a perfect formula because none exists in the mathematical sense.

## Advanced Features

### Progress Monitoring

The solver displays real-time progress:
```
[15.3s] Iter: 5000 | Best: 0.98234567 | Hall: 2 | Stag: 45 | It/s: 327 | Formula: ((x)^2)+3
```

- **Iter**: Current iteration count
- **Best**: Highest fitness score achieved
- **Hall**: Number of solutions in hall of fame
- **Stag**: Stagnation counter (iterations without improvement)
- **It/s**: Iterations per second (throughput)
- **Formula**: Current best formula (truncated)

### Interactive Mode

Without `--no-cli`, the solver runs in interactive mode:
- Type `stop` to halt gracefully
- Type `status` to see current progress
- Type `best` to show top formulas

### Library API

The solver can be used as a C++ library:

```cpp
#include <solver_core/solver_core.hpp>

std::vector<solver_core::Variable> vars = {{"x"}};
// ... load data ...
auto solution = solver_core::solve(vars, input, results);
```

See [include/solver_core/solver_core.hpp](include/solver_core/solver_core.hpp) for API details.

## Contributing

Contributions welcome! Areas for improvement:
- Additional mutation operators
- More sophisticated fitness functions
- Better stagnation escape strategies
- Support for symbolic simplification
- Integration with computer algebra systems

## Future Enhancements

- [ ] Checkpointing to save/resume long searches
- [ ] Distributed computing support
- [ ] GPU acceleration for fitness evaluation
- [ ] Symbolic differentiation and simplification
- [ ] Machine learning-guided operator selection
- [ ] Interactive formula visualization

## License

Copyright 2025, Eduardo Hahn Paredes

## References

- Genetic Programming: Koza, J. R. (1992). "Genetic Programming"
- Symbolic Regression: Schmidt, M. & Lipson, H. (2009)
- Formula Discovery: Udrescu, S. & Tegmark, M. (2020). "AI Feynman"
