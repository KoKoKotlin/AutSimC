import sys, os
import subprocess
import json

from dataclasses import dataclass, asdict
from os.path import basename, splitext, join, exists 

# TODO: remove redudant build if file is testet multiple times

@dataclass
class TestDesc:
    file: str
    args: str

@dataclass
class TestRes:
    path: str
    
    compile_code: int = 0
    compile_out: str = ""
    compile_err: str = ""

    prog_code: int = 0 
    prog_out: str = ""
    prog_err: str = ""
    
def err(msg):
    print(f"[ERROR] {msg}")
    
def warn(msg):
    print(f"[WARNING] {msg}")

def info(msg):
    print(f"[INFO] {msg}")

def exec_cmd(cmd):
    info(f"\t\t> {cmd}")
    return subprocess.run(cmd, shell=True, capture_output=True, text=True)
    
def compile_and_run_file(build_path, path, compile_command, run_command, arg):
    bin_path = join(build_path, basename(splitext(path)[0]))
    compile_command = compile_command \
        .replace("$INPUT_FILE", f"\"{path}\"") \
        .replace("$OUTPUT_FILE", f"\"{bin_path}\"")
    prog_cmd = run_command \
        .replace("$INPUT_FILE", bin_path) \
        .replace("$ARGS", arg)
    
    res = TestRes(path=path)
    
    res_gcc = exec_cmd(compile_command)
    res.gcc_code = res_gcc.returncode
    res.gcc_out = res_gcc.stdout
    res.gcc_err = res_gcc.stderr

    if res.gcc_code != 0: return res
    
    res_prog = exec_cmd(prog_cmd)
    res.prog_code = res_prog.returncode
    res.prog_out = res_prog.stdout
    res.prog_err = res_prog.stderr
    
    return res

def cmp_res(expected_res, actual_res):
    expected_res = asdict(expected_res)
    actual_res = asdict(actual_res)

    equal = True
    for key in expected_res.keys():
        if expected_res[key] != actual_res[key]:
            equal = False
            err(f"Expected:\n{expected_res[key]}\n\nActual:\n{actual_res[key]}")
    return equal
    
TEST_DESC_PATH = "tests.json"
TEST_RES_PATH = "test-res.json"

def get_opt(name, tests_json):
    if name not in tests_json.keys():
        err("Value `name` missing in tests.json!")
        exit(-1)
    
    return tests_json[name]
    

def run_tests(test_dir_path):
    test_desc_path = join(test_dir_path, TEST_DESC_PATH)
    if not exists(test_desc_path):
        print(f"Could not find args file {test_desc_path}!")
        exit(-1)
    
    build_dir_path = os.path.join(test_dir_path, "build")
    if not exists(build_dir_path):
        os.mkdir(build_dir_path)

    with open(test_desc_path) as f:
        tests_json = json.loads(f.read())
    
    compile_command = get_opt("compile_command", tests_json)
    run_command = get_opt("run_command", tests_json)
    
    test_desc = []
    for test in tests_json["tests"]:
        test_desc.append(TestDesc(**test))

    comp_res = []
    for idx, desc in enumerate(test_desc):
        info(f"Running test {idx+1} with file {desc.file} and args {desc.args}")
        file = join(test_dir_path, desc.file)
        if not exists(file):
            warn(f"File {file} from test {idx} does not exist. Skipping test!")
            continue
        res = compile_and_run_file(build_dir_path, file, compile_command, run_command, desc.args)

        comp_res.append(res)
    return comp_res

def record(test_dir_path):
    comp_res = run_tests(test_dir_path)

    test_json = { "test_res": list(map(lambda x: asdict(x), comp_res)) }
    test_res_path = join(test_dir_path, TEST_RES_PATH)
    with open(test_res_path, "w") as f:
        f.write(json.dumps(test_json))

def test(test_dir_path):
    test_res_path = join(test_dir_path, TEST_RES_PATH)
    if exists(test_res_path):
        test_cmp(test_dir_path)
    else:
        test_fails(test_dir_path)

def test_fails(test_dir_path):
    comp_res = run_tests(test_dir_path)

    fails = 0
    fail_idx = []
    for idx, res in enumerate(comp_res):
        failed = False
        if res.gcc_code != 0:
            err("Compilation failed with msg:")
            err(f"\t{res.gcc_out}")
            err(f"\t{res.gcc_err}")
            failed = True
        if res.prog_code != 0:
            err(f"Test failed with code: {res.prog_code}")
            err(f"\t{res.prog_out}")
            err(f"\t{res.prog_err}")
            failed = True
        if failed:
            fails += 1
            fail_idx.append(idx)

    print("Test summary:")
    print(f"\t\tTests passed: {len(comp_res) - fails}")
    print(f"\t\tTests failed: {fails}")
    if fails != 0:
        print("Failed Tests:")
        for idx in fail_idx:
            print(f"\t\t{idx+1}: {comp_res[idx].path}")
    
def test_cmp(test_dir_path):
    comp_res = run_tests(test_dir_path)

    test_res_path = join(test_dir_path, TEST_RES_PATH)
    recorded_comp_res = []
    with open(test_res_path, "r") as f:
        test_res_json = json.loads(f.read())

    fails = 0
    fail_idx = []
    for idx, (res, expected_res) in enumerate(zip(comp_res, map(lambda x: TestRes(**x), test_res_json["test_res"]))):
        if not cmp_res(expected_res, res):
            fails += 1
            fail_idx.append(idx)

    print("Test summary:")
    print(f"\t\tTests passed: {len(comp_res) - fails}")
    print(f"\t\tTests failed: {fails}")
    if fails != 0:
        print("Failed Tests:")
        for idx in fail_idx:
            print(f"\t\t{idx+1}: {comp_res[idx].path}")
        
def main():
    if len(sys.argv) != 3:
        err("Usage: python3 run-tests.py [record|test] <test-dir-path>")
        exit(-1)

    mode = sys.argv[1]
    if not mode in ["test", "record"]:
        err(f"Mode has to be `test` or `record`!")
        exit(-1)
    
    test_dir_path = sys.argv[2]
    if not exists(test_dir_path):
        print(f"Path {test_dir_path} does not exist!")
        exit(-1)

    match mode:
        case "record":
            record(test_dir_path)
        case "test":
            test(test_dir_path)

    
if __name__ == "__main__":
    main()
