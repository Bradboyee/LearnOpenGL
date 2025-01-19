mkdir ../build
pushd ../build
clang++ -g ../code/main.cpp -o main ../code/glad.c -I../include -L/opt/homebrew/lib -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit 
popd
