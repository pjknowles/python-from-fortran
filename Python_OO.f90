module Python_OO
    private
    public :: PythonRun
    type :: PythonRun
        character(:), allocatable, private :: script_
        logical, private :: initialised = .false.
    contains
        procedure, public :: init, add_path, load_module, run_string, stdout, stderr, script
        final :: shutdown
    end type PythonRun

    logical :: PythonRun_active = .false.

    interface
        subroutine Py_Initialize() bind(C, name = 'Py_Initialize')
        end subroutine Py_Initialize
        subroutine Py_Finalize() bind(C, name = 'Py_Finalize')
        end subroutine Py_Finalize
        subroutine PyRun_SimpleString(script) bind(C, name = 'PyRun_SimpleString')
            use iso_c_binding, only : c_char
            CHARACTER(kind = c_char), DIMENSION(*) :: script
        end subroutine PyRun_SimpleString
        function PythonRun_stream(name) bind(C, name = 'PythonRun_stream')
            use iso_c_binding, only : c_char, c_ptr
            type(c_ptr) :: PythonRun_stream
            character(kind = c_char), dimension(*) :: name
        end function PythonRun_stream
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
                        'sys.stdout = CatchOutErr()' // new_line(' ') // &
                        'sys.stderr = CatchOutErr()' // new_line(' ')))
        if (present(script)) then
            call self%run_string(script)
        end if
        self%initialised = .true.
    end subroutine init

    subroutine shutdown(self)
        type(PythonRun), intent(inout) :: self
        call Py_Finalize
        self%initialised = .false.
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
        if (.not. self%initialised) call self%init
        call PyRun_SimpleString(c_string_c(str))
        if (self%script_ .ne. '') self%script_ = self%script_ // new_line(' ')
        self%script_ = self%script_ // trim(str)
    end subroutine run_string

    function stdout(self)
        class(PythonRun), intent(inout) :: self
        character(:), allocatable :: stdout
        stdout = c_string_p_f(PythonRun_stream(c_string_c('stdout')))
    end function stdout

    function stderr(self)
        class(PythonRun), intent(inout) :: self
        character(:), allocatable :: stderr
        stderr = c_string_p_f(PythonRun_stream(c_string_c('stderr')))
    end function stderr

    function script(self)
        class(PythonRun), intent(inout) :: self
        character(:), allocatable :: script
        script = self%script_
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

    !> @brief Convert to Fortran string from C string
    SUBROUTINE c_string_to_f(cstring, fstring)
        use iso_c_binding, only : c_char, c_null_char
        CHARACTER(kind = c_char), DIMENSION(*), INTENT(in) :: cstring !< The null-terminated string to be converted.
        CHARACTER(*), INTENT(inout) :: fstring !< The fortran string to hold the result. If its length is shorter than cstring, there
        !< will be truncation.
        INTEGER :: i
        fstring = ' '
        DO i = 1, LEN(fstring)
            IF (cstring(i).EQ.C_NULL_CHAR) EXIT
            fstring(i:i) = cstring(i)
        END DO
    END SUBROUTINE c_string_to_f
    !> @brief Convenience function to convert to Fortran string from C char* contained in c_ptr
    FUNCTION c_string_p_f(cp)
        use iso_c_binding, only : c_ptr, c_char, c_f_pointer
        CHARACTER(:), ALLOCATABLE :: c_string_p_f
        TYPE(c_ptr), INTENT(in) :: cp
        CHARACTER(c_char), DIMENSION(:), POINTER :: sp
        CALL c_f_POINTER(cp, sp, [1])
        c_string_p_f = c_string_f(sp)
    END FUNCTION c_string_p_f
    !> @brief Convenience function to convert to Fortran string from C string
    FUNCTION c_string_f(cstring)
        use iso_c_binding, only : c_char, c_int8_t
        CHARACTER(:), ALLOCATABLE :: c_string_f, c_null_char
        CHARACTER(kind = c_char), DIMENSION(*), INTENT(in) :: cstring !< The null-terminated string to be converted.
        INTEGER :: i
        integer(c_int8_t) :: i1
        DO i = 0, 1000000000
            IF (cstring(i + 1).EQ.C_NULL_CHAR .or. transfer(cstring(i + 1), i1).eq.0) goto 1
        END DO
        stop 'Unterminated C-style string in c_string::c_string_f'
        1 CONTINUE
        ALLOCATE(CHARACTER(len = i) :: c_string_f)
        CALL c_string_to_f(cstring, c_string_f)
    END FUNCTION c_string_f

end module Python_OO