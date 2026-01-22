#ifndef PYTHONC_H
#define PYTHONC_H


extern "C" void PythonC_Initialize();
extern "C" void PythonC_Shutdown();
extern "C" void PythonC_Run(const char* script);
extern "C" void PythonC_LoadModule(const char* name);
extern "C" const char* PythonC_stdout();
extern "C" const char* PythonC_stderr();
extern "C" const char* PythonC_script();

#endif //PYTHONC_H
