#pragma once
#include "libraries.h"
#include "CustomTypeDefinitions.h"

/// <summary>
/// Function to read triangle data from an stl file.
/// ASCII File Format
///		Create an fstream instance and read the contents in the file word by word.
///		when the word facet is encountered enter a loop to store the values of normal components and triangle vertex coordinates.
///		When normal is read next three words are normal componnets
///		When the word vertex is read then next three words are vertex coordinates.
/// </summary>
/// <param name="stl_file"> File path of the stl_file </param>
/// <returns> Model_3D of the given stl file </returns>
Model_3D STLParser(string stl_file)
{
	Model_3D stl_model;
	vector<Triangle> stl_triangles;
	stl_triangles.clear();

	Point_3D pts[3];
	Vector_3D vct;
	Point_3D pt;
	Triangle tri(pt, pt, pt, vct);
	const float ext_limit = 1000000;

	stl_model.min_x = stl_model.min_y = stl_model.min_z = -ext_limit;
	stl_model.max_x = stl_model.max_y = stl_model.max_z = ext_limit;

	ifstream model_file(stl_file);
	if (!model_file.is_open())
	{
		cout << "File not found: " << stl_file << endl;
		exit(1);
	}
	else if (model_file.peek() == std::ifstream::traits_type::eof())
		return stl_triangles;
	std::string word;

	std::streampos start = model_file.tellg(), stop = model_file.tellg();
	
	// checking if file is of binary or ASCII type
	while (model_file >> word)
		if (word == "facet")
		{
			start = model_file.tellg();
			break;
		}

	while (model_file >> word)
		if (word == "endfacet")
		{
			stop = model_file.tellg();
			break;
		}

	if ((int)stop - (int)start > 80)	// it is in ASCII
	{
		model_file.seekg(0, std::ios::beg);
		while (model_file >> word)
		{
			if (word == "normal")
			{
				model_file >> vct.i >> vct.j >> vct.k;
				tri.n = vct;
				
				for (int i = 0; i < 3;)
				{
					model_file >> word;

					if (word == "vertex")
					{
						model_file >> pt.x >> pt.y >> pt.z;

						if (pt.x > stl_model.max_x)	stl_model.max_x = pt.x;
						if (pt.x < stl_model.min_x)	stl_model.min_x = pt.x;

						if (pt.y > stl_model.max_y)	stl_model.max_y = pt.y;
						if (pt.y < stl_model.min_y)	stl_model.min_y = pt.y;

						if (pt.z > stl_model.max_z)	stl_model.max_z = pt.z;
						if (pt.z < stl_model.min_z)	stl_model.min_z = pt.z;

						pts[i] = pt;
						//std::cout << "VERTEX" << std::endl;
						i++;
					}
				}
				tri.v1 = pts[0];
				tri.v2 = pts[1];
				tri.v3 = pts[2];
				stl_triangles.push_back(tri);
			}
		}
		model_file.close();
	}
	else	// it is in Binary
	{
		using namespace std;
		model_file.close();	// to re-open in binary mode
		ifstream bin_file(stl_file, ios::in | ios::binary);
		if (bin_file.is_open())
		{
			//std::cout << "\nBinary type stl.\n";
			char *temp = new char[4];
			bin_file.seekg(80, ios::beg);	// initial 80 bytes are insignificant
			bin_file.read(temp, 4);

			int num_triang = IEEEtoInt(temp);
			//cout << "\nNumber of triangles: " << num_triang << endl;
			// loop for getting the triangles' data
			for (int i = 0; i < num_triang; i++)
			{
				// ------getting the normal-------
				float norm[3];
				for (int j = 0; j < 3; j++)
				{
					bin_file.read(temp, 4);
					norm[j] = IEEEtoFloat(temp);
				}
				tri.n.i = norm[0];
				tri.n.j = norm[1];
				tri.n.k = norm[2];
				//--------- getting the 3 vertices
				for (int j = 0; j < 3; j++)
				{
					float coord[3];
					for (int k = 0; k < 3; k++)
					{
						bin_file.read(temp, 4);
						coord[k] = IEEEtoFloat(temp);
					}
					pt.x = coord[0];
					pt.y = coord[1];
					pt.z = coord[2];

					if (pt.x > stl_model.max_x)	stl_model.max_x = pt.x;
					if (pt.x < stl_model.min_x)	stl_model.min_x = pt.x;

					if (pt.y > stl_model.max_y)	stl_model.max_y = pt.y;
					if (pt.y < stl_model.min_y)	stl_model.min_y = pt.y;

					if (pt.z > stl_model.max_z)	stl_model.max_z = pt.z;
					if (pt.z < stl_model.min_z)	stl_model.min_z = pt.z;

					pts[j] = pt;
				}
				tri.v1 = pts[0];
				tri.v2 = pts[1];
				tri.v3 = pts[2];
				stl_triangles.push_back(tri);
				bin_file.seekg(2, std::ios_base::cur);	// attribute byte count
			}
			bin_file.close();
		}
	}

	stl_model.Store_triangles(stl_triangles);

	stl_model.base_center.x = (stl_model.min_x + stl_model.max_x) / 2;
	stl_model.base_center.y = (stl_model.min_y + stl_model.max_y) / 2;
	stl_model.base_center.z = stl_model.min_z;

	return stl_model;
}

/// <summary>
/// Binary to integer convertor
/// </summary>
/// <param name="ie"> Binary character </param>
/// <returns> Interger value of the binary character</returns>
int IEEEtoInt(char *ie)
{
	using namespace std;
	int val = 0;
	for (int i = 0; i < 4; i++)
	{
		for (int it = 0; it < 8; it++)
		{
			val += pow(2, 31 - i * 8 - it) * (int)((bitset<8>(ie[3 - i]))[7 - it]);
		}
	}
	return val;
}

/// <summary>
/// Binary to Floating point convertor
/// </summary>
/// <param name="ie"> Binary character </param>
/// <returns> Float value of the binary character</returns>
float IEEEtoFloat(char *ie)
{
	// refer to https://www3.ntu.edu.sg/home/ehchua/programming/java/datarepresentation.html
	using namespace std;
	float val, E = 0, F = 0;

	//cout << bitset<8>(ie[0]);

	// **** note: binary is stored in reverse manner (little Endian) | modify/add later for Big Endian
	//	applies to IEEEtoInt as well
	// get E value
	for (int it = 1; it < 8; it++)
	{
		E += pow(2, 8 - it) * (int)((bitset<8>(ie[3]))[7 - it]);
	}
	E += (int)((bitset<8>(ie[2]))[7]);

	// get F value

	for (int it = 1; it < 8; it++)
	{
		F += pow(2, -it) * (int)((bitset<8>(ie[2]))[7 - it]);
	}
	for (int it = 0; it < 8; it++)
	{
		F += pow(2, -8 - it) * (int)((bitset<8>(ie[1]))[7 - it]);
	}
	for (int it = 0; it < 8; it++)
	{
		F += pow(2, -16 - it) * (int)((bitset<8>(ie[0]))[7 - it]);
	}

	// calculate value
	if (E != 0)
		val = ((F + 1) * pow(2, E - 127)) * (1 - 2 * (int)((bitset<8>(ie[3]))[7]));
	else
		val = (F * pow(2, -126) * (1 - 2 * (int)((bitset<8>(ie[3]))[7])));

	return val;
}