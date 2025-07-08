import argparse
from pathlib import Path
import pandas as pd
from loguru import logger

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
    data_dir = Path(args.data_dir).resolve()
    test_results_dir = Path(args.test_results_dir).resolve()
    tmp_dir = Path(args.tmp_dir).resolve()
    
    # TODO
    #test existence
    
    test_results_dir.parent.mkdir(parents=True, exist_ok=True)
    with open(output_cmake_file, 'w') as cmake_file:
        cmake_file.write(f"# Generated tests with generate_tests.py script from {input_csv_file}\n\n")
        cmake_file.write("enable_testing()\n\n")
        cmake_file.write(f"set(TEST_DATA_DIR {data_dir})\n")
        cmake_file.write(f"set(TEST_RESULT_DIR {test_results_dir})\n")
        cmake_file.write(f"set(TEST_TMP_PATH {tmp_dir})\n\n")
    
    def add_test(test_id, executable, matrix_name, matrix_type, n_proc):
        with open(output_cmake_file, mode='a') as cmake_file:
            cmake_file.write(f"set(TEST_ID {test_id})\n")
            cmake_file.write( "add_test(\n")
            cmake_file.write( "  NAME ${TEST_ID}\n")
            cmake_file.write( "  COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/run_one_test.py\n")
            cmake_file.write( "    --test-id ${TEST_ID}\n")
            cmake_file.write(f"    --executable  ${{CMAKE_BINARY_DIR}}/{executable}\n")
            cmake_file.write(f"    --matrix-name {matrix_name}\n")
            cmake_file.write(f"    --matrix-type {matrix_type}\n")
            cmake_file.write(f"    --n-proc {n_proc}\n")
            cmake_file.write( "    --data ${TEST_DATA_DIR}\n")
            cmake_file.write( "    --result-path ${TEST_RESULT_DIR}\n")
            cmake_file.write( "    --tmp-path ${TEST_TMP_PATH}\n")
            cmake_file.write(")\n\n")
        return
    
    input_csv_df = pd.read_csv(input_csv_file)
    
    for index, row in input_csv_df.iterrows():
        test_id = row['test_id']
        executable = row['executable']
        matrix_name = row['matrix_name']
        matrix_type = row['matrix_type']
        n_proc_str = str(row['n_proc'])
        
        if not test_id:
            logger.error(f"Missing 'test_id' in row {index} of '{input_csv_file}'.")
            return 1
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

        n_proc_values = parse_n_proc(n_proc_str)
        for n_proc in n_proc_values:
            tid = test_id if len(n_proc_values) == 1 else f"{test_id}_nproc{n_proc}"
            add_test(tid, executable, matrix_name, matrix_type, n_proc)
            logger.info(f"'{tid}' generated")
        
    logger.info(f"All tests from {input_csv_file} succefully generated in {output_cmake_file}.")
    return 0
        
    
if __name__ == "__main__":
    generate_tests()