# Checkpoint 7

Starting from checkpoint 6. 

We replace our toy texture with the new font bitmap texture and adjust our UV coordinates to take out two glyphs from it. The left rect shows lower case "y", and the right rect shows upper case "G". We use the character data supplied by stb_truetype.h to know where to look up in the texture.

Since we only need 1 color component for the texture, we change from GL_RGB to GL_RED. We change the sampling in the fragment shader accordingly. 

Note that both letters are upside down. We also added one layer of padding at the right and bottom side. This will be important later. 

Each glyph fills the whole rectangle, which make them look distorted. This is due to the packing algorithm used by stb_truetype.h. We can use the character data given to appropriately scale and offset our glyph to make them proportional in size. 


### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             81              3            247
GLSL                             2              7              0             17
-------------------------------------------------------------------------------
SUM:                             3             88              3            264
-------------------------------------------------------------------------------
```
