#include <iostream>
#include <Python.h>
#include <filesystem>
#include <chrono>
#include "PythonRun.h"


auto load_and_call_python_function(const char* person = NULL)
{
    Py_Initialize();
    auto name = PyUnicode_FromString("pythonfile");
    auto load_module = PyImport_Import(name);
    auto func = PyObject_GetAttrString(load_module, "hello");

    auto callfunc = PyObject_CallObject(func, person == NULL ? NULL : PyTuple_Pack(1, PyUnicode_FromString(person)));
    return PyBytes_AS_STRING(PyUnicode_AsEncodedString(PyObject_Str(callfunc),"utf-8","~E~"));
}

int main(int argc, char* argv[])
{
    std::cout << "Hello World!" << std::endl;
    // Py_SetProgramName("test");
    Py_Initialize();
    PyRun_SimpleString("print('hello world')");
    PyRun_SimpleString("a=99");
    PyRun_SimpleString("print(a)");
    PyRun_SimpleString("import sys");
    PyRun_SimpleString((std::string{"sys.path.append('"}+std::filesystem::current_path().string()+"')").c_str());
    PyRun_SimpleString("import sys; print(sys.path)");
    PyRun_SimpleString("from pythonfile import hello; hello()");
    std::cout << "From python hello(): " << load_and_call_python_function() << std::endl;
    std::cout << "From python hello('Somebody'): " << load_and_call_python_function("Somebody") << std::endl;

    Py_Finalize();

    {
        auto run = PythonRun();
        run.run_string("name='somebody'");
        run.run_string("print(name);1+a");
        std::cout << "script: " << run.script() << std::endl;
        std::cout << "stdout: " << run.stdout() << std::endl;
        std::cout << "stderr: " << run.stderr() << std::endl;
    }

    {
        auto run = PythonRun("from pythonfile import hello; hello('somebody')",
                             std::filesystem::current_path().string());
        std::cout << "stdout: " << run.stdout() << std::endl;
        std::cout << "stderr: " << run.stderr() << std::endl;
    }

    {
        auto run = PythonRun();
        std::cout << "returned " << run.run_str_str_function("hello", "pythonfile", "friend",
                                                             std::filesystem::current_path().string()) << std::endl;
        std::cout << "stdout: " << run.stdout() << std::endl;
        std::cout << "stderr: " << run.stderr() << std::endl;
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        int repeat = 100;
        for (int i = 0; i < repeat; i++)
            auto run = PythonRun("i=1");
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = finish - start;
        std::cout << "Elapsed Time per instance: " << elapsed.count() / repeat << " milliseconds" << std::endl;
    }

    {
        auto start = std::chrono::high_resolution_clock::now();
        int repeat = 10000;
        auto run = PythonRun();
        for (int i = 0; i < repeat; i++)
            run.run_string("i=1");
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = finish - start;
        std::cout << "Elapsed Time per call: " << elapsed.count() / repeat << " milliseconds" << std::endl;
    }

    return 0;
}
