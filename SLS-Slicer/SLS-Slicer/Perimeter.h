#pragma once
#include "libraries.h"
#include "CustomTypeDefinitions.h"
#include "clipper.hpp"
#include "GlobalVariables.h"
#include "SlicerParameters.h"
#include "PolygonOperations.h"
#include "Infill.h"

void GeneratePerimeters(const polygonSet& polyline, float z_value, SlicerParameters parameter, polygonSet& perimeters, polygonSet& innermost_perimeter, polygonSet& infill_boundary);
void ConvertClipperPathsToPolygons(ClipperLib::Paths sol, float z_value, polygonSet* temp_layer);

vector<polygonSet> func_main(const vector<vector<polygonSet>>& sorted_slices, SlicerParameters parameter)
{

	/*inner perimeter index = 0;
	outermost perimeter index = 1;
	infill index = 2;*/
	//Assigning index for each polygon if it is infill || perimeter inner || outer perimeter to enable different speed settings;

	vector<pair<vector<pair<polygon, int>>, int>> ret_layers;
	vector<pair<polygon, int>> temp_set;
	//float slope = tan(angle*Pi / 180);
	int lay_num = 0;
	polygonSet peri_inner;
	//pair<vector<pair<polygon, int>>, pair<polygonSet, polygonSet>> temp_peri;
	vector<polygonSet> final_set;
	polygonSet perimeters;
	polygonSet infill_boundary;
	polygonSet innermost_perimeter;
	polygonSet final_infill;

	for (int a = 0; a < sorted_slices.size(); a++)
	{
		cout << "infill for layer " << a+1 << endl;
		perimeters.clear();
		infill_boundary.clear();
		innermost_perimeter.clear();
		final_infill.clear();
		auto layer = sorted_slices[a];
		lay_num++;
		for (const auto& polyset : layer)
		{
			if (polyset.size() != 0 && polyset[0].size() != 0)
			{
				GeneratePerimeters(polyset, polyset[0][0].first.z, parameter, perimeters, innermost_perimeter, infill_boundary);
				
				/*
				if (a >  parameters[params].perimeter_number - 1 && a < sorted_slices.size() - parameters[params].perimeter_number + 1)
				{


					for each(auto& polyset in sorted_slices[a + parameters[params].perimeter_number - 1])
					{
						for each (auto& polyn in polyset)
							nextlayerpolyset.push_back(polyn);

					}
					for each(auto& polyset in sorted_slices[a - parameters[params].perimeter_number + 1])
					{
						for each (auto& polyn in polyset)
							previouslayerpolyset.push_back(polyn);

					}
					polygonSet nonselect;
					using namespace ClipperLib;
					auto innernonselect = polygon_operations::clippolygons(temp_peri.second.first, polygonset_clip, ctDifference, pftPositive, pftPositive);

					if (polygonset_clip.size() != 0)
					{


						auto select = polygon_operations::clippolygons(temp_peri.second.first, polygonset_clip, ctIntersection, pftPositive, pftPositive);
						nonselect = polygon_operations::clippolygons(temp_peri.second.second, polygonset_clip, ctDifference, pftPositive, pftPositive);

						if (select.size() != 0)
						{
							auto select_offset = polygon_operations::polygonoffset(select, -parameters[params].perimeter_offset);

							temp_infill = add_infill(select, clip_angle, lay_num, clip_offset, select_offset);
							for each(const auto& pline in temp_infill)
								temp_set.push_back(pair<polygon, int>(pline, 2));

							temp_infill.clear();

						}
					}
					else
						nonselect = temp_peri.second.second;

					auto sol1 = polygon_operations::clippolygons(nonselect, nextlayerpolyset, ctIntersection, pftPositive, pftPositive);
					auto sol = polygon_operations::clippolygons(sol1, previouslayerpolyset, ctIntersection, pftPositive, pftPositive);


					if (sol.size() != 0)
					{

						auto sol2 = polygon_operations::clippolygons(innernonselect, sol, ctDifference, pftPositive, pftPositive);

						//sol_offset = polygon_operations::polygonoffset(sol2, perimeter_offset);
						auto sol_offset = polygon_operations::polygonoffset(sol, parameters[params].perimeter_offset);

						temp_infill = add_infill(sol2, parameters[params].infill_angle, lay_num, parameters[params].nozzle_diameter*1.5, nextlayerpolyset);
						for each(const auto& pline in temp_infill)
							temp_set.push_back(pair<polygon, int>(pline, 2));

						temp_infill.clear();

						if (parameters[params].infill_pattern == rectilinear)
							temp_infill = add_infill(sol_offset, parameters[params].infill_angle, lay_num, offset, sol); // here is where you can change to create different infill pattern 
						else if (parameters[params].infill_pattern == concentric)																						     //this has the regio which is intersection of all layers above and below and where we want the actual infill occurs.
							temp_infill = infill_concentric(sol_offset, offset);
						for each(const auto& pline in temp_infill)
							temp_set.push_back(pair<polygon, int>(pline, 2));
						temp_infill.clear();
						infill_choice = true;
					}


				}
				*/

				std::vector<polygon> polygonset_clip;
				polygonset_clip.clear();
				polygonSet nextlayerpolyset;
				polygonSet previouslayerpolyset;

				if (a >  parameter.surface_layers - 1 && a < sorted_slices.size() - parameter.surface_layers + 1)
				{
					for (auto& polyset : sorted_slices[a + parameter.surface_layers - 1])
					{
						for (auto& polyn : polyset)
							nextlayerpolyset.push_back(polyn);

					}
					for (auto& polyset : sorted_slices[a - parameter.surface_layers + 1])
					{
						for (auto& polyn : polyset)
							previouslayerpolyset.push_back(polyn);

					}
					polygonSet nonselect;
					using namespace ClipperLib;
					
					nonselect = innermost_perimeter;

					auto sol1 = polygon_operations::clippolygons(nonselect, nextlayerpolyset, ctIntersection, pftPositive, pftPositive);
					auto sol = polygon_operations::clippolygons(sol1, previouslayerpolyset, ctIntersection, pftPositive, pftPositive);


					if (sol.size() != 0)
					{

						auto sol2 = polygon_operations::clippolygons(infill_boundary, sol, ctDifference, pftPositive, pftPositive);
						//auto sol_offset = polygon_operations::clippolygons(infill_boundary, sol2, ctDifference, pftPositive, pftPositive);

						//sol_offset = polygon_operations::polygonoffset(sol2, perimeter_offset);
						auto sol_offset = polygon_operations::polygonoffset(sol, parameter.scan_spacing);

						polygon infill = add_infill(sol2, parameter.infill_angle, lay_num, 0.3);
						float slope = tan(parameter.infill_angle*Pi / 180);
						if (lay_num % 2 != 0)
							slope = -slope;
						final_infill = nearest_neighbour(infill, 2 * 0.3, innermost_perimeter, slope, lay_num, parameter.infill_angle);
						
						infill.clear();
						infill = add_infill(sol_offset, parameter.infill_angle, lay_num, parameter.scan_spacing); // here is where you can change to create different infill pattern 

						
						polygonSet temp_infill = nearest_neighbour(infill, 2 * parameter.scan_spacing, innermost_perimeter, slope, lay_num, parameter.infill_angle);
						for (const auto& inf : temp_infill)
							final_infill.push_back(inf);
						temp_infill.clear();
						
						polygonSet temp = perimeters;
						for (auto p : final_infill) {
							//temp.push_back(p);
						}

						final_set.push_back(temp);
					}
				}
				else {
					polygon infill = add_infill(infill_boundary, parameter.infill_angle, lay_num, parameter.scan_spacing);

					float slope = tan(parameter.infill_angle*Pi / 180);
					if (lay_num % 2 != 0)
						slope = -slope;
					final_infill = nearest_neighbour(infill, 2 * parameter.scan_spacing, innermost_perimeter, slope, lay_num, parameter.infill_angle);
					polygonSet temp = perimeters;
					for (auto p : final_infill) {
						//temp.push_back(p);
					}
					final_set.push_back(temp);

				}
			}
		}
	}
	return final_set;
}




