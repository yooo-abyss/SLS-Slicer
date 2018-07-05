#pragma once
#include "CustomTypeDefinitions.h"

class SlicerParameters
{
public:
	string name;					// Name of the Parameter
	int perimeter_number;			// Number of Perimeters
	float scan_spacing;				// set the distance between two Scans
	float layer_height;				// layer  height for slicing
	int surface_layers;				// No of surface layers
	float overlap_parameter;		// Overlap Parameter between Infill and Perimeter

	float infill_density;			// Infill Percentage
	float infill_angle;				// infill - hatching angle in degrees
	
	float scan_speed;				// Speed at which laser scanning happens
	float jump_speed;				// Speed at which laser moves in Non-print moves
	
	float laser_power;				// Laser Power to be set
	float bed_temperature;			// Set Print Bed temperature

	infill_type infill_pattern;		// Type of Infill

	SlicerParameters(
		string Name = "Default",
		int Perimeter_number = 4,
		float Scan_spacing = 0.15,
		float Layer_height = 0.2,
		int Surface_layers = 3,
		float Overlap_parameter = 1.5,
		float Infill_density = 100,
		float Infill_angle = 45,
		float Scan_speed = 2,
		float Jump_speed = 5,
		float Laser_power = 10,
		float Bed_temperature = 170,
		infill_type Infill_pattern = concentric
	):
		name(Name),
		perimeter_number(Perimeter_number),
		scan_spacing(Scan_spacing),
		layer_height(Layer_height),
		surface_layers(Surface_layers),
		overlap_parameter(Overlap_parameter),
		infill_density(Infill_density),
		infill_angle(Infill_angle),
		scan_speed(Scan_speed),
		jump_speed(Jump_speed),
		laser_power(Laser_power),
		bed_temperature(Bed_temperature) {}
};

