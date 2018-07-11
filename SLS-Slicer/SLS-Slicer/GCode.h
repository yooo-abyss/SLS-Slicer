#include "libraries.h"
#include "CustomTypeDefinitions.h"

namespace GCode /*G Code for 3D Printing*/
{
	void Write_GCode(vector<polygonSet> final_set) {
		cout << "writing GCODE" << endl;
		ofstream gfile("GCODE.txt");
		if (gfile.is_open())
		{
			for (auto layer_set : final_set) {
				gfile << "G92" << '\t' << "new\n";
				for (auto poly : layer_set) {
					for (auto line_seg : poly) {
						gfile << "G1" << '\t' 
							<< "X" << line_seg.first.x << '\t'
							<< "Y" << line_seg.first.y << '\t'
							<< "Z" << line_seg.first.z << '\n';
					}
				}
			}
		}
	}
}