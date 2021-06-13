#!/usr/bin/env python3

import subprocess
import os
import pathlib
import time


IMPL_TEST_DIR = pathlib.Path('impl/test')
COMPILER = 'target/pcc'

test_count = 0
failed_count = 0
passed_count = 0


def logError(cmd: str, msg: str):
    global failed_count
    failed_count += 1
    print(f'Test #{test_count} failed: {cmd}\n{msg}')


def logSuccess(cmd: str):
    global passed_count
    passed_count += 1
    print(f'Test #{test_count} passed: {cmd}')


def compile_file(script_path: str, optimize: bool):
    global test_count
    test_count += 1

    cmd = f'{COMPILER} "{script_path}" -v'
    
    if optimize:
        cmd += ' -O'

    try:
        subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True, universal_newlines=True)
    except subprocess.CalledProcessError as exc:
        logError(cmd, exc.output)
    else:
        logSuccess(cmd)


def compile_test():
    for filename in os.listdir(IMPL_TEST_DIR):
        if filename.endswith('.pf'):
            path = os.path.join(IMPL_TEST_DIR, filename)
            compile_file(path, False)
            compile_file(path, True)


if __name__ == '__main__':

    start_time = time.time()

    compile_test()

    end_time = time.time()

    print(f'\nTest report:\nTotal tests: {test_count}\nPassed tests: {passed_count}\nFailed tests: {failed_count}')
    print(f'Time elapsed: {round(end_time - start_time)} seconds')

