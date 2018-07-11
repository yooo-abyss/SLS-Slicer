#pragma once
#include "CustomTypeDefinitions.h"
#include "libraries.h"
#include "clipper.hpp"
#include "GlobalVariables.h"

const float OFFSET_THRESHOLD = 2;

namespace polygon_operations
{
	line_seg translate_lseg_toL(line_seg lsinv, float w) // translate ls by w to the left
	{
		// line segments stored as (head,tail), hence reverse
		line_seg ls = line_seg(lsinv.second, lsinv.first);
		float x1, x2, y1, y2; // co-ord of the translated points
		float ln = sqrt((ls.first.x - ls.second.x)*(ls.first.x - ls.second.x) + (ls.first.y - ls.second.y)*(ls.first.y - ls.second.y));
		if (ls.first.y>ls.second.y)
		{
			x1 = ls.first.x + w * abs(ls.second.y - ls.first.y) / ln;
			x2 = ls.second.x + w * abs(ls.second.y - ls.first.y) / ln;
		}
		else
		{
			x1 = ls.first.x - w * abs(ls.second.y - ls.first.y) / ln;
			x2 = ls.second.x - w * abs(ls.second.y - ls.first.y) / ln;
		}
		if (ls.second.x > ls.first.x)
		{
			y1 = ls.first.y + w * abs(ls.second.x - ls.first.x) / ln;
			y2 = ls.second.y + w * abs(ls.second.x - ls.first.x) / ln;
		}
		else
		{
			y1 = ls.first.y - w * abs(ls.second.x - ls.first.x) / ln;
			y2 = ls.second.y - w * abs(ls.second.x - ls.first.x) / ln;
		}
		return line_seg(Point_3D(x1, y1, ls.first.z), Point_3D(x2, y2, ls.second.z));
	}

	Point_3D get_extendPtAlongLine(float ext_len, line_seg& l)
	{
		//line_seg l = line_seg(linv.second, linv.first);
		// get a point along the line segment(p1 p2) at a disatnce ext_len from p2
		float x, y, len = sqrt(pow((l.first.x - l.second.x), 2) + pow((l.first.y - l.second.y), 2));
		x = l.second.x + ext_len * (l.second.y - l.first.y) / len;
		y = l.second.y + ext_len * (l.second.x - l.first.x) / len;
		return Point_3D(x, y, l.first.z);
	}

	Point_3D intersection_point(const Point_3D& p1, const Point_3D& p2, float z_value)  /*function to return the point of intersection of  a plane and linesegment joining two points*/
	{
		Point_3D intersection_pt1;
		float m;

		m = abs((z_value - p1.z) / (p2.z - p1.z));	/*find the ratio in which the plane divides the z coordiantes
															and divide x and y coordinates in the same ratio.*/


															/* if (m > 1)                           //Not possible as this case is filtered off just in case of debugging
															std::cout << "the given plane intersects the line externally " << std::endl;*/


		intersection_pt1.x = p1.x + m * (p2.x - p1.x);
		intersection_pt1.y = p1.y + m * (p2.y - p1.y);
		intersection_pt1.z = z_value;

		return intersection_pt1;
	}

	float x_intersection_2d(const Point_3D& p1, const Point_3D& p2, float y_value)
	{                                  /*Finding the point of intersection between two points by YZ plane. used this fucntion in figuring whether polygon is inside or outside later .*/
		float m;

		m = abs((y_value - p1.y) / (p2.y - p1.y));

		if (m > 1)
			std::cout << "Something's wrong" << std::endl;

		return(p1.x + m * (p2.x - p1.x));
	}

