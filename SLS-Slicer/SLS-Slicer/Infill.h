#pragma once
#include "libraries.h"
#include "CustomTypeDefinitions.h"
#include "GlobalVariables.h"
#include "PolygonOperations.h"

vector<pair<Point_3D, Point_3D>> sort_pts(const vector<Point_3D>& pts, const polygonSet& layer_polygons, float infill_angle);

polygon add_infill(const vector<polygon>& polyline, float infill_angle, int layer_num, const float thickness)
{
	vector<pair<Point_3D, Point_3D>> infill;
	vector<Point_3D> all_pts;
	vector<polygon> final_sorted_infill;
	float intercept, intercept_max;

	float slope = tan(infill_angle*Pi / 180);

	bool x = false, y = false;
	if (infill_angle == 90 || infill_angle == 0)
	{
		if ((infill_angle == 90 && layer_num % 2 == 0) || (infill_angle == 0 && layer_num % 2 == 1))
		{
			intercept = global_min_x;
			intercept_max = global_max_x;
			y = true; //slope=inf;
		}
		else if ((infill_angle == 0 && layer_num % 2 == 0) || (infill_angle == 90 && layer_num % 2 == 1))
		{
			intercept = global_min_y;
			intercept_max = global_max_y;
			x = true; //slope=0;
		}



		for (float cons = intercept; cons < intercept_max; cons += thickness)
		{

			for(const auto& poly : polyline)
			{

				vector<Point_3D> pts;
				if (x)
				{
					for (int i = 0; i < poly.size(); i++)
					{
						float dist_1 = poly[i].first.y - cons;
						float dist_2 = poly[i].second.y - cons;

						if (dist_1*dist_2 <= 0)
						{
							Point_3D pt_temp = polygon_operations::point_ratio_m_n(poly[i].first, poly[i].second, abs(dist_1), abs(dist_2));
							pts.push_back(pt_temp);
						}


					}
				}

				if (y)
				{
					for (int i = 0; i < poly.size(); i++)
					{
						float dist_1 = poly[i].first.x - cons;
						float dist_2 = poly[i].second.x - cons;

						if (dist_1*dist_2 <= 0)
						{
							Point_3D pt_temp = polygon_operations::point_ratio_m_n(poly[i].first, poly[i].second, abs(dist_1), abs(dist_2));
							pts.push_back(pt_temp);
						}


					}
				}

				if (pts.size() != 0)
				{
					for (auto& pt : pts)
						all_pts.push_back(pt);

				}
				pts.clear();

			}
			if (all_pts.size() != 0)
			{
				auto partial_infill = sort_pts(all_pts, polyline, infill_angle);
				for(auto& line : partial_infill)
					infill.push_back(line);
				all_pts.clear();
			}
		}
		//final_sorted_infill = nearest_neighbour(infill, 2 * thickness, polyset, 0, layer_num, infill_angle);
	}

	else
	{
		if (slope > 0)
		{
			if (layer_num % 2 == 0)
			{
				intercept = global_min_y - slope * global_max_x;
				intercept_max = global_max_y - slope * global_min_x;
			}

			else
			{
				slope = -slope;
				intercept = global_min_y - slope * global_min_x;
				intercept_max = global_max_y - slope * global_max_x;
			}
		}

		else
		{
			if (layer_num % 2 == 0)
			{
				intercept = global_min_y - slope * global_min_x;
				intercept_max = global_max_y - slope * global_max_x;
			}

			else
			{
				slope = -slope;
				intercept = global_min_y - slope * global_max_x;
				intercept_max = global_max_y - slope * global_min_x;
			}
		}


		for (float cons = intercept; cons < intercept_max; cons += thickness)
		{

			for (const auto& poly : polyline)
			{

				vector<Point_3D> pts;
				for (int i = 0; i < poly.size(); i++)
				{
					float dist_1 = poly[i].first.y - slope * poly[i].first.x - cons;
					float dist_2 = poly[i].second.y - slope * poly[i].second.x - cons;

					if (dist_1*dist_2 <= 0)
					{
						Point_3D pt_temp = polygon_operations::point_ratio_m_n(poly[i].first, poly[i].second, abs(dist_1), abs(dist_2));
						pts.push_back(pt_temp);
					}


				}

				if (pts.size() != 0)
				{
					for (auto& pt : pts)
						all_pts.push_back(pt);

				}
				pts.clear();

			}
			if (all_pts.size() != 0)
			{
				auto partial_infill = sort_pts(all_pts, polyline, infill_angle);
				for(auto& line : partial_infill)
					infill.push_back(line);
				all_pts.clear();
			}

		}
		//final_sorted_infill = nearest_neighbour(infill, 2 * thickness, polyset, 0, layer_num, infill_angle);
	}

	return infill;
	//return final_sorted_infill;

}

