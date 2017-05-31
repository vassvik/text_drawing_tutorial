# Checkpoint 2 -- Using OpenGL functions

Starting from checkpoint 1, with some slight reordering. 

Getting access to OpenGL functions is unfortunately not a trivial matter, primarily due to how vendor extensions and different versions works. Most OpenGL functions have to be accessed dynamically at runtime from either opengl32.dll (in Windows), or libGL.so (in Linux). These functions can be assigned to local variables called function pointers that you can use as any other function.

The most direct way to access these functions pointers is to use OS functionality (`wglGetProcAddress` in Windows, `glxGetProcAddress` in Linux), but since we use GLFW we can use its own wrapper (`glfwGetProcAddress`) around these functions, so that we can use the same code for any OS that support GLFW. 

We'll later use another tiny library to grab all the relevant functions based on which OpenGL version we want instead of grabbing only the functions we need as we do here. But for now, we'll keep it simple.

We will only use two functions:
 - `glClearColor` to set the background color, and 
 - `glClear` to actually clear the screen to this color each frame.



**Note:** Some legacy OpenGL functions can be accessed by linking to opengl32.lib, but these are restricted to OpenGL version 1.3 and below. glfwGetProcAddress can also be used to grab these, so we do that instead of linking to another library. 

### Screenshot

This should result in:

![illustration!](http://i.imgur.com/qgsGSRe.png)

## Compiling

### GCC (Linux, or MSYS2 in Windows)
Note: Make sure libglfw3-dev or equivalent is installed and visible to the compiler. 
```bash
gcc main.c -lglfw3
```

### MSVC (Windows)
To compile and link with `cl` (make sure `shell.bat` is executed from the parent directory prior), 
```bash
cl main.c -I..\include -L..\lib ..\lib\glfw3dll.lib
```

### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             19             18             45
-------------------------------------------------------------------------------
```
