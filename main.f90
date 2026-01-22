program main
    call one
    call two
    call three
    call procedural
    call procedural
end program main

subroutine one
    use Python_OO
    type(PythonRun) :: run
    call run%init(script = 'print("fortran!")')
    call run%run_string('print("another")')
    print *, run%stdout()

end subroutine one

subroutine two
    use Python_OO
    type(PythonRun) :: run
    call run%run_string('print("two")')
    call run%run_string('1+a')
    print *, 'script ', run%script()
    print *, 'stdout ', run%stdout()
    print *, 'stderr ', run%stderr()

end subroutine two

subroutine three
    use Python_OO
    type(PythonRun) :: run
    character(1024) :: wd
    call getcwd(wd)
    call run%init(script = 'from pythonfile import hello; hello("fortran programmer")', path = wd)
    print *, 'script ', run%script()
    print *, 'stdout ', run%stdout()
    print *, 'stderr ', run%stderr()

end subroutine three

subroutine procedural
    use Python
    character(1024) :: wd
    call getcwd(wd)
    call Python_initialise(script = 'from pythonfile import hello; hello("procedural programmer")', path = wd)
    call Python_run('b=99')
    call Python_run('print(b);assert b==99')
    print *, 'script ', Python_script()
    print *, 'stdout ', Python_stdout()
    print *, 'stderr ', Python_stderr()

end subroutine procedural
