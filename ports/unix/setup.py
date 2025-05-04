
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
            #get_pybind_include(),
            '/home/jon/projects/emlearn/venv/lib/python3.12/site-packages/pybind11/include'
        ],
        extra_objects=["libmicropython.a"],
        #extra_link_args=["-lffi"],
        language="c++"
    ),
]

setup(
    name="micropython_run",
    version="0.1.3",
    author="You",
    description="A minimal MicroPython CPython wrapper using pybind11",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    install_requires=["pybind11"],
)
