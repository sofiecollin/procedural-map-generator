#include "parameters.h"

int SEED{1};


//Road parameters
//float MIN_TILE_LENGTH{1};

//Road Generation parameters
int NEW_SEGMENT_BASE_NUM{2};            // Minimum number of new segment generateed
int NEW_SEGMENT_RAND_NUM{2};            // Random number of extra new segments generated
float DEFAULT_SEGMENT_LENGTH {40};      // The default length of a new segment.

// LC - Local Constraint
float LC_MIN_ANGLE {60};              // Minimum angle (degrees) allowed between roads in an intersection
float LC_ROTATION_STEP_SIZE {10};     // Size (in degrees) of steps taken when searching after suitable rotation for segment
float LC_MAX_INCLINE {20};            // Minimum incline (angle in degrees) allowed
float LC_MAX_ROTATION {120};          // Max rotation allowed when rotating road segment to fulfill condition (e.g avoiding water)
float LC_MAX_PRUNE_LENGTH{ 0.5};      /* Max percentage of a segment that is allowed to be pruned. (e.g 0.9 would allow 90% of the 
                                       original segment to be cut, leaving 10% of the original length) */
int LC_PRUNE_NUM_STEPS {10};          // The number of steps that is done in the pruning. More steps means higher precision
float LC_MIN_PERPENDICULAR_DIST {10}; // The minimum allowed perpendicular distance to a node from a segment. 
float LC_CONNECTION_RADIUS {20};      // The distance from another node at which a new node will instead connect to the existing node.
float HIGHWAY_WIDTH{10};             // Width of the highway roads
int NUMBER_OF_CITIES{4};            // Number of cities to be created
int SEGMENTS_PER_CITY{300};         // Number of segments created for each city

//Terrain parameters
int TERRAIN_RESOLUTION {128};
float TERRAIN_MAX_HEIGHT {65};       
float TERRAIN_SCALE_PARAMETER {15};
int TERRAIN_NUM_OCTAVES {3};         // Number of noise functions of different density, more octaves means higher level of detail
float TERRAIN_SMOOTHNESS {0.8};      // A value between 0 and 1, higher means a smoother terrain
int TERRAIN_NUM_TEXTURES {3};        // At this point, the shader only handles 3 textures
std::vector<std::string> TERRAIN_TEXTURE_PATHS{"res/textures/grass.tga", "res/textures/conc.tga","res/textures/conc.tga"};

// Pathfinder parameters
float STEEPNESS_PENALTY {10};
float EUCLIDEAN_PENALTY {1};

// Trafic

float ROAD_CHECK_WIDTH {2};