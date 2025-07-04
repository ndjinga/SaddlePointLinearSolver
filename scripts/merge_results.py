import argparse
import csv
import json
from pathlib import Path

REQUIRED_COLUMNS = [
    "test-id", "executable-name", "matrix-name", "matrix-type",
    "n-proc", "iter", "iter1", "iter2", "residual", "error"
]

def parse_args():
    parser = argparse.ArgumentParser(description="Merge JSON results into a CSV.")
    parser.add_argument("--results-dir", required=True, help="Directory with JSON result files")
    parser.add_argument("--tables-dir", required=True, help="Directory to write the CSV")
    parser.add_argument("--output-name", required=True, help="CSV filename")
    parser.add_argument("--reset", action="store_true", help="Overwrite existing CSV")
    return parser.parse_args()

def main():
    args = parse_args()
    results_dir = args.results_dir
    tables_dir = args.tables_dir
    output_name =args.output_name
    reset = args.reset

    results_path = Path(results_dir)
    if not results_path.is_dir():
        raise NotADirectoryError(f"Results directory does not exist: {results_path}")

    output_path = Path(tables_dir) / (output_name + ".csv")
    if not reset and output_path.exists():
        raise FileExistsError(f"Output CSV {output_path} already exists. Use --reset to overwrite.")
    output_path.parent.mkdir(parents=True, exist_ok=True)

    rows = []
    for file in results_path.rglob("*.json"):
        with open(file) as f:
            data = json.load(f)

        missing = [col for col in REQUIRED_COLUMNS if col not in data]
        if missing:
            raise ValueError(f"Missing columns in {file}: {missing}")

        row = [data[col] for col in REQUIRED_COLUMNS]
        rows.append(row)

    with open(output_path, mode="w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(REQUIRED_COLUMNS)
        writer.writerows(rows)

    print(f"[INFO] Merged {len(rows)} results into {output_path}")


if __name__ == "__main__":
    main()