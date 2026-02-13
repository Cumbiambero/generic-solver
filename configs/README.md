# Configuration Files

This directory contains pre-configured settings for different types of formula discovery problems.

## Available Configurations

### `default.conf`
Balanced configuration suitable for general-purpose formula discovery. Enables all operators and provides reasonable complexity constraints.

**Best for:** Unknown problem types, exploratory analysis

### `simple-algebraic.conf`
Optimized for discovering simple polynomial and algebraic formulas. Disables complex functions and focuses on basic arithmetic operations.

**Best for:** 
- Polynomial relationships (x², x³, etc.)
- Linear and quadratic equations
- Simple arithmetic formulas

**Examples:**
- Finding f(x) = x² + 3x + 5
- Discovering circle area (πr²)
- Basic kinematic equations

### `physics.conf`
Configured for physics and engineering problems. Enables trigonometric, exponential, and root functions commonly found in physical laws.

**Best for:**
- Kinetic energy formulas
- Gravitational laws
- Projectile motion
- Wave equations
- Thermodynamic relationships

**Examples:**
- E = ½mv²
- F = GMm/r²
- Hooke's Law: F = kx

### `number-theory.conf`
Aggressive configuration for number theory problems. Uses a larger population, allows modulo operations, and permits more complex formulas.

**Best for:**
- Prime number patterns
- Divisibility rules
- Sequence generation
- Cryptographic formulas
- Modular arithmetic

**Note:** This configuration allows very complex formulas and may take longer to converge.

## Using Configuration Files

To use a configuration file with the solver:

```bash
./solver input.csv results.csv x y z --config configs/physics.conf
```

## Configuration Options

You can create custom configuration files with these options:

### Population Settings
- `populationSize` - Number of formulas in the gene pool (default: 1000)
- `hallOfFameSize` - Top solutions to maintain (default: 3)

### Operator Control
- `enableAddition`, `enableSubtraction`, `enableMultiplication`, `enableDivision`
- `enablePower`, `enableModulo`
- `enableTrigonometric` - sin, cos, tan, etc.
- `enableHyperbolic` - sinh, cosh, tanh
- `enableExponential` - exp, exponential functions
- `enableLogarithmic` - log, log10, log2
- `enableRounding` - floor, ceil, round
- `enableRootFunctions` - sqrt, cbrt

### Formula Constraints
- `maxFormulaDepth` - Maximum nesting depth
- `maxFormulaNodes` - Maximum number of operations
- `maxFormulaLength` - Maximum string representation length
- `minConstantValue`, `maxConstantValue` - Range for numeric constants

### Fitness Weights
- `accuracyWeight` - Weight for prediction accuracy (default: 0.7)
- `complexityWeight` - Penalty for complex formulas (default: 0.1)
- `rangeConsistencyWeight` - Reward for consistent output ranges (default: 0.1)
- `monotonicityWeight` - Reward for smooth relationships (default: 0.1)

### Stopping Criteria
- `maxIterations` - Maximum evolution iterations
- `stopOnPerfectMatch` - Stop when exact solution found
- `stopOnHallOfFameFull` - Stop when hall of fame is filled with good solutions

### Progress Reporting
- `progressReportInterval` - How often to report progress (iterations)
- `verboseLogging` - Enable detailed logging

## Creating Custom Configurations

Configuration files use a simple `key = value` format:

```ini
# Comments start with #
populationSize = 2000
enableAddition = true
maxFormulaDepth = 8

# You can use inline comments too
accuracyWeight = 0.9  # Focus on accuracy
```

## Tips for Optimization

1. **For simple problems:** Disable unnecessary operators to speed up search
2. **For complex problems:** Increase population size and max iterations
3. **If formulas are too complex:** Lower maxFormulaDepth and increase complexityWeight
4. **If search is stuck:** Increase randomInjectionCount and lower stagnationThreshold
5. **For number theory:** Enable modulo operations and rounding functions
