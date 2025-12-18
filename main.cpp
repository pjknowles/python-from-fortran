#include <iostream>
#include <Python.h>
#include <filesystem>

auto load_and_call_python_function(char* person=NULL)
{
    Py_Initialize();
    auto name = PyUnicode_FromString("pythonfile");
    auto load_module = PyImport_Import(name);
    auto func = PyObject_GetAttrString(load_module, "hello");

    auto callfunc = PyObject_CallObject(func, person == NULL ? NULL : PyTuple_Pack(1,PyUnicode_FromString(person)));
    return PyBytes_AS_STRING(PyUnicode_AsEncodedString(PyObject_Repr(callfunc),"utf-8","~E~"));
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
    std::cout <<"From python hello(): "<< load_and_call_python_function() << std::endl;
    std::cout <<"From python hello('Somebody'): "<< load_and_call_python_function("Somebody") << std::endl;

    return 0;
}
