"""
build.py
"""

import subprocess
import os
import shutil


def _print_help_message():
    msg = """
    h: print this help message
    c: clean
    p: build both release and debug, then pack it
    i: install
    *: build debug only
    """
    print(msg)

    
def _clean():
    entries = os.listdir(".")
    for e in entries:
        if os.path.isfile(e):
            os.remove(e)
        else:
            shutil.rmtree(e)


    
def _build(opt):
    subprocess.run(["cmake", "-DCMAKE_BUILD_TYPE=Debug", ".."])  # cmake ..
    # cmake --build .
    subprocess.run(["cmake", "--build", "."])
    if opt is not None:
        # build release
        subprocess.run(["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."])
        subprocess.run(["cmake", "--build", "."])
        # build pkg TODO python version
        

def build(opt):
    old_cwd = os.getcwd()
    if os.getcwd() != "build":
        if not os.path.isdir("build"):
            os.mkdir("build")
        os.chdir("build")       # cd build
    result = {
        'h': lambda: _print_help_message(),
        'c': lambda: _clean(),
    }.get(opt, '*')()
    return result


if __name__ == "main":
    build()
