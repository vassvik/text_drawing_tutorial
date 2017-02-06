# Checkpoint 8 -- Proper scaling and positioning of glyphs

Starting from checkpoint 7. 

We first flip the UV coordinates per character. 

Next, we scale the vertex positions such that the proportions are correct.

Finally we move the two rectangles right next to each other using the character data given by stb_truetype.h, as if we were writing the string "yG"

We also correct for aspect ratio in the vertex shader. We add a half-texel offset to the texture sampling to move the glyph into the center of the quad. 

Specifying the vertex positions and uv coordinates are a huge pain, but we'll ease that pain next. 


### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             93              0            260
GLSL                             2              7              0             19
-------------------------------------------------------------------------------
SUM:                             3            100              0            279
-------------------------------------------------------------------------------
```