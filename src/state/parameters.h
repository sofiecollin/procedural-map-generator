#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <vector>
#include <string>

extern int SEED;

//Road parameters

//Road Generation parameters
extern int NEW_SEGMENT_BASE_NUM;
extern int NEW_SEGMENT_RAND_NUM;
extern float DEFAULT_SEGMENT_LENGTH;

// LC - Local Constraint
extern float LC_MIN_ANGLE;
extern float LC_ROTATION_STEP_SIZE;
extern float LC_MAX_INCLINE;
extern float LC_MAX_ROTATION;
extern float LC_ROTATION_STEP_SIZE;
extern float LC_MAX_PRUNE_LENGTH;
extern int LC_PRUNE_NUM_STEPS;
extern float LC_MIN_PERPENDICULAR_DIST;
extern float LC_CONNECTION_RADIUS;
extern float DEFAULT_SEGMENT_LENGTH;
extern float HIGHWAY_WIDTH;
extern int NUMBER_OF_CITIES;
extern int SEGMENTS_PER_CITY;


//Terrain parameters
extern int TERRAIN_RESOLUTION;
extern float TERRAIN_MAX_HEIGHT;
extern float TERRAIN_SCALE_PARAMETER;
extern int TERRAIN_NUM_OCTAVES;
extern float TERRAIN_SMOOTHNESS;
extern int TERRAIN_NUM_TEXTURES;
extern std::vector<std::string> TERRAIN_TEXTURE_PATHS;

// Pathfinder parameters
extern float STEEPNESS_PENALTY;
extern float EUCLIDEAN_PENALTY;
// Traffic parameters
extern float ROAD_CHECK_WIDTH;

#endif