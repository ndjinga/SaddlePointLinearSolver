# SaddlePointLinearSolver_SRC

This project provides tools and scripts for testing and analyzing saddle point linear solvers.

## Testing the Project

To test the project, follow these steps:

### 1. Generate Metadata

Before running tests, ensure that metadata JSON files are generated for the simulation matrices. Use the `generate-metadata` target:

```bash
cmake --build . --target generate-metadata
```

This step only needs to be run once unless the input data changes. It will generate metadata files in the `data/` directory.

### 2. Generate Tests

Generate the CMake tests from the CSV input file using the `generate-tests` target:

```bash
cmake --build . --target generate-tests
```

This will create a `generated_tests.cmake` file in the `tests/` directory, which contains the `add_test` entries for the tests. **Rebuild the project only after this step** to include the generated tests:

```bash
cmake --build .
```

### 3. Run Tests

Run the generated tests using CTest:

```bash
ctest
```

CTest will execute the tests defined in the `generated_tests.cmake` file and store the results in the `results/` directory.

### 4. Merge Test Results

After running the tests, merge the results into a CSV file using the `merge-results` target:

```bash
cmake --build . --target merge-results
```

This will create a CSV file in the `tables/` directory containing the aggregated test results.

### 5. Clean Test Results (Optional)

To clean previous test results, use the `clean-results` target:

```bash
cmake --build . --target clean-results
```

This will remove all files in the `results/` directory.

## Directory Structure

- `tests/` : [`SaddlePointLinearSolver_SRC/tests/`] Contains the `generated_tests.cmake` file and test configuration files.
- `data/` : [`SaddlePointLinearSolver_SRC/data/`] Contains input data files and metadata.  
  The `data/` directory must include:
  - `matrices/` : Directory containing matrix files.
  - `metadata/` : Directory for metadata JSON files.
  - `matrices_metadata.csv` : CSV file describing the matrices.
- `results/` : [`SaddlePointLinearSolver_SRC/results/`] Stores test output JSON files.
- `tables/` : [`SaddlePointLinearSolver_SRC/tables/`] Stores merged CSV files with test results.

## Customizing Directories

The following variables can be modified when configuring CMake to change the default directories:

- `DATA_DIR` : Path to the directory containing input data files (default: `tests/data`).
- `TEST_RESULTS_DIR` : Path to the directory where test results will be stored (default: `tests/test_results`).
- `TABLES_DIR` : Path to the directory where merged CSV files will be saved (default: `tests/tables`).

For example, you can set these variables when running `cmake`:

```bash
cmake -DDATA_DIR=/custom/data/path \
    -DTEST_RESULTS_DIR=/custom/results/path \
    -DTABLES_DIR=/custom/tables/path \
    ...
```

## Notes

- Ensure that Python3 is installed and available in your environment.
- The following Python packages are required:
  - `pandas`
- The `CMakeLists.txt` file in the `tests/` directory defines the custom targets used for testing.