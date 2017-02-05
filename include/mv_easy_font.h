#ifndef MV_EASY_FONT_H
#define MV_EASY_FONT_H

#ifdef __cplusplus
extern "C" {
#endif
// utility functions to load shaders
char *mv_ef_read_entire_file(const char *filename);
GLuint mv_ef_load_shaders(const char *vs_path, const char *fs_path);

#include "stb_truetype.h"

#define MAX_STRING_LEN 40000 // more glyphs than any reasonable person would show on the screen at once. you can only fit 20736 10x10 rects in a 1920x1080 window
#define NUM_GLYPHS 96

//
// Struct containing font info and OpenGL variables for vbos, vao and textures
//
typedef struct {
    int initialized; // to be able to initialize from the first call to mv_ef_draw()

    // character info
    // filled up by stb_truetype.h
    stbtt_packedchar cdata[96]; 

    // font info and data
    int height;      // bitmap height
    int width;       // bitmap width
    float font_size; // font size in pixels

    // displacement info
    float ascent;   // max distance above baseline for all glyphs
    float descent;  // max distance below baseline for all glyphs
    float linegap;  // distance betwen ascent of next line and descent of current line
    float linedist; // distance between the baseline of two lines

    // opengl stuff
    GLuint vao; 
    GLuint program;
    
    // font bitmap texture
    // generated using stb_truetype.h
    GLuint texture_fontdata; 

    // metadata texture. 
    // first row contains information on which parts of the bitmap correspond to a glyph. 
    // the second row contain information about the relative displacement of the glyph relative to the cursor position
    GLuint texture_metadata; 

    // vbos
    GLuint vbo_quad;      // vec2: simply just a regular [0,1]x[0,1] quad
    GLuint vbo_instances; // vec4: (char_pos_x, char_pos_y, char_index, color_index)
} mv_ef_font;

void mv_ef_init();
void mv_ef_draw(char *str, char *col, float offset[2], float size, float res[2]);
void mv_ef_string_dimensions(char *str, int *width, int *height, int font_size);
void mv_ef_set_colors(float *colors);
float *mv_ef_get_colors(int *num_colors);
mv_ef_font *mv_ef_get_font();

#ifdef __cplusplus
}
#endif

#endif // MV_EASY_FONT_H


#ifdef MV_EASY_FONT_IMPLEMENTATION


// include stb_truetype.h implementation. make sure to check if it hasn't been implemented before
#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#endif // STB_TRUETYPE_IMPLEMENTATION

// include stb_rect_pack.h implementation if requested. make sure to check if it hasn't been implemented before
#ifdef STB_INCLUDE_STB_RECT_PACK_H

#ifndef STB_RECT_PACK_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"
#endif // STB_RECT_PACK_IMPLEMENTATION

#endif // STB_INCLUDE_STB_RECT_PACK_H

// include stb_image_write.h implementation if saving bitmap is requested. make sure to check if it hasn't been implemented before
#ifdef MV_EF_OUTPUT_BITMAP_TO_FILE

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif // STB_IMAGE_WRITE_IMPLEMENTATION

#endif // MV_EF_OUTPUT_BITMAP_TO_FILE


// @TODO: Add larger color palette. currently only support 9 colors
float mv_ef_colors[9*3] = {
    248/255.0, 248/255.0, 242/255.0, // foreground color
    249/255.0,  38/255.0, 114/255.0, // operator
    174/255.0, 129/255.0, 255/255.0, // numeric
    102/255.0, 217/255.0, 239/255.0, // function
    249/255.0,  38/255.0, 114/255.0, // keyword
    117/255.0, 113/255.0,  94/255.0, // comment
    102/255.0, 217/255.0, 239/255.0, // type
     73/255.0,  72/255.0,  62/255.0, // background color
     39/255.0,  40/255.0,  34/255.0  // clear color
};

// private global variable that all the functions use. return with 
mv_ef_font font = {0};

//
// Return the whole font struct, in case the user want access to individual data in it
//
mv_ef_font *mv_ef_get_font()
{
    return &font;
}

float *mv_ef_get_colors(int *num_colors)
{
    *num_colors = sizeof(mv_ef_colors)/sizeof(float)/3;
    return mv_ef_colors;
}

void mv_ef_set_colors(float *colors)
{
    for (int i = 0; i < 9*3; i++)
        mv_ef_colors[i] = colors[i];

    glUseProgram(font.program);
    glUniform3fv(glGetUniformLocation(font.program, "colors"), 9, mv_ef_colors);
}

//
// Calculates the size of a string, in the pixel size specified. 
// Note: Stray newlines are also counted
//
void mv_ef_string_dimensions(char *str, int *width, int *height, int font_size)
{
    int X = 0;
    int Y = 0;

    int W = 0;
    char *ptr = str;
    while (*ptr) {
        if (*ptr == '\n') {
            if (X > W)
                W = X;
            X = 0;
            Y++;
        } else {
           X += (font.cdata[*ptr-32].xadvance)*font_size/font.font_size;
        }
        ptr++;
    }

    // if it ended on a line with no newline
    if (X != 0) {
        Y++;
        if (W == 0)
            W = X;
    } 

    *width = W;
    *height = Y*(font.linedist)*font_size/font.font_size;
}

