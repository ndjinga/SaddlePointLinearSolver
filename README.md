# SaddlePointLinearSolver_SRC

This project provides tools and scripts for testing and analyzing saddle point linear solvers.

The main prerequisite is PETSc. Python is optional, as well as MPI for parallel execution.

## Install
After cloning, the typical installation is done via the commands  
```bash
mkdir build; cd build

cmake /path/to/SOURCE/DIR -DCMAKE_INSTALL_PREFIX=/path/to/INSTALL/DIR  -DCMAKE_BUILD_TYPE=Release -DSaddlePointLinearSolver_WITH_PYTHON=ON -DSaddlePointLinearSolver_WITH_TESTS=ON -DSaddlePointLinearSolver_WITH_MPI=ON -DPETSC_DIR=/path/to/PETSC/DIR -DPETSC_ARCH=arch-linux-c-opt
```  

```bash
make
```  

The parameter PETSC_ARCH should correspond to your PETSc installation (usually arch-linux-c-opt or arch-linux-c-opt on linux computers).

## Extra Prerequisites for curve plot and notebook generation

- Ensure that Python3 is installed and available in your environment. Set SaddlePointLinearSolver_WITH_PYTHON to ON 
- The following Python packages are required:
  - `pandas` to generate and manipulate tables
  - `matplotlib` to generate plots (cpu times, memory consumption, iteration number)
  - `numpy`
  - jupyter to update the jupyter notebook

## Run Tests

Run the existing tests using CTest:

```bash
make test
```

CTest will execute the tests defined in the project. Result data are recorded in the `tests/results/` directory.
- The `CMakeLists.txt` file in the `tests/` directory defines the custom targets used for testing.

## Merge Test Results

After running the individual tests, you can merge the results into a CSV file using the `merge-results` target:

```bash
make merge-results
```

This will create a CSV file in the `tests/tables/` directory containing the aggregated test results.


## Update Notebook & Figures

To execute the performance analysis notebook and export it as a PDF, use:

```bash
make notebook
```

- The notebook is located at [`tests/notebook/performance_analysis.ipynb`](tests/notebook/performance_analysis.ipynb).
- The generated PDF will have the **same name as the notebook** (`performance_analysis.pdf`) and will be saved in the `tests/notebook/` directory.

To generate figures using the dedicated Python script, use:

```bash
make generate-figures
```

- The script is [`tests/notebook/generate_figures.py`](tests/notebook/generate_figures.py).
- Figures are saved in the `tests/notebook/figures` directory.

**Note:**  
The **path to the CSV data file** must be set in [`tests/notebook/plot_utils.py`](tests/notebook/plot_utils.py).## Adding more  matrices to the Project

## Clean Test Results (Optional)

To clean previous test results, use the `clean-results` target:

```bash
make clean-results
```

This will remove all files in the `tests/results/` directory.


## Directory Structure

- `tests/` : Contains the `generated_tests.cmake` file and test configuration files.
- `tests/data/` :  Contains input data files and metadata.  
  The `data/` directory must include:
  - `matrices/` : Directory containing matrix files.
  - `metadata/` : Directory for metadata JSON files.
  - `matrices_metadata.csv` : CSV file describing the matrices.
- `results/` : Stores test output JSON files.
- `tests/tables/` :  Stores merged CSV files with test results.

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

## Add more matrices
To add more  matrices to the project, copy them in the subfolder tests/data/matrices , update the file tests_parameters.csv listing the matrix names then follow the following steps:

### Generate New Metadata 

Before running tests, ensure that metadata JSON files are generated for the simulation matrices. Use the `generate-metadata` target:

```bash
make generate-metadata
```

This step only needs to be run once unless the input data changes. It will generate metadata files in the `tests/data/metadata` directory.

### Generate New Tests

Generate the CMake tests from the CSV input file [tests/test_parameters.csv](tests/tests_parameters.csv) using the `generate-tests` target:

```bash
make generate-tests
```

This will create a `generated_tests.cmake` file in the `tests/` directory, which contains the `add_test` entries for the new tests. Copy this file in the source directory and then **Rebuild the project after this step** to include the newly generated tests:

```bash
cp ./tests/generated_tests.cmake /path/to/SOURCE/DIR/tests/
cp -R ./tests/generated_tests.cmake /path/to/SOURCE/DIR/tests/
make
```

For more details about the CSV format, refer to the [CSV Format section in the Scripts README](scripts/README.md#csv-format).

You can now run the newly generated tests (as well as the initial ones) using CTest:

```bash
make test
```
CTest will execute the tests defined in the `generated_tests.cmake` file. The mode (minimal or complete) is specified in the CSV file used to generate the tests. In minimal mode, only the command is executed, while in complete mode, data is also recorded in the `tests/results/` directory.

After running the tests, you can merge the results into a CSV file using the `merge-results` target:

```bash
make merge-results
```

This will create a CSV file in the `tests/tables/` directory containing the aggregated test results.

If you wish to use these new results to update the source directory you do 
```bash
cp -r ./tests/test_results /path/to/SOURCE/DIR/tests/
cp -r ./tests/table /path/to/SOURCE/DIR/tests/
make
```

