import argparse
from pathlib import Path
import pandas as pd
import json
import shutil

def generate_metadata():
    parser = argparse.ArgumentParser(description="Generate metadata for simulation tests.")
    parser.add_argument("--data-path", type=str, required=True, help="Root directory containing 'matrices' and 'metadata' subfolders, and the 'matrices_metadata' CSV file with matrices parameters.")
    parser.add_argument("--reset", action="store_true", help="Reset the metadata directory before generating new metadata.")
    args = parser.parse_args()

    # Validate the provided paths
    data_path = Path(args.data_path)
    matrices_path = Path(data_path) / "matrices"
    metadata_path = Path(data_path) / "metadata"
    matrices_metadata_path = Path(data_path) / "matrices_metadata.csv"

    if not data_path.is_dir():
        print(f"[ERROR] Invalid data path: {data_path}")
        return 1
    if not matrices_path.is_dir():
        print(f"[ERROR] 'matrices' directory not found in {data_path}")
        return 1
    if not matrices_metadata_path.is_file():
        print(f"[ERROR] 'matrices_metadata.csv' file not found in {data_path}")
        return 1

    data = pd.read_csv(matrices_metadata_path)
    if data.empty:
        print("[ERROR] 'matrices_metadata.csv' is empty.")
        return 1

    if args.reset:
        if metadata_path.is_dir():
            print(f"[INFO] Resetting metadata directory content (except README.md): {metadata_path}")
            for item in metadata_path.iterdir():
                if item.name == ".gitkeep":
                    continue
                if item.is_dir():
                    shutil.rmtree(item)
                else:
                    item.unlink()

    for index, row in data.iterrows():
        matrix_name = row['name']
        if not matrix_name:
            print(f"[ERROR] Missing 'name' in row {index} of 'matrices_metadata.csv'.")
            return 1
        matrix_type = row['type']
        if not matrix_type:
            print(f"[ERROR] Missing 'type' in row {index} of 'matrices_metadata.csv'.")
            return 1
        matrix_nU = row['nU']
        if not matrix_nU:
            print(f"[ERROR] Missing 'nU' in row {index} of 'matrices_metadata.csv'.")
            return 1
        matrix_nP = row['nP']
        if not matrix_nP:
            print(f"[ERROR] Missing 'nP' in row {index} of 'matrices_metadata.csv'.")
            return 1


        generic_path = f"{matrix_type}/{matrix_name}"
        matrix_path = matrices_path / (generic_path + ".petsc")
        if not matrix_path.is_file():
            print(f"[ERROR] Matrix file '{matrix_path}' does not exist.")
            return 1

        output_path = metadata_path / (generic_path + "_metadata.json")

        output_path.parent.mkdir(parents=True, exist_ok=True)
        metadata = {
            "name": matrix_name,
            "type": matrix_type,
            "nU": matrix_nU,
            "nP": matrix_nP,
            "matrix_path": str(matrix_path.resolve()),
        }
        with open(output_path, 'w') as f:
            json.dump(metadata, f, indent=2)

    return 0

if __name__ == "__main__":
    generate_metadata()
