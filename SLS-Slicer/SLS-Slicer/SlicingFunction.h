#pragma once
#include "libraries.h"
#include "CustomTypeDefinitions.h"
#include "SlicerParameters.h"
#include "GlobalVariables.h"
#include "PolygonOperations.h"

/// <summary>
/// Function Declarations
/// </summary>
void FindSurfaceLayers(Model_3D* stl_model, vector<Triangle>* required_triangles);
vector<Triangle> GetIntersectingTriangles(int& count, float z_value, vector<Triangle>& required_triangles);



/// <summary>
/// Find all intersecting line segments and store them as polygon.  Do this for all layers
/// </summary>
/// <param name="parameter"></param>
/// <param name="stl_model"></param>
/// <returns></returns>
vector<polygon> slice_by_planes(const SlicerParameters& parameter, Model_3D& stl_model)
{
	//
	float z_start = 0, z_end = 40;
	
	surface_z_values.clear();
	
	// Create a vector of polygons to store all the polygons cooresponding to the solid.
	// Each polygon is collection of line segments in a layer not connected or sorted
	vector<polygon> unrefined_layers;

	// Remove all the triangles which have same x, y and z coordinates as they are useless for calculating the slices. 
	// Vector to store the triangles used in slicing ie, removed triangles which lie on the slicing plane
	vector<Triangle> required_triangles; 

	// Track all surface layer and remove those from the triangles not required in slicing list
	FindSurfaceLayers(&stl_model, &required_triangles);
	
	polygon layer_pts;
	vector<Triangle> intersecting_triangles;
	Point_3D intersection_pt1, intersection_pt2, intersection_pt;
	int count = 0;

	for (float z_value = z_start; z_value <= z_end; z_value += parameter.layer_height)
	{
		layer_pts.clear();

		intersecting_triangles.clear();

		intersecting_triangles = GetIntersectingTriangles(count, z_value, required_triangles);
		
		float set = 0;
		bool is_steep = false;
		float max_slope = 0;
		
		for (const auto& triangle : intersecting_triangles)
		{
			// No vertex is on on the cutting plane
			if (triangle.v1.z != z_value && triangle.v2.z != z_value && triangle.v3.z != z_value)  
			{
				// If no vertex is on cutting plane then the plane intersects the triangle at two points.
				// Check which two edges the plane is cutting and then find the intersection point using the fucntion above.
				if ((triangle.v1.z - z_value)*(triangle.v2.z - z_value) > 0)
				{

					intersection_pt1 = polygon_operations::intersection_point(triangle.v1, triangle.v3, z_value);

					intersection_pt2 = polygon_operations::intersection_point(triangle.v2, triangle.v3, z_value);

					if (triangle.v3.z > z_value)
						layer_pts.push_back(pair<Point_3D, Point_3D>(intersection_pt2, intersection_pt1));      
						// The pair's first coordinate termed as head and second coordinate 
						// termed as tail are obtained using the right hand rule. So when polygon is formed
						// the outer polygon has a normal along +ve Z axis and inner loops have along -ve Z axis. Brep convention.
					else
						layer_pts.push_back(pair<Point_3D, Point_3D>(intersection_pt1, intersection_pt2));



				}
				else if ((triangle.v2.z - z_value)*(triangle.v3.z - z_value) > 0)
				{
					intersection_pt1 = polygon_operations::intersection_point(triangle.v3, triangle.v1, z_value);

					intersection_pt2 = polygon_operations::intersection_point(triangle.v2, triangle.v1, z_value);

					if (triangle.v1.z > z_value)
						layer_pts.push_back(pair<Point_3D, Point_3D>(intersection_pt1, intersection_pt2));
					else
						layer_pts.push_back(pair<Point_3D, Point_3D>(intersection_pt2, intersection_pt1));



				}
				else
				{
					intersection_pt1 = polygon_operations::intersection_point(triangle.v1, triangle.v2, z_value);

					intersection_pt2 = polygon_operations::intersection_point(triangle.v3, triangle.v2, z_value);

					if (triangle.v2.z > z_value)
						layer_pts.push_back(pair<Point_3D, Point_3D>(intersection_pt1, intersection_pt2));
					else
						layer_pts.push_back(pair<Point_3D, Point_3D>(intersection_pt2, intersection_pt1));

				}
			}
			else             //If one or two points are cutting the polygon
			{
				// Single point intersection
				// When one point is on the plane then check if other two veertices lie on either side of the plane. IF yes then find the point of intersection.
				// Again follow right hand rule Brep convetion to order the pair. 

				if (triangle.v1.z == z_value && (triangle.v2.z - z_value)*(triangle.v3.z - z_value) < 0)
				{
					if (triangle.v2.z > z_value)
						layer_pts.push_back(pair<Point_3D, Point_3D>(triangle.v1, polygon_operations::intersection_point(triangle.v2, triangle.v3, z_value)));
					else
						layer_pts.push_back(pair<Point_3D, Point_3D>(polygon_operations::intersection_point(triangle.v2, triangle.v3, z_value), triangle.v1));

				}

				if (triangle.v2.z == z_value && (triangle.v1.z - z_value)*(triangle.v3.z - z_value) < 0)
				{
					if (triangle.v1.z > z_value)
						layer_pts.push_back(pair<Point_3D, Point_3D>(polygon_operations::intersection_point(triangle.v3, triangle.v1, z_value), triangle.v2));
					else
						layer_pts.push_back(pair<Point_3D, Point_3D>(triangle.v2, polygon_operations::intersection_point(triangle.v3, triangle.v1, z_value)));

				}

				if (triangle.v3.z == z_value && (triangle.v1.z - z_value)*(triangle.v2.z - z_value) < 0)
				{
					if (triangle.v1.z > z_value)
						layer_pts.push_back(pair<Point_3D, Point_3D>(triangle.v3, polygon_operations::intersection_point(triangle.v1, triangle.v2, z_value)));
					else
						layer_pts.push_back(pair<Point_3D, Point_3D>(polygon_operations::intersection_point(triangle.v1, triangle.v2, z_value), triangle.v3));

				}


				// Two point (edge) coincidence with the plane

				// When two points lie on the plane, you need to be l'll careful. It has all the data of contours on the top bottom layers and
				// any other intermediate layers with parallel surface along xy plane. In such a case you need to consider only triangles with other vertex lying below the plane.
				// All the triangles whose other vertex lie above that plane is considered in the next layers.
				// Again follow right hand rule Brep convetion to order the pair.

				if (triangle.v1.z == z_value && triangle.v2.z == z_value)
				{
					bool z_value_is_in_plane = false;

					for (int a = 0; a<surface_z_values.size(); a++)
					{
						if ((z_value - surface_z_values[a]) < 0.01)
						{
							z_value_is_in_plane = true;
							break;
						}


					}
					if (!z_value_is_in_plane || triangle.v3.z < z_value)
					{
						if (triangle.v3.z > z_value)
							layer_pts.push_back(pair<Point_3D, Point_3D>(triangle.v2, triangle.v1));
						else
							layer_pts.push_back(pair<Point_3D, Point_3D>(triangle.v1, triangle.v2));

					}


				}


				if (triangle.v2.z == z_value && triangle.v3.z == z_value)
				{
					bool z_value_is_in_plane = false;

					for (int a = 0; a<surface_z_values.size(); a++)
					{
						if ((z_value - surface_z_values[a])<0.01)
						{
							z_value_is_in_plane = true;
							break;
						}
					}
					if (!z_value_is_in_plane || triangle.v1.z < z_value)
					{
						if (triangle.v1.z > z_value)
							layer_pts.push_back(pair<Point_3D, Point_3D>(triangle.v3, triangle.v2));
						else
							layer_pts.push_back(pair<Point_3D, Point_3D>(triangle.v2, triangle.v3));
					}

				}


				if (triangle.v3.z == z_value && triangle.v1.z == z_value)
				{
					bool z_value_is_in_plane = false;

					for (int a = 0; a<surface_z_values.size(); a++)
					{
						if ((z_value - surface_z_values[a])<0.01)
						{
							z_value_is_in_plane = true;
							break;
						}
					}
					if (!z_value_is_in_plane || triangle.v2.z < z_value)
					{
						if (triangle.v2.z > z_value)
							layer_pts.push_back(pair<Point_3D, Point_3D>(triangle.v1, triangle.v2));
						else
							layer_pts.push_back(pair<Point_3D, Point_3D>(triangle.v3, triangle.v1));
					}


				}


			}

		}
		
		unrefined_layers.push_back(layer_pts);  /*Store all the layer line segments*/
	}

	return unrefined_layers;

}

