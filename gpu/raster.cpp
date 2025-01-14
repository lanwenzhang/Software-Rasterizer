#include "raster.h"
#include "../math/math.h"
#include "dataStructures.h"

Raster::Raster(){}

Raster::~Raster(){}

void Raster::rasterize(std::vector<VsOutput>& results, const uint32_t& drawMode, std::vector<VsOutput>& inputs) {

	if (drawMode == DRAW_LINES) {
		for (uint32_t i = 0; i < inputs.size(); i += 2) {
			rasterizeLine(results, inputs[i], inputs[i + 1]);
		}
	}

	if (drawMode == DRAW_TRIANGLES) {
		for (uint32_t i = 0; i < inputs.size(); i += 3) {
			rasterizeTriangle(results, inputs[i], inputs[i + 1], inputs[i + 2]);
		}
	}

}

void Raster::rasterizeLine(std::vector<VsOutput>& results, const VsOutput& v0, const VsOutput& v1) {

	VsOutput start = v0;
	VsOutput end = v1;

	// 1 Make sure v0.x < v1.x
	if (start.mPosition.x > end.mPosition.x) {
		auto tmp = start;
		start = end;
		end = tmp;
	}

	results.push_back(start);

	// 2 Make sure v0.y < v1.y
	bool flipY = false;
	if (start.mPosition.y > end.mPosition.y) {
		start.mPosition.y *= -1.0f;
		end.mPosition.y *= -1.0f;
		flipY = true;
	}

	// 3 Make sure the slope > 0
	int deltaX = static_cast<int>(end.mPosition.x - start.mPosition.x);
	int deltaY = static_cast<int>(end.mPosition.y - start.mPosition.y);

	bool swapXY = false;
	if (deltaX < deltaY) {
		std::swap(start.mPosition.x, start.mPosition.y);
		std::swap(end.mPosition.x, end.mPosition.y);
		std::swap(deltaX, deltaY);
		swapXY = true;
	}

	// 4 Brensenham
	int currentX = static_cast<int>(start.mPosition.x);
	int currentY = static_cast<int>(start.mPosition.y);

	int resultX = 0;
	int resultY = 0;

	VsOutput currentVsOutput;
	int p = 2 * deltaY - deltaX;

	for (int i = 0; i < deltaX; ++i) {
		if (p >= 0) {
			currentY += 1;
			p -= 2 * deltaX;
		}

		currentX += 1;
		p += 2 * deltaY;

		

		resultX = currentX;
		resultY = currentY;
		if (swapXY) {
			std::swap(resultX, resultY);
		}

		if (flipY) {
			resultY *= -1;
		}

	
		currentVsOutput.mPosition.x = resultX;
		currentVsOutput.mPosition.y = resultY;

		interpolateLine(start, end, currentVsOutput);

		results.push_back(currentVsOutput);
	}

}

void Raster::interpolateLine(const VsOutput& v0, const VsOutput& v1, VsOutput& target) {

	float weight = 1.0f;
	if (v1.mPosition.x != v0.mPosition.x) {
		
		weight = (float)(target.mPosition.x - v0.mPosition.x) / (float)(v1.mPosition.x - v0.mPosition.x);
	}
	else if (v1.mPosition.y != v0.mPosition.y) {
		
		weight = (float)(target.mPosition.y - v0.mPosition.y) / (float)(v1.mPosition.y - v0.mPosition.y);
	}

	target.mColor = math::lerp(v0.mColor, v1.mColor, weight);
	target.mUV = math::lerp(v0.mUV, v1.mUV, weight);

}

void Raster::rasterizeTriangle(std::vector<VsOutput>& results, const VsOutput& v0, const VsOutput& v1, const VsOutput& v2){


	// 1 Calculate the boundary
	int maxX = static_cast<int>(std::max(v0.mPosition.x, std::max(v1.mPosition.x, v2.mPosition.x)));
	int minX = static_cast<int>(std::min(v0.mPosition.x, std::min(v1.mPosition.x, v2.mPosition.x)));
	int maxY = static_cast<int>(std::max(v0.mPosition.y, std::max(v1.mPosition.y, v2.mPosition.y)));
	int minY = static_cast<int>(std::min(v0.mPosition.y, std::min(v1.mPosition.y, v2.mPosition.y)));

	// 2 Check if the point is inside the triangle
	math::vec2f pv0, pv1, pv2;
	VsOutput result;
	for (int i = minX; i <= maxX; ++i) {
		for (int j = minY; j <= maxY; ++j) {
			pv0 = math::vec2f(v0.mPosition.x - i, v0.mPosition.y - j);
			pv1 = math::vec2f(v1.mPosition.x - i, v1.mPosition.y - j);
			pv2 = math::vec2f(v2.mPosition.x - i, v2.mPosition.y - j);

			auto cross1 = math::cross(pv0, pv1);
			auto cross2 = math::cross(pv1, pv2);
			auto cross3 = math::cross(pv2, pv0);

			bool negativeAll = cross1 < 0 && cross2 < 0 && cross3 < 0;
			bool positiveAll = cross1 > 0 && cross2 > 0 && cross3 > 0;

			if (negativeAll || positiveAll) {
				result.mPosition.x = i;
				result.mPosition.y = j;
				interpolateTriangle(v0, v1, v2, result);

				results.push_back(result);
			}
		}
	}

}

void Raster::interpolateTriangle(const VsOutput& v0, const VsOutput& v1, const VsOutput& v2, VsOutput& p){

	auto e1 = math::vec2f(v1.mPosition.x - v0.mPosition.x, v1.mPosition.y - v0.mPosition.y);
	auto e2 = math::vec2f(v2.mPosition.x - v0.mPosition.x, v2.mPosition.y - v0.mPosition.y);
	float sumArea = std::abs(math::cross(e1, e2));

	auto pv0 = math::vec2f(v0.mPosition.x - p.mPosition.x, v0.mPosition.y - p.mPosition.y);
	auto pv1 = math::vec2f(v1.mPosition.x - p.mPosition.x, v1.mPosition.y - p.mPosition.y);
	auto pv2 = math::vec2f(v2.mPosition.x - p.mPosition.x, v2.mPosition.y - p.mPosition.y);


	float v0Area = std::abs(math::cross(pv1, pv2));
	float v1Area = std::abs(math::cross(pv0, pv2));
	float v2Area = std::abs(math::cross(pv0, pv1));

	float weight0 = v0Area / sumArea;
	float weight1 = v1Area / sumArea;
	float weight2 = v2Area / sumArea;

	p.mColor = math::lerp(v0.mColor, v1.mColor, v2.mColor, weight0, weight1, weight2);
	p.mUV = math::lerp(v0.mUV, v1.mUV, v2.mUV, weight0, weight1, weight2);

}
