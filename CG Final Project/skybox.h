#pragma once
#include <windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "include/stb_image.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "shader.h"
#include <vector>

float skyboxVertices[] = {
	// positions          
	-2000.0f,  2000.0f, -2000.0f,
	-2000.0f, -2000.0f, -2000.0f,
	2000.0f, -2000.0f, -2000.0f,
	2000.0f, -2000.0f, -2000.0f,
	2000.0f,  2000.0f, -2000.0f,
	-2000.0f,  2000.0f, -2000.0f,

	-2000.0f, -2000.0f,  2000.0f,
	-2000.0f, -2000.0f, -2000.0f,
	-2000.0f,  2000.0f, -2000.0f,
	-2000.0f,  2000.0f, -2000.0f,
	-2000.0f,  2000.0f,  2000.0f,
	-2000.0f, -2000.0f,  2000.0f,

	2000.0f, -2000.0f, -2000.0f,
	2000.0f, -2000.0f,  2000.0f,
	2000.0f,  2000.0f,  2000.0f,
	2000.0f,  2000.0f,  2000.0f,
	2000.0f,  2000.0f, -2000.0f,
	2000.0f, -2000.0f, -2000.0f,

	-2000.0f, -2000.0f,  2000.0f,
	-2000.0f,  2000.0f,  2000.0f,
	2000.0f,  2000.0f,  2000.0f,
	2000.0f,  2000.0f,  2000.0f,
	2000.0f, -2000.0f,  2000.0f,
	-2000.0f, -2000.0f,  2000.0f,

	-2000.0f,  2000.0f, -2000.0f,
	2000.0f,  2000.0f, -2000.0f,
	2000.0f,  2000.0f,  2000.0f,
	2000.0f,  2000.0f,  2000.0f,
	-2000.0f,  2000.0f,  2000.0f,
	-2000.0f,  2000.0f, -2000.0f,

	-2000.0f, -2000.0f, -2000.0f,
	-2000.0f, -2000.0f,  2000.0f,
	2000.0f, -2000.0f, -2000.0f,
	2000.0f, -2000.0f, -2000.0f,
	-2000.0f, -2000.0f,  2000.0f,
	2000.0f, -2000.0f,  2000.0f
};

class SkyBox {
private:
	unsigned int skyboxVAO, skyboxVBO;
	
	unsigned int cubemapTexture;
	unsigned int loadCubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrComponents;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				//cout << "success" << endl;
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}


public:
	SkyBox() {
		glGenVertexArrays(1, &skyboxVAO);
		glGenBuffers(1, &skyboxVBO);
		glBindVertexArray(skyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		std::vector<std::string> faces
		{

			"skybox/purplenebula_ft.tga",
			"skybox/purplenebula_bk.tga",

			"skybox/purplenebula_dn.tga",
			"skybox/purplenebula_up.tga",

			"skybox/purplenebula_rt.tga",
			"skybox/purplenebula_lf.tga"

		};
		cubemapTexture = loadCubemap(faces);

		
	}

	void display(glm::mat4 view, glm::mat4 projection) {
		glDepthFunc(GL_LEQUAL);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default
	}
};