# Scripts Documentation

## generate_metadata.py

This script generates metadata JSON files for simulation matrices based on a CSV file.

### Usage

```bash
python3 scripts/generate_metadata.py \
  --data-path <data_directory> \
  [--reset]
```

### Arguments
- --data-path : Root directory containing matrices/, metadata/, and matrices_metadata.csv (required)
- --reset : If set, clears the metadata/ directory before generating new metadata (optional)

## generate_tests

This script reads a CSV file describing test configurations and generates a CMake file containing the corresponding `add_test` entries. Each test runs a Python wrapper that executes a simulation binary and records the results.

The generated CMake file should then be included in a `CMakeLists.txt` (for example, the one in the tests directory) to integrate the tests into the build and testing process.

### Usage

```bash
python3 scripts/generate_tests.py \
  --input-csv-file <test_parameters_csv> \
  --output-cmake-file <generated_cmake_file> \
  --data-dir <data_directory> \
  --test-results-dir <results_output_directory> \
  --tmp-dir <temporary_directory>
```

### Arguments

- `--input-csv-file` : Path to the CSV file describing test configurations (required)
- `--output-cmake-file` : Path to the generated CMake file (required)
- `--data-dir` : Directory containing meshes, metadata, and matrix metadata CSV (required)
- `--test-results-dir` : Where test output JSONs will be stored (required)
- `--tmp-dir` : Directory for temporary JSON outputs (required)

### CSV Format & `n_proc` Behavior

The input CSV must include the following columns:
test_id, executable, matrix_name, matrix_type, n_proc

The n_proc field supports 3 formats:

- Single value: 2 → generates one test with 2 processes.
- List: {1,3,5} → generates 3 tests: one for each value in the list.
- Range: [2-5] → generates 4 tests: 2, 3, 4, and 5 processes.

Each test will receive a unique test name. For multiple n_proc values, a suffix is appended:
e.g., my_test becomes my_test_nproc3, my_test_nproc5, etc.

## merge_results.py

This script merges multiple JSON result files into a single CSV table.

### Usage

```bash
python3 scripts/merge_results.py \
  --results-dir <path_to_results_directory> \
  --tables-dir <output_directory_for_csv> \
  --output-name <csv_filename_without_extension> \
  [--reset]
```

### Arguments

- `--results-dir` : Directory containing JSON result files (required)
- `--tables-dir` : Directory where the merged CSV will be saved (required)
- `--output-name` : Name of the resulting CSV file, without the `.csv` extension (required)
- `--reset` : If set, allows overwriting an existing CSV file (optional)

### Behavior

- Recursively scans the results-dir for .json files.
- Verifies that each file contains the required fields:  
`test-id`, `executable-name`, `matrix-name`, `matrix-type`, `n-proc`, `iter`, `iter1`, `iter2`, `residual`, and `error`.
- Aggregates all valid entries into a CSV file saved under `tables-dir/output-name.csv`. 
- The script raises an error if the file already exists unless `--reset` is specified.

## run_one_test.py

This script runs a test on a specified matrix using a given executable, collects the output, and saves both input parameters and results in a JSON file.

### Usage

```bash
python3 scripts/run_one_test.py \
  --test-id <test_id> \
  --executable <path_to_executable> \
  --matrix-name <matrix_name> \
  --matrix-type <matrix_type> \
  --n-proc <number_of_processes> \
  --data <data_directory> \
  --result-path <results_directory> \
  --tmp-path <temporary_output_json>
```

### Arguments

- `--test-id` : Identifier for the test (string, required)
- `--executable` : Path to the executable to run (string, required)
- `--matrix-name` : Name of the matrix to test (string, required)
- `--matrix-type` : Type of the matrix (string, required)
- `--n-proc` : Number of processes to use (int, default: 1)
- `--data` : Root directory containing meshes, metadata, and matrices_metadata (string, required)
- `--result-path` : Directory to save the final result JSON (string, required)
- `--tmp-path` : Path to store the intermediate output JSON (string, required)

### Output
The script writes a JSON file in the results directory, containing:

- Input parameters (executable, matrix name/type, n-proc)
- Output data from the executable

```json
{
  "executable-name": "executable_name",
  "matrix-name": "matrix_name",
  "matrix-type": "matrix_type",
  "n-proc": 1,
  "iter": 12,
  "iter1": 1,
  "iter2": 14,
  "residual": 1.1402e-06,
  "error": 2.383882e-07
}
```
