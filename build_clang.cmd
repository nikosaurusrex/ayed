@echo off

set WARNINGS=-Wall -Wextra -Wconversion -Wno-sign-conversion -Wno-unused-but-set-variable -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-char-subscripts
set CFLAGS=%WARNINGS% -std=c++20
set DEBUG_FLAGS=-DBUILD_DEBUG -g3

set RELEASE_FLAGS=-O2 -flto

set PROFILER_FLAGS=

set COMPILE_FLAGS=-I./ -I ./vendor/glfw/include -I ./vendor/glew/include -I ./vendor/freetype/include -I ./vendor/treesitter/src -I ./vendor/treesitter/include
set LINK_FLAGS=-L./vendor/glfw/libs -L./vendor/glew/libs -L./vendor/freetype/libs -lglfw3_mt -lglew32s -lfreetype -lopengl32 -luser32 -lgdi32 -lshell32 -lkernel32

clang++ %DEBUG_FLAGS% %CFLAGS% %COMPILE_FLAGS% editor/editor.cpp %LINK_FLAGS% -o editor_clang.exe
rem clang++ %DEBUG_FLAGS% %CFLAGS% %COMPILE_FLAGS% tests/tests.cpp %LINK_FLAGS% -o tests_clang.exe
