#include "Game.h"
#include "dirent.h"

#include <iostream>

float planeVertices[] = {
	// positions			//Normals		// texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
	-5.0f, -0.5f, -5.0f,	0.0, 1.0, 0.0,	0.0f, 2.0f,
	-5.0f, -0.5f,  5.0f,	0.0, 1.0, 0.0,	0.0f, 0.0f,
	5.0f, -0.5f,  5.0f,		0.0, 1.0, 0.0,	2.0f, 0.0f,

	5.0f, -0.5f, -5.0f,		0.0, 1.0, 0.0,	2.0f, 2.0f,
	-5.0f, -0.5f, -5.0f,	0.0, 1.0, 0.0,	0.0f, 2.0f,
	5.0f, -0.5f,  5.0f,		0.0, 1.0, 0.0,	2.0f, 0.0f
};

float triangleVertices[] = {
	// positions			//Normals		// texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
	0.0f, 0.5f, 0.0f,		0.0, 1.0, 0.0,	0.0f, 2.0f,
	0.5f, 0.0f,  0.0f,		0.0, 1.0, 0.0,	0.0f, 0.0f,
	-0.5f, 0.0f,  0.0f,		0.0, 1.0, 0.0,	2.0f, 0.0f
};

glm::vec3 lightPositions[] = {
	glm::vec3(-10.0f,  10.0f, 10.0f),
	glm::vec3(10.0f,  10.0f, 10.0f),
	glm::vec3(-10.0f, -10.0f, 10.0f),
	glm::vec3(10.0f, -10.0f, 10.0f),
};
glm::vec3 lightColors[] = {
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f)
};

Game::Game() :
	mExitValue(Engine::SUCCESS)
{
}

Game::~Game()
{
}

Engine::EXIT_CODE Game::GetExitValue() { return this->mExitValue; }

GLboolean Game::init()
{
	/*this->mEngine = new Engine();
	if (this->mEngine->init(NULL))
	{
		this->mExitValue = Engine::FAILURE;
		return GL_FALSE;
	}*/
	Engine::StartModule(NULL);
	this->bamboo = Engine::GetInstance().mLoader->LoadFile("resources\\models\\bamboo.gltf");
	/*struct dirent **dirp;
	modelsCount = scandir("D:\\etc\\naturekit\\Models\\glTF format\\", &dirp, [](const struct dirent *dir) 
	{
		const char *s = dir->d_name;
		int len = strlen(s) - 5; // index of start of . in .mp3
		if (len >= 0)
		{
			if (strncmp(s + len, ".gltf", 5) == 0)
			{
				return 1;
			}
		}
		return 0;
	}, alphasort);
	if (modelsCount > 0)
	{
		this->models = new glTFFile*[modelsCount];
		for (GLuint i = 0; i < modelsCount; i++)
		{
			std::cout << dirp[i]->d_name << std::endl;
			if (i == 150)
				std::cout << "to crash" << std::endl;
			this->models[i] = this->mEngine->mLoader->LoadFile(std::string("D:\\etc\\naturekit\\Models\\glTF format\\").append(dirp[i]->d_name).c_str());
		}
	}*/
	//this->bamboo = this->mEngine->mLoader->LoadFile("resources\\models\\bamboo.gltf");
	for (GLuint i = 0; i < this->bamboo->nodesCount; i++)
	{
		Engine::GetInstance().registerBoundingBox(this->bamboo->nodes[i].boundingBox);
	}
	basicShader = new Shader("resources/shaders/shader.vs", "resources/shaders/shader.fs");
	simpleShader = new Shader("resources/shaders/simple.vs", "resources/shaders/simple.fs");
	pbrShader = new Shader("resources/shaders/PBR.vs", "resources/shaders/PBR.fs");

	pbrShader->use();
	for (GLuint i = 0; i < 4; i++)
	{
		pbrShader->setVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
		pbrShader->setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
	}

	GLuint planeVBO, triangleVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);
	
	projection = glm::perspective(glm::radians(Engine::GetInstance().GetCamera()->Zoom), Engine::GetInstance().GetAspectRatio(), Engine::GetInstance().GetNearPlane(), Engine::GetInstance().GetFarPlane());

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	return GL_TRUE;
}

void Game::run()
{
	double previous = glfwGetTime();
	double lag = 0.0;
	while (!Engine::GetInstance().GetShouldClose())
	{
		double current = glfwGetTime();
		double elapsed = current - previous;
		previous = current;
		lag += elapsed;
		this->currentFrame = glfwGetTime();
		this->deltaTime = this->currentFrame - this->lastFrame;
		this->lastFrame = this->currentFrame;

		this->processInput();

		this->update();

		this->render();
	}
}

void Game::update()
{
	Engine::GetInstance().update(this->deltaTime);
	projection = glm::perspective(glm::radians(Engine::GetInstance().GetCamera()->Zoom), Engine::GetInstance().GetAspectRatio(), Engine::GetInstance().GetNearPlane(), Engine::GetInstance().GetFarPlane());
	view = Engine::GetInstance().GetCamera()->GetViewMatrix();
}

void Game::render()
{
	glm::mat4 model;
	//model = glm::scale(model, glm::vec3(1.0f));
	basicShader->use();
	basicShader->setMat4("projection", this->projection);
	basicShader->setMat4("view", this->view);
	//this->bamboo->draw(0, *basicShader);

	pbrShader->use();
	pbrShader->setMat4("projection", this->projection);
	pbrShader->setMat4("view", this->view);
	pbrShader->setVec3("camPos", Engine::GetInstance().GetCamera()->Position);
	this->bamboo->draw(0, pbrShader);
	for (GLuint i = 0; i < modelsCount; i++)
	{
		this->models[i]->draw(0, pbrShader);
	}

	glBindVertexArray(planeVAO);
	basicShader->use();
	basicShader->setMat4("model", glm::mat4());
	basicShader->setVec4("baseColor", glm::vec4(1.0f));
	glDrawArrays(GL_TRIANGLES, 0, 6);

	Engine::GetInstance().render();
}

void Game::release()
{

	FREE_MEMORY(basicShader);
	FREE_MEMORY(bamboo);
	FREE_MEMORY(pbrShader);
	FREE_MEMORY(simpleShader);
	//delete this->models;
	Engine::CloseModule();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Game::processInput()
{
	if (Engine::GetInstance().mouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
	{
		GLdouble x, y;
		Engine::GetInstance().GetCursosPos(&x, &y);

		Ray ray = this->CastRay(x, y);

		GLint index = Engine::GetInstance().CheckCollision(ray);

		std::cout << index << std::endl;
	}
}

Ray Game::CastRay(GLfloat x, GLfloat y)
{
	Line line;

	x = (2.0f * x) / Engine::GetInstance().GetWindowWidth() - 1.0f;
	y = 1.0f - (2.0f * y) / Engine::GetInstance().GetWindowHeight();

	glm::vec4 ray_origin(x, y, -1.0f, 1.0f);
	glm::vec4 ray_end(x, y, 1.0f, 1.0f);
	glm::mat4 clipWorldMat = glm::inverse(this->projection * this->view);
	glm::vec4 pointNear4 = clipWorldMat * ray_origin;
	glm::vec4 pointFar4 = clipWorldMat * ray_end;
	line.start = glm::vec3(pointNear4) / pointNear4.w;
	line.end = glm::vec3(pointFar4) / pointFar4.w;

	Ray result(line.start, line.end - line.start);

	return result;
}