vector<pair<Point_3D, Point_3D>> sort_pts(const vector<Point_3D>& pts, const polygonSet& layer_polygons, float infill_angle)
{
	vector < Point_3D > sorted_pts = pts;
	vector<pair<Point_3D, Point_3D>> ret_linseg;

	for (int alp = 0; alp < sorted_pts.size(); alp++)
	{

		for (int bet = 0; bet < sorted_pts.size(); bet++)
		{
			if (infill_angle != 90 && infill_angle != 0)
			{
				if (sorted_pts[alp].x < sorted_pts[bet].x)
					swap(sorted_pts[alp], sorted_pts[bet]);
			}
			else
			{
				if (abs(sorted_pts[alp].x - sorted_pts[bet].x) < 0.01)
				{
					if (sorted_pts[alp].y < sorted_pts[bet].y)
						swap(sorted_pts[alp], sorted_pts[bet]);
				}
				else

				{
					if (sorted_pts[alp].x < sorted_pts[bet].x)
						swap(sorted_pts[alp], sorted_pts[bet]);
				}
			}

		}
	}


	for (int i = 0; i < (sorted_pts.size() - 1); i++)
	{

		int value = 0;
		Point_3D mid_point = (sorted_pts[i] + sorted_pts[i + 1]) / 2;
		if (polygon_operations::in_or_out(mid_point, layer_polygons) == 1)
			ret_linseg.push_back(pair<Point_3D, Point_3D>(sorted_pts[i], sorted_pts[i + 1]));
	}
	return ret_linseg;
}


