# Checkpoint 4.

Starting from checkpoint 3. Buffer setup is moved to a single function (setup()). All the drawing related parts are moved to a single function (draw()).

Added three more triangles, that together make two rectangles, slightly offset from each other. Total 12 vertices. 

Added a color index component (think palette) to each vertex, so that we can color each rectangle differently in the fragment shader. 

The vertex shader also shrinks the vertices by half, and passes through the color index