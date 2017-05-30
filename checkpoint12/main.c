#include <stdio.h>  // printf, fprintf, stderr, sprintf, FILE, fopen, fclose, fread
#include <stdlib.h> // malloc, free
#include <math.h>   // sqrt

#include <glad/glad.h>
#include <glad/glad.c>

#include <GLFW/glfw3.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

GLFWwindow* window;
double resx = 1600, resy = 900;

GLuint program;
GLuint vertex_array_object;
GLuint vertex_buffer_quad;
GLuint vertex_buffer_instances;

GLuint texture_bitmap;
GLuint texture_metadata;
GLuint texture_colors;

int num_vertices;
int num_instances;
int max_instances;
int num_glyphs;
int num_colors;

double last_frame_time;

stbtt_packedchar cdata[96]; 

float ascent, descent, linegap, linedist;

void init();
void setup();
void draw(int count);
void calculate_frame_timings();
void error_callback(int error, const char *description);
int update_instances(char *str);

char *read_entire_file(const char *filename);
int compile_shader(const char *file_path, GLuint shader_ID);
GLuint load_shaders(const char *vertex_file_path, const char *fragment_file_path);


unsigned int seed = 123;
double rng() {
    seed *= 16807;
    return seed / (double)0x100000000ULL;
}

int main()
{
    init();
    setup();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        calculate_frame_timings();

        char str[max_instances];
        sprintf(str, "last frame time = %.3fms\n", last_frame_time*1000.0);
        
        // add a line of all glyphs in order
        int start = strlen(str);
        for (int i = 0; i < 'A'-32; i++) {
            str[start++] = 32+i;
        }
        str[start++] = '\n';
        for (int i = 'A'-32; i < 'a'-32; i++) {
            str[start++] = 32+i;
        }
        str[start++] = '\n';
        for (int i = 'a'-32; i < 95; i++) {
            str[start++] = 32+i;
        }
        str[start++] = '\n';

        seed = 123;

        // add random glyphs
        for (int j = 0; j < 7; j++) {
            for (int i = 0; i < 40; i++) {
                str[start++] = 32 + ((int)(95*rng() + 6.0*glfwGetTime()*(0.5 + rng())) % 95);
            }
            str[start++] = '\n';
        }
        str[start++] = '\0';

        int count = update_instances(str);
        draw(count);

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

    // @NOTE: you don't want multisampling when doing the fonts, as you might get artifacts if you scale down too much
    // due to how multisampling works. 
    //glfwWindowHint(GLFW_SAMPLES, 4); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(resx, resy, "Title", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(-2);
    }
    glfwMakeContextCurrent(window); 
    glfwSwapInterval(0);

    if(!gladLoadGL()) {
        fprintf(stderr, "Something went wrong while loading GLAD!\n");
        glfwTerminate();
        exit(-3);
    }
}

