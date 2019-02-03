#OBJS specifies which files to compile as part of the project
OBJS = src\main.cpp src\LTimer.cpp src\LTexture.cpp src\Animation.cpp src\Actor.cpp src\Collider.cpp src\StaticCollider.cpp src\essentials.cpp src\Decal.cpp src\Character.cpp src\CollisionDetection.cpp src\Level.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -IC:\mingw_dev_lib\include\SDL2 -IC:\include

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -LC:\mingw_dev_lib\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -w -Wl,-v,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = CHK.exe

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $(OBJ_NAME)
#all : $(OBJS)
#	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
