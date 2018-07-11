#pragma once
#include "libraries.h"

//Model_3D global_model;
float	global_max_x = -1000000, global_min_x = 1000000,
		global_max_y = -1000000, global_min_y = 1000000,
		global_max_z = -1000000, global_min_z = 1000000;

const float accuracy = 0.001;
const float clipper_scale = 1000000;
const float Pi = 3.1415;

vector<float> surface_z_values;