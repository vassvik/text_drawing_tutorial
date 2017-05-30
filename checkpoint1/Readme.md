# Checkpoint 1 -- Creating a window

This is the starting point of any OpenGL project: You create a blank window that you keep drawing to in a loop until the window is closed. You can create a window in multiple ways, either using GLFW (like we do here), or alternative libraries such as SDL, or use OS functions directly if you know what you're doing.

The code presented here is completely boilerplate -- it's code you'll use in every project with almost no changes.

The result is a boring black window, but it's something!

To compile and link with `gcc` (in linux, or using MSYS2 in Windows, make sure libglfw3-dev or equivalent is installed)
```bash
gcc main.c -lglfw3
```
To compile and link with `cl` (make sure `shell.bat` is executed from the parent directory prior), 
```bash
cl main.c -I..\include -L..\lib ..\lib\glfw3dll.lib
```

This should result in:
![illustration!](http://i.imgur.com/HgO4s9o.png)


### Code statistics

Output from [cloc](https://github.com/AlDanial/cloc):
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                1             13             17             29
-------------------------------------------------------------------------------
```