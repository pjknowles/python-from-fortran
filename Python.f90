module Python
    private
    public :: PythonRun
    type :: PythonRun
        character(:), allocatable, private :: script_
    contains
        procedure, public :: init, add_path, load_module, run_string, out, err, script
        final :: shutdown
    end type PythonRun

    interface
        subroutine Py_Initialize() bind(C, name = 'Py_Initialize')
        end subroutine Py_Initialize
        subroutine Py_Finalize() bind(C, name = 'Py_Finalize')
        end subroutine Py_Finalize
        subroutine PyRun_SimpleString(script) bind(C, name = 'PyRun_SimpleString')
            use iso_c_binding, only : c_char
            CHARACTER(kind = c_char), DIMENSION(*) :: script
        end subroutine PyRun_SimpleString
    end interface

contains
    subroutine init(self, script, path)
        use iso_c_binding, only : c_null_char
        class(PythonRun), intent(inout) :: self
        character(*), intent(in), optional :: script
        character(*), intent(in), optional :: path
        call Py_Initialize();
        self%script_ = ''
        if (present(path)) then
            call self%add_path(path)
        end if
        call PyRun_SimpleString(c_string_c(&
                'import sys' // new_line(' ') // &
                        'class CatchOutErr:' // new_line(' ') // &
                        '  def __init__(self):' // new_line(' ') // &
                        '    self.value = ""' // new_line(' ') // &
                        '  def write(self, txt):' // new_line(' ') // &
                        '    self.value += txt' // new_line(' ') // &
                        '    sys.stdout = CatchOutErr()' // new_line(' ') // &
                        '    sys.stderr = CatchOutErr()' // new_line(' ')))
        if (present(script)) then
            call self%run_string(script)
        end if
    end subroutine init

    subroutine shutdown(self)
        type(PythonRun), intent(inout) :: self
        call Py_Finalize
    end subroutine shutdown

    subroutine add_path(self, path)
        class(PythonRun), intent(inout) :: self
        character(*), intent(in) :: path
        call self%run_string('import sys; sys.path.append("' // trim(path) // '")')
    end subroutine add_path

    subroutine load_module(self, name)
        class(PythonRun), intent(inout) :: self
        character(*), intent(in) :: name
        call self%run_string('import ' // trim(name) // '")')
    end subroutine load_module

    subroutine run_string(self, str)
        class(PythonRun), intent(inout) :: self
        character(*), intent(in) :: str
        call PyRun_SimpleString(c_string_c(str))
        if (self%script_ .ne. '') self%script_ = self%script_ // new_line(' ')
        self%script_ = self%script_ // trim(str)
    end subroutine run_string

    function out(self)
        class(PythonRun), intent(inout) :: self
        character(:), allocatable :: out
        out = 'not implemented'
    end function out

    function err(self)
        class(PythonRun), intent(inout) :: self
        character(:), allocatable :: err
        err = 'not implemented'
    end function err

    function script(self)
        class(PythonRun), intent(inout) :: self
        character(:), allocatable :: script
    end function script

    FUNCTION c_string_c(fstring)
        use iso_c_binding, only : c_char, c_null_char
        CHARACTER(*), INTENT(in) :: fstring
        CHARACTER(kind = c_char), DIMENSION(:), ALLOCATABLE :: c_string_c
        INTEGER :: i
        ALLOCATE(CHARACTER(kind = c_char) :: c_string_c(len_TRIM(fstring) + 1))
        DO i = 1, len_TRIM(fstring)
            c_string_c(i) = fstring(i:i)
        END DO
        c_string_c(len_TRIM(fstring) + 1) = c_null_char
    END FUNCTION c_string_c

end module Python