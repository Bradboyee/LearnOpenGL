#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/string_cast.hpp>

typedef float  real32;
typedef double real64;

typedef uint8_t  uint8;
typedef uint16_t uitn16;
typedef uint32_t uint32;

typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;

typedef int32 bool32;

bool32 First = true;
real32 LastCursorX = 800.0f/2;
real32 LastCursorY = 600.0f/2;

glm::vec3 CameraPos = glm::vec3(0.0f, 0.0f, 3.0f); 
glm::vec3 CameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); 
glm::vec3 CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

real32 Yaw = -90.0f;
real32 Pitch = 0.0f;

real32 FOV = 45.0f;

#define internal static
#define local_persist static
#define global_variable static

#define ArrayCount(Count) sizeof(Count)/sizeof(Count[0])

glm::vec3 From = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 To = glm::vec3(0.0f ,0.0f, 0.0f);
glm::vec3 TmpUp = glm::vec3(0.0f, 1.0f, 0.0f);

internal glm::mat4 LookAtMatrix(glm::vec3 From, glm::vec3 To, glm::vec3 TmpUp)
{
    //Z axis
    glm::vec3 Forward = glm::normalize(From - To);
    //X axis
    glm::vec3 Right = glm::normalize(glm::cross(TmpUp, Forward));
    //Y axis
    glm::vec3 Up = glm::cross(Forward, Right);

    //NOTE(Brad) this is reverse matrix.
    glm::mat4 Rotation = glm::mat4(1.0f);

    Rotation[0][0] = Right.x;
    Rotation[1][0] = Right.y;
    Rotation[2][0] = Right.z;
    
    Rotation[0][1] = Up.x;
    Rotation[1][1] = Up.y;
    Rotation[2][1] = Up.z;

    Rotation[0][2] = Forward.x;
    Rotation[1][2] = Forward.y;
    Rotation[2][2] = Forward.z;

    glm::mat4 Translation = glm::mat4(1.0f);
    Translation[3][0] = -From.x;
    Translation[3][1] = -From.y;
    Translation[3][2] = -From.z;

    return Rotation * Translation;
}

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
void MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (First)
    {
        LastCursorX = xpos;
        LastCursorY = ypos;
        First = false;
    }

    float xoffset = xpos - LastCursorX;
    float yoffset = LastCursorY - ypos; // reversed since y-coordinates go from bottom to top
    LastCursorX = xpos;
    LastCursorY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    CameraDirection = glm::normalize(front);
}
void MouseCallbackBrad(GLFWwindow* Window, double Xpos, double Ypos)
{
    if(First)
    {
        LastCursorX = Xpos;
        LastCursorY = Ypos;
        First = false;
    }

    real32 OffsetX = Xpos - LastCursorX;
    real32 OffsetY = LastCursorY - Ypos;
    LastCursorX = Xpos;
    LastCursorY = Ypos;

    float Sen = 0.075f;
    OffsetX *= Sen;
    OffsetY *= Sen;

    Yaw += OffsetX;
    Pitch += OffsetY;
    if (Pitch > 89.0f)
    {
        Pitch = 89.0f;
    }    
    if (Pitch < -89.0f)
    {
        Pitch = -89.0f;
    }
    glm::vec3 Front;
    Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front.y = sin(glm::radians(Pitch));
    Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    std::cout << "Pitch : " << Pitch << " Yaw : " << Yaw << std::endl;
    CameraDirection = glm::normalize(Front);
    
}
void ScrollCallback(GLFWwindow* Window, double XOffset, double YOffset)
{
    FOV += (real32) YOffset;
    if(FOV < 1.0f)
    {
        FOV = 1.0f;
    }
    if(FOV > 45.0f)
    {
        FOV = 45.0f;
    }
	
}

