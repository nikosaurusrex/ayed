@echo off

set CFLAGS=/W3 /wd4100 /wd4189 /std:c++20
set DEBUG_FLAGS=/DBUILD_DEBUG /Z7

set RELEASE_FLAGS_ARM64=/O2 /GL
set RELEASE_FLAGS_X64=/O2 /GL

set PROFILER_FLAGS=

cl.exe %TEST_FLAGS% %DEBUG_FLAGS% %CFLAGS% /I./ editor/main.cpp /Fe:editor
