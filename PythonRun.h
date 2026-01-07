#ifndef PYTHON_FROM_FORTRAN_PYTHONRUN_HPP
#define PYTHON_FROM_FORTRAN_PYTHONRUN_HPP

#include <string>

class PythonRun
{
public:
    PythonRun(std::string script = "", std::string path = "");

    ~PythonRun();

    void add_path(std::string path);

    void load_module(std::string name);

    void run_string(std::string str);

    /* @brief Run a python function that expects zero or 1 string arguments and returns a string
     *
     * @param function_name
     * @param module
     * @param argument
     * @param path
     * @return
     */
    std::string run_str_str_function(std::string function_name, std::string module, std::string argument = "",
                                     std::string path = "");


    const std::string stdout() { return stream("stdout"); }
    const std::string stderr() { return stream("stderr"); }
    const std::string& script() { return script_; }

private:
    const std::string stream(const char* name);
    std::string script_;
};


#endif //PYTHON_FROM_FORTRAN_PYTHONRUN_HPP