/// <summary>
/// Find all the layer which have triangle in the slicing plane
/// </summary>
/// <param name="stl_model"></param>
/// <param name="required_triangles"></param>
void FindSurfaceLayers(Model_3D* stl_model, vector<Triangle>* required_triangles) {
	
	for (auto& triangle : stl_model->elements)
	{
		if (!(abs(triangle.v1.z - triangle.v2.z) < accuracy && abs(triangle.v1.z - triangle.v1.z) < accuracy))
		{
			// Storing required triangles here
			required_triangles->push_back(triangle);
		}
		else
		{
			bool new_surface_z = true;

			for (int a = 0; a < surface_z_values.size(); a++)
			{
				// Here we are storing all the z values of planes which have same x,y,z coordinates.
				// These parts of the solid model has to have a different infill pattern form the interior.
				// And also the triangles whcih are having two vertice on these planes are not considered "while slicing in that layer" as they create intersecting contours.
				if (abs(triangle.v1.z - surface_z_values[a]) < accuracy)
					new_surface_z = false;
			}
			if (new_surface_z)
				surface_z_values.push_back(triangle.v1.z);
		}
	}
}


/// <summary>
/// Find all triangle which are intersecting a given plane as z_value
/// </summary>
/// <param name="count"> Starting point to search for from the whole triangle list</param>
/// <param name="z_value"> layer for which intersecting triangle are found fro</param>
/// <param name="required_triangles"> Full triangle list to search from</param>
/// <returns> Collection of triangle which are intesecting a given plane </returns>
vector<Triangle> GetIntersectingTriangles(int& count, float z_value, vector<Triangle>& required_triangles) {
	
	vector<Triangle> intersecting_triangles;
	intersecting_triangles.clear();

	for (int b = count; b < required_triangles.size(); b++)
	{

		auto element = required_triangles[b];
		// Sort it by max_z values for all same min_z value
		if (element.max_z < z_value)
		{
			if (b != count)
				swap(required_triangles[b], required_triangles[count]);
			count++;

		}
		else if (!(element.min_z > z_value)) /*Filter of the triangles whcih do not intersec the cuttting plane*/
		{
			intersecting_triangles.push_back(element);
		}
		else if (element.min_z > z_value)
			break;
	}
	return intersecting_triangles;
}
