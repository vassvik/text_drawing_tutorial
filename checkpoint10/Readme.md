# Checkpoint 10 -- Adding textures

Next we'll add the two new textures (the bitmap texture we'll add later, since we already know how to do it) - one for the metadata for each glyph (we'll make up something to put in it), and one for colors. For now we'll make both *one-dimensional* textures. The first texture will contain values we'll use to stretch our quad. 

We'll assume we only have 2 glyphs, for simplicity. In the next section we'll expand to actually using the font data.

One thing to note is that both the texture data and the values we use to sample it have to be *normalized*. We send both the number of glyphs, and number of colors to the shader to help normalize. 

We'll use the same colors as we used in checkpoint5, but put it in an 1D texture this time. 

We add 0.5 to each of the texture fetches, to sample the middle of the texels.


### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             72              9            252
GLSL                             2             13              5             24
-------------------------------------------------------------------------------
SUM:                             3             85             14            276
-------------------------------------------------------------------------------
```