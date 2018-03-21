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

#include <vector>
using namespace std;

const float maxAge = 800;
const float maxPosition = 1000;
const int maxAmount = 10000;

class TailPartical
{
public:
	float age = 0;
	glm::vec3 position;
	glm::vec3 speed;
	float scale;
};

class OneCosmic
{
public:
	glm::vec3 position;
	glm::vec3 speed;
};

class Cosmic
{
public:
	vector<OneCosmic> cosmics;
	vector<TailPartical> tails;
	unsigned int buffer;
	Shader *shader;
	Model *model;
	glm::mat4 *modelMatrices;
	int whichTexture = 0;

	Cosmic(Model *model, Shader *shader)
	{
		this->model = model;
		this->shader = shader;
		modelMatrices = new glm::mat4[maxAmount];

		// 将创建的粒子信息存在buffer中
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, maxAmount * sizeof(glm::mat4), &modelMatrices[0], GL_STREAM_DRAW);

		// 分割buffer内容，确保每一个粒子对应一个mat4(4*4矩阵) 
		for (unsigned int i = 0; i < model->meshes.size(); i++)
		{
			unsigned int VAO = model->meshes[i].VAO;
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

	void newCosmic()
	{
		OneCosmic one;
		one.position = glm::vec3(200, rand()%50, rand()%50);
		glm::vec3 destination = glm::vec3(0, 0, 0);
		one.speed = 0.1f * glm::normalize(destination - one.position);
		cosmics.push_back(one);
	}

	void newCosmicEverywhere()
	{
		OneCosmic one;
		float angle1 = rand() % 20/20.0 * 3.1415, angle2 = rand() % 20/10.0 * 3.1415;
		one.position = glm::vec3(200*sin(angle1)*cos(angle2), 200*sin(angle1)*sin(angle2), 200*cos(angle1));
		glm::vec3 destination = glm::vec3(0, 0, 0);
		one.speed = 0.1f * glm::normalize(destination - one.position);
		cosmics.push_back(one);
	}

	void anotherColor()
	{
		whichTexture = 1 - whichTexture;
	}

	void display(float deltaTime, glm::mat4 view)
	{
		shader->use();
		shader->setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model->textures_loaded[whichTexture].id);

		// 更新每颗彗星
		for (int i = 0; i < cosmics.size(); i++)
		{
			// 决定彗星的死亡
			if (abs(cosmics[i].position.x) > maxPosition ||
				abs(cosmics[i].position.y) > maxPosition ||
				abs(cosmics[i].position.z) > maxPosition ||
				abs(cosmics[i].position.x) < 10) {
				cosmics.erase(cosmics.begin() + i);
				i--;
				continue;
			}

			cosmics[i].position += deltaTime * cosmics[i].speed;
			if (tails.size() < maxAmount) {
				TailPartical tail;
				tail.position = glm::vec3(cosmics[i].position);
				tail.speed = glm::vec3(0.01, 0, 0);
				tail.scale = 1;
				tails.push_back(tail);
			}

			if (tails.size() < maxAmount) {
				TailPartical tail;
				tail.position = glm::vec3(cosmics[i].position - cosmics[i].speed*0.5f);
				tail.speed = glm::vec3(0.01, 0, 0);
				tail.scale = 1;
				tails.push_back(tail);
			}
		}

		// 更新每个小尾巴的空间信息
		for (unsigned int i = 0; i < tails.size(); i++)
		{
			// 决定小尾巴的死亡
			if (tails[i].age > maxAge) {
				tails.erase(tails.begin() + i);
				i--;
				continue;
			}

			tails[i].position += deltaTime * tails[i].speed;
			tails[i].age += deltaTime;
			tails[i].scale = 1 - (tails[i].age / maxAge);
			glm::mat4 model;

			// 公告板
			// 通过转置view矩阵 消除旋转的影响 使得粒子永远正对镜头
			model[0][0] = view[0][0];
			model[0][1] = view[1][0];
			model[0][2] = view[2][0];
			model[1][0] = view[0][1];
			model[1][1] = view[1][1];
			model[1][2] = view[2][1];
			model[2][0] = view[0][2];
			model[2][1] = view[1][2];
			model[2][2] = view[2][2];

			model = glm::translate(model, tails[i].position);
			model = glm::scale(model, glm::vec3(5 * tails[i].scale));
			modelMatrices[i] = model;
		}

		// 将空间信息更新到buffer种
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, maxAmount * sizeof(glm::mat4), &modelMatrices[0], GL_STREAM_DRAW);

		// 进行实例化绘制
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		for (unsigned int i = 0; i < model->meshes.size(); i++)
		{
			glBindVertexArray(model->meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, tails.size());
			glBindVertexArray(0);
		}
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
};