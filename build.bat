@echo off

set project_name=noname
set compiler=cl
set main_file=..\src\main.c
REM Linker errors for GLFW. Use /MD on release and /MDd on debug builds
set cl_default_flags=/Isrc /nologo /FC /Zi /MDd

set static_include=/I"../external/glfw-3.3.9/include" 
set external_include=/I"..\external\opengl"

set linker_flags=user32.lib gdi32.lib Shell32.lib opengl32.lib "../external/glfw-3.3.9/lib/glfw3.lib"

set executable=/Fe"%project_name%.exe"

if not exist build mkdir build
pushd build
if "%1" == "-d" (
	echo Deleting build/*
	del /q *.*
)
%compiler% %main_file% %cl_default_flags% %external_include% %static_include% %linker_flags% %executable%
popd