vector<polygon> nearest_neighbour(const polygon& grid_pts, float offset, const polygonSet& polygonset, float slope_infill, int layernum, float infill_angle)
{
	//vector<polygon> sorted_grid;
	vector<polygon> layer_polygons;

	polygon new_layer_pts;

	polygon sorted_layer_pts;

	new_layer_pts.clear();
	new_layer_pts = grid_pts;

	if (new_layer_pts.size() != 0)
	{
		new_layer_pts.erase(new_layer_pts.begin());
		sorted_layer_pts.push_back(grid_pts[0]);
	}


	int vals = 0;
	int count = 0;
	for (int i = 0; i < grid_pts.size(); i++)
	{

		float dist = 100;
		int index = 0;
		bool dist_bool;
		bool nearest_pt_exists = false;
		float dist_1;
		float dist_2;
		Point_3D pt(0, 0, 0);

		if (sorted_layer_pts.size() != 0)
			pt = sorted_layer_pts[sorted_layer_pts.size() - 1].second;



		for (int j = 0; j < new_layer_pts.size(); j++)
		{
			float c;
			if ((infill_angle == 90 && layernum % 2 == 0) || (infill_angle == 0 && layernum % 2 == 1))
			{
				c = (new_layer_pts[j].first.x) -
					(pt.x);
			}

			else if ((infill_angle == 0 && layernum % 2 == 0) || (infill_angle == 90 && layernum % 2 == 1))
			{
				c = (new_layer_pts[j].first.y) -
					(pt.y);
			}

			else
			{
				c = ((new_layer_pts[j].first.y - slope_infill * new_layer_pts[j].first.x) -
					(pt.y - slope_infill * pt.x));
			}

			if (abs(abs(c) - offset / 2) < 0.1)
			{


				/*Iterate to all the linesegments in a plane and find the distance of the current tail to the head and tail of all the line segments
				Find minimum of that distance and store the line segment. Then carryon the same search with the rest of the line segment pairs.*/

				/*dist_1 = (sorted_layer_pts[i].second*new_layer_pts[j].first);
				dist_2 = (sorted_layer_pts[i].second*new_layer_pts[j].second);
				*/
				dist_1 = (pt*new_layer_pts[j].first);
				dist_2 = (pt*new_layer_pts[j].second);

				if (dist_1 <= 1.25*offset || dist_2 <= 1.25*offset)
				{


					nearest_pt_exists = true;
					if (dist_1 >= dist_2)
					{
						if (dist >= dist_2)
						{
							dist = dist_2;
							index = j;
							dist_bool = true;
						}

					}
					else
					{
						if (dist >= dist_1)
						{
							dist = dist_1;
							index = j;
							dist_bool = false;

						}
					}
				}
			}

		}
		bool real_nearest_point = false;

		if (nearest_pt_exists)
		{
			if (dist_bool)
			{
				if (dist > 0.1 && sorted_layer_pts.size() != 0)
				{
					auto lnseg = sorted_layer_pts[sorted_layer_pts.size() - 1];
					auto lseg = pair<Point_3D, Point_3D>(lnseg.second, (new_layer_pts[index]).second);

					if (!polygon_operations::polygon_line_intersection(lseg, polygonset))
					{
						sorted_layer_pts.push_back(lseg);
						real_nearest_point = true;
					}
				}
			}

			else
			{
				if (dist > 0.1 && sorted_layer_pts.size() != 0)
				{
					auto lnseg = sorted_layer_pts[sorted_layer_pts.size() - 1];
					auto lseg = pair<Point_3D, Point_3D>(lnseg.second, (new_layer_pts[index]).first);
					if (!polygon_operations::polygon_line_intersection(lseg, polygonset))
					{
						sorted_layer_pts.push_back(lseg);
						real_nearest_point = true;
					}
				}
			}
		}

		if (real_nearest_point) /*If nearest point exists then check if you have to swap the
								coordinates of the point and then you push the point pair into vector and remove it from furthur searching.
								*/
		{

			sorted_layer_pts.push_back(new_layer_pts[index]);

			if (new_layer_pts.size() != 0)
				new_layer_pts.erase(new_layer_pts.begin() + index);

			if (dist_bool)
			{
				int sz = sorted_layer_pts.size();
				swap(sorted_layer_pts[sz - 1].first, sorted_layer_pts[sz - 1].second);

			}

		}

		if ((!real_nearest_point || !nearest_pt_exists) && new_layer_pts.size() != 0)//if (i == sorted_layer_pts.size();  

		{					/*When nearest point is not found it os considered that the loop exits and new loop starts so push the vector of poin pairs and clear it for furthur storage.
							Take the first coordinate and start searching as said above.*/


			layer_polygons.push_back(vector<pair<Point_3D, Point_3D>>(sorted_layer_pts.begin() + count, sorted_layer_pts.end()));
			//vals = i + 1;
			count = sorted_layer_pts.size();

			//cout<< "there is a loop inside the other"<<layer_number<<endl;

			sorted_layer_pts.push_back(new_layer_pts[0]);
			new_layer_pts.erase(new_layer_pts.begin());
		}

		if (i == grid_pts.size() - 1)  /*Filter off all the polyons with size less 2 as they are invalid. if exists.*/
		{
			layer_polygons.push_back(vector<pair<Point_3D, Point_3D>>(sorted_layer_pts.begin() + count, sorted_layer_pts.end()));
		}

	}
	//sorted_grid.push_back(sorted_layer_pts);

	return layer_polygons;
}