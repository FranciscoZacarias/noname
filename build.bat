@echo off

set compiler_and_entry=cl ..\src\main.c

REM Linker errors for GLFW. Use /MD on release and /MDd on debug builds
set cl_default_flags=/Isrc /nologo /FC /Zi /MDd

set external_include= /I"..\src\external\opengl" ^
											/I"..\src\external\glfw-3.3.9\include" ^
											/I"..\src\f_base"

set linker_flags= user32.lib ^
									gdi32.lib ^
									Shell32.lib ^
									opengl32.lib ^
									winmm.lib ^
									"..\src\external\glfw-3.3.9\lib\glfw3.lib"

if not exist build mkdir build
pushd build
%compiler_and_entry% %cl_default_flags% %external_include% %static_include% %linker_flags% /Fe"noname.exe"
popd