	bool line_line_intersection(line_seg l1, line_seg l2)
	{
		float intx, inty, denom;	// intx,inty: intersection point coordinates
									/* based on https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection */
		denom = (l1.first.x - l1.second.x)*(l2.first.y - l2.second.y) - (l2.first.x - l2.second.x)*(l1.first.y - l1.second.y);
		if (denom != 0)
		{
			intx = ((l1.first.x*l1.second.y - l1.second.x*l1.first.y)*(l2.first.x - l2.second.x) - (l2.first.x*l2.second.y - l2.second.x*l2.first.y)*(l1.first.x - l1.second.x)) / denom;
			inty = ((l1.first.x*l1.second.y - l1.second.x*l1.first.y)*(l2.first.y - l2.second.y) - (l2.first.x*l2.second.y - l2.second.x*l2.first.y)*(l1.first.y - l1.second.y)) / denom;

			float alpha1, alpha2;

			if (abs(l1.first.x - l1.second.x) < 0.01 && abs(l2.first.x - l2.second.x) < 0.01)
			{
				alpha1 = (inty - l1.first.y) / (l1.second.y - l1.first.y);
				alpha2 = (inty - l2.first.y) / (l2.second.y - l2.first.y);
			}
			else if (abs(l1.first.x - l1.second.x) > 0.01 && abs(l2.first.x - l2.second.x) < 0.01)
			{
				alpha1 = (intx - l1.first.x) / (l1.second.x - l1.first.x);
				alpha2 = (inty - l2.first.y) / (l2.second.y - l2.first.y);
			}
			else if (abs(l1.first.x - l1.second.x) < 0.01 && abs(l2.first.x - l2.second.x) > 0.01)
			{
				alpha1 = (inty - l1.first.y) / (l1.second.y - l1.first.y);
				alpha2 = (intx - l2.first.x) / (l2.second.x - l2.first.x);
			}
			else
			{
				alpha1 = (intx - l1.first.x) / (l1.second.x - l1.first.x);
				alpha2 = (intx - l2.first.x) / (l2.second.x - l2.first.x);
			}


			if (alpha1 <= 1 && alpha1 >= 0 &&
				alpha2 <= 1 && alpha2 >= 0)	// internal intersection at one point
			{
				return true;
			}
			// not clubbing the two to avoid calculation in case of internal intersection
			else if (sqrt((l1.second.x - intx)*(l1.second.x - intx) + (l1.second.y - inty)*(l1.second.y - inty)) <= OFFSET_THRESHOLD)	// does not intersect internally; external, but point not blown away
			{
				return false;
			}
			else	// intersection point is too far; cut off and add line segment
			{
				return false;
			}
		}
		else
		{
			if (abs(l1.first.x - l1.second.x) < 0.01) //if parallel lines are vertical
			{
				if (abs(l1.first.x - l2.first.x) < 0.01)
				{
					if (min(max(l1.first.y, l1.second.y), max(l2.first.y, l2.second.y)) < max(min(l1.first.y, l1.second.y), min(l2.first.y, l2.second.y)))
						return false;
					else
						return true;
				}
				else
					return false;

			}
			else
			{
				float slope1 = (l1.first.y - l1.second.y) / (l1.first.x - l1.second.x);
				float slope2 = (l2.first.y - l2.second.y) / (l2.first.x - l2.second.x);
				if (abs((l1.first.y - (slope1)*l1.first.x) - (l2.first.y - (slope2)*l2.first.x)) < 0.01)
				{
					if (min(max(l1.first.x, l1.second.x), max(l2.first.x, l2.second.x)) < max(min(l1.first.x, l1.second.x), min(l2.first.x, l2.second.x)))
						return false;
					else
						return true;
				}
				else
					return false;
			}
		}
		return false;
	}

	bool polygon_line_intersection(const line_seg& ls, const labelled_polygonSet& poly)
	{	// this function is used to chceck if the line segment intersects the given set of polygons	
		// just checking if given line intersects any of the polygon contained in the set of polygons
		// used by the rectilinear infill function
		// to get the linsegments joining the parallel lines drawn initally
		// only valid linsegments which are not intersecting the polygon are taken

		for(const auto& polyline : poly)
		{
			for(const auto& line : polyline.second)
			{
				if (line_line_intersection(ls, line))
					return true;
			}
		}

		return false;
	}

	bool polygon_line_intersection(const line_seg& ls, const polygonSet& poly)
	{
		// this function is used to chceck if the line segment intersects the given set of polygons	
		// just checking if given line intersects any of the polygon contained in the set of polygons
		// used by the rectilinear infill function
		// to get the linsegments joining the parallel lines drawn initally
		// only valid linsegments which are not intersecting the polygon are taken

		for (const auto& ply : poly)
		{
			for(const auto& line : ply)
			{
				if (line_line_intersection(ls, line))
					return true;
			}

		}

		return false;
	}

