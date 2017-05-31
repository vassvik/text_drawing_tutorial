#include <stdio.h>  // fprintf, sprintf, stderr
#include <stdlib.h> // exit
#include <math.h>   // sqrt

#include <GLFW/glfw3.h>

// global variables
// we'll use global variables, since the GLFW callback functions we will add
// later will require access to these variables. 
GLFWwindow* window;
double resx = 640, resy = 480;

// this function will be called internally by GLFW whenever an error occur.
void error_callback(int error, const char* description);

// we move all the initialization step into a function for brevity
// we'll put all future additional initialization of GLFW and similar functionality in here
int init();

int main()
{
    if (!init()) {
        printf("Could not init. Exiting\n");
        return -1;
    }
    
    // grab the relevant function pointers
    // function pointer notation might seem quite daunting at first...
    // Check: http://fuckingfunctionpointers.com
    void (*glClear)(GLbitfield) = (void (*)(GLbitfield))glfwGetProcAddress("glClear");
    void (*glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = (void (*)(GLfloat, GLfloat, GLfloat, GLfloat))glfwGetProcAddress("glClearColor");

    // set the background color to red
    glClearColor(1.0, 0.0, 0.0, 1.0);

    // main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT); // actually clear the window

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    
    return 0;
}

int init()
{
    // initialize all the internal state of GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Error initializing GLFW\n");
        return 0;
    }

    // tell GLFW to call error_callback if an internal error ever occur at some point inside GLFW functions.
    glfwSetErrorCallback(error_callback);

    // create the window
    window = glfwCreateWindow(resx, resy, "Checkpoint 2: Using OpenGL functions.", NULL, NULL);

    // check if the opening of the window failed whatever reason and clean up
    if (!window) {
        glfwTerminate();
        return 0;
    }

    // in principle we can have multiple windows, 
    // so we set the newly created on as "current"
    glfwMakeContextCurrent(window); 

    // Enable v-sync for now, if possible
    glfwSwapInterval(1);
    return 1; // success
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s (%d)\n", description, error);
}