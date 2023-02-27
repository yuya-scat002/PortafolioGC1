#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

// FIGURAS DE COLORES

//1) Modifica el shader
const char* vertexShaderSourceWithColor =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"

"out vec3 ourColor;"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;"
"}\0";


const char* fragmentShaderSourceWithColor = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor.x, ourColor.y, ourColor.z, 1.0f);\n"
"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSourceWithColor, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceWithColor, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
  
    //Modifica el arreglo de vertices
    float verticesTriangulosColores[] = {
     //Triangulo 1
     -0.2f,1.0f,0.0f,    0.976, 1, 0.101,  // 0
     0.0f, 0.7f, 0.0f,   0.976, 1, 0.101,  // 1
     -0.2f, 0.5f, 0.0f, 0.976, 1, 0.101,  // 2 
        

     //Triangulo 2
     0.0f, 0.7f, 0.0f,   0.290, 0.643, 0.254,  // 3
     0.2f, 1.0f, 0.0f,   0.290, 0.643, 0.254,  // 4
     0.2f,  0.5f, 0.0f, 0.290, 0.643, 0.254,   // 5

      
     //Triangulo 3
     0.2f,  0.5f, 0.0f, 1, 0, 0,        // 6
     0.0f, 0.7f, 0.0f,  1, 0, 0,        // 7
     0.0f,  0.3f, 0.0f, 1, 0, 0,        // 8
     //Triangulo 4
     0.0f, 0.7f, 0.0f,  1, 0, 0,        // 9
     -0.2f, 0.5f, 0.0f, 1, 0, 0, // 10
     0.0f,  0.3f, 0.0f, 1, 0, 0,        // 11


     //Triangulo 5
     0.1f,  0.4f, 0.0f,  0.474, 0.207, 0.207,  // 12
     -0.2f,  0.1f, 0.0f,  0.474, 0.207, 0.207,  // 13
     0.1f, -0.3f, 0.0f,  0.474, 0.207, 0.207,  // 14


     //Triangulo 6
     0.1f,  0.4f, 0.0f, 0.490, 0.180, 0.556,  // 15
     0.35f,  0.1f, 0.0f, 0.490, 0.180, 0.556,  // 16
     0.3f,  0.0f, 0.0f, 0.490, 0.180, 0.556,  // 17
     //Triangulo 7
     0.3f,  0.0f, 0.0f, 0.490, 0.180, 0.556,  // 18
     0.1f, -0.3f, 0.0f,0.490, 0.180, 0.556,   // 19
     0.1f,  0.4f, 0.0f, 0.490, 0.180, 0.556,   // 20
     //Triangulo 8
     0.3f,  0.0f, 0.0f, 0.490, 0.180, 0.556,  // 21
     0.1f, -0.3f, 0.0f,0.490, 0.180, 0.556,   // 22
     0.1f,  -0.4f, 0.0f, 0.490, 0.180, 0.556,  // 23


     //Triangulo 9
     0.3f,  0.0f, 0.0f, 0.556, 0.415, 0.180,  // 24
     0.3f,  -0.6f, 0.0f, 0.556, 0.415, 0.180,  // 25
     0.1f, -0.3f, 0.0f,0.556, 0.415, 0.180,  // 26
     //Triangulo 10
     -0.2f,  -0.6f, 0.0f, 0.556, 0.415, 0.180,   // 27
     0.1f, -0.3f, 0.0f,0.556, 0.415, 0.180,   // 28
     0.3f,  -0.6f, 0.0f, 0.556, 0.415, 0.180,   // 29

              
     //Triangulo 11
     0.3f,  -0.6f, 0.0f, 0.180, 0.207, 0.556,   // 30
     0.6f,  -0.6f, 0.0f,  0.180, 0.207, 0.556,    // 31
     0.6f,  -0.3f, 0.0f, 0.180, 0.207, 0.556,    // 32
     //Triangulo 12
     0.6f,  -0.3f, 0.0f, 0.180, 0.207, 0.556,   // 33
     0.6f,  -0.6f, 0.0f,  0.180, 0.207, 0.556,   // 34
     0.9f,  -0.3f, 0.0f, 0.180, 0.207, 0.556,    // 35
           
     
    };

    
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,   // top left 

        -1.0f,  -0.5f, 0.0f,  // top right
        -1.0f,   0.5f, 0.0f,  // bottom right
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
      3, 2, 1,   // second Triangle

        //3, 2, 5,   // third Triangle
       // 5, 4, 2   // fourth Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTriangulosColores), verticesTriangulosColores, GL_STATIC_DRAW);

 /*   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
*/
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //Column Major
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 1);
    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    glBindVertexArray(1);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glLineWidth(20.0);

        glPointSize(20.0);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 40);
        //glDrawElements(GL_TRIANGLES, 15, GL_UNSIGNED_INT, 0);

        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}