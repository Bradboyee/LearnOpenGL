@echo off
IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
clang -g -MP ..\code\main.cpp ..\code\glad.c -I..\include -L..\lib\win32 -lglfw3dll -lgdi32 -lopengl32
IF NOT EXIST glfw3.dll (copy ..\lib\win32\glfw3.dll .)
popd
