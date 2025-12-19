program main
    call doit
end program main

subroutine doit
    use Python
    type(PythonRun) :: run
    call run%init(script = 'print("fortran!")')
    print *, run%out()
end subroutine doit
