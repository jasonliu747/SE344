#define GLUT_DISABLE_ATEXIT_HACK
#include <windows.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "include/stb_image.h"
#include "include/irrKlang/irrKlang.h"

#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "skybox.h"
#include "camera.h"
#include "model.h"
#include "object.h"
#include "asteroid.h"
#include "cosmic.h"

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 155.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float intensity = 1;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float tmptime = 0.0f;

// ̫����ը
bool destroy = 0;

//ָ��gpu���ڴ�ռ��ָ��
unsigned int buffer;

Shader *asteroidShader, *planetShader, *cosmicShader, *skyboxShader, *earthShader;
Model *rockModel, *sunModel, *earthModel, *tailModel;
Object *sunObj, *earthObj;
Asteroid *asteroid;
Cosmic *cosmic;
SkyBox *skyBox;

void processKey(float deltaTime)
{
	// ����WASD���������ƶ�
	if (GetKeyState('W') < 0)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (GetKeyState('S') < 0)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (GetKeyState('A') < 0)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (GetKeyState('D') < 0)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	// ����ǿ�ȸ���
	if (GetKeyState('Q') < 0) {
		intensity += deltaTime * 0.0005;
		if (intensity > 1)
			intensity = 1;
	}
	if (GetKeyState('E') < 0) {
		intensity -= deltaTime * 0.0005;
		if (intensity < 0)
			intensity = 0;
	}
}

void processInput(unsigned char key, int xpos, int ypos)
{
	irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();
	switch (key)
	{
	case 49:// 1
		cosmic->newCosmic();
		break;
	case 50:// 2
		for (int i = 0; i < 200; i++) {
			cosmic->newCosmicEverywhere();
		}
		break;
	case 51: // 3
		cosmic->anotherColor();
		break;
	case 52: //4
		asteroid->anotherModel();
		break;
	case 53: //5
		tmptime = 0.0f;
		if (destroy == 0)
			engine->play2D("bomb.wav", false);
		destroy = !destroy;
		break;
	case 27:
		exit(0);
		break;
	}
}

void mouse_callback(int xpos, int ypos)
{
	// ��������ƶ�x�����y�����ƶ�camera��ʵ�������ƶ�
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void display()
{
	// ���㵱ǰ֡����һ֡��ʱ����
	float currentFrame = glutGet(GLUT_ELAPSED_TIME);
	
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	processKey(deltaTime);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ��͸�Ӿ����view���󴫵�shader��
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	
	// ����̫��
	planetShader->use();
	planetShader->setMat4("projection", projection);
	planetShader->setMat4("view", view);
	planetShader->setFloat("time", tmptime*0.01f);
	if(destroy)tmptime += deltaTime;
	sunObj->display(deltaTime);

	// ���Ƶ���
	earthShader->use();
	earthShader->setMat4("projection", projection);
	earthShader->setMat4("view", view);
	earthObj->display(deltaTime);

	// ������ʯ��ɫ��
	asteroidShader->use();
	asteroidShader->setMat4("projection", projection);
	asteroidShader->setMat4("view", view);
	asteroidShader->setFloat("intensity", intensity);
	asteroid->display(deltaTime);

	// ��պ�
	skyboxShader->use();
	skyboxShader->setInt("skybox", 0);
	skyboxShader->setMat4("view", view);
	skyboxShader->setMat4("projection", projection);
	skyBox->display(view, projection);

	// ����
	cosmicShader->use();
	cosmicShader->setMat4("projection", projection);
	cosmicShader->setMat4("view", view);
	cosmic->display(deltaTime, view);

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	// �������� Creating window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("Solar System");

	glewExperimental = GL_TRUE;
	glewInit();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// ������Ȳ���
	glEnable(GL_DEPTH_TEST);
	// ȡ��������ʾ
	glEnable(GL_CULL_FACE);

	// ����������ɫ��
	asteroidShader = new Shader("asteroids.vs", "asteroids.fs");
	planetShader = new Shader("planet.vs", "planet.fs", "planet.gs");
	//planetShader = new Shader("planet.vs", "planet.fs");
	earthShader = new Shader("planet.vs", "planet.fs");
	cosmicShader = new Shader("cosmic.vs", "cosmic.fs");
	skyboxShader = new Shader("skybox.vs", "skybox.fs");

	// ��ȡģ��
	rockModel = new Model("objects/rock/rock.obj");
	sunModel = new Model("objects/planet/planet.obj");
	earthModel = new Model("objects/earth/planet.obj");
	tailModel = new Model("objects/cosmic/cosmic.obj");


	sunObj = new Object(sunModel, planetShader, 0, 0, 0, 0, 0.0f, 10.0f);
	earthObj = new Object(earthModel, earthShader, 60.0f, 0, 0.001f, 0, 0.003f, 4.0f);
	asteroid = new Asteroid(rockModel, asteroidShader, 2000, 150, 25);
	cosmic = new Cosmic(tailModel, cosmicShader);
	skyBox = new SkyBox();

	// ÿ֡����display
	glutDisplayFunc(display);
	glutIdleFunc(display);
	// �����������processInput
	glutKeyboardFunc(processInput);
	// ����������mouse_callback
	glutPassiveMotionFunc(mouse_callback);

	// ��ʼѭ��
	glutMainLoop();

	return 0;
}