// 
// Reads and compiles the shaders 
// 
// Calls stb_truetype.h routines to read and parse a .ttf file, 
// creates a bitmap that is uploaded to the gpu using opengl
//
// calculates and saves a bunch of useful variables and put them in the global font variable
// 
void mv_ef_init()
{
    font.initialized = 1;

    // @TODO: Should probably hard code this eventually, as the shaders are finalized? 2 less files
    font.program = mv_ef_load_shaders( "vertex_shader_text.vs", "fragment_shader_text.fs" );

    // load .ttf into a bitmap using stb_truetype.h
    font.width = 512;
    font.height = 256;
    font.font_size = 48.0;

    // Read the data from file
    int ttf_size_max = 1e6;
    unsigned char *ttf_buffer = malloc(ttf_size_max); // sufficient size for consola.ttf

    FILE *fp = fopen("consola.ttf", "rb");
    fread(ttf_buffer, 1, ttf_size_max, fp);
    fclose(fp);
    

    // Pack and create bitmap
    unsigned char *bitmap = malloc(font.height*font.width);
    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, bitmap, font.width, font.height, 0, 1, NULL);   
    stbtt_PackSetOversampling(&pc, 1, 1);
    stbtt_PackFontRange(&pc, ttf_buffer, 0, font.font_size, 32, 96, font.cdata);
    stbtt_PackEnd(&pc);

#ifdef MV_EF_OUTPUT_BITMAP_TO_FILE
    stbi_write_png("font.png", font.width, font.height, 1, bitmap, 0);
#endif

    // calculate vertical font metrics
    stbtt_fontinfo info;
    stbtt_InitFont(&info, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));

    float s = stbtt_ScaleForPixelHeight(&info, 48.0);
    int a, d, l;
    stbtt_GetFontVMetrics(&info, &a, &d, &l);
    printf("ascent %f  descent %f  linegap %f  yadvance %f\n", a*s, d*s, l*s, s*(a-d+l));

    font.ascent = a*s;
    font.descent = d*s;
    font.linegap = l*s;
    font.linedist = font.ascent - font.descent + font.linegap;

    free(ttf_buffer);

    /*
    // output char metrics per char
    int max_y1 = 0; // for truncating packed texture if nescessary
    for (int i = 0; i < 96; i++) {
        printf("%3d %2c: (%3u, %3u, %3u, %3u), %+6.2f, %+6.2f, %+6.2f, %+6.2f, %f\n", i, i+32, 
                                                                                      font.cdata[i].x0,    font.cdata[i].y0, 
                                                                                      font.cdata[i].x1,    font.cdata[i].y1,
                                                                                      font.cdata[i].xoff,  font.cdata[i].yoff, 
                                                                                      font.cdata[i].xoff2, font.cdata[i].yoff2,
                                                                                      font.cdata[i].xadvance);
        if (font.cdata[i].y1 > max_y1)
            max_y1 = font.cdata[i].y1;
    }
    */


    // vaos
    glGenVertexArrays(1, &font.vao);
    glBindVertexArray(font.vao);


    // quad vbo setup, used for glyph vertex positions, 
    // just uv coordinates that will be stretched accordingly by the glyphs width and height
    float v[] = {0.0, 0.0, 
                 1.0, 0.0, 
                 0.0, 1.0,
                 0.0, 1.0,
                 1.0, 0.0,
                 1.0, 1.0};

    glGenBuffers(1, &font.vbo_quad);
    glBindBuffer(GL_ARRAY_BUFFER, font.vbo_quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(void*)0);
    glVertexAttribDivisor(0, 0);

    // instance vbo setup.
    // for glyph positions, glyph index and color index
    glGenBuffers(1, &font.vbo_instances);
    glBindBuffer(GL_ARRAY_BUFFER, font.vbo_instances);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4*MAX_STRING_LEN, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,0,(void*)0);
    glVertexAttribDivisor(1, 1);

    // setup and upload font bitmap texture
    glGenTextures(1, &font.texture_fontdata);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.texture_fontdata);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font.width, font.height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    free(bitmap);

    // setup and upload font metadata texture
    // used for lookup in the bitmap texture    
    glGenTextures(1, &font.texture_metadata);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, font.texture_metadata);

    float *texture_metadata = (float*)malloc(sizeof(float)*8*NUM_GLYPHS);
    
    for (int i = 0; i < NUM_GLYPHS; i++) {
        int k1 = 0*NUM_GLYPHS + i;
        int k2 = 1*NUM_GLYPHS + i;
        texture_metadata[4*k1+0] = font.cdata[i].x0/(double)font.width;
        texture_metadata[4*k1+1] = font.cdata[i].y0/(double)font.height;
        texture_metadata[4*k1+2] = (font.cdata[i].x1-font.cdata[i].x0)/(double)font.width;
        texture_metadata[4*k1+3] = (font.cdata[i].y1-font.cdata[i].y0)/(double)font.height;

        texture_metadata[4*k2+0] = font.cdata[i].xoff/(double)font.width;
        texture_metadata[4*k2+1] = font.cdata[i].yoff/(double)font.height;
        texture_metadata[4*k2+2] = font.cdata[i].xoff2/(double)font.width;
        texture_metadata[4*k2+3] = font.cdata[i].yoff2/(double)font.height;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, NUM_GLYPHS, 2, 0, GL_RGBA, GL_FLOAT, texture_metadata);

    free(texture_metadata);


    // upload constant uniforms
    glUseProgram(font.program);
    glUniform1i(glGetUniformLocation(font.program, "sampler_font"), 0);
    glUniform1i(glGetUniformLocation(font.program, "sampler_meta"), 1);

    glUniform2f(glGetUniformLocation(font.program, "res_bitmap"), font.width, font.height);
    glUniform2f(glGetUniformLocation(font.program, "res_meta"),  NUM_GLYPHS, 2);
    glUniform1f(glGetUniformLocation(font.program, "offset_firstline"), font.linedist-font.linegap/2.0);
    glUniform3fv(glGetUniformLocation(font.program, "colors"), 9, mv_ef_colors);
}