internal void WriteStencil()
{
    glStencilMask(0xFF); // enable writing to the stencil buffer || and oper the byte write
    //NOTE(Brad): stencil buffer default is 0.
    glStencilFunc(GL_ALWAYS, 1
		  , 0xFF);//mask all ref and buffer
    //NOTE(Brad) : stencil then depth test.
    glStencilOp(
	GL_KEEP,//s fail
	GL_KEEP,//s pass but d fail
	GL_REPLACE);//pass both
}

struct texture_image
{
    union
    {
	uint32 TextureID[5];
	struct
	{
	    uint32 ContainerTexture;
	    uint32 AwesomeFaceTexture;
	    uint32 Container2Texture;
	    uint32 Container2SpecularTexture;
	    uint32 GrassTexture;
	};
    };
};

//using pointers, need to move the position of the pointer
int strcmp_ptr(char *src1, char *src2)
{
    while((*src1!='\0') || (*src2!='\0'))
    {
        if(*src1 > *src2)
            return 0;
        if(*src1 < *src2)
            return 0;
        src1++;
        src2++;
    }
    return 1;
}

internal void LoadTexture(uint32 *TextureID, char *TexturePath)
{
    char *FileExtension;
    char *FileEx;
    FileEx = TexturePath;
    FileEx += 2;//prevent relative path.
    while(*FileEx++)
    {
	if(*FileEx == '.')
	{
	    FileEx++;
	    break;
	}
    }
	
    int Width, Height, NRChannel;
    glGenTextures(1, TextureID);
    glBindTexture(GL_TEXTURE_2D, *TextureID);

    //NOTE(Brad): str = xyz
    bool32 IsPNG = strcmp_ptr(FileEx, (char *) "png");
    if(IsPNG)
    {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
    }
    else
    {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    uint8 *GrassData = stbi_load(TexturePath, &Width, &Height, &NRChannel, 0);
    if(GrassData)
    {
	if(IsPNG)
	{
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, GrassData);
	}
	else
	{
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, GrassData);	    
	}
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load Image " << TexturePath << std::endl;
    }
    stbi_image_free(GrassData);
}

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
    glfwSetCursorPosCallback(Window, MouseCallback);
    glfwSetScrollCallback(Window, ScrollCallback);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // -1 to 1 and y -1 start from the bottom.
    // NOTE(Brad): position 3 texture 2 normal 3
    real32 CubeVertices[] = {
        // Front face
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Top right
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom right
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom left
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Top left

        // Back face
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, // Top right
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, // Bottom right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, // Bottom left
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, // Top left

        // Left face
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, // Top right
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom left
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // Top left

        // Right face
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Top right
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom right
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Bottom left
        0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // Top left

        // Top face
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top left
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom left
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom right
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top right

        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, // Top left
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, // Bottom left
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, // Bottom right
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f // Top right
    };

    uint32 CubeIndices[] = {
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

    real32 PlainVertices[] = {
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Top right
        0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom right
        -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom left
        -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // Top left
    };

    
    uint32 PlainIndices[] = {
        // Front face
        0, 3, 1,
        1, 2, 0,
    };
    glm::vec3 CubePosition[] =
        {
            glm::vec3( 0.0f,  0.0f,  0.0f),
        };

    
    texture_image Sampler2d = {};
    stbi_set_flip_vertically_on_load(true);
    LoadTexture(&Sampler2d.ContainerTexture, (char *) "../data/container.jpg");
    LoadTexture(&Sampler2d.AwesomeFaceTexture, (char *) "../data/awesomeface.png");
    LoadTexture(&Sampler2d.Container2Texture, (char *) "../data/container2.png");
    LoadTexture(&Sampler2d.Container2SpecularTexture, (char *) "../data/container2_specular.png");
    LoadTexture(&Sampler2d.GrassTexture, (char *) "../data/grass.png");
    
    uint32 VAO;
    glGenVertexArrays(1, &VAO);

    //NOTE(Brad): start using VAO.
    glBindVertexArray(VAO);

    uint32 VBO;
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

    // NOTE(Brad): must be after VBO.
    uint32 EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeIndices), CubeIndices, GL_STATIC_DRAW);

    // NOTE(Brad): aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)0);
    glEnableVertexAttribArray(0);
    // NOTE(Brad): aTexCoords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);
    // NOTE(Brad): aNormal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)(sizeof(float)*5));
    glEnableVertexAttribArray(2);
    
    
    // NOTE(Brad) transformation matrix order is matter !!! scale -> rotate -> translate.
    const char *VertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec2 aTexCoords;\n"
        "layout (location = 2) in vec3 aNormal;\n"
        "out vec2 TexCoord;\n"
        "out vec3 FragPos;\n"
        "out vec3 Normal;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "	TexCoord = aTexCoords;\n"
        "	Normal = mat3(transpose(inverse(model))) * aNormal;\n"
        "	FragPos = vec3(model * vec4(aPos, 1.0f));\n"
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
        
        "struct material\n"
        "{\n"
        "	sampler2D Diffuse;\n"
        "	sampler2D Specular;\n"
        "	float Shininess;\n"
        "};\n"
        
        "struct direction_light\n"
        "{\n"
        "	vec3 Direction;\n"         	

        "	vec3 Ambiant;\n"
        "	vec3 Diffuse;\n"
        "	vec3 Specular;\n"         	
        "};\n"
        
        "struct point_light\n"
        "{\n"
        "	vec3 Position;\n"

        "	vec3 Ambiant;\n"
        "	vec3 Diffuse;\n"
        "	vec3 Specular;\n"
        
        "	float Constant;\n"
        "	float Linear;\n"//NOTE(Brad): faster falloff.
        "	float Quadratic;\n"//NOTE(Brad): slower.
        "};\n"

        "struct spot_light\n"
        "{\n"
        "	vec3 Position;\n"
        "	vec3 Direction;\n"
        
        "	float CutOff;\n"
        "	float OuterCutOff;\n"
        
        "	vec3 Ambiant;\n"
        "	vec3 Diffuse;\n"
        "	vec3 Specular;\n"
        "};\n"
        
        "out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "in vec3 Normal;\n"
        "in vec3 FragPos;\n"
        "uniform sampler2D UniTexture1;\n"
        "uniform sampler2D UniTexture2;\n"
        "uniform sampler2D UniTexture3;\n"

        "uniform vec3 UniViewPosition;\n"
        "uniform material Material;\n"
        
        "uniform spot_light UniSpot;\n"
        "uniform point_light PointLight;\n"
        "uniform direction_light DirectionLight;\n"

        "vec3 PDirectionLight(direction_light DirLight, vec3 NormalVec, vec3 ViewDirection)\n"
        "{\n"
        "       vec3 LightDir = normalize(-DirLight.Direction);\n"
        "       vec3 ReflectDir = reflect(-LightDir, NormalVec);\n"
        
        "       float Diff = max(dot(NormalVec, LightDir), 0.0f);\n"
        "       float Spec = pow(max(dot(ReflectDir, ViewDirection), 0.0f), Material.Shininess);\n"
        
        "       vec3 Ambiant = DirLight.Ambiant * vec3(texture(Material.Diffuse, TexCoord));\n"
        "       vec3 Specular = Spec * DirLight.Specular * vec3(texture(Material.Specular, TexCoord));\n"
        "       vec3 Diffuse = Diff * DirLight.Diffuse * vec3(texture(Material.Diffuse, TexCoord));\n"
        
        "       return (Ambiant + Diffuse + Specular);\n"
        "}\n"
        
        "vec3 APointLight(point_light PointLight, vec3 NormalVec, vec3 ViewDirection, vec3 FragPos)\n"
        "{\n"
        "       vec3 LightDir = normalize(PointLight.Position - FragPos);\n"
        "       vec3 ReflectDir = reflect(-LightDir, NormalVec);\n"
        
        "       float Diff = max(dot(NormalVec, LightDir), 0.0f);\n"
        "       float Spec = pow(max(dot(ReflectDir, ViewDirection), 0.0f), Material.Shininess);\n"

        "       float Distance = length(PointLight.Position - FragPos);\n"
        "       float Attenuation = 1.0f / (PointLight.Constant + (PointLight.Linear * Distance) + (PointLight.Quadratic * (Distance * Distance)));\n"

        "       vec3 Ambiant = PointLight.Ambiant * vec3(texture(Material.Diffuse, TexCoord));\n"
        "       vec3 Specular = Spec * PointLight.Specular * vec3(texture(Material.Specular, TexCoord));\n"
        "       vec3 Diffuse = Diff * PointLight.Diffuse * vec3(texture(Material.Diffuse, TexCoord));\n"
        
        "       Ambiant *= Attenuation;\n"	
        "       Diffuse *= Attenuation;\n"	
        "       Specular *= Attenuation;\n"

        "       return (Ambiant + Diffuse + Specular);\n"
        "}\n"

	"vec3 CSpotLight(spot_light Spot, vec3 NormalVec, vec3 ViewDirection, vec3 FragPos)\n"
        "{\n"
        "       vec3 LightDir = normalize(Spot.Position - FragPos);\n"
        "       vec3 ReflectDir = reflect(-LightDir, NormalVec);\n"
        
        "       float Diff = max(dot(NormalVec, LightDir), 0.0f);\n"
        "       float Spec = pow(max(dot(ReflectDir, ViewDirection), 0.0f), Material.Shininess);\n"
        
        "       float Theta = dot(LightDir, normalize(-Spot.Direction));\n"// angle more = value less.
        "       float Epsilon = Spot.CutOff - Spot.OuterCutOff;\n"// angle more = value less.
        "       float Intensity = clamp((Theta - Spot.OuterCutOff) / Epsilon, 0.0f, 1.0f);\n"

        "       vec3 Ambiant = Spot.Ambiant * vec3(texture(Material.Diffuse, TexCoord));\n"
        "       vec3 Specular = Spec * Spot.Specular * vec3(texture(Material.Specular, TexCoord));\n"
        "       vec3 Diffuse = Diff * Spot.Diffuse * vec3(texture(Material.Diffuse, TexCoord));\n"
        
        "       Ambiant *= Intensity;\n"	
        "       Diffuse *= Intensity;\n"	
        "       Specular *= Intensity;\n"

        "       return (Ambiant + Diffuse + Specular);\n"
        "}\n"
	
        "float near = 0.1f;\n"
        "float far = 100.0f;\n"
	
        "float LinearDepth(float depth)\n"
        "{\n"
	"       float z = depth * 2.0f - 1.0f;\n"
	"       return (2.0 * near * far) / (far + near - z * (far - near));\n"	
	"}\n"	
        "void main()\n"
        "{\n"
        	
        "       vec3 Norm = normalize(Normal);\n"	
        "       vec3 ViewDir = normalize(UniViewPosition - FragPos);\n"
        
        "       vec3 Result = PDirectionLight(DirectionLight, Norm, ViewDir);\n"
        "       Result += APointLight(PointLight, Norm, ViewDir, FragPos);\n"
	
        "       Result += CSpotLight(UniSpot, Norm, ViewDir, FragPos);\n"
        
        "       float depth = LinearDepth(gl_FragCoord.z) / far;\n"
