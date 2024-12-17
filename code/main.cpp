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

#define ArrayCount(Count) sizeof(Count)/sizeof(Count[0])
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

    int width = 800;
    int height = 600;
    GLFWwindow *Window = glfwCreateWindow(width, height, "Learn OpenGL", 0, 0);
    
    
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
	// Front face
	0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // Top right
	0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // Bottom right
	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // Bottom left
	-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, // Top left

	// Back face
	0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // Top right
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // Bottom right
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom left
	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // Top left

	// Left face
	-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // Top right
	-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // Bottom right
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom left
	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // Top left

	// Right face
	0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // Top right
	0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // Bottom right
	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // Bottom left
	0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // Top left

	// Top face
	-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // Top left
	-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, // Bottom left
	0.5f,  0.5f,  0.5f, 1.0f, 0.0f, // Bottom right
	0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // Top right

	// Bottom face
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Top left
	-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // Bottom left
	0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // Bottom right
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f  // Top right
    };

    uint32 Indices[] = {
	// Front face
	0, 1, 3,
	1, 2, 3,

	// Back face
	4, 5, 7,
	5, 6, 7,

	// Left face
	8, 9, 11,
	9, 10, 11,

	// Right face
	12, 13, 15,
	13, 14, 15,

	// Top face
	16, 17, 19,
	17, 18, 19,

	// Bottom face
	20, 21, 23,
	21, 22, 23
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
    glEnableVertexAttribArray(0);
    // NOTE(Brad): aTexCoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);
    // NOTE(Brad) transformation matrix order is matter !!! scale -> rotate -> translate.
    const char *VertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"layout (location = 1) in vec2 aTexCoords;\n"
	"out vec2 TexCoord;\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 projection;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
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
	"in vec2 TexCoord;\n"
	"uniform sampler2D UniTexture1;\n"
	"uniform sampler2D UniTexture2;\n"
	"void main()\n"
	"{\n"
	"       FragColor = mix(texture(UniTexture1, TexCoord), texture(UniTexture2, TexCoord), 0.2);\n"
	"}\0";

    uint32 FragmentShader1;
    FragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(FragmentShader1, 1, &FragmentShaderSource1, 0);
    glCompileShader(FragmentShader1);

    uint32 ShaderProgram1;
    ShaderProgram1 = glCreateProgram();
    
    glAttachShader(ShaderProgram1, VertexShader);    
    glAttachShader(ShaderProgram1, FragmentShader1);
        
    glLinkProgram(ShaderProgram1);

    glGetProgramiv(ShaderProgram1, GL_LINK_STATUS, &Success);
    if(!Success)
    {
	glGetProgramInfoLog(ShaderProgram1, 512, 0, InfoLog);
	std::cout << "ERROR::SHADER1::PROGRAM::STATUS_FAILED" << InfoLog << std::endl;
    }
    
    //NOTE(Brad): can delete obj.
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader1);

    //NOTE(Brad): set location sampler2d.
    glUseProgram(ShaderProgram1);
    glUniform1i(glGetUniformLocation(ShaderProgram1, "UniTexture1"), 0);
    glUniform1i(glGetUniformLocation(ShaderProgram1, "UniTexture2"), 1);        

    glEnable(GL_DEPTH_TEST); 
    while(!glfwWindowShouldClose(Window))
    {
        // NOTE(Brad): input.
        InputHandler(Window);
        
        // NOTE(Brad): render part.
        // NOTE(Brad): state setting. 
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // NOTE(Brad): state using.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// NOTE(Brad): select VAO to draw.
	glBindVertexArray(VAO);
	
	glUseProgram(ShaderProgram1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Texture2);

	real32 TimeValue = glfwGetTime();
	real32 tValue = (sin(TimeValue * 2) / 2.0f) + 0.5f;
	//glUseProgram(ShaderProgram2);
	// NOTE(Brad): assign value to uniform.

	glm::mat4 view = glm::mat4(1.0f);// transform local -> world space matrix.
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 model = glm::mat4(1.0f);// transform world -> view space matrix.
	model = glm::rotate(model, TimeValue * glm::radians(50.0f), glm::vec3(1.0f, 1.0f, 0.0f));
	
	glm::mat4 projection = glm::mat4(1.0f);// view space -> cliped space matrix.	
	projection = glm::perspective(glm::radians(45.0f), (real32)width/(real32)height, 0.1f, 100.0f);
		
	uint32 ModelUniLocation = glGetUniformLocation(ShaderProgram1, "model");
	uint32 ViewUniLocation = glGetUniformLocation(ShaderProgram1, "view");
	uint32 ProjectionUniLocation = glGetUniformLocation(ShaderProgram1, "projection");
	//std::cout << ModelUniLocation << ViewUniLocation << ProjectionUniLocation << std::endl;
	glUniformMatrix4fv(ModelUniLocation, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(ViewUniLocation, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(ProjectionUniLocation, 1, GL_FALSE, glm::value_ptr(projection));
	
	glDrawElements(GL_TRIANGLES, ArrayCount(Indices), GL_UNSIGNED_INT, 0);

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
    glDeleteTextures(1, &Texture1);
    glDeleteTextures(1, &Texture2);
    glfwTerminate();
    return 0;
}
