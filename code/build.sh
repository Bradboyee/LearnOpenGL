mkdir ../build
pushd ../build
clang++ -g -O0 -std=c++11 ../code/main.cpp ../code/glad.c -o main -I../include -L../lib -lglfw3 -lassimp -framework Cocoa -framework OpenGL -framework IOKit -Wl,-rpath,../lib
popd
