import argparse
import os
from pathlib import Path
import json
import subprocess

def run_one_test():
    parser = argparse.ArgumentParser(description="Run a single test with specified matrix and executable.")
    parser.add_argument("--test-id", type=str, required=True, help="Identifier for the test.")
    parser.add_argument("--executable", type=str, required=True, help="Path of the executable to run the test.")
    parser.add_argument("--matrix-name", type=str, required=True, help="Name of the matrix to test (without extension).")
    parser.add_argument("--matrix-type", type=str, required=True, help="Type of the matrix to test.")
    parser.add_argument("--n-proc", type=int, default=1, help="Number of processes to use for the test.")
    parser.add_argument("--data", type=str, required=True, help="Root directory containing 'meshes' and 'metadata' subfolders, and the 'matrices_metadata' CSV file with matrices parameters.")
    parser.add_argument("--result-path", type=str, required=True, help="Path to save the results of the test.")
    parser.add_argument("--tmp-path", type=str, required=True, help="Temporary path to store intermediate results.")
    args = parser.parse_args()

    test_id = args.test_id
    executable = args.executable
    matrix_name = args.matrix_name
    matrix_type = args.matrix_type
    n_proc = args.n_proc
    data = args.data
    result_directory = Path(args.result_path)
    tmp_path = Path(args.tmp_path)

    executable_path = Path(executable)
    if not executable_path.is_file():
        raise FileNotFoundError(f"Executable does not exist: {executable_path}")
    if not os.access(executable_path, os.X_OK):
        raise PermissionError(f"Executable is not executable: {executable_path}")

    data_path = Path(data)
    if not data_path.exists():
        raise FileNotFoundError(f"Data path does not exist: {data_path}")

    matrix_path = data_path / "matrices" / f"{matrix_type}/{matrix_name}.petsc"
    if not matrix_path.exists():
        raise FileNotFoundError(f"Matrix file does not exist: {matrix_path}")

    metadata_path = data_path / "metadata" / f"{matrix_type}/{matrix_name}_metadata.json"
    if not metadata_path.exists():
        raise FileNotFoundError(f"Metadata file does not exist: {metadata_path}")

    # Load metadata
    with open(metadata_path) as f:
        metadata = json.load(f)
    if "nU" not in metadata or "nP" not in metadata:
        raise KeyError(f"Metadata file {metadata_path} must contain 'nU' and 'nP' keys.")
    nU = metadata["nU"]
    nP = metadata["nP"]

    # Prepare command
    if n_proc <= 0:
        raise ValueError("Number of processes must be greater than 0.")

    cmd = [
        str(executable_path),
        "-f0", str(matrix_path),
        "-nU", str(nU),
        "-nP", str(nP)
    ]

    if n_proc > 1:
        cmd = ["mpirun", "-n", str(n_proc)] + cmd


    print(f"Running test: {test_id}")
    print(f"Command: {' '.join(cmd)}")

    subprocess.run(cmd, check=True)


    with open(tmp_path) as f:
        output_data = json.load(f)


    result = {
        "test-id": test_id,
        "executable-name": str(executable_path.stem),
        "matrix-name": matrix_name,
        "matrix-type": matrix_type,
        "n-proc": n_proc,
    }
    result.update(output_data)

    result_path = result_directory / f"{executable_path.stem}/{matrix_type}/{matrix_name}__{test_id}.json"

    result_path.parent.mkdir(parents=True, exist_ok=True)
    with open(result_path, "w") as f:
        json.dump(result, f, indent=2)

if __name__ == "__main__":
    run_one_test()