########## There is no makefile of this project, for that a makefile is hard to be linked in this involuted environment with too many head files. ##########

########## Use Visual Studio 2019 to open the project "openglwarm.sln" ###########

1. Visual Studio 2019 should have downloaded the workloads "Desktop Development with C++"

2. A folder "GLFW" is contained in the project "openglwarm". Inside the folder is a "libraries" folder which contains 3 subfolders: glm-master, Includes, Libs.

   Grant the necessary permissions when prompted.

3. Adding Include and Libs: Open the project, go to 

   Project Properties -> Configuration Properties -> VC++ Directories -> Include Directories.

   Edit the path. Modify the path of "Includes" folder to the current path in "GLFW//libraries".

   Go to Configuration Properties -> VC++ Directories -> General -> Library Directories.

   Edit the path. Modify the path of "Libs" folder to the current path in "GLFW//libraries".

4. Adding glm-master:

   Go to Project Properties -> Configuration Properties -> C/C++ -> General -> Additional Include Directories.

   Edit the path. Modify the path of "glm-master" to the current path in "GLFW//libraries".

5. Copy the "glew32.dll" file in the project into the fole "C:\Windows\System32".


6. Click the "Local Windows Debugger" to test the project.



########## Test the project ##########


1. To test a new obj file --
   warm_up.cpp -> line 193
   Replace the content of char map[] with the path of this new obj file.

2. To test a new shaders file --
   warm_up.cpp -> line 220 and line 221
   Replace the content of "vertexPath" and "fragmentPath" with the new shaders files' paths.

3. To use the shaders of type const char* defined in the beginnning of the warm_up.cpp
   warm_up.cpp -> line 226 and line 227
   Annotate the line 224 and line 225, and delete the "//" at the front of line 226 and 227.

4. To test the "separate triangles.obj", replace the path in map of line 193 with "separate triangles.obj".
	
5. If made any changes to the project, please recompile the project.

