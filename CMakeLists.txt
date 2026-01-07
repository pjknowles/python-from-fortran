cmake_minimum_required(VERSION 3.5...4.0)
project(python-from-fortran LANGUAGES CXX Fortran)

execute_process(COMMAND which python3)
execute_process(COMMAND python3 --version OUTPUT_VARIABLE PYTHON_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX REPLACE "^.* " "" PYTHON_VERSION "${PYTHON_VERSION}")
string(REGEX REPLACE "\.[0-9]+$" "" PYTHON_VERSION_MAJOR "${PYTHON_VERSION}")
execute_process(COMMAND python3 -c "import sys; print(sys.prefix)" --version OUTPUT_VARIABLE PYTHON_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND find ${PYTHON_PREFIX} -type f -name Python.h -print -quit OUTPUT_VARIABLE PYTHON_PYTHON_H OUTPUT_STRIP_TRAILING_WHITESPACE)
get_filename_component(PYTHON_INCLUDE ${PYTHON_PYTHON_H} DIRECTORY)
set(PYTHON_LIBDIR "${PYTHON_PREFIX}/lib")

message(STATUS "PYTHON_PREFIX ${PYTHON_PREFIX}")
message(STATUS "PYTHON_VERSION ${PYTHON_VERSION}")
message(STATUS "PYTHON_VERSION_MAJOR ${PYTHON_VERSION_MAJOR}")
message(STATUS "PYTHON_INCLUDE ${PYTHON_INCLUDE}")

add_library(PythonRun
        PythonRun.cpp
        PythonRun.h)
target_include_directories(PythonRun PUBLIC ${PYTHON_INCLUDE})
target_link_libraries(PythonRun PUBLIC python${PYTHON_VERSION_MAJOR})
target_link_directories(PythonRun PUBLIC ${PYTHON_PREFIX}/lib)
set_property(TARGET PythonRun PROPERTY CXX_STANDARD 17)

add_executable(cpp main.cpp)
target_link_libraries(cpp PRIVATE PythonRun)
set_property(TARGET cpp PROPERTY CXX_STANDARD 17)

add_executable(f main.f90 Python.f90)
target_link_libraries(f PRIVATE PythonRun)

configure_file(pythonfile.py . COPYONLY)