#include "libraries.h"
#include "CustomTypeDefinitions.h"

namespace GCode /*G Code for 3D Printing*/
{
	void Write_GCode(vector<polygonSet> final_set) {
		cout << "writing GCODE" << endl;
		ofstream gfile("GCODE.txt");
		line_seg line_segment;
		if (gfile.is_open())
		{
			for (auto layer_set : final_set) {
				for (auto poly : layer_set) {
					if (poly.size() > 0) {
						gfile << "G92" << '\t' << "new\n";
						gfile << "G1" << '\t'
							<< "X" << poly[0].first.x << '\t'
							<< "Y" << poly[0].first.y << '\t'
							<< "Z" << poly[0].first.z << '\n';
					}
					for (auto line_segment : poly) {
						gfile << "G1" << '\t' 
							<< "X" << line_segment.second.x << '\t'
							<< "Y" << line_segment.second.y << '\t'
							<< "Z" << line_segment.second.z << '\n';
					}
				}
			}
		}
	}
}