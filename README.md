# MRL_LabelTool

(The video was speeking in Chinese)  
How to use: https://www.youtube.com/watch?v=m87yXTsJ6vg&t=5s    
Build Tutorial: https://www.youtube.com/watch?v=PMmqYjvkA2Y

Noticed that the name of the project folder(root folder) must be `MRL_LabelTool`

This is an label tool for MRL Project, it include two windows, which can help you label the data and check the Model.   

This project use C++20, Eigen3, ImGUI, ImPlot, ImGuiFileDialog to build, it was all included in `3rdparty` folder.  
This project also supports C++17.

On Windows10, it builed by Vscode and CMake, you can use Visual Studio, too.  
On Linux you can just build by CMake.  

If you wanna change the feature, modify the "include/make_feature.h" file.
  
![](https://github.com/NcuMathRoboticsLab/MRL_LabelTool/blob/main/docs/LabelToolDemo.gif?raw=true)

# Dependencies

+ Eigen3
+ ImGUI
+ ImPlot
+ ImGuiFileDialog
+ OpenGL    
    On Windows, you can download it in Visual Studio.
+ glfw  
    On Windows, you have to build it manually.

# Linux

just clone it and build it.

```bash
git clone https://github.com/Mes0903/MRL_LabelTool.git
cd MRL_LabelTool
mkdir build && cd build
cmake ..
cmake --build .
```

# Windows

You can check the Demo Video to help you build it, it's a little complicated.
## glfw

There is a lib file `glfw3.lib` in the `3rdparty/windows/lib`, you should build it by yourself. (Since the lib file on the github is depends on my environment.)

1. download the source code from [glfw](https://www.glfw.org/download).
2. make a `build` folder and get into it (You can do it by Vscode) 
3. cmake ..
4. Open the GLFW.sln, build the project by Visual Studio.
5. There would be a lib file called `glfw3.lib` in Debug folder, delete my `glfw3.lib` and copy your `glfw3.lib` into `3rdparty/windows/lib`.
6. Also, there is a folder called `GLFW` in the glfw `3rdparty/windows`, delete my `GLFW` and copy your `GLFW` into `3rdparty/windows`.

## glad

1. There is a `CMakeLists.txt` in the `3rdparty/windows/glad`, copy it to somewhere first, then delete the `3rdparty/windows` folder in the project.

2. Download the glad correspond to your Opengl Version on the [glad loader website](https://glad.dav1d.de/). Then move the `glad` folder into `3rdparty/windows`.
