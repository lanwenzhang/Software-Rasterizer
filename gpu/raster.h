#pragma once
#include "../global/base.h"
#include "dataStructures.h"

class Raster {

public:

	Raster();
	~Raster();

	static void rasterize(std::vector<VsOutput>& results, const uint32_t& drawMode, std::vector<VsOutput>& inputs);

private:

	static void rasterizeLine(std::vector<VsOutput>& results, const VsOutput& v0, const VsOutput& v1);
	
	static void interpolateLine(const VsOutput& v0, const VsOutput& v1, VsOutput& target);

	static void rasterizeTriangle(std::vector<VsOutput>& results, const VsOutput& v0, const VsOutput& v1, const VsOutput& v2);

	static void interpolateTriangle(const VsOutput& v0, const VsOutput& v1, const VsOutput& v2, VsOutput& p);

};