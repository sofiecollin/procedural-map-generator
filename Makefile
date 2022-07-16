# 2020-10-12: New Makefile!
# 	Hopefully this will be a lot faster than before. Tell Eric if something is 
#	broken. (Or fix it!)
#	
#	To add a new file: add the name to the o-file list and create a new sub-goal 
#	for each .cc file (follow the pattern of the others)
#
#	This new Makefile might have some problems with detecting changes in some h-files.
#	If your changes in the code seems to do nothing, do a 'make clean'.
#

# ===== Flags and Variables =====

cc = g++ -std=c++17 -Wall -g
gl_flags = -lm -lGL -lX11 -DGL_GLEXT_PROTOTYPES
include_directory = -I lib -I lib/common -I lib/common/Linux -I src/state -I src/object -I src/shader -I src/camera -I src/light -I src/misc -I src/gui  -I ./

o_common = GL_utilities.o VectorUtils3.o LoadTGA.o loadobj.o MicroGlut.o
o_states = Game_State.o parameters.o
o_object = Object.o Terrain.o Heights_Generator.o Skybox.o Driveable.o Road.o Roads_Generator.o Driver.o
o_shader = Shader.o Skybox_Shader.o
o_camera = Camera.o
o_lights = Light.o
o_misc   = Pathfinder.o Pnode.o Traffic.o
ofiles = $(o_common) $(o_states) $(o_object) $(o_shader) $(o_camera) $(o_lights) $(o_misc) $(o_gui) 

# ===== Main Goal =====

main: main.cc $(ofiles)
	$(cc) -o cdio main.cc $(ofiles) $(include_directory) $(gl_flags)

# ===== Sub Goals =====

# Common

GL_utilities.o: lib/common/GL_utilities.c lib/common/GL_utilities.h
	$(cc) -c lib/common/GL_utilities.c $(include_directory) $(gl_flags)

VectorUtils3.o: lib/common/VectorUtils3.c lib/common/VectorUtils3.h
	$(cc) -c lib/common/VectorUtils3.c $(include_directory) $(gl_flags)

LoadTGA.o: lib/common/LoadTGA.c lib/common/LoadTGA.h
	$(cc) -c lib/common/LoadTGA.c $(include_directory) $(gl_flags)

loadobj.o: lib/common/loadobj.c lib/common/loadobj.h
	$(cc) -c lib/common/loadobj.c $(include_directory) $(gl_flags)

MicroGlut.o: lib/common/Linux/MicroGlut.c lib/common/Linux/MicroGlut.h
	$(cc) -c lib/common/Linux/MicroGlut.c $(include_directory) $(gl_flags)

# State

Game_State.o: src/state/Game_State.cc src/state/Game_State.h
	$(cc) -c src/state/Game_State.cc $(include_directory) $(gl_flags)

parameters.o: src/state/parameters.cc src/state/parameters.h
	$(cc) -c src/state/parameters.cc $(include_directory) $(gl_flags)

# Object

Object.o: src/object/Object.cc src/object/Object.h
	$(cc) -c src/object/Object.cc $(include_directory) $(gl_flags)

Terrain.o: src/object/Terrain.cc src/object/Terrain.h
	$(cc) -c src/object/Terrain.cc $(include_directory) $(gl_flags)

Heights_Generator.o: src/object/Heights_Generator.cc src/object/Heights_Generator.h
	$(cc) -c src/object/Heights_Generator.cc $(include_directory) $(gl_flags)

Skybox.o: src/object/Skybox.cc src/object/Skybox.h
	$(cc) -c src/object/Skybox.cc $(include_directory) $(gl_flags)

Driveable.o: src/object/Driveable.cc src/object/Driveable.h
	$(cc) -c src/object/Driveable.cc $(include_directory) $(gl_flags)

Road.o: src/object/Road.cc src/object/Road.h
	$(cc) -c src/object/Road.cc $(include_directory) $(gl_flags)

Roads_Generator.o: src/object/Roads_Generator.cc src/object/Roads_Generator.h
	$(cc) -c src/object/Roads_Generator.cc $(include_directory) $(gl_flags)

Driver.o: src/object/Driver.cc src/object/Driver.h
	$(cc) -c src/object/Driver.cc $(include_directory) $(gl_flags)

# Shader

Shader.o: src/shader/Shader.cc src/shader/Shader.h
	$(cc) -c src/shader/Shader.cc $(include_directory) $(gl_flags)

Skybox_Shader.o: src/shader/Skybox_Shader.cc src/shader/Skybox_Shader.h
	$(cc) -c src/shader/Skybox_Shader.cc $(include_directory) $(gl_flags)

# Camera

Camera.o: src/camera/Camera.cc src/camera/Camera.h
	$(cc) -c src/camera/Camera.cc $(include_directory) $(gl_flags)

# Light

Light.o: src/light/Light.cc src/light/Light.h
	$(cc) -c src/light/Light.cc $(include_directory) $(gl_flags)

# Misc

Pathfinder.o: src/misc/Pathfinder.cc src/misc/Pathfinder.h
	$(cc) -c src/misc/Pathfinder.cc $(include_directory) $(gl_flags)

Pnode.o: src/misc/Pnode.cc src/misc/Pnode.h
	$(cc) -c src/misc/Pnode.cc $(include_directory) $(gl_flags)

Traffic.o: src/misc/Traffic.cc src/misc/Traffic.h
	$(cc) -c src/misc/Traffic.cc $(include_directory) $(gl_flags)	

# ===== Clean =====

clean :
	rm *.o
	rm cdio
