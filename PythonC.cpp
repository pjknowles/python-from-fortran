#include "PythonC.h"

#include <iostream>
#include <ostream>
#include <Python.h>
#include <string>

static int initialized = 0;
static std::string script_;

extern "C" void PythonC_Initialize()
{
if (initialized) PythonC_Shutdown();
    Py_Initialize();
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
initialized = 1;
}

extern "C" void PythonC_Shutdown()
{
if (!initialized) return;
initialized = 1;
    Py_Finalize();
}


extern "C" void PythonC_Run(const char* script)
{
    PyRun_SimpleString(script);
    if (!script_.empty())
        script_ += "\n";
    script_ += script;
}
extern "C" void PythonC_LoadModule(const char* name)
{
    auto nam = PyUnicode_FromString(name);
    auto load_module = PyImport_Import(nam);
}


extern  "C" const char* PythonC_Stream(const char* name)
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

extern "C" const char* PythonC_Script() { return strdup(script_.c_str()); }
extern "C" const char* PythonC_stdout() { return PythonC_Stream("stdout"); }
extern "C" const char* PythonC_stderr() { return PythonC_Stream("stderr"); }
