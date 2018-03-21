#pragma once
#include <windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "include/stb_image.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "model.h"

class Object
{
public:
	glm::vec3 position;
	float radius;
	float angle;
	float angleSpeed;
	float rotate;
	float rotateSpeed;
	float scale;

	Model *model;
	Shader *shader;

	Object(Model *model, Shader *shader, float radius, float angle, float angleSpeed, float rotate, float rotateSpeed, float scale)
		:model(model), shader(shader), radius(radius), angle(angle), angleSpeed(angleSpeed), rotate(rotate), rotateSpeed(rotateSpeed), scale(scale)
	{

	}

	void display(float deltaTime)
	{
		angle += deltaTime * angleSpeed;
		rotate += deltaTime * rotateSpeed;
		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(radius * sin(angle), -3.0f, radius * cos(angle)));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
		modelMatrix = glm::rotate(modelMatrix, rotate, glm::vec3(0, 1, 0));
		shader->setMat4("model", modelMatrix);
		model->Draw(*shader);
	}
};