#pragma once

#include "libraries.h"
#include "CustomCoordinates.h"

typedef pair<Point_3D, Point_3D> line_seg;
typedef vector<line_seg> polygon;
typedef vector<polygon> polygonSet;
typedef vector<pair<int, polygon>> labelled_polygonSet;
typedef vector<labelled_polygonSet> sorted_polySet;

class Triangle;
class Model_3D;

enum infill_type 
{
	rectilinear, 
	concentric 
};

/// <summary>
/// Creating a  triangle class containing  three vertices (point_3D objects) an done normal (Vector_3D) as its members.
/// </summary>
class Triangle
{
public:
	Point_3D v1, v2, v3;
	Vector_3D n;
	float min_z = 0;
	float max_z = 0;

	/// <summary>
	/// Operators to do basic sorting by hieght
	/// </summary>
	/// <param name="other"> Other Traingle object</param>
	/// <returns> True if first Triangle min_z small than second Triangle min_z </returns>
	bool operator< (const Triangle &other) const {
		return min_z < other.min_z;
	}

	/// <summary>
	/// Operators to do basic sorting by hieght
	/// </summary>
	/// <param name="other"> Other Traingle object</param>
	/// <returns> True if first Triangle and second Triangle are equal </returns>
	bool operator== (const Triangle &other) const {
		if (v1 == other.v1 && v2 == other.v2 && v3 == other.v3)
			return true;
		else
			return false;
	}

	/// <summary>
	/// Constructor for the triangle class
	/// </summary>
	Triangle(const Point_3D& pt1, const Point_3D& pt2, const Point_3D& pt3, const Vector_3D& normal) :v1(pt1), v2(pt2), v3(pt3), n(normal) {};

};

/// <summary>
/// 3D Model of a STL file stores as vector of triangles, model boundaries and base center of model
/// </summary>
class Model_3D
{
public:

	std::vector<Triangle> elements;

	float max_x, min_x, max_y, min_y, max_z, min_z;
	Point_3D base_center;

	Model_3D() {};

	Model_3D(std::vector<Triangle>& Elements) :elements(Elements) {};


	void Store_triangles(std::vector<Triangle>& triangle_list)
	{
		for(auto& triangle : triangle_list)
		{
			elements.push_back(triangle);
		}
	}
};
