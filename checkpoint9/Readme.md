# Checkpoint 9 -- Changing gears: instancing

Currently, to render a single glyph we do the following:

- Specify 6 vertices (2 floats each) moved and scaled to be the appropriate size and at the appropriate position. That's 12 floats total.
- Specify 6 UV coordinates, one for each vertex, to know where in the bitmap to lookup the glyph texels. That's another 12 floats. 

We could also specify other attributes, like color, which would have to be specified per-vertex, unless we want to do one draw-call per glyph (we don't). 

This means that we need at least 24 floats per glyph, and each time we want to update one glyph we need to push 24 floats to the graphics card. This is a fairly slow process, so we'd want to do better.

Luckily there's an elegant solution: We're drawing the same thing over and over. Each glyph is two triangles making a quad, just stretched differently and placed in the right position. This is the perfect use case for *instancing*. 

With instancing, we'll have one vertex buffer that keeps getting reused. This is our quad. In addition we'll have another vertex buffer that contains information about the glyph we want to draw, namely its position (2 floats), its character code (1 float) and a color index (1 float). We'll use floats for all, so that we can pack everything into a single vec4 attribute.

The position will be used to place the glyph into the right place. The character code will be used to look up information about that glyph from a new *metadata texture*. Finally the color index will be used to look up a third *color texture*. We can now update one glyph by only sending 4 floats instead of 24, and we get the color in addition. This means that the scaling and positioning of each glyph happens in the vertex shader, not on in the CPU code. We'll see that this actually makes it a lot simpler. 

Let's not get ahead of ourselves. We'll do this in several steps for pedagogical reasons. First we'll start by setting up instancing, then moving on to the rest in the next sections. 

We'll temporarily remove all the texture stuff to emphasize the instancing part as cleanly as possible We'll add this back in at a later point. We'll also ignore the third value in the instance buffer, and just focus on getting the quad in the right position. We'll use the fourth value to color them differently

The rest is fairly simple: We'll use `glDrawArraysInstanced` instead of `glDrawArrays`, which takes an additional parameter - the number of instances. We'll use `glVertexAttribDivisor` to tell OpenGL which vertex buffer objects are instanced and which are not. 

The vertex shader doesn't really know about the instancing, and only sees two vertex attributes, which makes it convenient for us to work with. Currently it only offsets the vertex positions depending on the instanced data, and sends on the color index to the fragment shader, which colors each quad differently. 



### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             62              5            203
GLSL                             2             10              4             25
-------------------------------------------------------------------------------
SUM:                             3             72              9            228
-------------------------------------------------------------------------------
```