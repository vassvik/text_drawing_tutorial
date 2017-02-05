# Checkpoint 3 -- Drawing a triangle!

Starting from checkpoint 2, with old explanatory comments removed for brevity's sake.

We actually start using OpenGL, and draw our first thing! This part adds a whopping 110 lines of code, where the bulk of it is the functions we need to read the shaders from file and compile them, and the error checking therein. 

We add window hints to the window creation routines to specify which version and profile we want to use for OpenGL. We use *glad* to load OpenGL function pointers and symbols. We load the shaders. We Create a Vertex Array Object and Vertex Buffer Object, upload vertex data corresponding to a single triangle to the VBO, and finally use that to draw a triangle using the shaders we loaded.

The shaders are very simple. The vertex shader just passes through the vertex positions, and the fragment shader paints all the pixels red. 

### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             54             19            173
GLSL                             2              4              0             12
-------------------------------------------------------------------------------
SUM:                             3             58             19            185
-------------------------------------------------------------------------------
```