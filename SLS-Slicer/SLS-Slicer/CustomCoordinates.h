#pragma once
#include "libraries.h"

class Vector_3D;
class Point_3D;

/// <summary>
/// Custom Class to store a 3D vector using i,j and k
/// Contains the following operations
/// Dot Product (*),
/// Cross Product (^),
/// Subtraction (-),
/// Scalar Multiplication (*),
/// Scalar Division (/)
/// </summary>
class Vector_3D
{
public:
	float i, j, k;

	/// <summary>
	/// Constructor for Vector_3D
	/// </summary>
	Vector_3D(float I = 0, float J = 0, float K = 0) : i(I), j(J), k(K) {};

	/// <summary>
	/// vector dot product represented by the operator 
	/// </summary>
	/// <param name="alp"> Second Vector_3D ie the Vector_3D in the right of the operator </param>
	/// <returns> Dot product value </returns>
	inline float operator *(const Vector_3D& alp)const
	{
		return (this->i*alp.i + this->j*alp.j + this->k*alp.k);
	}

	/// <summary>
	/// using the operator '^' as operator for vector cross product
	/// </summary>
	/// <param name="alp"> Second Vector_3D ie the Vector_3D in the right of the operator </param>
	/// <returns> Vector_3D Cross Product result</returns>
	inline Vector_3D operator ^(const Vector_3D& alp)const
	{
		return Vector_3D(j * alp.k - alp.j*k, alp.i*k - alp.k*i, i * alp.j - j * alp.i);
	}

	/// <summary>
	/// using the operator '-' as operator for creating point from difference of two vectors
	/// </summary>
	/// <param name="alp"> Second Vector_3D ie the Vector_3D in the right of the operator</param>
	/// <returns> Result of Vector Subtraction </returns>
	inline Vector_3D operator -(const Vector_3D& alp)const
	{
		return Vector_3D(i - alp.i, j - alp.j, k - alp.k);
	}


	/// <summary>
	/// operator '*' with a constant is used for scaling the components of vector 
	/// </summary>
	/// <param name="c"> Constant value to be multiplied for scaling</param>
	/// <returns> Scaled Vector_3D </returns>
	inline Vector_3D operator *(const float c)const
	{
		return Vector_3D(i*c, j*c, k*c);
	}


	/// <summary>
	/// operator '/' with a constant is used for scaling the components of vector 
	/// </summary>
	/// <param name="c"> Constant value to be divided for scaling</param>
	/// <returns> Scaled Vector_3D </returns>
	inline Vector_3D operator /(const float c)const  /* operator '*' with a constant is used for scaling the components of vector  */
	{
		return Vector_3D(i/c, j/c, k/c);
	}

};


/// <summary>
/// Custom Class to store a 3D point in space using x,y and z coordinates
/// Contains the following operations
/// Addition (+),
/// Euclidean Distance (*),
/// Scalar Multiplication (*),
/// Scalar Division (/),
/// Comparisons (== !=),
/// Vector Generation(-)
/// </summary>
class Point_3D
{
public:
	float x, y, z; 

	/// <summary>
	/// Constructor for Point_3D
	/// </summary>
	Point_3D(float X = 0, float Y = 0, float Z = 0) : x(X), y(Y), z(Z) {};

	/// <summary>
	/// Addition of two Point_3D using operator overloading 
	/// </summary>
	/// <param name="pt"> Second Point_3d ie the Point_3D on the right of the operator</param>
	/// <returns> Point_3D which is the sum of two Point_3D </returns>
	inline Point_3D operator +(const Point_3D& pt)const
	{
		return Point_3D(this->x + pt.x, this->y + pt.y, this->z + pt.z);
	}

	/// <summary>
	///  operator '*' is used for calcualting the euclidean distance between two points in space 
	/// </summary>
	/// <param name="beta"> Second Point_3d ie the Point_3D on the right of the operator </param>
	/// <returns> Euclidean distance between these 2 Point_3D</returns>
	inline float operator *(const Point_3D& beta)const
	{
		return (sqrt((x - beta.x)*(x - beta.x) + (y - beta.y)*(y - beta.y) + (z - beta.z)*(z - beta.z)));
	}

	/// <summary>
	/// operator '*' with a constant is used for scaling the coordinates of  points in space 
	/// </summary>
	/// <param name="c"> Constant value to be multiplied for scaling</param>
	/// <returns> Scaled Point_3d </returns>
	inline Point_3D operator *(const float c)const
	{
		return Point_3D(c*x, c*y, c*z);
	}

	/// <summary>
	/// operator '/' with a constant is used for scaling the coordinates of  points in space 
	/// </summary>
	/// <param name="c"> Constant value to be divided for scaling</param>
	/// <returns> Scaled Point_3d </returns>
	inline Point_3D operator /(const float c)const
	{
		return Point_3D(x/c, y/c, z/c);
	}

	/// <summary>
	/// Compares the 2 Point_3D to check if they are equal
	/// </summary>
	/// <param name="pt"> Second Point_3D ie the point on the right of the operator </param>
	/// <returns> Boolean true if equal, Boolean false if NOT equal</returns>
	inline bool operator ==(const Point_3D& pt)const
	{
		return (x == pt.x && y == pt.y && z == pt.z) ? true : false;
	}
	
	/// <summary>
	/// Compares the 2 Point_3D to check if they are NOT equal
	/// </summary>
	/// <param name="pt"> Second Point_3D ie the point on the right of the operator </param>
	/// <returns> Boolean true if NOT equal, Boolean false if equal</returns>
	inline bool operator !=(const Point_3D& pt)const
	{
		return (x == pt.x && y == pt.y && z == pt.z) ? false : true;
	}

	/// <summary>
	/// Generate Vector_3D between two points using operator overloading.
	/// </summary>
	/// <param name="pt"> Second Point_3D ie the point on the right of the operator </param>
	/// <returns> Unit Vector_3D from First Point_3D to second Point_3D </returns>
	Vector_3D operator -(const Point_3D& pt)const
	{
		Vector_3D temp;
		float norm = sqrt((this->x - pt.x)*(this->x - pt.x) + (this->y - pt.y)*(this->y - pt.y) + (this->z - pt.z)*(this->z - pt.z));
		temp.i = (this->x - pt.x) / norm;
		temp.j = (this->y - pt.y) / norm;
		temp.k = (this->z - pt.z) / norm;
		return temp;
	}
};

