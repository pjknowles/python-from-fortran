cmake_minimum_required(VERSION 3.5...4.0)
project(python-from-fortran LANGUAGES CXX Fortran)

execute_process(COMMAND conda info --base OUTPUT_VARIABLE CONDA_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${CONDA_PREFIX}/bin/python --version OUTPUT_VARIABLE PYTHON_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX REPLACE "^.* " "" PYTHON_VERSION "${PYTHON_VERSION}")
string(REGEX REPLACE "\.[0-9]+$" "" PYTHON_VERSION_MAJOR "${PYTHON_VERSION}")
message(STATUS "CONDA_PREFIX ${CONDA_PREFIX}")
message(STATUS "PYTHON_VERSION ${PYTHON_VERSION}")
message(STATUS "PYTHON_VERSION_MAJOR ${PYTHON_VERSION_MAJOR}")

add_library(PythonRun
        PythonRun.cpp
        PythonRun.h)
target_include_directories(PythonRun PUBLIC ${CONDA_PREFIX}/include/python${PYTHON_VERSION_MAJOR})
target_link_libraries(PythonRun PUBLIC python${PYTHON_VERSION_MAJOR})
target_link_directories(PythonRun PUBLIC ${CONDA_PREFIX}/lib)
set_property(TARGET PythonRun PROPERTY CXX_STANDARD 17)

add_executable(cpp main.cpp)
target_link_libraries(cpp PRIVATE PythonRun)
set_property(TARGET cpp PROPERTY CXX_STANDARD 17)

add_executable(f main.f90 Python.f90)
target_link_libraries(f PRIVATE PythonRun)

configure_file(pythonfile.py . COPYONLY)