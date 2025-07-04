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
