#include "clipper.h"
#include "../math/math.h"


Clipper::Clipper(){}
Clipper::~Clipper(){}

void Clipper::doClipSpace(const uint32_t& drawMode, const std::vector<VsOutput>& primitives, std::vector<VsOutput>& outputs) {

	outputs.clear();

	// 1 Clip Triangles
	if (drawMode == DRAW_TRIANGLES) {
		std::vector<VsOutput> primitive;
		std::vector<VsOutput> results;

		
		for (uint32_t i = 0; i < primitives.size(); i += 3) {
			primitive.clear();
			results.clear();

			// 1.1 Get single triangle
			auto start = primitives.begin() + i;
			auto end = primitives.begin() + i + 3;
			primitive.assign(start, end);

			// 1,2 Do clip
			Clipper::sutherlandHodgman(drawMode, primitive, results);

			if (results.empty()) {
				continue;
			}

			// 1.3 Re-assemble the triangles if it is a polygon after clip
			for (uint32_t c = 0; c < results.size() - 2; ++c) {
				outputs.push_back(results[0]);
				outputs.push_back(results[c + 1]);
				outputs.push_back(results[c + 2]);
			}
		}
	}

	// 2 Clip lines
	else if (drawMode == DRAW_LINES) {
		std::vector<VsOutput> primitive;
		std::vector<VsOutput> results;

		for (uint32_t i = 0; i < primitives.size(); i += 2) {
			primitive.clear();
			results.clear();

			// 2.1 Get single line
			auto start = primitives.begin() + i;
			auto end = primitives.begin() + i + 2;
			primitive.assign(start, end);

			// 2.2 Do clip
			Clipper::sutherlandHodgman(drawMode, primitive, results);

			outputs.push_back(results[0]);
			outputs.push_back(results[1]);
		}
	}

}

void Clipper::sutherlandHodgman(const uint32_t& drawMode, const std::vector<VsOutput>& primitive, std::vector<VsOutput>& outputs) {

	assert(outputs.size() == 0);

	std::vector<math::vec4f> clipPlanes = {
		//judge w > 0
		math::vec4f(0.0f, 0.0f, 0.0f, 1.0f),
		//near
		math::vec4f(0.0f, 0.0f, 1.0f, 1.0f),
		//far
		math::vec4f(0.0f, 0.0f, -1.0f, 1.0f),
		//left
		math::vec4f(1.0f, 0.0f, 0.0f, 1.0f),
		//right
		math::vec4f(-1.0f, 0.0f, 0.0f, 1.0f),
		//top
		math::vec4f(0.0f, -1.0f, 0.0f, 1.0f),
		//bottom
		math::vec4f(0.0f, 1.0f, 0.0f, 1.0f)
	};

	outputs = primitive;
	std::vector<VsOutput> inputs;


	for (uint32_t i = 0; i < clipPlanes.size(); ++i) {

		inputs = outputs;
		outputs.clear();


		for (uint32_t p = 0; p < inputs.size(); ++p) {
	
			auto current = inputs[p];
		
			auto last = inputs[(p + inputs.size() - 1) % inputs.size()];

			
			if (inside(current.mPosition, clipPlanes[i])) {
			
				if (drawMode != DRAW_LINES || p != inputs.size() - 1) {

					if (!inside(last.mPosition, clipPlanes[i])) {
				
						auto intersectPoint = intersect(last, current, clipPlanes[i]);

						outputs.push_back(intersectPoint);
					}
				}

				outputs.push_back(current);
			}

			else {

				if (drawMode != DRAW_LINES || p != inputs.size() - 1) {

			
					if (inside(last.mPosition, clipPlanes[i])) {
						auto intersectPoint = intersect(last, current, clipPlanes[i]);
						outputs.push_back(intersectPoint);
					}
				}

			}
		}
	}

}

bool Clipper::inside(const math::vec4f& point, const math::vec4f& plane) {

	return math::dot(point, plane) >= 0.0f;
}

VsOutput Clipper::intersect(const VsOutput& last, const VsOutput& current, const math::vec4f& plane){

	VsOutput output;

	float distanceLast = math::dot(last.mPosition, plane);
	float distanceCurrent = math::dot(current.mPosition, plane);
	float weight = distanceLast / (distanceLast - distanceCurrent);

	output.mPosition = math::lerp(last.mPosition, current.mPosition, weight);
	output.mColor = math::lerp(last.mColor, current.mColor, weight);
	output.mUV = math::lerp(last.mUV, current.mUV, weight);

	return output;
}