//        "       FragColor = vec4(vec3(depth), 1.0f);\n"
//        "       FragColor = vec4(Result, 1.0f);\n"
	
        "       vec4 Grass = texture(UniTexture3, TexCoord);\n"
        "       if(Grass.a < 0.1)\n"
        "         discard;\n"
        "       FragColor = texture(UniTexture3, TexCoord);\n"

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
        std::cout << "ERROR::SHADER::PROGRAM::STATUS_FAILED" << InfoLog << std::endl;
    }
    //------------------- LIGHT SHADER -------------------
    
    const char *FragmentLightShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "       FragColor = vec4(1.0f);\n"
        "}\0";

    uint32 FragmentLightShader;
    FragmentLightShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(FragmentLightShader, 1, &FragmentLightShaderSource, 0);
    glCompileShader(FragmentLightShader);

    uint32 ShaderLightProgram;
    ShaderLightProgram = glCreateProgram();
    
    glAttachShader(ShaderLightProgram, VertexShader);    
    glAttachShader(ShaderLightProgram, FragmentLightShader);
        
    glLinkProgram(ShaderLightProgram);

    glGetProgramiv(ShaderLightProgram, GL_LINK_STATUS, &Success);
    if(!Success)
    {
        glGetProgramInfoLog(ShaderLightProgram, 512, 0, InfoLog);
        std::cout << "ERROR::LIGHT SHADER::PROGRAM::STATUS_FAILED" << InfoLog << std::endl;
    }
    //------------------- LIGHT SHADER -------------------
    
    //NOTE(Brad): can delete obj.
    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    glDeleteShader(FragmentLightShader);

    //NOTE(Brad): set location sampler2d.
    glUseProgram(ShaderProgram);
    glUniform1i(glGetUniformLocation(ShaderProgram, "UniTexture1"), 0);
    glUniform1i(glGetUniformLocation(ShaderProgram, "UniTexture2"), 1);    
    glUniform1i(glGetUniformLocation(ShaderProgram, "UniTexture3"), Sampler2d.GrassTexture - 1);//grass
    //NOTE(Brad): Lighting Material.
    
    glUniform1i(glGetUniformLocation(ShaderProgram, "Material.Diffuse"), 2);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Material.Specular"), 3);

    real32 Shininess = 32.0f;
    glUniform1f(glGetUniformLocation(ShaderProgram, "Material.Shininess"), Shininess);
    
    //NOTE(Brad): set light.
    glUseProgram(ShaderLightProgram);
    
    glUseProgram(0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
    
    real32 DeltaTime = 0.0f;
    real32 LastFrame = glfwGetTime();
    real32 CameraSpeed = 2.0f;

    glm::vec3 lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
    while(!glfwWindowShouldClose(Window))
    {
        real32 CurrentTime = glfwGetTime();
        DeltaTime = CurrentTime - LastFrame;
        LastFrame = CurrentTime;
        //std::cout << "Dt Time : " << DeltaTime << std::endl;
        // NOTE(Brad): input.
        InputHandler(Window);

        if(glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
        {
            CameraPos += CameraSpeed * CameraDirection * DeltaTime;
        }
        if(glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
        {
            CameraPos -= CameraSpeed * CameraDirection * DeltaTime;
        }
	
        if(glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
        {
            CameraPos -= glm::normalize(glm::cross(CameraDirection, CameraUp)) * CameraSpeed * DeltaTime;
        }
        if(glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
        {
            CameraPos += glm::normalize(glm::cross(CameraDirection, CameraUp)) * CameraSpeed * DeltaTime;
        }
        if(glfwGetKey(Window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            CameraPos += CameraSpeed * CameraUp * DeltaTime;
        }
        if(glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            CameraPos -= CameraSpeed * CameraUp * DeltaTime;
        }
        // NOTE(Brad): render part.
        // NOTE(Brad): state setting. 
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // NOTE(Brad): state using.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        real32 TimeValue = glfwGetTime();
        real32 tValue = (sin(TimeValue * 2) / 2.0f) + 0.5f;

        // NOTE(Brad): assign value to uniform.	
        glm::mat4 model = glm::mat4(1.0f);// transform local -> world space matrix.
        //model = glm::rotate(model, glm::radians(50.0f), glm::vec3(1.0f, 1.0f, 0.0f));
	
        glm::mat4 view = glm::mat4(1.0f);// transform world -> view space matrix.
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
	
        glm::mat4 projection = glm::mat4(1.0f);// view space -> cliped space matrix.	
        projection = glm::perspective(glm::radians(FOV), (real32)width/(real32)height, 0.1f, 100.0f);

	
        view = LookAtMatrix(
                            CameraPos,
                            CameraPos + CameraDirection,//target
                            CameraUp
                            );
       
        // NOTE(Brad): select VAO to draw.
        glBindVertexArray(VAO);


	glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glUseProgram(ShaderProgram);

	for(uint32 i = 0;
	    i < ArrayCount(Sampler2d.TextureID);
	    i++)
	{
	    glActiveTexture(GL_TEXTURE0 + i);
	    glBindTexture(GL_TEXTURE_2D, Sampler2d.TextureID[i]);
	}
	
        uint32 ModelUniLocation = glGetUniformLocation(ShaderProgram, "model");
        uint32 ViewUniLocation = glGetUniformLocation(ShaderProgram, "view");
        uint32 ProjectionUniLocation = glGetUniformLocation(ShaderProgram, "projection");

        glUniformMatrix4fv(ViewUniLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(ProjectionUniLocation, 1, GL_FALSE, glm::value_ptr(projection));
        
        glUniform3fv(glGetUniformLocation(ShaderProgram, "UniViewPosition"), 1, &CameraPos[0]);
        
        //DirectionLight
                
        glm::vec3 LightColor;
        LightColor.r = 1.0f;
        LightColor.g = 0.0f;
        LightColor.b = 0.0f;
        
        glm::vec3 DirectionLightDirection = glm::vec3(1.0f, 0.0f, 0.0f);//NOTE light --> object direction
        
        glm::vec3 DirectionLightDiffuse = LightColor * glm::vec3(0.5f);
        glm::vec3 DirectionLightAmbiant = DirectionLightDiffuse * glm::vec3(0.2f);
        glm::vec3 DirectionLightSpecular = glm::vec3(1.0f);
        glUniform3fv(glGetUniformLocation(ShaderProgram, "DirectionLight.Direction"), 1, &DirectionLightDirection[0]);   
        glUniform3fv(glGetUniformLocation(ShaderProgram, "DirectionLight.Ambiant"), 1, &DirectionLightAmbiant[0]);   
        glUniform3fv(glGetUniformLocation(ShaderProgram, "DirectionLight.Diffuse"), 1, &DirectionLightDiffuse[0]);
        glUniform3fv(glGetUniformLocation(ShaderProgram, "DirectionLight.Specular"), 1, &DirectionLightSpecular[0]);
        
        //PointLight
        
        glUniform3fv(glGetUniformLocation(ShaderProgram, "PointLight.Position"), 1, &CameraPos[0]);

        glm::vec3 LightDiffuse = LightColor * glm::vec3(0.5f);
        glm::vec3 LightAmbiant = LightDiffuse * glm::vec3(0.2f);
        glm::vec3 LightSpecular = glm::vec3(1.0f);
        glUniform3fv(glGetUniformLocation(ShaderProgram, "PointLight.Ambiant"), 1, &LightAmbiant[0]);   
        glUniform3fv(glGetUniformLocation(ShaderProgram, "PointLight.Diffuse"), 1, &LightDiffuse[0]);
        glUniform3fv(glGetUniformLocation(ShaderProgram, "PointLight.Specular"), 1, &LightSpecular[0]);

        glUniform1f(glGetUniformLocation(ShaderProgram, "PointLight.Constant"), 1.0f);
        glUniform1f(glGetUniformLocation(ShaderProgram, "PointLight.Linear"), 0.09f);
        glUniform1f(glGetUniformLocation(ShaderProgram, "PointLight.Quadratic"), 0.032f);

        //SpotLight
        glm::vec3 SpotLightColor;
        SpotLightColor.r = 0.0f;
        SpotLightColor.g = 0.0f;
        SpotLightColor.b = 1.0f;        
        glm::vec3 SpotLightDiffuse = SpotLightColor * glm::vec3(0.5f);
        glm::vec3 SpotLightAmbiant = SpotLightDiffuse * glm::vec3(0.2f);
        glm::vec3 SpotLightSpecular = glm::vec3(1.0f);        
        glUniform3fv(glGetUniformLocation(ShaderProgram, "UniSpot.Position"), 1, &CameraPos[0]);
        glUniform3fv(glGetUniformLocation(ShaderProgram, "UniSpot.Direction"), 1, &CameraDirection[0]);
        
        glUniform3fv(glGetUniformLocation(ShaderProgram, "UniSpot.Ambiant"), 1, &SpotLightAmbiant[0]);   
        glUniform3fv(glGetUniformLocation(ShaderProgram, "UniSpot.Diffuse"), 1, &SpotLightDiffuse[0]);
        glUniform3fv(glGetUniformLocation(ShaderProgram, "UniSpot.Specular"), 1, &SpotLightSpecular[0]);
        
        glUniform1f(glGetUniformLocation(ShaderProgram, "UniSpot.Constant"), 1.0f);
        glUniform1f(glGetUniformLocation(ShaderProgram, "UniSpot.Linear"), 0.09f);
        glUniform1f(glGetUniformLocation(ShaderProgram, "UniSpot.Quadratic"), 0.032f);
        
        glUniform1f(glGetUniformLocation(ShaderProgram, "UniSpot.CutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(ShaderProgram, "UniSpot.OuterCutOff"), glm::cos(glm::radians(17.5f)));
        
        for(int CubeIndex = 0;
            CubeIndex < ArrayCount(CubePosition);
            CubeIndex++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, CubePosition[CubeIndex]);
            model = glm::rotate(model, glm::radians(CubeIndex * 20.0f), glm::vec3(1.0f, 1.0f, 0.0f));
            glUniformMatrix4fv(ModelUniLocation, 1, GL_FALSE, glm::value_ptr(model));
            
            glDrawElements(GL_TRIANGLES, ArrayCount(CubeIndices), GL_UNSIGNED_INT, 0);   
        }

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(ShaderLightProgram);

        uint32 LightModelUniLocation = glGetUniformLocation(ShaderLightProgram, "model");
        uint32 LightViewUniLocation = glGetUniformLocation(ShaderLightProgram, "view");
        uint32 LightProjectionUniLocation = glGetUniformLocation(ShaderLightProgram, "projection");
        //std::cout << ModelUniLocation << ViewUniLocation << ProjectionUniLocation << std::endl;
        glUniformMatrix4fv(LightViewUniLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(LightProjectionUniLocation, 1, GL_FALSE, glm::value_ptr(projection));

        model = glm::translate(glm::mat4(1.0f), CubePosition[0]);
        model = glm::scale(model, glm::vec3(1.01f));
        glUniformMatrix4fv(LightModelUniLocation, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, ArrayCount(CubeIndices), GL_UNSIGNED_INT, 0);	    


        WriteStencil();
        glEnable(GL_DEPTH_TEST);
        // NOTE(Brad): remove VAO to draw.
        glBindVertexArray(0);		
        
        // NOTE(Brad): swap buffer. 
        glfwSwapBuffers(Window);
        glfwPollEvents();

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL Error: " << err << std::endl;
        }
    }

    // clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(ShaderProgram);
    glDeleteProgram(ShaderLightProgram);
    for(uint32 i = 0;
	i < ArrayCount(Sampler2d.TextureID);
	i++)
    {
	glDeleteTextures(1, &Sampler2d.TextureID[i]);
    }
    glfwTerminate();
    return 0;
}