void setup()
{
    // bgColor
    glClearColor(3.0/255, 72/255.0, 133/255.0, 1.0f);

    // shaders
    program = load_shaders("vertex_shader.vs", "fragment_shader.fs");
        if (program == 0) {
        fprintf(stderr, "Could not load shaders. Exiting\n");
        glfwTerminate();
        exit(-4);
    }

    // VAO
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);

    // quad VBO
    num_vertices = 6;
    GLfloat vertex_buffer_pos_data[] = {
       0.0, 0.0,
       1.0, 0.0,
       0.0, 1.0,
       0.0, 1.0,
       1.0, 0.0,
       1.0, 1.0
    };

    glGenBuffers(1, &vertex_buffer_quad);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_pos_data), vertex_buffer_pos_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(0, 0); // not instanced: resets every instance

    // get font data
    int font_bitmap_width = 1024;
    int font_bitmap_height = 1024;
    int font_size = 64.0;

    int ttf_size_max = 1e6;
    unsigned char *ttf_buffer = malloc(ttf_size_max); // sufficient size for consola.ttf

    FILE *fp = fopen("../consola.ttf", "rb");
    fread(ttf_buffer, 1, ttf_size_max, fp);
    fclose(fp);
    
    unsigned char *bitmap = malloc(font_bitmap_height*font_bitmap_width);
    stbtt_pack_context pc;

    stbtt_PackBegin(&pc, bitmap, font_bitmap_width, font_bitmap_height, 0, 1, NULL);   
    stbtt_PackSetOversampling(&pc, 1, 1);
    stbtt_PackFontRange(&pc, ttf_buffer, 0, font_size, 32, 96, cdata);
    stbtt_PackEnd(&pc);

    int max_y = 0;
    for (int i = 0; i < 95; i++) {

        if (cdata[i].y1 > max_y) {
            max_y = cdata[i].y1;
        }
    }
    font_bitmap_height = max_y;


    // calculate vertical font metrics
    stbtt_fontinfo info;
    stbtt_InitFont(&info, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));

    float s = stbtt_ScaleForPixelHeight(&info, font_size);
    int a, d, l;
    stbtt_GetFontVMetrics(&info, &a, &d, &l);
    
    ascent = a*s;
    descent = d*s;
    linegap = l*s;
    linedist = ascent - descent + linegap;

    free(ttf_buffer);

    // instance VBO
    max_instances = 10000;
    num_instances = 0;

    glGenBuffers(1, &vertex_buffer_instances);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_instances);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*max_instances, NULL, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(1, 1); // instanced: advanced every instance

    // bitmap texture, texture unit 0
    glGenTextures(1, &texture_bitmap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_bitmap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font_bitmap_width, font_bitmap_height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

    free(bitmap);

    // metadata texture, texture unit 1
    // we fill this with (x0, y0, x1-x0, y1-x0) and (xoff, yoff, xoff2, yoff2) that we get from stb_truetype.h
    // in two rows. (x0, y0, x1-x0, y1-x0) om the first row, and (xoff, yoff, xoff2, yoff2) in the second row
    // these will be normalized
    num_glyphs = 96;

    GLfloat metadata[num_glyphs*4*2];
    for (int i = 0; i < num_glyphs; i++) {
        metadata[4*i+0] = cdata[i].x0/(double)font_bitmap_width;
        metadata[4*i+1] = cdata[i].y0/(double)font_bitmap_height;
        metadata[4*i+2] = (cdata[i].x1-cdata[i].x0)/(double)font_bitmap_width;
        metadata[4*i+3] = (cdata[i].y1-cdata[i].y0)/(double)font_bitmap_height;

        metadata[4*num_glyphs + 4*i+0] = cdata[i].xoff/(double)font_bitmap_width;
        metadata[4*num_glyphs + 4*i+1] = cdata[i].yoff/(double)font_bitmap_height;
        metadata[4*num_glyphs + 4*i+2] = (cdata[i].xoff2-cdata[i].xoff)/(double)font_bitmap_width;
        metadata[4*num_glyphs + 4*i+3] = (cdata[i].yoff2-cdata[i].yoff)/(double)font_bitmap_height;
    }

    glGenTextures(1, &texture_metadata);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture_metadata);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, num_glyphs, 2, 0, GL_RGBA, GL_FLOAT, metadata);

    // color texture, texture unit 2
    num_colors = 16;
    unsigned char colors[] = {
        255, 152,   0, // orange
        156,  39, 176, // purple
          3, 169, 244, // light blue
        139, 195,  74, // light green

        255,  87,  34, // deep orange
        103,  58, 183, // deep purple
          0, 188, 212, // cyan
        205, 220,  57, // lime

        244,  67,  54, // red
         63,  81, 181, // indigo
          0, 150, 137, // teal
        255, 235,  59, // yellow
        
        233,  30,  99, // pink
         33, 150, 243, // blue
         76, 175,  80, // green
        255, 193,   7, // amber
    };

    glGenTextures(1, &texture_colors);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, texture_colors);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, num_colors, 0, GL_RGB, GL_UNSIGNED_BYTE, colors);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);


    // write constant uniforms to shader
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "sampler_bitmap"), 0);
    glUniform1i(glGetUniformLocation(program, "sampler_metadata"), 1);
    glUniform1i(glGetUniformLocation(program, "sampler_colors"), 2);
    
    glUniform2f(glGetUniformLocation(program, "resolution_bitmap"), font_bitmap_width, font_bitmap_height);
    glUniform1f(glGetUniformLocation(program, "num_glyphs"), num_glyphs);
    glUniform1f(glGetUniformLocation(program, "num_colors"), num_colors);
    glUniform1f(glGetUniformLocation(program, "offset_firstline"), linedist-linegap);
}

