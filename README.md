#SuperMarioBros1-1

Attempt to remake the first level of Super Mario Bros (the level is not perfect and some features may be missing)
This project was done as an exercise which consisted in making a game without using an engine written by somebody else.
In fact, the library featured in this game is my C7Engine

HOW TO PLAY THE LEVEL:

Requirements:

1) The latest version of Visual Studio (preferably)
2) CMake GUI (version 3.12 or higher)
3) GIT BASH

Steps:
1)Download and compile the C7Engine (steps on how to get there can be found in my C7Engine repo README)



2)Clone this repo in a folder of your choice



3)Open the CMake gui and set as the source code folder the freshly downloaded repo and as the binaries folder
another folder of your choice



4)Add two new entries in the variable section: set ENGINE_PATH to the physical engine folder and set ENGINE_BUILD_PATH to
the folder which contains the build of the engine generated via CMake (copy and paste the folder paths)
for example:
ENGINE_PATH = C:/Engine/C7Engine
ENGINE_BUILD_PATH = C:/Engine/C7Engine/build



5)Click Configure, set the project platform to Win32 and click Generate



6)Go to the build folder and open the sln file with Visual Studio



7)Run the project, and you will find yourself with an error because your pc cannot find the assimp dll. So copy the dll stored in the
	assimpdll folder and paste it into build/Debug



8)And you should be good to go, enjoy!
