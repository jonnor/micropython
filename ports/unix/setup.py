
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as build_ext_original
import sys
import setuptools
import subprocess

class get_pybind_include:
    """Helper class to determine the pybind11 include path"""

    def __str__(self):
        import pybind11
        return pybind11.get_include()


class build_ext(build_ext_original):
    """
    Run neccesary make steps before Python module build

    This enables to use pip install for the entire build flow,
    such as when doing building from a git repo. Example:
    pip install "git+https://github.com/micropython/micropython.git@main#subdirectory=ports/unix"
    """
    def run(self):
        subprocess.check_call(["make", "submodules"], cwd=".")
        subprocess.check_call(["make", "clean", "libmicropython", "CFLAGS_EXTRA='-fPIC'"], cwd=".")
        super().run()

ext_modules = [
    Extension(
        "micropython_run",
        ["micropython_run.cpp"],
        include_dirs=[
            get_pybind_include(),
        ],
        extra_objects=["build-standard/libmicropython.a"],
        extra_link_args=["-lffi"],
        #extra_link_args=["-s", "FORCE_FILESYSTEM=1"],
        #extra_compile_args=["-s", "FORCE_FILESYSTEM=1"],
        language="c++"
    ),
]

setup(
    name="jonnor-micropython-unix",
    version="1.28.0.0",
    author="You",
    description="CPython wrapper for MicroPython Unix port",
    py_modules=["micropython_unix"],
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    install_requires=["pybind11"],
    entry_points={
        "console_scripts": [
            "micropython=micropython_unix:main",
        ],
    },
)
