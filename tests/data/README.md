# Data Directory Overview

## Matrices Subdirectory

This directory contains the mesh files used in simulations. The structure inside `matrices` reflects the different mesh types and categories. Each mesh file corresponds to an entry in the `matrices_metadata.csv` file, which holds the parameters used to generate associated metadata.

## Metadata Subdirectory

### Purpose

This directory is used to store auto-generated metadata files.

The `metadata/.gitkeep` ensures the folder is tracked by Git when empty, to maintain the project structure.


```json
{
  "name": "mesh_filename_without_extension",
  "type": "mesh_type",
  "nU": number_of_velocity_lines,
  "nP": number_of_pressure_lines,
  "matrix_path": "/absolute/path/to/the/mesh/file.petsc"
}
```

> **Note:** The metadata files generated here contain absolute paths to matrix
> files, which vary depending on the machine and filesystem. As a result, they
> are not meant to be committed or shared between environments.

### Generating Metadata

To generate the metadata JSON files automatically, run the script:

```bash
python3 scripts/generate_metadata.py --data-path /path/to/data [--reset]
```

#### Arguments:

- `--data-path` (required): Path to the root directory containing the following:
  - A `matrices` folder with the mesh files.
  - A `metadata` folder where metadata JSON files will be created.
  - A `matrices_metadata.csv` file containing parameters for each matrix.

- `--reset` (optional): If set, the metadata directory will be deleted and recreated before generating new metadata files.

#### Directory constraints:

The script validates that the data-path contains:

    A matrices directory.

    A matrices_metadata.csv CSV file with data.

    The metadata directory will be created if it does not exist, or reset if --reset is passed.

## CSV file format:

The CSV file matrices_metadata.csv should include columns like:

- `name` (mesh filename without extension)
- `type` (type of mesh)
- `nU` (number of velocity lines)
- `nP` (number of pressure lines)

The script uses this information to generate one JSON metadata file per mesh under the metadata directory, mirroring the folder structure of matrices.