	std::vector<std::vector<polygon>> cleanpolygons(std::vector<std::vector<polygon>>& sorted_grid)
	{
		/*removing all those line segments which are collinear and merging them into one coordinate.
		Find consecutive linesegments whcih are having same slope or y coordinate and merge them into a single apir.
		easy for future steps and also while generating gcode*/

		for (int alp = 0; alp < sorted_grid.size(); alp++)
		{
			for (int bet = 0; bet < sorted_grid[alp].size(); bet++)
			{
				for (int i = 0; i < sorted_grid[alp][bet].size() && sorted_grid[alp][bet].size() >= 2; i++)
				{

					if (i != 0) {

						if (abs((sorted_grid[alp][bet][i - 1].first.x - sorted_grid[alp][bet][i].first.x)) < 0.01 && abs((sorted_grid[alp][bet][i].second.x - sorted_grid[alp][bet][i].first.x)) < 0.01)
						{

							sorted_grid[alp][bet][i - 1].second = sorted_grid[alp][bet][i].second;
							sorted_grid[alp][bet].erase(sorted_grid[alp][bet].begin() + i);
							i = i - 1;

						}
						else
						{
							if (abs((sorted_grid[alp][bet][i - 1].first.y - sorted_grid[alp][bet][i].first.y) /
								(sorted_grid[alp][bet][i - 1].first.x - sorted_grid[alp][bet][i].first.x) -
								(sorted_grid[alp][bet][i].second.y - sorted_grid[alp][bet][i].first.y) /
								(sorted_grid[alp][bet][i].second.x - sorted_grid[alp][bet][i].first.x)) < 0.1)
							{
								sorted_grid[alp][bet][i - 1].second = sorted_grid[alp][bet][i].second;
								sorted_grid[alp][bet].erase(sorted_grid[alp][bet].begin() + i);
								i = i - 1;
							}
						}

					}
					else
					{
						if (abs((sorted_grid[alp][bet][sorted_grid[alp][bet].size() - 1].first.x - sorted_grid[alp][bet][i].first.x)) < 0.01 &&
							abs((sorted_grid[alp][bet][i].second.x - sorted_grid[alp][bet][i].first.x)) < 0.01)
						{

							sorted_grid[alp][bet][sorted_grid[alp][bet].size() - 1].second = sorted_grid[alp][bet][i].second;
							sorted_grid[alp][bet].erase(sorted_grid[alp][bet].begin() + i);
							i = i - 1;

						}
						else
						{
							if (abs((sorted_grid[alp][bet][sorted_grid[alp][bet].size() - 1].first.y - sorted_grid[alp][bet][i].first.y) /
								(sorted_grid[alp][bet][sorted_grid[alp][bet].size() - 1].first.x - sorted_grid[alp][bet][i].first.x) -
								(sorted_grid[alp][bet][i].second.y - sorted_grid[alp][bet][i].first.y) /
								(sorted_grid[alp][bet][i].second.x - sorted_grid[alp][bet][i].first.x)) < 0.1)
							{
								sorted_grid[alp][bet][sorted_grid[alp][bet].size() - 1].second = sorted_grid[alp][bet][i].second;
								sorted_grid[alp][bet].erase(sorted_grid[alp][bet].begin() + i);
								i = i - 1;
							}
						}

					}

				}
				if (sorted_grid[alp][bet].size() <= 2)
				{
					sorted_grid[alp].erase(sorted_grid[alp].begin() + bet);
					bet = bet - 1;

				}

			}
		}
		return sorted_grid;
	}

	Point_3D point_ratio_m_n(const Point_3D& pt1, const Point_3D& pt2, float m, float n) /*returns point which devides the
																						 line segment joining two points in the ratio m:n*/
	{
		Point_3D pt;   /*function to find a point whcih divides the line segment in the ratio m:n */
		pt.x = (m*pt2.x + n * pt1.x) / (m + n);
		pt.y = (m*pt2.y + n * pt1.y) / (m + n);
		pt.z = pt1.z;
		return pt;
	}

