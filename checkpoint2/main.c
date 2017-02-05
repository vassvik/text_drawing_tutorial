#include <stdio.h> // fprintf, sprintf, stderr
#include <math.h>  // sqrt
#include <GLFW/glfw3.h>

GLFWwindow* window;
double resx = 640, resy = 480;

void error_callback(int error, const char* description);
void calculate_frame_timings();

int main()
{
    if (!glfwInit()) {
        fprintf(stderr, "Error initializing GLFW\n");
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    window = glfwCreateWindow(resx, resy, "Title", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -2;
    }
    glfwMakeContextCurrent(window); 
    glfwSwapInterval(0);

    while (!glfwWindowShouldClose(window)) {
        // we add a timings functions, to calcualate the time it takes 
        // to draw each frame. We will use this for benchmarking later
        calculate_frame_timings();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    
    return 0;
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// this function will calculate the average frame time and its variation.
// about once every second it will display the average in the window title 
// and restart the counting. 
void calculate_frame_timings()
{
    // these variables (and their values) will stay alive after the function call is over
    static double t1 = 0.0;
    static double avg_dt = 0.0;
    static double avg_dt2 = 0.0;
    static int num_samples = 60; // start by counting the first 60 frames
    static int counter = 0;

    // get the current time and use that to calculate the time since the last
    // time this function was called. this is the time between each frame
    double t2 = glfwGetTime();
    double dt = t2-t1;
    t1 = t2;

    // accumulate moments
    avg_dt += dt;
    avg_dt2 += dt*dt;
    counter++;

    if (counter == num_samples) {
        // calculate average, standard deviation and standard error
        avg_dt  /= num_samples;
        avg_dt2 /= num_samples;
        double std_dt = sqrt(avg_dt2 - avg_dt*avg_dt);
        double ste_dt = std_dt/sqrt(num_samples);

        // update the window title
        char window_title_string[128];
        sprintf(window_title_string, "dt: avg = %.3fms, std = %.3fms, ste = %.4fms. fps = %.1f", 1000.0*avg_dt, 1000.0*std_dt, 1000.0*ste_dt, 1.0/avg_dt);
        glfwSetWindowTitle(window, window_title_string);

        // we want to get a new average approximitely every second
        // so we use the previous average frame time to determine the
        // number of frames needed for one second to pass
        num_samples = 1.0/avg_dt;
        
        // reset values
        avg_dt = 0.0;
        avg_dt2 = 0.0;
        counter = 0;
    }
}