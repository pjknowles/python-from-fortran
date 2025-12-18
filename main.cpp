#include <iostream>
#include <Python.h>
#include <filesystem>
#include <chrono>

class PythonRun
{
public:
    PythonRun(std::string script = "", std::string path = "")
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

    ~PythonRun()
    {
        Py_Finalize();
    }

    void add_path(std::string path)
    {
        PyRun_SimpleString((std::string{"import sys;sys.path.append('"}+path+"')").c_str());
    }

    void load_module(std::string name)
    {
        // PyRun_SimpleString((std::string{"import "}+name).c_str());
        auto nam = PyUnicode_FromString(name.c_str());
        auto load_module = PyImport_Import(nam);
    }

    void run_string(std::string str)
    {
        PyRun_SimpleString(str.c_str());
        if (!this->script_.empty())
            this->script_ += "\n";
        this->script_ += str;
    }

    /* @brief Run a python function that expects zero or 1 string arguments and returns a string
     *
     * @param function_name
     * @param module
     * @param argument
     * @param path
     * @return
     */
    std::string run_str_str_function(std::string function_name, std::string module, std::string argument = "",
                                     std::string path = "")
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

    const std::string out() { return stream("stdout"); }
    const std::string err() { return stream("stderr"); }
    const std::string& script() { return script_; }

private:
    std::string stream(char* name)
    {
        PyObject* catcher = PyObject_GetAttrString(PyImport_ImportModule("sys"), name);
        PyErr_Print();
        PyObject* output = PyObject_GetAttrString(catcher, "value");
        auto py_bytes_as_string = std::string{PyBytes_AS_STRING(PyUnicode_AsEncodedString(output,"utf-8","~E~"))};
        if (!py_bytes_as_string.empty() && py_bytes_as_string.back() == '\n')
            py_bytes_as_string.pop_back();
        return py_bytes_as_string;
    }

    std::string script_;
};

auto load_and_call_python_function(char* person = NULL)
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
        std::cout << "stdout: " << run.out() << std::endl;
        std::cout << "stderr: " << run.err() << std::endl;
    }

    {
        auto run = PythonRun("from pythonfile import hello; hello('somebody')",
                             std::filesystem::current_path().string());
        std::cout << "stdout: " << run.out() << std::endl;
        std::cout << "stderr: " << run.err() << std::endl;
    }

    {
        auto run = PythonRun();
        std::cout << "returned " << run.run_str_str_function("hello", "pythonfile", "friend",
                                                             std::filesystem::current_path().string()) << std::endl;
        std::cout << "stdout: " << run.out() << std::endl;
        std::cout << "stderr: " << run.err() << std::endl;
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
        int repeat = 1000;
        auto run = PythonRun();
        for (int i = 0; i < repeat; i++)
            run.run_string("i=1");
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = finish - start;
        std::cout << "Elapsed Time per instance: " << elapsed.count() / repeat << " milliseconds" << std::endl;
    }

    return 0;
}