int update_instances(char *str)
{
    int len = strlen(str);

    if (len > max_instances) {
        printf("Error: string too long. Returning\n");
        return 0;
    } 

    GLfloat vertex_buffer_instances_data[max_instances*4];

    float X = 0.0;
    float Y = 0.0;

    int ctr = 0;
    for (char *c = str; *c; c++) {

        if ((*c) == '\n') {
            X = 0.0;
            Y -= linedist;
            continue;
        }

        int code_base = (*c)-32; // first glyph is ' ', i.e. ascii code 32

        vertex_buffer_instances_data[4*ctr+0] = X;
        vertex_buffer_instances_data[4*ctr+1] = Y;
        vertex_buffer_instances_data[4*ctr+2] = code_base;
        vertex_buffer_instances_data[4*ctr+3] = ctr % num_colors;

        X += cdata[code_base].xadvance;
        ctr++;
    }    

    glBindVertexArray(vertex_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_instances);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*4*ctr, vertex_buffer_instances_data);

    return ctr;
}

void draw(int count)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(program);

    // uniforms that might change
    glUniform1f(glGetUniformLocation(program, "scale_factor"), 1.0);
    glUniform2f(glGetUniformLocation(program, "string_offset"), 0.0, 0.0);
    glUniform2f(glGetUniformLocation(program, "resolution_window"), resx, resy);

    glBindVertexArray(vertex_array_object);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_bitmap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_metadata);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, texture_colors);

    glDrawArraysInstanced(GL_TRIANGLES, 0, num_vertices, count);
}


void calculate_frame_timings()
{
    static double t1 = 0.0;
    static double avg_dt = 0.0;
    static double avg_dt2 = 0.0;
    static int num_samples = 60;
    static int counter = 0;

    double t2 = glfwGetTime();
    double dt = t2-t1;
    t1 = t2;
    last_frame_time = dt;

    avg_dt += dt;
    avg_dt2 += dt*dt;
    counter++;

    if (counter == num_samples) {
        avg_dt  /= num_samples;
        avg_dt2 /= num_samples;
        double std_dt = sqrt(avg_dt2 - avg_dt*avg_dt);
        double ste_dt = std_dt/sqrt(num_samples);

        char window_title_string[128];
        sprintf(window_title_string, "dt: avg = %.3fms, std = %.3fms, ste = %.4fms. fps = %.1f", 1000.0*avg_dt, 1000.0*std_dt, 1000.0*ste_dt, 1.0/avg_dt);
        glfwSetWindowTitle(window, window_title_string);

        num_samples = 1.0/avg_dt;
        
        avg_dt = 0.0;
        avg_dt2 = 0.0;
        counter = 0;
    }
}

void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s (%d)\n", description, error);
}

char *read_entire_file(const char *filename) {
    FILE *f = fopen(filename, "rb");

    if (f == NULL) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = (char*)malloc(fsize + 1);
    fread(string, fsize, 1, f);
    string[fsize] = '\0';
    fclose(f);

    return string;
}

int compile_shader(const char *file_path, GLuint shader_ID) {
    char *shader_code = read_entire_file(file_path);
    if (shader_code == NULL) {
        fprintf(stderr, "Error: Could not read shader file: \"%s\"\n", file_path);
        return -1;
    }
    printf("Compiling shader : %s\n", file_path);
    glShaderSource(shader_ID, 1, (const char**)&shader_code , NULL);
    glCompileShader(shader_ID);

    GLint result;
    glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &result);

    if ( result == GL_FALSE ){
        GLint info_log_length;
        glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);

        char shader_error_message[info_log_length+1];
        glGetShaderInfoLog(shader_ID, info_log_length, NULL, shader_error_message);
        fprintf(stderr, "Error while compiling shader \"%s\":\n%s", file_path, shader_error_message);

        free(shader_code);
        return -2;
    }

    free(shader_code);

    return 0;
}

GLuint load_shaders(const char *vertex_file_path,const char *fragment_file_path){
    GLuint vertex_shader_ID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);

    int err1 = compile_shader(vertex_file_path, vertex_shader_ID);
    int err2 = compile_shader(fragment_file_path, fragment_shader_ID);

    if (err1 || err2) {
        glDeleteShader(vertex_shader_ID);
        glDeleteShader(fragment_shader_ID);
        return 0;
    }

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
        printf("Error while linking program:\n%s\n", program_error_message);
        
        glDeleteShader(vertex_shader_ID);
        glDeleteShader(fragment_shader_ID);
        return 0;
    }

    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);

    return program_ID;
}