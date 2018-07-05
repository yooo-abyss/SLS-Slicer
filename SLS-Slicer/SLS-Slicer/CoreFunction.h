#pragma once
#include "libraries.h"
#include "CustomTypeDefinitions.h"
#include "GlobalVariables.h"

void CoreFunction(Model_3D stl_model) {

	SetGlobalBoundaries(&stl_model);
	
}

void SetGlobalBoundaries(Model_3D* stl_model) {

	if (global_max_x < stl_model->max_x)
		global_max_x = stl_model->max_x;

	if (global_min_x > stl_model->min_x)
		global_min_x = stl_model->min_x;

	if (global_max_y < stl_model->max_y)
		global_max_y = stl_model->max_y;

	if (global_min_y > stl_model->min_y)
		global_min_y = stl_model->min_y;

	if (global_max_z < stl_model->max_z)
		global_max_z = stl_model->max_z;

	if (global_min_z > stl_model->min_z)
		global_min_z = stl_model->min_z;
}