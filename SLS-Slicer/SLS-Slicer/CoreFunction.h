#pragma once
#include "libraries.h"
#include "CustomTypeDefinitions.h"
#include "GlobalVariables.h"
#include "SlicingFunction.h"
#include "SlicerParameters.h"
#include "SortingFunctions.h"
#include "Perimeter.h"
#include "GCode.h"

/// <summary>
/// Function declarations
/// </summary>
void UpdateGlobalBoundaries(Model_3D* stl_model);

void CoreFunction(Model_3D stl_model, SlicerParameters parameter) {

	UpdateGlobalBoundaries(&stl_model);
	
	sort(stl_model.elements.begin(), stl_model.elements.end());
	
	polygonSet unsorted_layers =  slice_by_planes(parameter, stl_model);
	
	vector<polygonSet> unlabelled_layers = sort_contours(unsorted_layers);

	sorted_polySet labelled_layers = LabelPolygons(unlabelled_layers);

	vector<vector<polygonSet>> all_layers = GroupPolygons(labelled_layers);

	vector<polygonSet> final_set = func_main(all_layers, parameter);

	/*
	vector<polygonSet> final_set;
	
	for (auto layer : all_layers) {
		for (auto polyset : layer) {
			final_set.push_back(polyset);
		}
	}
	*/
	GCode::Write_GCode(final_set);

}

void UpdateGlobalBoundaries(Model_3D* stl_model) {

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