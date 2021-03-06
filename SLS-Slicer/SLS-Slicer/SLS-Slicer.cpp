// SLS-Slicer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// SlicerParametes data type
#include "SlicerParameters.h"

// ReadSettings function call
#include "Initialisation.h"

// STLParser function call
#include "STLParser.h"

// CoreFunction function call
#include "CoreFunction.h"

int main()
{
	vector<SlicerParameters> parameters = ReadSettings("D:\\Tvasta\\SLS-Slicer\\SLS-Slicer\\SLS-Slicer\\x64\\Debug\\settings.tcf");

	//string filePath = "D:\\Tvasta\\Models\\40mm_Cube_Test_Object\\40mmcube.stl";
	//string filePath = "D:\\Tvasta\\Models\\Steps.stl";
	string filePath = "D:\\Tvasta\\Models\\syringe_mech.stl";
	Model_3D stl_model = STLParser(filePath);
	
	CoreFunction(stl_model, parameters[0]);
	
	
	
	
	return 0;
}

