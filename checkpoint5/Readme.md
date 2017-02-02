# Checkpoint 5

Starting from checkpoint 4. We add a texture, and sample that for the color instead of a color index. We change the color index (a single float) into UV coordinates, which are then used to look up in the texture. 

We use a small texture of size 4x4, with each texel having its own unique color. The way we set up the UV coordinates results in half of the colors are on each rectangle.