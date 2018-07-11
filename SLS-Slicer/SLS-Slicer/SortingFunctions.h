#pragma once
#include "libraries.h"
#include "CustomTypeDefinitions.h"
#include "SlicerParameters.h"
#include "GlobalVariables.h"
#include "PolygonOperations.h"


/// <summary>
/// Create connected polygon set for each layer
/// </summary>
/// <param name="unrefined_layers"> unsorted and disconnected line segment collection for all layers </param>
/// <returns> Collection of polygon_set(all polygons for that layer) for every layer </returns>
vector<polygonSet> sort_contours(std::vector<polygon>& unrefined_layers)
{
	std::vector<polygon>layer_polygonset;
	polygon poly;
	std::vector<std::vector<polygon>> all_layers;

	for (auto layer : unrefined_layers)
	{
		if (layer.size() != 0)
			poly.push_back(layer[0]);
		int value = (layer.size() - 1);
		for (int i = 0; i < value; i++)
		{
			int index = 0;
			float dist = 1000;
			float min_dist = 1000;
			for (int j = i + 1; j < layer.size(); j++)
			{
				dist = layer[i].second*layer[j].first;
				if (dist < min_dist)
				{
					min_dist = dist;
					index = j;
				}
			}
			auto temp = layer[index];
			layer[index] = layer[i + 1];
			layer[i + 1] = temp;


			if (min_dist <= accuracy)
			{
				poly.push_back(layer[i + 1]);
			}
			else
			{
				layer_polygonset.push_back(poly);
				poly.clear();
				//std::cout << poly.size();
				poly.push_back(layer[i + 1]);
			}
			if (i == layer.size() - 2)
			{
				layer_polygonset.push_back(poly);
				poly.clear();
			}
		}
		all_layers.push_back(layer_polygonset);
		layer_polygonset.clear();
	}

	auto sorted_layers = polygon_operations::cleanpolygons(all_layers);

	return sorted_layers;
}


/// <summary>
/// label the polygons if they are interior boundaries or exterior boundaries.
/// If interior then key assigned is 1
/// If exterior key assigned is 0
/// </summary>
/// <param name="sorted_layers"> Collection of sorted polygon_set which is cleaned for overlapping lines </param>
/// <returns name = "final_sorted_labelled_data"> Created a labelled version of the sorted polygon_set </returns>
sorted_polySet LabelPolygons(vector<polygonSet> sorted_layers) {

	sorted_polySet final_sorted_labelled_data;
	labelled_polygonSet layer_labelled_data;

	for (auto layer : sorted_layers)
	{
		for (int i = 0; i < layer.size(); i++)
		{
			int number_inner_polygons = 0;
			ClipperLib::Path sub;

			std::reverse(layer[i].begin(), layer[i].end());
			for (int l = 0; l<layer[i].size(); l++)
			{
				/*auto pt =layer[i][l].first;
				layer[i][l].first = layer[i][l].second;
				layer[i][l].second = pt;*/
				std::swap(layer[i][l].first, layer[i][l].second);
			}


			for (auto& lseg : layer[i])	//lseg is a line segment (pair of 3d points)
			{

				sub << ClipperLib::IntPoint(clipper_scale*lseg.second.x, clipper_scale*lseg.second.y);
			}

			layer_labelled_data.push_back(std::pair<int, polygon>(ClipperLib::Orientation(sub), layer[i]));

		}
		final_sorted_labelled_data.push_back(layer_labelled_data);
		layer_labelled_data.clear();
	}
	return final_sorted_labelled_data;
}


/// <summary>
/// Within a layer group every outer polygon with all their inner polygons and do this operation for all layers
/// </summary>
/// <param name="labelled_polygons"> Labelled polygons for every layer labelled if its inner polygon or outer</param>
/// <returns> Grouped polygon_set for all layers </returns>
vector<vector<polygonSet>> GroupPolygons(sorted_polySet labelled_polygons){

	vector<vector<polygonSet>> all_layers;

	for (auto& layer : labelled_polygons)
	{
		polygonSet outerset, innerset;
		polygonSet newpairs;
		polygon temp_polygon;
		vector<polygonSet> layersets;
		for(auto& poly : layer)
		{

			if (poly.first == 1)
			{
				outerset.push_back(poly.second);
			}

			else
			{
				innerset.push_back(poly.second);
			}

		}
		for (int i = 0; i < outerset.size(); i++)
		{
			newpairs.push_back(outerset[i]);
			for (int j = 0; j < innerset.size() && innerset[j].size() != 0; j++)
			{
				ClipperLib::Path sub;
				for(auto& lseg : outerset[i])	//lseg is a line segment (pair of 3d points)			
					sub << ClipperLib::IntPoint(clipper_scale*lseg.second.x, clipper_scale*lseg.second.y);
				ClipperLib::IntPoint pt(clipper_scale*innerset[j][0].first.x, clipper_scale*innerset[j][0].first.y);
				if (ClipperLib::PointInPolygon(pt, sub) == 1)
				{
					newpairs.push_back(innerset[j]);
					innerset.erase(innerset.begin() + j);
					j--;
				}

			}
			outerset.erase(outerset.begin() + i);
			i--;
			layersets.push_back(newpairs);
			newpairs.clear();
		}
		if (innerset.size() != 0)
		{
			std::cout << " Error: some inner perimeters doesn't come insideany outer perimeters" << std::endl;

		}
		all_layers.push_back(layersets);
		layersets.clear();
	}
	return all_layers;
}