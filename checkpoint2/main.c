#include <stdio.h> // printf, fprintf, stderr, sprintf
#include <math.h>  // sqrt
#include <GLFW/glfw3.h>


GLFWwindow* window;

void error_callback(int error, const char* description);
void calculate_frame_timings();

int main()
{
    if (!glfwInit()) {
        printf("Error initializeing GLFW\n");
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    window = glfwCreateWindow(640, 480, "Title", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -2;
    }
    glfwMakeContextCurrent(window); 
    glfwSwapInterval(0);

    while (!glfwWindowShouldClose(window)) {
        calculate_frame_timings();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    return 0;
}

// This function is called internally by GLFW whenever an error occur.
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// This function will calculate the average frame time and its standard deviation
// About once every second it will display the average in the window title and 
// restart the counting. 
void calculate_frame_timings()
{
    static double t1 = 0.0;
    static double avg_dt = 0.0;
    static double avg_dt2 = 0.0;
    static int avg_counter = 0;
    static int num_samples = 60;

    double t2 = glfwGetTime();
    double dt = t2-t1;
    t1 = t2;

    avg_dt += dt;
    avg_dt2 += dt*dt;
    avg_counter++;

    if (avg_counter == num_samples) {
        avg_dt  /= num_samples;
        avg_dt2 /= num_samples;
        double std_dt = sqrt(avg_dt2 - avg_dt*avg_dt);
        double ste_dt = std_dt / sqrt(num_samples);

        char window_title_string[128];
        sprintf(window_title_string, "dt: avg = %.3fms, std = %.3fms, ste = %.4fms. fps = %.1f", 1000.0*avg_dt, 1000.0*std_dt, 1000.0*ste_dt, 1.0/avg_dt);
        glfwSetWindowTitle(window, window_title_string);

        num_samples = 1.0/avg_dt;
        
        avg_dt = 0.0;
        avg_dt2 = 0.0;
        avg_counter = 0;
    }
}