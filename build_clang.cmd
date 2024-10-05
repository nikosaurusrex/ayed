@echo off

set WARNINGS=-Wall -Wextra -Wconversion -Wno-sign-conversion -Wno-unused-but-set-variable -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-char-subscripts
set CFLAGS=%WARNINGS% -std=c++20
set DEBUG_FLAGS=-DBUILD_DEBUG -g3

set RELEASE_FLAGS=-O2

set PROFILER_FLAGS=

set COMPILE_FLAGS=-I./ -I ./vendor/glfw/include -I ./vendor/glew/include -I ./vendor/freetype/include -I ./vendor/treesitter/include
set LINK_FLAGS=-L./vendor/glfw/libs -L./vendor/glew/libs -L./vendor/freetype/libs -lglfw3_mt -lglew32s -lfreetype -lopengl32 -luser32 -lgdi32 -lshell32 -lkernel32

rem clang -Wno-everything %RELEASE_FLAGS% ./vendor/treesitter/src -I ./vendor/treesitter/include -c -o treesitter.o ./vendor/treesitter/build/build.c
clang++ %DEBUG_FLAGS% %CFLAGS% %COMPILE_FLAGS% editor/editor.cpp treesitter.o %LINK_FLAGS% -o editor_debug.exe
rem clang++ %RELEASE_FLAGS% %CFLAGS% %COMPILE_FLAGS% editor/editor.cpp treesitter.o %LINK_FLAGS% -o editor_release.exe
rem clang++ %RELEASE_FLAGS% %CFLAGS% %COMPILE_FLAGS% tests/tests.cpp %LINK_FLAGS% -o tests.exe
