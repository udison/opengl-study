#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Vertex data of a primitive triangle
const float triangle[] = {
     .0f,  .5f, .0f, // top
    -.5f, -.5f, .0f, // bottom left
     .5f, -.5f, .0f  // bottom right
};

// Vertex data of rectangle
const float rectangle[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};

// Order in which each vertex is going to be drawn
// This exists to prevent storing duplicate vertex data, so we store these only once and then tell OpenGL to render
// in a specific order one or more times (through EBOs), hence the duplicated ones and threes below
unsigned const int rectangleIndices[] = {
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

// Simple vertex shader - responsible for handling vertices positions
const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// Simple fragment shader - responsible for handling pixel colors
const char* fragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    // Tells OpenGL the size of the viewport
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {

    // Closes on ESC press
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Set fullscreen on F11
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        glfwSetWindowAttrib(window, GLFW_DECORATED, GL_FALSE);
        glfwMaximizeWindow(window);
    }

    // Set normal rendering on F1
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Set wireframe on F2
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

template <size_t N>
void bufferVertexArray(const float (&vertexData)[N]) {
    unsigned int VBO;

    // Generates the Vertex array buffer object
    glGenBuffers(1, &VBO);

    // Tells OpenGL the buffer type (in this case, a vertex buffer is GL_ARRAY_BUFFER)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copies the data provided to a buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    // Tells OpenGL the format of the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

template <size_t N, size_t I>
void bufferIndexedVertexArray(const float(&vertexData)[N], unsigned const int (&indices)[I]) {
    unsigned int VBO, EBO;

    glGenBuffers(1, &VBO); // Generates the Vertex array buffer object
    glGenBuffers(1, &EBO); // Generates the Element array buffer object

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

int main() {

    // Initializes GLFW
    glfwInit();

    // Window configurations
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Needed for MacOS only

    // Creates a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello OpenGL!", NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);

    // GLAD initialization needs the function that gets OpenGL function pointers (OS specific)
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;

        return -1;
    }

    // Assigns 'framebuffer_size_callback()' as the callback for the window resize event
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Creates the vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Binds the shader source code and compiles it
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Checks if shader compilation went successful
    int success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "[ERROR] Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // Creates the fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "[ERROR] Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // Creates the shader program - responsible for linking shaders outputs and inputs
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "[ERROR] Shader program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertex Buffer Object - stores data that gets sent to the GPU all at once (transiting data from CPU to GPU is slow)
    // Vertex Array Object - stores vertex attribute calls
    unsigned int VAO;
    glGenVertexArrays(1, &VAO); // Generates VAO

    glBindVertexArray(VAO);

    //bufferVertexArray(triangle);
    bufferIndexedVertexArray(rectangle, rectangleIndices);

    // Unbinds the vertex array after use
    glBindVertexArray(0);

    // The  R E N D E R  L O O P !
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(.2f, .3f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window); // Swaps front and back buffer
        glfwPollEvents(); // Handle events like mouse movement or keyboard input
    }

    glfwTerminate(); // Terminates GLFW
    return 0;
}