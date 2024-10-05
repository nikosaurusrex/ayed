@echo off

set CFLAGS=/W3 /wd4100 /wd4189 /std:c++20
set DEBUG_FLAGS=/DBUILD_DEBUG /Z7

set PROFILER_FLAGS=

set COMPILE_FLAGS=/I./ /I ./vendor/glfw/include /I ./vendor/glew/include /I ./vendor/freetype/include /I ./vendor/treesitter/src /I ./vendor/treesitter/include
set LINK_FLAGS=/LIBPATH:./vendor/glfw/libs /LIBPATH:./vendor/glew/libs /LIBPATH:./vendor/freetype/libs glfw3_mt.lib glew32s.lib freetype.lib opengl32.lib user32.lib gdi32.lib shell32.lib kernel32.lib

cl.exe %DEBUG_FLAGS% %CFLAGS% %COMPILE_FLAGS% editor/editor.cpp /link %LINK_FLAGS% /OUT:editor_debug_msvc.exe