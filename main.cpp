#include <Windows.h>
#include <iostream>
#include "application/application.h"
#include "gpu/gpu.h"
#include "gpu/dataStructures.h"
#include "gpu/shader/lightShader.h"
#include "application/image.h"
#include "application/camera.h"
#include "application/model.h"

#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup" )

uint32_t WIDTH = 1200;
uint32_t HEIGHT = 900;

LightShader* shader = nullptr;
DirectionalLight directionalLight;
math::vec3f envLight;

Camera* camera = nullptr;
Model* model = nullptr;
float angle = 0;

void prepare() {


	camera = new Camera(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f, { 0.0f, 1.0f, 0.0f });
	APP->setCamera(camera);

	shader = new LightShader();
	directionalLight.color = { 1.0f, 1.0f, 1.0f };
	directionalLight.direction = { -1.0f, -0.5f, -0.7f };
	envLight = { 0.5f, 0.5f, 0.5f };

	sgl->enable(CULL_FACE);

	model = new Model();
	model->read("assets/model/cyborg/cyborg.obj");

}

void transform() {

	angle -= 0.02f;
	auto rotateMatrix = math::rotate(math::mat4f(), angle, { 0.0f,1.0f,0.0f });
	auto translateMatrix = math::translate(math::mat4f(), 0.0f, 0.0f, -5.0f);
	auto m = translateMatrix * rotateMatrix;

	m = math::scale(m, 0.1f, 0.1f, 0.1f);
	model->setModelMatrix(m);
}

void render() {

	transform();

	shader->mViewMatrix = camera->getViewMatrix();
	shader->mProjectionMatrix = camera->getProjectionMatrix();

	shader->mDirectionalLight = directionalLight;
	shader->mEnvLight = envLight;

	sgl->clear();
	sgl->useProgram(shader);
	model->draw(shader);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	if (!APP->initApplication(hInstance, 800, 600)) {
		
		return -1;
	}

	sgl->initSurface(APP->getWidth(), APP->getHeight(), APP->getCanvas());

	prepare();

	bool alive = true;
	while (alive) {

		alive = APP->peekMessage();
		camera->update();

		render();

		APP->show();
	}
											
	delete shader;
	delete camera;
	delete model;

	return 0;
}