#include <stdio.h> // fprintf, stderr

#include <GLFW/glfw3.h>

// global variables
// we'll use global variables, since the GLFW callback functions we will add
// later will require access to these variables. 
GLFWwindow* window;
double resx = 640, resy = 480;

// this function will be called internally by GLFW whenever an error occur.
void error_callback(int error, const char* description);

int main()
{
    // initialize all the internal state of GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Error initializing GLFW\n");
        return -1;
    }

    // tell GLFW to call error_callback if an internal error ever occur at some point inside GLFW functions.
    glfwSetErrorCallback(error_callback);

    // create the window
    window = glfwCreateWindow(resx, resy, "Checkpoint 1: Creating a window.", NULL, NULL);

    // check if the opening of the window failed whatever reason and clean up
    if (!window) { 
        glfwTerminate();
        return -2;
    }

    // in principle we can have multiple windows, 
    // so we set the newly created on as "current"
    glfwMakeContextCurrent(window);

    // Enable v-sync for now, if possible
    glfwSwapInterval(1);

    // main loop
    while (!glfwWindowShouldClose(window)) {
        // listen for events (keyboard, mouse, etc.). ignored for now, but useful later
        glfwPollEvents();

        // swap buffers (replace the old image with a new one)
        // this won't have any visible effect until we add actual drawing
        glfwSwapBuffers(window);
    }

    // clean up
    glfwTerminate();
    
    return 0;
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s (%d)\n", description, error);
}