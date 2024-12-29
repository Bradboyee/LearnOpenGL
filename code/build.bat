mkdir ..\build
pushd ..\build
clang++ -g ..\code\main.cpp ..\code\glad.c -I..\include -L..\lib\win32 -lglfw3dll -luser32 -lgdi32 -lopengl32
copy ..\lib\win32\glfw3.dll .
popd
