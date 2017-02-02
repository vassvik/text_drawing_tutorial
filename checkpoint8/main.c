#include <stdio.h> // printf, fprintf, stderr, sprintf
#include <stdlib.h> // malloc, free
#include <math.h>  // sqrt

#include <glad/glad.h>
#include <glad/glad.c>

#include <GLFW/glfw3.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

GLFWwindow* window;
GLuint program;

GLuint vertex_array_object;

GLuint vertex_buffer_object_pos;
GLuint vertex_buffer_object_uv;

GLuint texture;


int num_vertices;

void error_callback(int error, const char* description);
void calculate_frame_timings();
void init();
void setup();
void draw();

char *read_entire_file(const char *filename);
void compile_shader(const char * file_path, GLuint shader_ID);
GLuint load_shaders(const char * vertex_file_path,const char * fragment_file_path);



int main()
{
    init();
    setup();

    while (!glfwWindowShouldClose(window)) {
        calculate_frame_timings();

        glfwPollEvents();

        draw();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    
    return 0;
}

void init()
{
    if (!glfwInit()) {
        printf("Error initializeing GLFW\n");
        exit(-1);
    }

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // matches 330 in the shaders
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(640, 480, "Title", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(-2);
    }
    glfwMakeContextCurrent(window); 
    glfwSwapInterval(0);

    if(!gladLoadGL()) {
        printf("Something went wrong loading GLAD!\n");
        glfwTerminate();
        exit(-3);
    }

    glClearColor(3.0/255, 72/255.0, 133/255.0, 1.0f);
}

void setup()
{
    program = load_shaders("vertex_shader.vs", "fragment_shader.fs");

    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);



    int font_bitmap_width = 512;
    int font_bitmap_height = 256;
    int font_size = 48.0;

    // Read the data from file
    int ttf_size_max = 1e6;
    unsigned char *ttf_buffer = malloc(ttf_size_max); // sufficient size for consola.ttf

    FILE *fp = fopen("../consola.ttf", "rb");
    fread(ttf_buffer, 1, ttf_size_max, fp);
    fclose(fp);
    
    
    // Pack and create bitmap
    unsigned char *bitmap = malloc(font_bitmap_height*font_bitmap_width);
    stbtt_pack_context pc;
    stbtt_packedchar cdata[96]; 

    stbtt_PackBegin(&pc, bitmap, font_bitmap_width, font_bitmap_height, 0, 1, NULL);   
    stbtt_PackSetOversampling(&pc, 1, 1);
    stbtt_PackFontRange(&pc, ttf_buffer, 0, font_size, 32, 96, cdata);
    stbtt_PackEnd(&pc);


    free(ttf_buffer);

    float x0_y = cdata['y'-32].x0;
    float y0_y = cdata['y'-32].y0;

    float x1_y = cdata['y'-32].x1;
    float y1_y = cdata['y'-32].y1;

    float xoff_y = cdata['y'-32].xoff;
    float yoff_y = cdata['y'-32].yoff;
    float xoff2_y = cdata['y'-32].xoff2;
    float yoff2_y = cdata['y'-32].yoff2;

    float dx_y = (x1_y - x0_y);
    float dy_y = (y1_y - y0_y);

    float xadvance_y = cdata['y'-32].xadvance;

    float x0_G = cdata['G'-32].x0;
    float y0_G = cdata['G'-32].y0;

    float x1_G = cdata['G'-32].x1;
    float y1_G = cdata['G'-32].y1;

    float xoff_G = cdata['G'-32].xoff;
    float yoff_G = cdata['G'-32].yoff;

    float xoff2_G = cdata['G'-32].xoff2;
    float yoff2_G = cdata['G'-32].yoff2;

    float dx_G = (xoff2_G - xoff_G);
    float dy_G = (yoff2_G - yoff_G);


    num_vertices = 12;
    GLfloat vertex_buffer_pos_data[] = {
       -1.0 + (xoff_y + 0*dx_y)/font_size,  0.0 + (-yoff2_y + 0*dy_y)/font_size,
       -1.0 + (xoff_y + 1*dx_y)/font_size,  0.0 + (-yoff2_y + 0*dy_y)/font_size,
       -1.0 + (xoff_y + 0*dx_y)/font_size,  0.0 + (-yoff2_y + 1*dy_y)/font_size,

       -1.0 + (xoff_y + 0*dx_y)/font_size,  0.0 + (-yoff2_y + 1*dy_y)/font_size,
       -1.0 + (xoff_y + 1*dx_y)/font_size,  0.0 + (-yoff2_y + 0*dy_y)/font_size,
       -1.0 + (xoff_y + 1*dx_y)/font_size,  0.0 + (-yoff2_y + 1*dy_y)/font_size,

       -1.0 + (xoff_y + xadvance_y + 0*dx_G)/font_size,  0.0 + (-yoff2_G + 0*dy_G)/font_size,
       -1.0 + (xoff_y + xadvance_y + 1*dx_G)/font_size,  0.0 + (-yoff2_G + 0*dy_G)/font_size,
       -1.0 + (xoff_y + xadvance_y + 0*dx_G)/font_size,  0.0 + (-yoff2_G + 1*dy_G)/font_size,

       -1.0 + (xoff_y + xadvance_y + 0*dx_G)/font_size,  0.0 + (-yoff2_G + 1*dy_G)/font_size,
       -1.0 + (xoff_y + xadvance_y + 1*dx_G)/font_size,  0.0 + (-yoff2_G + 0*dy_G)/font_size,
       -1.0 + (xoff_y + xadvance_y + 1*dx_G)/font_size,  0.0 + (-yoff2_G + 1*dy_G)/font_size,

    };


    glGenBuffers(1, &vertex_buffer_object_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*num_vertices, vertex_buffer_pos_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);




    // @TODO: PADDING?


    GLfloat vertex_buffer_uv_data[] = {
        (cdata['y'-32].x0+0)/(double)font_bitmap_width, (cdata['y'-32].y1+1)/(double)font_bitmap_height,
        (cdata['y'-32].x1+1)/(double)font_bitmap_width, (cdata['y'-32].y1+1)/(double)font_bitmap_height,
        (cdata['y'-32].x0+0)/(double)font_bitmap_width, (cdata['y'-32].y0+0)/(double)font_bitmap_height,

        (cdata['y'-32].x0+0)/(double)font_bitmap_width, (cdata['y'-32].y0+0)/(double)font_bitmap_height,
        (cdata['y'-32].x1+1)/(double)font_bitmap_width, (cdata['y'-32].y1+1)/(double)font_bitmap_height,
        (cdata['y'-32].x1+1)/(double)font_bitmap_width, (cdata['y'-32].y0+0)/(double)font_bitmap_height,
        
        
        (cdata['G'-32].x0+0)/(double)font_bitmap_width, (cdata['G'-32].y1+1)/(double)font_bitmap_height,
        (cdata['G'-32].x1+1)/(double)font_bitmap_width, (cdata['G'-32].y1+1)/(double)font_bitmap_height,
        (cdata['G'-32].x0+0)/(double)font_bitmap_width, (cdata['G'-32].y0+0)/(double)font_bitmap_height,

        (cdata['G'-32].x0+0)/(double)font_bitmap_width, (cdata['G'-32].y0+0)/(double)font_bitmap_height,
        (cdata['G'-32].x1+1)/(double)font_bitmap_width, (cdata['G'-32].y1+1)/(double)font_bitmap_height,
        (cdata['G'-32].x1+1)/(double)font_bitmap_width, (cdata['G'-32].y0+0)/(double)font_bitmap_height,
        
    };

    glGenBuffers(1, &vertex_buffer_object_uv);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_uv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*num_vertices, vertex_buffer_uv_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);







    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font_bitmap_width, font_bitmap_height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

    free(bitmap);

}

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(program);

    glBindVertexArray(vertex_array_object);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(program, "texture_sampler"), 0);

    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
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

