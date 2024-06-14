#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

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

    // The  R E N D E R  L O O P !
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(.2f, .3f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window); // Swaps front and back buffer
        glfwPollEvents(); // Handle events like mouse movement or keyboard input
    }

    glfwTerminate(); // Terminates GLFW
    return 0;
}