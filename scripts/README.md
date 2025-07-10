# Scripts Documentation

This directory contains scripts for generating metadata, configuring tests, merging results, and running individual tests.

---

## `generate_metadata.py`

Generates metadata JSON files for simulation matrices.

### Usage
```bash
python3 scripts/generate_metadata.py \
  --data-dir <data_directory> \
  [--reset]
```

### Arguments
- `--data-dir` : Root directory containing `matrices/`, `metadata/`, and `matrices_metadata.csv` (required).
- `--reset` : Clears the `metadata/` directory before generating new metadata (optional).

---

## `generate_tests.py`

Reads a CSV file describing test configurations and generates a CMake file with `add_test` entries.

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
- `--input-csv-file` : Path to the CSV file describing test configurations (required).
- `--output-cmake-file` : Path to the generated CMake file (required).
- `--data-dir` : Directory containing `matrices/`, `metadata/`, and `matrices_metadata.csv` (required).
- `--test-results-dir` : Directory for test output JSONs (required).
- `--tmp-dir` : Directory for temporary JSON outputs (required).

### CSV Format
The input CSV must include:
`test_id`, `executable`, `matrix_name`, `matrix_type`, `n_proc`.

#### `n_proc` Behavior
- Single value: `2` → one test with 2 processes.
- List: `{1,3,5}` → tests for each value in the list.
- Range: `[2-5]` → tests for 2, 3, 4, and 5 processes.

---

## `merge_results.py`

Merges multiple JSON result files into a single CSV table.

### Usage
```bash
python3 scripts/merge_results.py \
  --test-results-dir <path_to_results_directory> \
  --tables-dir <output_directory_for_csv> \
  --output-name <csv_filename_without_extension> \
  [--reset]
```

### Arguments
- `--test-results-dir` : Directory containing JSON result files (required).
- `--tables-dir` : Directory for the merged CSV (required).
- `--output-name` : Name of the resulting CSV file, without `.csv` (required).
- `--reset` : Overwrites an existing CSV file if set (optional).

### Behavior
- Scans `test-results-dir` for `.json` files.
- Verifies required fields: `test-id`, `executable-name`, `matrix-name`, `matrix-type`, `n-proc`, `iter`, `iter1`, `iter2`, `residual`, `error`.
- Saves the aggregated CSV in `tables-dir/output-name.csv`.

---

## `run_one_test.py`

Runs a test on a specified matrix using a given executable and saves the results in a JSON file.

### Usage
```bash
python3 scripts/run_one_test.py \
  --test-id <test_id> \
  --executable <path_to_executable> \
  --matrix-name <matrix_name> \
  --matrix-type <matrix_type> \
  --n-proc <number_of_processes> \
  --data-dir <data_directory> \
  --result-path <results_directory> \
  --tmp-path <temporary_output_json>
```

### Arguments
- `--test-id` : Identifier for the test (required).
- `--executable` : Path to the executable (required).
- `--matrix-name` : Name of the matrix (required).
- `--matrix-type` : Type of the matrix (required).
- `--n-proc` : Number of processes (default: 1).
- `--data-dir` : Directory containing `matrices/`, `metadata/`, and `matrices_metadata.csv` (required).
- `--result-path` : Directory to save the final result JSON (required).
- `--tmp-path` : Directory for intermediate JSON output (required).

### Output
The script generates a JSON file with:
- Input parameters: `executable`, `matrix-name`, `matrix-type`, `n-proc`.
- Output data: `iter`, `iter1`, `iter2`, `residual`, `error`.

Example:
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
