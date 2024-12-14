#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

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
	// position 3 ,color 3, texture coordinate 2
	0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f // top right
	,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f // bottom right
        , -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f // bottom left
	, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f // top left
    };

    uint32 Indices[] = {
	    0, 1, 3
	  , 1, 2, 3 
    };
    
    uint32 Texture1;
    glGenTextures(1, &Texture1);
    //NOTE(Brad): gl texture 0 always active so we still dont need this line.
    glBindTexture(GL_TEXTURE_2D, Texture1);

    //NOTE(Brad): str = xyz
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    int Width, Height, NRChannel;
    stbi_set_flip_vertically_on_load(true);
    uint8 *ContainerData = stbi_load("../data/container.jpg", &Width, &Height, &NRChannel, 0);    
    if(ContainerData)
    {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, ContainerData);
	glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
	std::cout << "Failed to load Image Container" << std::endl;
    }
    stbi_image_free(ContainerData);

    //NOTE(Brad): switch to texture2.
    uint32 Texture2;
    glGenTextures(1, &Texture2);
    glBindTexture(GL_TEXTURE_2D, Texture2);

    //NOTE(Brad): str = xyz
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    uint8 *AwesomeFaceData = stbi_load("../data/awesomeface.png", &Width, &Height, &NRChannel, 0);
    if(AwesomeFaceData)
    {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, AwesomeFaceData);
	glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
	std::cout << "Failed to load Image AwesomeFace" << std::endl;
    }
    stbi_image_free(AwesomeFaceData);
    
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

    // NOTE(Brad): aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)0);
    glEnableVertexAttribArray(0);
    // NOTE(Brad): aColor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);
    // NOTE(Brad): aTexCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)(sizeof(float)*6));
    glEnableVertexAttribArray(2);
    const char *VertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec3 aColor;\n"
	"layout (location = 2) in vec2 aTexCoords;\n"
	"out vec3 VertexColor;\n"
	"out vec2 TexCoord;\n"
	"uniform mat4 transform;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = transform * vec4(aPos, 1.0);\n"
	"	VertexColor = aColor;\n"
	"	TexCoord = aTexCoords;\n"
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

        
    const char *FragmentShaderSource1 = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec3 VertexColor;\n"
	"in vec2 TexCoord;\n"
	"uniform sampler2D UniTexture1;\n"
	"uniform sampler2D UniTexture2;\n"
	"void main()\n"
	"{\n"
	"       FragColor = mix(texture(UniTexture1, TexCoord), texture(UniTexture2, TexCoord), 0.2);\n"
	"}\0";
    
    const char *FragmentShaderSource2 = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"uniform vec4 UniColor;\n"
	"void main()\n"
	"{\n"
	"       FragColor = vec4(UniColor.r, UniColor.g, UniColor.b, UniColor.a);\n"
	"}\0";

    uint32 FragmentShader1;
    FragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    uint32 FragmentShader2;
    FragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(FragmentShader1, 1, &FragmentShaderSource1, 0);
    glCompileShader(FragmentShader1);
    
    glShaderSource(FragmentShader2, 1, &FragmentShaderSource2, 0);    
    glCompileShader(FragmentShader2);    

    uint32 ShaderProgram1;
    ShaderProgram1 = glCreateProgram();
    uint32 ShaderProgram2;
    ShaderProgram2 = glCreateProgram();
    
    glAttachShader(ShaderProgram1, VertexShader);    
    glAttachShader(ShaderProgram1, FragmentShader1);
    
    glAttachShader(ShaderProgram2, VertexShader);    
    glAttachShader(ShaderProgram2, FragmentShader2);
        
    glLinkProgram(ShaderProgram1);
    glLinkProgram(ShaderProgram2);

    glGetProgramiv(ShaderProgram1, GL_LINK_STATUS, &Success);
    if(!Success)
    {
	glGetProgramInfoLog(ShaderProgram1, 512, 0, InfoLog);
	std::cout << "ERROR::SHADER1::PROGRAM::STATUS_FAILED" << InfoLog << std::endl;
    }
    
    glGetProgramiv(ShaderProgram2, GL_LINK_STATUS, &Success);
    if(!Success)
    {
	glGetProgramInfoLog(ShaderProgram2, 512, 0, InfoLog);
	std::cout << "ERROR::SHADER2::PROGRAM::STATUS_FAILED" << InfoLog << std::endl;
    }
    
    //NOTE(Brad): can delete obj.
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader1);
    glDeleteShader(FragmentShader2);

    int UniColorLocation = glGetUniformLocation(ShaderProgram2, "UniColor");
    if(UniColorLocation == -1)
    {
	std::cout << "get UniColorLocation FAILED" << std::endl;
    }

    //NOTE(Brad): set location sampler2d.
    glUseProgram(ShaderProgram1);
    glUniform1i(glGetUniformLocation(ShaderProgram1, "UniTexture1"), 0);
    glUniform1i(glGetUniformLocation(ShaderProgram1, "UniTexture2"), 1);        
    
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
	
	glUseProgram(ShaderProgram1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Texture2);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	real32 TimeValue = glfwGetTime();
	real32 tValue = (sin(TimeValue * 2) / 2.0f) + 0.5f;
	//glUseProgram(ShaderProgram2);
	// NOTE(Brad): assign value to uniform.
	glUniform4f(UniColorLocation, tValue, 0.2f, 0.0f, 1.0f);

	//NOTE(Brad): transform.    
	glm::mat4 trans = glm::mat4(1.0f);// identity matrix.
	trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));// scale.
	trans = glm::rotate(trans, TimeValue, glm::vec3(0.0f, 1.0f, 0.0f));
	//std::cout << glm::to_string(trans) << std::endl;
	std::cout << TimeValue << std::endl;
		
	uint32 transformLocation = glGetUniformLocation(ShaderProgram1, "transform");
	std::cout << transformLocation << std::endl;
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans));
	
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(uint32)*3));

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
    glDeleteProgram(ShaderProgram1);
    glDeleteProgram(ShaderProgram2);
    glDeleteTextures(1, &Texture1);
    glDeleteTextures(1, &Texture2);
    glfwTerminate();
    return 0;
}