void GeneratePerimeters(const polygonSet& polygroup, float z_value, SlicerParameters parameter, polygonSet& perimeters, polygonSet& innermost_perimeter, polygonSet& infill_boundary) {

	using namespace ClipperLib;
	Paths subj, subjs;
	Path sub;
	Paths sol;
	for(auto& poly : polygroup)	//lseg is a line segment (pair of 3d points)
	{
		for (const auto& lseg : poly)
			sub << IntPoint(clipper_scale*lseg.second.x, clipper_scale*lseg.second.y);

		subjs.push_back(sub);
		sub.clear();
	}

	
	ClipperOffset co;
	co.AddPaths(subjs, jtSquare, etClosedPolygon);
	co.Execute(subj, -(parameter.scan_spacing/2)*clipper_scale);

	int siz;

	for (int i = 0; i <= parameter.perimeter_number; i++) {
		ClipperOffset co_1;
		// Infills are drawn using this perimeter as base
		if (i == parameter.perimeter_number) {
			co_1.AddPaths(subj, jtSquare, etClosedPolygon);
			if(i != 0)
				co_1.Execute(sol, -(parameter.scan_spacing*clipper_scale*(i - parameter.overlap_parameter)));
			else
				co_1.Execute(sol, -(parameter.scan_spacing*clipper_scale*i));
			ConvertClipperPathsToPolygons(sol, z_value, &infill_boundary);

		}
		else {
			co_1.AddPaths(subj, jtSquare, etClosedPolygon);
			co_1.Execute(sol, -(parameter.scan_spacing*clipper_scale*i));
			ConvertClipperPathsToPolygons(sol, z_value, &perimeters);
			if (i == parameter.perimeter_number - 1) {
				co_1.Execute(sol, -(parameter.scan_spacing*clipper_scale*(i - floor(parameter.overlap_parameter))));
				ConvertClipperPathsToPolygons(sol, z_value, &innermost_perimeter);
			}
		}
	}
}

void ConvertClipperPathsToPolygons(ClipperLib::Paths sol, float z_value, polygonSet* temp_layer) {

	polygon poly;

	for (const auto& result_path : sol)
	{
		int num_pairs = result_path.size();
		for (int i = 0; i < num_pairs - 1; i++) // to avoid going beyond limit: i+1
		{
			Point_3D beginP = Point_3D(result_path.at(i).X / clipper_scale, result_path.at(i).Y / clipper_scale, z_value);
			Point_3D endP = Point_3D(result_path.at(i + 1).X / clipper_scale, result_path.at(i + 1).Y / clipper_scale, z_value);
			poly.push_back(make_pair(beginP, endP));
		}
		//adding the last pair
		if (num_pairs != 0)
		{
			Point_3D beginP = Point_3D(result_path.at(num_pairs - 1).X / clipper_scale, result_path.at(num_pairs - 1).Y / clipper_scale, z_value);
			Point_3D endP = Point_3D(result_path.at(0).X / clipper_scale, result_path.at(0).Y / clipper_scale, z_value);
			poly.push_back(make_pair(beginP, endP));
		}

	}

	temp_layer->push_back(poly);
}