#include <iostream>
#include <Python.h>

int main(int argc, char* argv[])
{
    std::cout << "Hello World!" << std::endl;
    Py_Initialize();
    auto name = PyUnicode_FromString("pythonfile");
    auto load_module = PyImport_Import(name);
    auto func = PyObject_GetAttrString(load_module, "hello");
    auto callfunc = PyObject_CallObject(func, NULL);
}
