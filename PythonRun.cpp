#include "PythonRun.h"

#include <iostream>
#include <ostream>
#include <Python.h>
extern "C" char* PythonRun_stream(char*);

PythonRun::PythonRun(std::string script, std::string path)
{
    Py_Initialize();
    if (!path.empty()) add_path(path);
    PyRun_SimpleString(
        "import sys\n\
class CatchOutErr:\n\
    def __init__(self):\n\
        self.value = ''\n\
    def write(self, txt):\n\
        self.value += txt\n\
sys.stdout = CatchOutErr()\n\
sys.stderr = CatchOutErr()\n\
");
    if (!script.empty()) run_string(script);
}

PythonRun::~PythonRun()
{
    Py_Finalize();
}

void PythonRun::add_path(std::string path)
{
    PyRun_SimpleString((std::string{"import sys;sys.path.append('"}+path+"')").c_str());
}

void PythonRun::load_module(std::string name)
{
    // PyRun_SimpleString((std::string{"import "}+name).c_str());
    auto nam = PyUnicode_FromString(name.c_str());
    auto load_module = PyImport_Import(nam);
}

void PythonRun::run_string(std::string str)
{
    PyRun_SimpleString(str.c_str());
    if (!this->script_.empty())
        this->script_ += "\n";
    this->script_ += str;
}

std::string PythonRun::run_str_str_function(std::string function_name, std::string module, std::string argument,
                                            std::string path)
{
    if (!path.empty()) add_path(path);
    auto nam = PyUnicode_FromString(module.c_str());
    auto load_module = PyImport_Import(nam);
    auto func = PyObject_GetAttrString(load_module, function_name.c_str());

    auto callfunc = PyObject_CallObject(func, argument.empty()
                                                  ? NULL
                                                  : PyTuple_Pack(1, PyUnicode_FromString(argument.c_str())));
    return std::string{PyBytes_AS_STRING(PyUnicode_AsEncodedString(PyObject_Str(callfunc),"utf-8","~E~"))};
}

std::string PythonRun::stream(char* name)
{
    auto resultc = PythonRun_stream(name);
    std::string result = std::string{resultc};
    free(resultc);
    return result;
}
extern  "C" char* PythonRun_stream(char* name)
{
    auto py_import_import_module = PyImport_ImportModule("sys");
    PyObject* catcher = PyObject_GetAttrString(py_import_import_module, name);
    PyErr_Print();
    PyObject* output = PyObject_GetAttrString(catcher, "value");
    auto py_bytes_as_string = std::string{PyBytes_AS_STRING(PyUnicode_AsEncodedString(output,"utf-8","~E~"))};
    if (!py_bytes_as_string.empty() && py_bytes_as_string.back() == '\n')
        py_bytes_as_string.pop_back();
    return strdup(py_bytes_as_string.c_str());
}
