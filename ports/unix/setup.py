
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import setuptools

class get_pybind_include:
    """Helper class to determine the pybind11 include path"""

    def __str__(self):
        import pybind11
        return pybind11.get_include()

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
    name="micropython_run",
    version="0.1.8",
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
