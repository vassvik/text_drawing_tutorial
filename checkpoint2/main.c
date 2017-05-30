#include <stdio.h>  // fprintf, sprintf, stderr
#include <stdlib.h> // exit
#include <math.h>   // sqrt

#include <GLFW/glfw3.h>

GLFWwindow* window;
double resx = 640, resy = 480;

void init();
void error_callback(int error, const char* description);

int main()
{
    // we move all the initialization step into a function for brevity
    if (!init()) {
        printf("Could not init. Exiting\n");
        return;
    }
    
    // function pointer notation might seem quite daunting at first...
    void (*glClear)(GLbitfield) = (void (*)(GLbitfield))glfwGetProcAddress("glClear");
    void (*glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = (void (*)(GLfloat, GLfloat, GLfloat, GLfloat))glfwGetProcAddress("glClearColor");

    glClearColor(1.0, 0.0, 0.0, 1.0);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT); // actually clear the window

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    
    return 0;
}

// we'll put all future additional initialization of GLFW 
// and similar functionality in here
int init()
{
    if (!glfwInit()) {
        fprintf(stderr, "Error initializing GLFW\n");
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    if (!window) {
    window = glfwCreateWindow(resx, resy, "Checkpoint 2: Using OpenGL functions.", NULL, NULL);
        glfwTerminate();
        return -2;
    }

    glfwMakeContextCurrent(window); 
    glfwSwapInterval(1);

    return 0; // success
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s (%d)\n", description, error);
}