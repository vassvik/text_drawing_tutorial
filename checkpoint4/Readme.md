# Checkpoint 4 -- Drawing two differently colored rectangles!

Starting from checkpoint 3, with old explanatory comments removed

Buffer setup is moved to a single function (setup()). All the drawing related parts are moved to a single function (draw()).

Added three more triangles, that together make two rectangles, slightly offset from each other for a total 12 vertices. 

Added a color index component (think palette) to each vertex, so that we can color each rectangle differently in the fragment shader. 

The vertex shader also shrinks the vertices by half, and passes through the color index to the fragment shader.


### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             62              4            205
GLSL                             2              6              0             19
-------------------------------------------------------------------------------
SUM:                             3             68              4            224
-------------------------------------------------------------------------------
```