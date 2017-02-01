#include <stdio.h>
#include <math.h>

#include <GLFW/glfw3.h>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main()
{
    if (!glfwInit()) {
        printf("Error initializeing GLFW\n");
        return -1;
    }

    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Title", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -2;
    }
    glfwMakeContextCurrent(window); 
    glfwSwapInterval(1);


    double t1 = glfwGetTime();
    
    double avg_dt = 0.0;
    double avg_dt2 = 0.0;
    int avg_counter = 0;
    int num_samples = 60;

    while (!glfwWindowShouldClose(window)) {
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

            avg_dt = 0.0;
            avg_dt2 = 0.0;
            avg_counter = 0;
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    
    return 0;
}