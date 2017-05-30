# Checkpoint 2 -- Adding a timer

Starting from checkpoint 1, with explanatory comments removed for brevity's sake.

We'll want to look at performance later, so we might as well just add some timings functionality. 

Since the time to render a frame can vary quite significantly we'll want to calculate the average value over multiple frames. We will also calculate the standard deviation (variation in the frame time) and the standard error (variation in the average frame time) to get a grasp on how large the fluctuations are. 

We calculate the average *and* error without having to store all the values (praise mathematics). 

On my (quite beefy) graphics card, the GTX 1070, I get a frame time of 140 µs, which is equivalent to 7142 frames per second! Quite good!

Additionally we will refactor our initialization code and put it into its own function.

### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             19             18             66
-------------------------------------------------------------------------------
```

```diff
$ diff main.c ../checkpoint1/main.c
1,3c1
< #include <stdio.h>  // fprintf, sprintf, stderr
< #include <stdlib.h> // exit
< #include <math.h>   // sqrt
---
> #include <stdio.h> // fprintf, stderr
6a5,7
> // global variables
> // we'll use global variables, since the GLFW callback functions we will add
> // later will require access to these variables.
10,14c11,12
< void init();
< void calculate_frame_timings();
< void error_callback(int error, const char* description);
<
< int main()
---
> // this function is called internally by GLFW whenever an error occur.
> void error_callback(int error, const char* description)
16,36c14
<     // we move all the initialization step into a function for brevity
<     init();
<
<     void (*glClear)(GLbitfield) = (void (*)(GLbitfield))glfwGetProcAddress("glClear");
<     void (*glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = (void (*)(GLfloat, GLfloat, GLfloat, GLfloat))glfwGetProcAddress("glClearColor");
<
<     glClearColor(1.0, 0.0, 0.0, 1.0);
<     while (!glfwWindowShouldClose(window)) {
<         // we add a timings functions, to calcualate the time it takes
<         // to draw each frame. We will use this for benchmarking later
<         calculate_frame_timings();
<
<         glfwPollEvents();
<
<         glClear(GL_COLOR_BUFFER_BIT); // actually clear the window
<         glfwSwapBuffers(window);
<     }
<
<     glfwTerminate();
<
<     return 0;
---
>     fprintf(stderr, "Error: %s (%d)\n", description, error);
38,41c16
<
< // we'll put all future additional initialization of GLFW
< // and similar functionality in here
< void init()
---
> int main()
42a18
>     // initialize all the internal state of GLFW
45c21
<         exit(-1); // replace return with exit
---
>         return -1;
47a24
>     // tell GLFW to call error_callback if an internal error ever occur later
50,51c27,32
<     window = glfwCreateWindow(resx, resy, "Title", NULL, NULL);
<     if (!window) {
---
>     // create the window
>     window = glfwCreateWindow(resx, resy, "Checkpoint 1: Creating a window.", NULL, NULL);
>
>     // check if the opening of the window failed whatever reason
>     // and clean up
>     if (!window) {
53c34
<         exit(-2);
---
>         return -2;
55c36,41
<     glfwMakeContextCurrent(window);
---
>
>     // in principle we can have multiple windows,
>     // so we set the newly created on as "current"
>     glfwMakeContextCurrent(window);
>
>     // disable v-sync for now, if possible
57d42
< }
59,102c44,51
< // this function will calculate the average frame time and its variation.
< // about once every second it will display the average in the window title
< // and restart the counting.
< void calculate_frame_timings()
< {
<     // these variables (and their values) will stay alive after the function call is over
<     static double t1 = 0.0;
<     static double avg_dt = 0.0;
<     static double avg_dt2 = 0.0;
<     static int num_samples = 60; // start by counting the first 60 frames
<     static int counter = 0;
<
<     // get the current time and use that to calculate the time since the last
<     // time this function was called. this is the time between each frame
<     double t2 = glfwGetTime();
<     double dt = t2-t1;
<     t1 = t2;
<
<     // accumulate moments
<     avg_dt += dt;
<     avg_dt2 += dt*dt;
<     counter++;
<
<     if (counter == num_samples) {
<         // calculate average, standard deviation and standard error
<         avg_dt  /= num_samples;
<         avg_dt2 /= num_samples;
<         double std_dt = sqrt(avg_dt2 - avg_dt*avg_dt);
<         double ste_dt = std_dt/sqrt(num_samples);
<
<         // update the window title
<         char window_title_string[128];
<         sprintf(window_title_string, "dt: avg = %.3fms, std = %.3fms, ste = %.4fms. fps = %.1f", 1000.0*avg_dt, 1000.0*std_dt, 1000.0*ste_dt, 1.0/avg_dt);
<         glfwSetWindowTitle(window, window_title_string);
<
<         // we want to get a new average approximitely every second
<         // so we use the previous average frame time to determine the
<         // number of frames needed for one second to pass
<         num_samples = 1.0/avg_dt;
<
<         // reset values
<         avg_dt = 0.0;
<         avg_dt2 = 0.0;
<         counter = 0;
---
>     // main loop
>     while (!glfwWindowShouldClose(window)) {
>         // listen for events (keyboard, mouse, etc.). ignored for now, but useful later
>         glfwPollEvents();
>
>         // swap buffers (replace the old image with a new one)
>         // this won't have any visible effect until we add actual drawing
>         glfwSwapBuffers(window);
104d52
< }
106,108c54,57
< void error_callback(int error, const char* description)
< {
<     fprintf(stderr, "Error: %s (%d)\n", description, error);
---
>     // clean up
>     glfwTerminate();
>
>     return 0;
```