#pragma once
#include "libraries.h"

vector<SlicerParameters> ReadSettings(string file_name);
SlicerParameters CreateSlicerParameters(map<string, string> parameter_map);

vector<SlicerParameters> ReadSettings(string file_name) {
	vector<SlicerParameters> parameters;
	ifstream input_file(file_name);
	if (input_file.is_open()) {
		string line;
		while (getline(input_file, line))
		{
			if (line == "START PROFILE") {
				map<string, string> parameter_map;
				getline(input_file, line);
				do
				{
					stringstream ss(line);
					string key, value;
					getline(ss, key, ' ');
					getline(ss, value, ' ');
					parameter_map[key] = value;
				} while (line != "END PROFILE" && getline(input_file, line));

				parameters.push_back(CreateSlicerParameters(parameter_map));
			}
		}
	}
	else {
		cout << "File not found: " << file_name << endl;
	}
	return parameters;
}

SlicerParameters CreateSlicerParameters(map<string, string> parameter_map) {
	SlicerParameters parameter = SlicerParameters();
	
	if(parameter_map.find("name") != parameter_map.end())
		parameter.name = parameter_map["name"];
	
	if (parameter_map.find("perimeter_number") != parameter_map.end())
		parameter.perimeter_number = atoi(&parameter_map["perimeter_number"][0u]);
	
	if (parameter_map.find("scan_spacing") != parameter_map.end())
		parameter.scan_spacing = atof(&parameter_map["scan_spacing"][0u]);
	
	if (parameter_map.find("layer_height") != parameter_map.end())
		parameter.layer_height = atof(&parameter_map["layer_height"][0u]);
	
	if (parameter_map.find("surface_layers") != parameter_map.end())
		parameter.surface_layers = atoi(&parameter_map["surface_layers"][0u]);
	
	if (parameter_map.find("overlap_parameter") != parameter_map.end())
		parameter.overlap_parameter = atof(&parameter_map["overlap_parameter"][0u]);
	
	if (parameter_map.find("infill_density") != parameter_map.end())
		parameter.infill_density = atof(&parameter_map["infill_density"][0u]);
	
	if (parameter_map.find("infill_angle") != parameter_map.end())
		parameter.infill_angle = atof(&parameter_map["infill_angle"][0u]);
	
	if (parameter_map.find("scan_speed") != parameter_map.end())
		parameter.scan_speed = atof(&parameter_map["scan_speed"][0u]);
	
	if (parameter_map.find("jump_speed") != parameter_map.end())
		parameter.jump_speed = atof(&parameter_map["jump_speed"][0u]);
	
	if (parameter_map.find("laser_power") != parameter_map.end())
		parameter.laser_power = atof(&parameter_map["laser_power"][0u]);
	
	if (parameter_map.find("bed_temperature") != parameter_map.end())
		parameter.bed_temperature = atof(&parameter_map["bed_temperature"][0u]);
	
	if (parameter_map.find("infill_type") != parameter_map.end()) {
		if (parameter_map["infill_type"] == "concentric") {
			parameter.infill_pattern = concentric;
		}
		else if (parameter_map["infill_type"] == "rectilinear") {
			parameter.infill_pattern = rectilinear;
		}
	}
	return parameter;
}