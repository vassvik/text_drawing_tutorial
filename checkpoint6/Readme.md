# Checkpoint 6

Starting from checkpoint 5. 

We now begin the real work! Instead of using a toy texture, we can generate a real bitmap texture directly from a font file and paste a part of that onto our rectangles to draw a different characters. We will be using stb_truetype.h to simplify this process. We will safe the bitmap to file to have a look at it, but we won't be ready  to use it just yet. 


### Font bitmap

![font!](font.png)


### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             88              6            278
GLSL                             2              7              0             17
-------------------------------------------------------------------------------
SUM:                             3             95              6            295
-------------------------------------------------------------------------------
```