	bool in_or_out(const Point_3D& pt, const polygon& polygon2)
	{
		// function to check if the given line is inside the polygon 
		// returns true if the point is on the polygon or inside the polygon
		// algorithm is standard ray tracing algorithm .


		/*If inside   return	true
		outside		return	fasle */

		int intersection_pts = 0;
		for (int i = 0; i < polygon2.size(); i++)
		{
			if ((pt.y - polygon2[i].first.y)*(pt.y - polygon2[i].second.y) < 0)
			{
				if (x_intersection_2d(polygon2[i].first, polygon2[i].second, pt.y)>pt.x)
					intersection_pts++;
			}

			else if ((pt.y - polygon2[i].first.y)*(pt.y - polygon2[i].second.y) == 0)
			{
				if ((pt.y - polygon2[i].first.y) == 0 && (pt.y - polygon2[i].second.y) != 0)
				{
					if (i == 0)
					{
						if ((pt.y - polygon2[polygon2.size() - 1].first.y)*(pt.y - polygon2[i].second.y) < 0)
						{

							if (x_intersection_2d(polygon2[i].first, polygon2[i].second, pt.y)>pt.x)
								intersection_pts++;
						}
					}

					else
					{
						if ((pt.y - polygon2[i - 1].first.y)*(pt.y - polygon2[i].second.y) < 0)
						{
							if (x_intersection_2d(polygon2[i].first, polygon2[i].second, pt.y)>pt.x)
								intersection_pts++;
						}
					}

					/**/

				}
				else if ((pt.y - polygon2[i].first.y) != 0 && (pt.y - polygon2[i].second.y) == 0)
				{
					if (i == polygon2.size() - 1)
					{
						if ((pt.y - polygon2[i].first.y)*(pt.y - polygon2[0].second.y) < 0)
						{
							if (x_intersection_2d(polygon2[i].first, polygon2[i].second, pt.y)>pt.x)
								intersection_pts++;
						}
					}
					else
					{
						if ((pt.y - polygon2[i].first.y)*(pt.y - polygon2[i + 1].second.y) < 0)
						{
							if (x_intersection_2d(polygon2[i].first, polygon2[i].second, pt.y)>pt.x)
								intersection_pts++;
						}

					}

				}

				else
				{
					//std::cout << "............The ray touches both the end points............" << std::endl;
					std::exit;
				}


			}
		}
		if (intersection_pts % 2 == 0)
			return false;
		if (intersection_pts % 2 == 1)
			return true;
	}

	int in_or_out(const Point_3D& pt, const labelled_polygonSet& layer_polygons)
	{ // function used to check if there exists material at the given point 


		int value = 0;

		for (const auto& poly : layer_polygons)
		{
			ClipperLib::Path subj;
			subj.clear();
			for(auto& lseg : poly.second)	//lseg is a line segment (pair of 3d points)			
				subj << ClipperLib::IntPoint(clipper_scale*lseg.second.x, clipper_scale*lseg.second.y);

			if (ClipperLib::PointInPolygon(ClipperLib::IntPoint(clipper_scale*pt.x, clipper_scale*pt.y), subj) == 1)
			{
				value++;
			}

		}
		return (value % 2);
	}

	int in_or_out(const Point_3D& pt, const polygonSet& layer_polygons)
	{
		int value = 0;

		for (const auto& poly : layer_polygons)
		{
			ClipperLib::Path subj;
			subj.clear();
			for(auto& lseg : poly)	//lseg is a line segment (pair of 3d points)			
				subj << ClipperLib::IntPoint(clipper_scale*lseg.second.x, clipper_scale*lseg.second.y);

			if (ClipperLib::PointInPolygon(ClipperLib::IntPoint(clipper_scale*pt.x, clipper_scale*pt.y), subj) == 1)
			{
				value++;
			}

		}
		return (value % 2);
	}

