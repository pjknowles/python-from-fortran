program main
    call one
    call two
    call three
end program main

subroutine one
    use Python
    type(PythonRun) :: run
    character(:), allocatable :: o
    call run%init(script = 'print("fortran!")')
    call run%run_string('print("another")')
    print *,  run%out()

end subroutine one

subroutine two
    use Python
    type(PythonRun) :: run
    character(:), allocatable :: o
    call run%init()
    call run%run_string('print("two")')
    call run%run_string('1+a')
    print *,  'out',run%out()
    print *,  'err',run%err()

end subroutine two

subroutine three
    use Python
    type(PythonRun) :: run
    character(:), allocatable :: o
    character(1024) :: wd
    call getcwd(wd)
    call run%init(script='from pythonfile import hello; hello("fortran programmer")', path=wd)
    print *,  'out',run%out()
    print *,  'err',run%err()

end subroutine three
