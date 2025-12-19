program main
    call one
    call two
    call three
end program main

subroutine one
    use Python
    type(PythonRun) :: run
    call run%init(script = 'print("fortran!")')
    call run%run_string('print("another")')
    print *, run%stdout()

end subroutine one

subroutine two
    use Python
    type(PythonRun) :: run
    call run%init()
    call run%run_string('print("two")')
    call run%run_string('1+a')
    print *, 'script ', run%script()
    print *, 'stdout ', run%stdout()
    print *, 'stderr ', run%stderr()

end subroutine two

subroutine three
    use Python
    type(PythonRun) :: run
    character(1024) :: wd
    call getcwd(wd)
    call run%init(script = 'from pythonfile import hello; hello("fortran programmer")', path = wd)
    print *, 'script ', run%script()
    print *, 'stdout ', run%stdout()
    print *, 'stderr ', run%stderr()

end subroutine three
