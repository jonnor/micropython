
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as build_ext_original
import sys
import os
import setuptools
import subprocess

class get_pybind_include:
    """Helper class to determine the pybind11 include path"""

    def __str__(self):
        import pybind11
        return pybind11.get_include()

is_pyodide = os.environ.get("PYODIDE") == "1"

class build_ext(build_ext_original):
    """
    Run neccesary make steps before Python module build

    This enables to use pip install for the entire build flow,
    such as when doing building from a git repo. Example:
    pip install "git+https://github.com/micropython/micropython.git@main#subdirectory=ports/unix"
    """
    def run(self):

        extra_args = []

        extra_args += [
            # btree uses headers not available in emscripten
            # btree #include <sys/cdefs.h> fails gives warning/erro on musl
            "MICROPY_PY_BTREE=0",
        ]

        subprocess.check_call(["make", "submodules"], cwd=".")
        subprocess.check_call(["make", "clean", "libmicropython",
            "V=1",
            "CFLAGS_EXTRA=-fPIC -fno-omit-frame-pointer -DMICROPY_UNIX_NO_MAIN=1",
            "MICROPY_PY_FFI=0", # libffi causes linking error
            "VARIANT=standard",
        ] + extra_args)
        super().run()

ext_modules = [
    Extension(
        "micropython_run",
        ["micropython_run.cpp"],
        include_dirs=[
            get_pybind_include(),
        ],
        extra_objects=["build-standard/libmicropython.a"],
        #extra_link_args=["-lffi"],
        #extra_link_args=["-s", "FORCE_FILESYSTEM=1"],
        #extra_compile_args=["-s", "FORCE_FILESYSTEM=1"],
        language="c++"
    ),
]

# NOTE: static metadata is in pyproject.toml
setup(
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)