	polygonSet clippolygons(const polygonSet& polys1, const polygonSet& polys2, ClipperLib::ClipType cliptype, ClipperLib::PolyFillType pftsub, ClipperLib::PolyFillType pftclip)
	{
		float value_z = -1;
		using namespace ClipperLib;
		Paths sub1, sub2, solution;
		Path temp_sub;
		for(auto poly : polys1)
		{
			for(auto lseg : poly)	//lseg is a line segment (pair of 3d points)
			{
				temp_sub << ClipperLib::IntPoint(clipper_scale*lseg.second.x, clipper_scale*lseg.second.y);
				if (value_z == -1)
				{
					value_z = lseg.second.z;
				}
			}
			sub1.push_back(temp_sub);
			temp_sub.clear();
		}
		for (auto poly : polys2)
		{
			for (auto lseg : poly)	//lseg is a line segment (pair of 3d points)	
			{
				temp_sub << ClipperLib::IntPoint(clipper_scale*lseg.second.x, clipper_scale*lseg.second.y);
				if (value_z == -1)
				{
					value_z = lseg.second.z;
				}
			}
			sub2.push_back(temp_sub);
			temp_sub.clear();
		}
		Clipper c;
		c.AddPaths(sub1, ptSubject, true);
		c.AddPaths(sub2, ptClip, true);
		c.Execute(cliptype, solution, pftsub, pftclip);
		polygon temp_poly;
		polygonSet temp_layer;
		for (auto result_path : solution)
		{

			int num_pairs = result_path.size();
			for (int i = 0; i < num_pairs - 1 && value_z != -1; i++) // to avoid going beyond limit: i+1
			{
				Point_3D beginP = Point_3D(result_path[i].X / clipper_scale, result_path[i].Y / clipper_scale, value_z);
				Point_3D endP = Point_3D(result_path[i + 1].X / clipper_scale, result_path[i + 1].Y / clipper_scale, value_z);
				temp_poly.push_back(std::make_pair(beginP, endP));
			}
			//adding the last pair
			if (num_pairs != 0 && value_z != -1)
			{
				Point_3D beginP = Point_3D(result_path[num_pairs - 1].X / clipper_scale, result_path[num_pairs - 1].Y / clipper_scale, value_z);
				Point_3D endP = Point_3D(result_path[0].X / clipper_scale, result_path[0].Y / clipper_scale, value_z);
				temp_poly.push_back(std::make_pair(beginP, endP));
			}

			temp_layer.push_back(temp_poly);
			temp_poly.clear();

		}

		return temp_layer;
	}

	polygonSet polygonoffset(const polygonSet& polys1, float offset)
	{
		using namespace ClipperLib;
		Paths sub1, sub2, solution;
		Path temp_sub;
		for (auto& poly : polys1)
		{
			for(auto& lseg : poly)	//lseg is a line segment (pair of 3d points)			
				temp_sub << ClipperLib::IntPoint(clipper_scale*lseg.second.x, clipper_scale*lseg.second.y);
			sub1.push_back(temp_sub);
			temp_sub.clear();
		}
		ClipperOffset co;
		co.AddPaths(sub1, jtSquare, etClosedPolygon);
		co.Execute(solution, -offset * clipper_scale);
		polygon temp_poly;
		polygonSet temp_layer;
		ClipperLib::CleanPolygons(solution);

		for (const auto& result_path : solution)
		{

			int num_pairs = result_path.size();
			for (int i = 0; i < num_pairs - 1; i++) // to avoid going beyond limit: i+1
			{
				Point_3D beginP = Point_3D(result_path.at(i).X / clipper_scale, result_path.at(i).Y / clipper_scale, polys1[0][0].first.z);
				Point_3D endP = Point_3D(result_path.at(i + 1).X / clipper_scale, result_path.at(i + 1).Y / clipper_scale, polys1[0][0].first.z);
				temp_poly.push_back(std::make_pair(beginP, endP));
			}
			//adding the last pair
			if (num_pairs != 0)
			{
				Point_3D beginP = Point_3D(result_path.at(num_pairs - 1).X / clipper_scale, result_path.at(num_pairs - 1).Y / clipper_scale, polys1[0][0].first.z);
				Point_3D endP = Point_3D(result_path.at(0).X / clipper_scale, result_path.at(0).Y / clipper_scale, polys1[0][0].first.z);
				temp_poly.push_back(std::make_pair(beginP, endP));
			}

			temp_layer.push_back(temp_poly);
			temp_poly.clear();

		}
		return temp_layer;
	}

	std::vector<Triangle> Translate(Point_3D& point, const std::vector<Triangle>& elements)
	{
		std::vector<Triangle> new_elements;

		for (auto & tri_angle : elements)
		{
			Point_3D pt1(tri_angle.v1.x - point.x, tri_angle.v1.y - point.y, tri_angle.v1.z - point.z);
			Point_3D pt2(tri_angle.v2.x - point.x, tri_angle.v2.y - point.y, tri_angle.v2.z - point.z);
			Point_3D pt3(tri_angle.v3.x - point.x, tri_angle.v3.y - point.y, tri_angle.v3.z - point.z);
			Triangle tri(pt1, pt2, pt3, tri_angle.n);
			new_elements.push_back(tri);
		}
		return new_elements;
	}

};