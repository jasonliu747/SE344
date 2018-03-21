#pragma once

#include <windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "include/stb_image.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

class Partical {
public:
	float angle;
	glm::vec3 displacement;
	float scale;
	float rotAngle;
};

class Asteroid
{
public:
	Model *rock;
	Model *monkey;
	Shader *shader;
	Partical *particals;
	glm::mat4 *modelMatrices;
	int amount;
	float radius;
	float offset;

	unsigned int buffer;
	int whichModel;
	Asteroid(Model *rock, Shader *shader, int amount, float radius, float offset)
	{
		this->rock = rock;
		this->monkey = new Model("objects/monkey.obj");
		this->amount = amount;
		this->radius = radius;
		this->offset = offset;
		this->shader = shader;
		particals = new Partical[amount];
		modelMatrices = new glm::mat4[amount];
		whichModel = 0;
		srand(glutGet(GLUT_ELAPSED_TIME)); // initialize random seed	
		for (unsigned int i = 0; i < amount; i++)
		{
			glm::mat4 model;
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)amount * 360.0f;
			float displacement1 = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement1;
			float displacement2 = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement2 * 0.4f; // keep height of asteroid field smaller compared to width of x and z
			float displacement3 = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement3;
			model = glm::translate(model, glm::vec3(x, y, z));

			// 2. scale: Scale between 0.05 and 0.25f
			float scale = (rand() % 20) / 50.0f + 0.1;
			model = glm::scale(model, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			modelMatrices[i] = model;
			particals[i].angle = angle;
			particals[i].displacement = glm::vec3(displacement1, displacement2*0.4, displacement3);
			particals[i].rotAngle = rotAngle;
			particals[i].scale = scale;
		}

		// 将创建的粒子信息存在buffer中
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STREAM_DRAW);

		// 分割buffer内容，确保每一个粒子对应一个mat4(4*4矩阵) 
		for (unsigned int i = 0; i < rock->meshes.size(); i++)
		{
			unsigned int VAO = rock->meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}

		for (unsigned int i = 0; i < monkey->meshes.size(); i++)
		{
			unsigned int VAO = monkey->meshes[i].VAO;
			glBindVertexArray(VAO);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}

	void display(float deltaTime)
	{
		shader->use();
		shader->setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock->textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.

		// 更新每颗陨石的空间信息
		for (unsigned int i = 0; i < amount; i++)
		{
			particals[i].angle += deltaTime * 0.0002;
			glm::mat4 model;
			model = glm::translate(model, glm::vec3(radius*sin(particals[i].angle), 0, radius*cos(particals[i].angle)) + particals[i].displacement);
			model = glm::scale(model, glm::vec3(particals[i].scale));
			model = glm::rotate(model, particals[i].rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
			modelMatrices[i] = model;
		}

		// 将空间信息更新到buffer种
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STREAM_DRAW);

		// 进行实例化绘制
		if (whichModel == 0) {
			for (unsigned int i = 0; i < rock->meshes.size(); i++)
			{
				glBindVertexArray(rock->meshes[i].VAO);
				glDrawElementsInstanced(GL_TRIANGLES, rock->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
				glBindVertexArray(0);
			}
		}
		else if (whichModel == 1) {
			for (unsigned int i = 0; i < monkey->meshes.size(); i++)
			{
				glBindVertexArray(monkey->meshes[i].VAO);
				glDrawElementsInstanced(GL_TRIANGLES, monkey->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
				glBindVertexArray(0);
			}
		}
	}

	void anotherModel()
	{
		whichModel = 1 - whichModel;
	}
};