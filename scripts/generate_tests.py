import argparse
from pathlib import Path
import pandas as pd
from custom_logger import get_logger
import json
import re

logger = get_logger(__name__)

def parse_n_proc(n_proc_str):
    n_proc_str = n_proc_str.strip()
    if n_proc_str.startswith("{") and n_proc_str.endswith("}"):
        inner = n_proc_str[1:-1]
        return [int(x.strip()) for x in inner.split(",") if x.strip()]
    elif n_proc_str.startswith("[") and n_proc_str.endswith("]"):
        inner = n_proc_str[1:-1]
        start_str, end_str = inner.split("-")
        start, end = int(start_str.strip()), int(end_str.strip())
        return list(range(start, end + 1))
    else:
        return [int(n_proc_str)]

def safe_filename(title):
    return re.sub(r'[^\w\-]', '_', title)

def format_option(option):
    return " ".join(f"\\\"{opt}\\\"" for opt in option.split())

def generate_tests():
    parser = argparse.ArgumentParser(description="Generate a CMake file containing tests based on parameters specified in a CSV file.")    
    parser.add_argument("--input-csv-file", type=str, required=True, help="Path to the CSV file containing test parameters.")
    parser.add_argument("--output-cmake-file", type=str, required=True, help="Path to the output CMake file containing the generated tests.")
    parser.add_argument("--data-dir", type=str, required=True, help="Root directory containing 'meshes' and 'metadata' subfolders, and the 'matrices_metadata' CSV file with matrices parameters.")
    parser.add_argument("--test-results-dir", type=str, required=True, help="Directory where the results of the tests will be saved.")
    parser.add_argument("--tmp-dir", type=str, required=True, help="Temporary path to store intermediate results.")

    args = parser.parse_args()
    
    input_csv_file = Path(args.input_csv_file).resolve()
    output_cmake_file = Path(args.output_cmake_file).resolve()
    data_path = Path(args.data_dir)
    test_results_dir = Path(args.test_results_dir).resolve()
    tmp_dir = Path(args.tmp_dir).resolve()
    
    # TODO
    #test existence

    def fetch_command(executable, matrix_name, matrix_type, n_proc, option):
        metadata_path = data_path / "metadata" / f"{matrix_type}/{matrix_name}_metadata.json"
        if not metadata_path.exists():
            raise FileNotFoundError(f"Metadata file does not exist: {metadata_path}")

        with open(metadata_path) as f:
            metadata = json.load(f)
        if "nU" not in metadata or "nP" not in metadata:
            raise KeyError(f"Metadata file {metadata_path} must contain 'nU' and 'nP' keys.")
        nU = metadata["nU"]
        nP = metadata["nP"]
        matrix = f"${{CMAKE_SOURCE_DIR}}/tests/data/matrices/{matrix_type}/{matrix_name}.petsc"
        command = f'\\\"${{CMAKE_BINARY_DIR}}/{executable}\\\" \\\"-f0\\\" \\\"{matrix}\\\" \\\"-nU\\\" \\\"{nU}\\\" \\\"-nP\\\" \\\"{nP}\\\" \\\"-tmp_file\\\" \\\"${{TMP_FILE}}\\\"' + \
                  (f' {format_option(option)}' if option else "")
        if n_proc == 1:
            return command
        else:
            return f'\\\"${{MPIEXEC}}\\\" \\\"-n\\\" \\\"{n_proc}\\\" {command}'

    def add_test(test_id, executable, matrix_name, matrix_type, n_proc, mode, option):
        command = fetch_command(executable, matrix_name, matrix_type, n_proc, option)
        with open(output_cmake_file, mode='a') as cmake_file:
            cmake_file.write(f"set(TEST_ID {test_id})\n")
            cmake_file.write(f"set(TMP_FILE ${{TMP_DIR}}/tmp_output_${{TEST_ID}}.json)\n")

            cmake_file.write("add_test(\n")
            cmake_file.write("  NAME ${TEST_ID}\n")
            cmake_file.write("  COMMAND bash -c \"\n")
            cmake_file.write(f"    {command};")
            if mode == "complete": #Collect the output result             
                cmake_file.write( " \\\n ")
                cmake_file.write( "   python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \\\n")
                cmake_file.write( "      --test-id \\\"${TEST_ID}\\\" \\\n")
                cmake_file.write(f"      --executable \\\"${{CMAKE_BINARY_DIR}}/{executable}\\\" \\\n")
                cmake_file.write(f"      --matrix-name {matrix_name} \\\n")
                cmake_file.write(f"      --matrix-type {matrix_type} \\\n")
                cmake_file.write(f"      --n-proc {n_proc} \\\n")
                cmake_file.write(f"      --option \\\"{option}\\\" \\\n")
                cmake_file.write( "      --data-dir \\\"${TEST_DATA_DIR}\\\" \\\n")
                cmake_file.write( "      --test-results-dir \\\"${TEST_RESULT_DIR}\\\" \\\n")
                cmake_file.write( "      --tmp-file \\\"${TMP_FILE}\\\"\n")
            else:#Do not collect the output result
                cmake_file.write("\n")
            cmake_file.write("  \"\n)\n\n")
        return

    data_dir = data_path.resolve()
    test_results_dir.parent.mkdir(parents=True, exist_ok=True)
    with open(output_cmake_file, 'w') as cmake_file:
        cmake_file.write("# Tests generated with generate_tests.py script in folder ${CMAKE_SOURCE_DIR}/scripts/ and tests metadata in file ${CMAKE_SOURCE_DIR}/tests/tests_parameters.csv\n\n")

        cmake_file.write("enable_testing()\n\n")
        cmake_file.write("set(TEST_DATA_DIR   ${CMAKE_SOURCE_DIR}/tests/data)\n")#anciennement {data_dir}
        cmake_file.write("set(TEST_RESULT_DIR ${CMAKE_SOURCE_DIR}/tests/test_results)\n")#anciennement {test_results_dir}
    
    input_csv_df = pd.read_csv(input_csv_file)

    success_count = 0
    failure_count = 0
    
    for index, row in input_csv_df.iterrows():
        test_id = row['test_id']
        executable = row['executable']
        matrix_name = row['matrix_name']
        matrix_type = row['matrix_type']
        n_proc_str = str(row['n_proc'])
        mode = row['mode']
        option = "" if pd.isna(row['option']) else row['option']      
    
        if not executable:
            logger.error(f"Missing 'executable' in row {index} of '{input_csv_file}'.")
            return 1
        if not matrix_name:
            logger.error(f"Missing 'matrix_name' in row {index} of '{input_csv_file}'.")
            return 1
        if not matrix_type:
            logger.error(f"Missing 'matrix_type' in row {index} of '{input_csv_file}'.")
            return 1
        if not n_proc_str:
            logger.error(f"Missing 'n_proc' in row {index} of '{input_csv_file}'.")
            return 1
        if not mode:
            logger.error(f"Missing 'mode' in row {index} of '{input_csv_file}'.")
            return 1


        n_proc_values = parse_n_proc(n_proc_str)
        for n_proc in n_proc_values:
            tid = test_id if not pd.isna(test_id) else f"{mode}_{executable}_{matrix_type}_{matrix_name}_nproc{n_proc}" + (f'_{safe_filename(option)}' if option else '')
            try:
                add_test(tid+"_"+str(n_proc)+"procs", executable, matrix_name, matrix_type, n_proc, mode, option)
            except Exception as e:
                logger.error(f"'{tid}' not generated : {e}")
                failure_count += 1
                continue
            logger.info(f"'{tid}' generated")
            success_count += 1

    logger.info(f"Test generation completed from {input_csv_file}. {success_count} tests successfully added, {failure_count} errors encountered. Output written to {output_cmake_file}.")
    return 0
        
    
if __name__ == "__main__":
    generate_tests()
