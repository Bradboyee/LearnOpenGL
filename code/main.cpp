#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdint.h>
typedef float  real32;
typedef double real64;

typedef uint8_t  uint8;
typedef uint16_t uitn16;
typedef uint32_t uint32;

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;

void WindowSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glViewport(0, 0, Width, Height);
};

void InputHandler(GLFWwindow *Window)
{
    if(glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Window, true);
    }
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow *Window = glfwCreateWindow(800, 600, "Learn OpenGL", 0, 0);
    
    
    if(!Window)
    {
        // TODO(Brad): diagnostic.
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(Window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }    

    glfwSetFramebufferSizeCallback(Window, WindowSizeCallback);

    // -1 to 1 and y -1 start from the bottom.
    real32 Vertices[] = {
	// first triangle
           0.5f, 0.5f, 0.0f // top right
	,  0.5f, -0.5f, 0.0f // bottom right
        , -0.5f, -0.5f, 0.0f // bottom left
	, -0.5f,  0.5f, 0.0f // top left
    };

    uint32 Indices[] = {
	    0, 1, 3
	  , 1, 2, 3 
    };

    uint32 VAO;
    glGenVertexArrays(1, &VAO);

    //NOTE(Brad): start using VAO.
    glBindVertexArray(VAO);

    uint32 VBO;
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    // NOTE(Brad): must be after VBO.
    uint32 EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
    glEnableVertexAttribArray(0);
    const char *VertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";
    
    uint32 VertexShader;
    VertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(VertexShader, 1, &VertexShaderSource, 0);
    glCompileShader(VertexShader);

    int Success;
    char InfoLog[512];
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Success);

    if(!Success)
    {
	glGetShaderInfoLog(VertexShader, 512, 0, InfoLog);
	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << InfoLog << std::endl;
    }

        
    const char *FragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"       FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\0";

    uint32 FragmentShader;
    FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(FragmentShader, 1, &FragmentShaderSource, 0);
    glCompileShader(FragmentShader);    

    uint32 ShaderProgram;
    ShaderProgram = glCreateProgram();

    glAttachShader(ShaderProgram, VertexShader);
    glAttachShader(ShaderProgram, FragmentShader);
    glLinkProgram(ShaderProgram);

    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if(!Success)
    {
	glGetProgramInfoLog(ShaderProgram, 512, 0, InfoLog);
	std::cout << "ERROR::SHADER::PROGRAM::STATUS_FAILED\n" << InfoLog << std::endl;
    }
    //NOTE(Brad): can delete obj.
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    
    glUseProgram(ShaderProgram);
    while(!glfwWindowShouldClose(Window))
    {
        // NOTE(Brad): input.
        InputHandler(Window);
        
        // NOTE(Brad): render part.
        // NOTE(Brad): state setting. 
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // NOTE(Brad): state using.
        glClear(GL_COLOR_BUFFER_BIT);

	// NOTE(Brad): select VAO to draw.
	glBindVertexArray(VAO);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// NOTE(Brad): remove VAO to draw.
	glBindVertexArray(0);		
        
        // NOTE(Brad): swap buffer. 
        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    // clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(ShaderProgram);
    
    glfwTerminate();
    return 0;
}
