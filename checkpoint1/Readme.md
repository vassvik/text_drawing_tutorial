# Checkpoint 1 -- Creating a window

This is the starting point of any OpenGL project: You create a window that you keep drawing to in a loop until the window is closed. You can create a window in multiple ways, either using GLFW (like we do here), or alternative libraries such as SDL, or use OS functions directly if you know what you're doing.

The code presented here is completely boilerplate -- it's code you'll use in every project with almost no changes.

The result is a boring black window, but it's something!

Compile and link with
```bash
gcc main.c -lglfw3
```

### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             14             17             30
-------------------------------------------------------------------------------
```