// 
// draw a string
// 
// will call mv_ef_init() if it's the first time it's called. 
// can optionally call this manually
//
// will parse the string and update the instance vbo, then upload it
//
// finally draws
// 
void mv_ef_draw(char *str, char *col, float offset[2], float size, float res[2]) 
{
    static float text_glyph_data[4*MAX_STRING_LEN] = {0};

    if (font.initialized == 0) {
        mv_ef_init();
    }

    int len = strlen(str);

    if (len > MAX_STRING_LEN) {
        printf("Error: string too long. Returning\n");
        return;
    } 

    // parse string, convert to vbo data
    float X = 0.0;
    float Y = 0.0;

    int ctr = 0;
    for (int i = 0; i < len; i++) {

        if (str[i] == '\n') {
            X = 0.0;
            Y -= (font.linedist);
            continue;
        }

        int code_base = str[i]-32; // first glyph is ' ', i.e. ascii code 32

        float x1 = X*size/font.font_size;
        float y1 = Y*size/font.font_size;

        int ctr1 = 4*ctr;
        text_glyph_data[ctr1++] = x1;
        text_glyph_data[ctr1++] = y1;
        text_glyph_data[ctr1++] = code_base;
        text_glyph_data[ctr1++] = col ? col[i] : 0;

        X += font.cdata[code_base].xadvance;
        ctr++;
    }

    // update bindings
    glBindVertexArray(font.vao);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.texture_fontdata);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, font.texture_metadata);

    // actual uploading
    glBindBuffer(GL_ARRAY_BUFFER, font.vbo_instances);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 4*4*ctr, text_glyph_data);

    // update uniforms
    glUseProgram(font.program);
    glUniform1f(glGetUniformLocation(font.program, "scale_factor"), size/font.font_size);
    glUniform2fv(glGetUniformLocation(font.program, "string_offset"), 1, offset);
    glUniform2fv(glGetUniformLocation(font.program, "resolution"), 1, res);

    // actual drawing
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, ctr);
}


//

char *mv_ef_read_entire_file(const char *filename) {
    // Read content of "filename" and return it as a c-string.
    printf("Reading %s\n", filename);
    FILE *f = fopen(filename, "rb");

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    printf("Filesize = %d\n", (int)fsize);

    char *string = (char*)malloc(fsize + 1);
    fread(string, fsize, 1, f);
    string[fsize] = '\0';
    fclose(f);

    return string;
}

GLuint mv_ef_load_shaders(const char * vertex_file_path,const char * fragment_file_path){
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Create the Vertex shader
    GLuint VertexShaderID;
    VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    char *VertexShaderCode   = mv_ef_read_entire_file(vertex_file_path);

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path); fflush(stdout);
    glShaderSource(VertexShaderID, 1, (const char**)&VertexShaderCode , NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( InfoLogLength > 0 ){
        char VertexShaderErrorMessage[9999];
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, VertexShaderErrorMessage);
        printf("%s\n", VertexShaderErrorMessage); fflush(stdout);
    }


    // Create the Fragment shader
    GLuint FragmentShaderID;
    FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    char *FragmentShaderCode = mv_ef_read_entire_file(fragment_file_path);

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path); fflush(stdout);
    glShaderSource(FragmentShaderID, 1, (const char**)&FragmentShaderCode , NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        char FragmentShaderErrorMessage[9999];
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage);
        printf("%s\n", FragmentShaderErrorMessage); fflush(stdout);
    }


    // Create and Link the program
    printf("Linking program\n"); fflush(stdout);
    GLuint ProgramID;
    ProgramID= glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if ( InfoLogLength > 0 ){
        GLchar ProgramErrorMessage[9999];
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]); fflush(stdout);
    }

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    free(FragmentShaderCode);
    free(VertexShaderCode);

    return ProgramID;
}


#endif // MV_EASY_FONT_IMPLEMENTATION