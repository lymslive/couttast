# AGENTS.md

This file provides essential information for AI agents (including Terminal Assistant Agent/CodeBuddy) to work effectively with the couttast C++ unit testing framework.

> Note: For backward compatibility, this file is also accessible via the symlink `CODEBUDDY.md`.

## Project Overview

couttast is a lightweight C++ unit testing library and framework that provides:
- Header-only library (`tinytast.hpp`) for basic functionality
- Extended static library (`libcouttast.a`) with advanced features
- Simple assertion macros using `COUT` for expression evaluation
- Command-line test runner with filtering capabilities

## Build Commands

### Using Make
```bash
# Build the entire project
make

# Build examples
make example

# Run unit tests
make test

# Clean build artifacts
make clean

# Rebuild from scratch
make rebuild

# Install to custom prefix (defaults to $HOME)
make install PREFIX=/custom/path
```

### Using CMake
```bash
# Standard CMake build
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME
make

# Run tests
make test

# Install
make install

# Build with debug symbols
make cmake.debug
```

## Testing Commands

```bash
# Run main unit tests
cd build && ./utCoutTast

# Run C++98 compatibility tests
cd build && ./utCxx98Tast

# Run specific test cases
./utCoutTast test_name

# List available test cases
./utCoutTast --list

# Run with configuration file
./utCoutTast --cwd=../utest
```

## Architecture Overview

### Core Components
- **Header-only mode**: `include/tinytast.hpp` - Minimal C++98 compatible testing
- **Extended library**: `libcouttast.a` - Advanced features, color output, parallel testing
- **Test driver**: `bin/tast_drive` - Runs test cases from dynamic libraries

### Key Headers
- `tinytast.hpp` - Core testing framework (C++98 compatible)
- `couttast.hpp` - Main header including all functionality
- `unittast.hpp` - Unit test organization utilities
- `classtast.hpp` - Class-based test organization
- `extra-macros.hpp` - Extended assertion macros
- `gtest-macros.hpp` - gtest-style assertion compatibility

### Source Organization
- `src/` - Implementation files for static libraries
- `include/` - Header files for both header-only and library usage
- `utest/` - Unit tests for the framework itself
- `example/` - Usage examples with different build approaches

## Development Patterns

### Writing Test Cases
```cpp
#include "tinytast.hpp"

DEF_TAST(test_name, "Test description") {
    COUT(1 + 1);           // Print expression value
    COUT(1 + 1, 2);        // Assert equality
    COUT(1.0 + 1.0, 2.0);  // Float comparison
}

int main(int argc, char** argv) {
    return RUN_TAST(argc, argv);
}
```

### Test Case Filtering
- No arguments: Run all `DEF_TAST` test cases
- Name arguments: Run specific test cases
- `--list`: List available test cases
- `--cout=silent`: Suppress output for cleaner results

### Library Usage Options
1. **Header-only**: Include `tinytast.hpp` and write your own `main()`
2. **Static library**: Link `libcouttast.a` and omit `main()` (uses weak symbol)
3. **Dynamic library**: Build tests as `.so` and use `tast_drive` to run them

## Important Notes

- The framework uses weak symbols for `main()`, allowing custom `main()` to override
- Test cases are automatically registered using static initialization
- Output formatting can be controlled via `COUT_BIT_*` constants
- Command-line options support ini file configuration mapping
- Multi-process parallel testing is available in the extended library

## File Conventions

- Test files should use `test-` prefix (e.g., `test-mymodule.cpp`)
- Example files are in `example/` subdirectories
- Unit tests for the framework are in `utest/` directory
- Header files in `include/` are designed to be installed system-wide