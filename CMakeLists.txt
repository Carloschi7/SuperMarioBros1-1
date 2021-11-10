cmake_minimum_required(VERSION 3.12)

if("${ENGINE_PATH}" STREQUAL "" OR "${ENGINE_BUILD_PATH}" STREQUAL "")
	message(FATAL_ERROR ": You need to define the vars ENGINE_PATH and ENGINE_BUILD_PATH, Check out the Github page to see what they should be bound to")
endif()


project(C7Platformer)

add_executable(${PROJECT_NAME}
	src/Application.cpp
	src/Background.h
	src/Background.cpp
	src/Counter.h
	src/Counter.cpp
	src/Enemy.h
	src/Enemy.cpp
	src/Flagpole.h
	src/Flagpole.cpp
	src/Level.h
	src/Level.cpp
	src/LevelScene.h
	src/LevelScene.cpp
	src/Pipe.h
	src/Pipe.cpp
	src/Player.h
	src/Player.cpp
	src/Powerup.h
	src/Powerup.cpp
	src/Renderer.h
	src/Renderer.cpp
	src/Tile.h
	src/Tile.cpp
	src/Timer.h
	src/Timer.cpp
)


target_link_libraries(${PROJECT_NAME}
	opengl32.lib
	${ENGINE_PATH}/Dependencies/ASSIMP/bin/assimp.lib
	${ENGINE_PATH}/Dependencies/GLEW/lib/Release/Win32/glew32s.lib
	${ENGINE_PATH}/Dependencies/GLFW/lib-vc2019/glfw3.lib
	${ENGINE_BUILD_PATH}/Debug/C7Engine.lib)

target_include_directories(${PROJECT_NAME} PUBLIC
	${ENGINE_PATH}/engine 
	${ENGINE_PATH}/Dependencies/ASSIMP/include
	${ENGINE_PATH}/Dependencies/GLM
	${ENGINE_PATH}/Dependencies/stbi
	${ENGINE_PATH}/Dependencies/GLFW
	${ENGINE_PATH}/Dependencies/GLEW)

#set the working directory of the exe to the same dir containing the CMakeLists.txt
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

