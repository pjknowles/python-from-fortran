cmake_minimum_required(VERSION 3.5...4.0)
project(python-from-fortran LANGUAGES CXX Fortran)

set(USE_FINDPYTHON ON)

add_library(PythonRun
        PythonRun.cpp
        PythonRun.h)
set_property(TARGET PythonRun PROPERTY CXX_STANDARD 17)

add_library(PythonC
        PythonC.h
        PythonC.cpp
)
set_property(TARGET PythonC PROPERTY CXX_STANDARD 17)

if (USE_FINDPYTHON)
    # actual
    # cmake_policy(SET CMP0094 OLD)
    find_package(Python3 COMPONENTS Development)
    target_link_libraries(PythonRun PUBLIC Python3::Python)
    target_link_libraries(PythonC PRIVATE Python3::Python)
    message(STATUS "Python version ${Python3_VERSION} from ${Python3_INCLUDE_DIRS}")
else ()
    # illustrative
    execute_process(COMMAND python3 --version OUTPUT_VARIABLE PYTHON_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX REPLACE "^.* " "" PYTHON_VERSION "${PYTHON_VERSION}")
    string(REGEX REPLACE "\.[0-9]+$" "" PYTHON_VERSION_MAJOR "${PYTHON_VERSION}")
    set(PYTHON_NAME "python${PYTHON_VERSION_MAJOR}")
    execute_process(COMMAND python3 -c "import sys; print(sys.prefix)" --version OUTPUT_VARIABLE PYTHON_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(PYTHON_INCLUDE ${PYTHON_PREFIX}/include/${PYTHON_NAME})
    set(PYTHON_LIBDIR "${PYTHON_PREFIX}/lib")

    message(STATUS "PYTHON_PREFIX ${PYTHON_PREFIX}")
    message(STATUS "PYTHON_VERSION ${PYTHON_VERSION}")
    message(STATUS "PYTHON_VERSION_MAJOR ${PYTHON_VERSION_MAJOR}")
    message(STATUS "PYTHON_NAME ${PYTHON_NAME}")
    message(STATUS "PYTHON_INCLUDE ${PYTHON_INCLUDE}")

    target_include_directories(PythonRun PUBLIC "${PYTHON_INCLUDE}")
    target_link_libraries(PythonRun PUBLIC ${PYTHON_NAME})
    target_link_directories(PythonRun PUBLIC "${PYTHON_LIBDIR}")
endif ()

add_executable(cpp main.cpp)
target_link_libraries(cpp PRIVATE PythonRun)
set_property(TARGET cpp PROPERTY CXX_STANDARD 17)

add_executable(f main.f90 Python.f90 Python_OO.f90)
target_link_libraries(f PRIVATE PythonRun PythonC)

configure_file(pythonfile.py . COPYONLY)
