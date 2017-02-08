# Checkpoint 11 -- Bringing back the font bitmap texture

We finally bring back the font bitmap texture, which we bind to texture unit 0. 

We'll move the font metadata texture to texture unit 1 and make it two-dimensional. 

The first row contains the pixel coordinates of the lower left corner of each glyph and the width in pixels of each glyph in the bitmap texture. 

The second row contains the offset of the lower left and upper right corner of the glyph in the bitmap texture relative to the baseline. We'll use the first row to look up in the bitmap texture, and the second row to offset and scale the glyph. 

The color texture will be moved to texture unit 2.

We don't use the metadata texture data yet, but instead focus on making sure the bitmap works, and the colors work. We'll scale and move the glyphs properly in the next section.


### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             86             13            292
GLSL                             2             17             13             30
-------------------------------------------------------------------------------
SUM:                             3            103             26            322
-------------------------------------------------------------------------------
```