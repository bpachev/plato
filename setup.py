from setuptools import Extension, setup
from Cython.Build import cythonize

setup(
    name = "platopillars",
    version = "1.0",
    description = "A Python wrapper around a C engine for 3D Connect4",
    author = "Benjamin Pachev",
    author_email = "benjaminpachev@gmail.com",
    ext_modules = cythonize([Extension("plato",["plato.pyx", "board.c", "search.c"])])
)
