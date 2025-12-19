program main
    call doit
end program main

subroutine doit
    use Python
    type(PythonRun) :: run
    call run%init(script = 'print("fortran!")')
    call run%run_string('print("another")')
    print *, run%out()
end subroutine doit