// This function is called internally by GLFW whenever an error occur.
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// Shader utility functions. 
// NOTE: No error checking whatsoever
char *read_entire_file(const char *filename) {
    FILE *f = fopen(filename, "rb");

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = (char*)malloc(fsize + 1);
    fread(string, fsize, 1, f);
    string[fsize] = '\0';
    fclose(f);

    return string;
}

void compile_shader(const char * file_path, GLuint shader_ID) {

    char *shader_code   = read_entire_file(file_path);

    // Compile Shader
    printf("Compiling shader : %s\n", file_path);
    glShaderSource(shader_ID, 1, (const char**)&shader_code , NULL);
    glCompileShader(shader_ID);

    // Check Shader
    GLint result;
    glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &result);

    if ( result == GL_FALSE ){
        GLint info_log_length;
        glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);

        char shader_error_message[info_log_length+1];
        glGetShaderInfoLog(shader_ID, info_log_length, NULL, shader_error_message);
        printf("%s", shader_error_message);
    }

    free(shader_code);
}

GLuint load_shaders(const char * vertex_file_path,const char * fragment_file_path){
    GLuint vertex_shader_ID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);

    compile_shader(vertex_file_path, vertex_shader_ID);
    compile_shader(fragment_file_path, fragment_shader_ID);

    GLuint program_ID = glCreateProgram();
    glAttachShader(program_ID, vertex_shader_ID);
    glAttachShader(program_ID, fragment_shader_ID);
    glLinkProgram(program_ID);

    GLint result;
    glGetProgramiv(program_ID, GL_LINK_STATUS, &result);

    if ( result == GL_FALSE ){
        GLint info_log_length;
        glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar program_error_message[info_log_length+1];
        glGetProgramInfoLog(program_ID, info_log_length, NULL, program_error_message);
        printf("%s\n", program_error_message);
    }

    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);

    return program_ID;
}


