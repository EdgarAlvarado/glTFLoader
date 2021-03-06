#pragma once
#include <glm\glm.hpp>
#include <glad\glad.h>
#include <string>
#include <vector>
#include <limits>

#include "Shader.h"
#include "Ray.h"
#include "Box.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord0;
	glm::vec4 tangent;
	glm::vec3 bitangent;
};

struct Material
{
	glm::vec4 color;
	GLfloat metallic;
};

struct Line
{
	glm::vec3 start;
	glm::vec3 end;
};

class Primitive
{
public:
	Vertex *vertices;
	GLuint *indices;
	GLuint material;
	GLuint verticesCount;
	GLuint indicesCount;
	GLint intersectID;
	Primitive() :vertices(nullptr), indices(nullptr), material(0), verticesCount(0), indicesCount(0), intersectID(-1) {}
	~Primitive()
	{
		delete[] this->vertices;
		delete[] this->indices;
	}
	void setup(Vertex *_vertices, GLuint _verticesCount, GLuint *_indices, GLuint _indicesCount, GLuint _material);

	void draw();
private:
	GLuint VAO, VBO, EBO;
};

class Mesh
{
public:
	Primitive* primitives;
	Box *boundingBoxes;
	GLuint primitivesCount;
	Mesh() : primitives(nullptr), boundingBoxes(nullptr), primitivesCount(0) {}
	~Mesh()
	{
		delete[] primitives;
		delete[] boundingBoxes;
	}

};

struct Node
{
	glm::vec3 translation;
	glm::vec3 scale;
	glm::mat4 model;
	GLuint *children;
	GLuint childrenCount;
	GLboolean isRoot;
	GLuint parent;
	GLuint mesh;
	GLboolean hasMesh;
	Box boundingBox;
	Node() : children(nullptr), childrenCount(0), isRoot(GL_TRUE), hasMesh(GL_FALSE) {}
	~Node()
	{
		if(nullptr != children)
			delete[] children;
	}
};

struct Scene
{
	GLuint *nodes;
	GLuint nodesCount;
	Scene() :nodes(nullptr), nodesCount(0) {}
	~Scene()
	{
		delete[] nodes;
	}
};

class glTFFile
{
public:
	Scene *scenes;
	Mesh *meshes;
	Node *nodes;
	Material *materials;
	GLuint scenesCount;
	GLuint meshesCount;
	GLuint nodesCount;
	GLuint materialsCount;
	glTFFile() : scenes(nullptr), meshes(nullptr), nodes(nullptr), materials(nullptr), scenesCount(0), meshesCount(0), nodesCount(0), materialsCount(0) {}
	~glTFFile()
	{
		delete[] scenes;
		delete[] meshes;
		delete[] nodes;
		delete[] materials;
	}
	void draw(GLuint sceneIndex, Shader *shader);

	void setup();

private:
	void drawNode(GLuint index, glm::mat4 parentM, Shader *shader);

	Box calculateBoundingBox(GLuint index, glm::mat4 parentModel);
};

struct Buffer
{
	GLubyte *data;
	GLuint size;
	Buffer() : data(nullptr), size(0) {}
	~Buffer()
	{
		delete[] data;
	}
};

struct BufferView
{
	GLuint buffer;
	GLuint offset;
	GLuint size;
};

struct Accessor
{
	GLuint view;
	GLuint componentType;
	GLuint size;
	GLuint count;
	std::string type;
	GLchar* min;
	GLchar* max;
	Accessor() : min(nullptr), max(nullptr), count(0) {}
	~Accessor()
	{
		delete[] min;
		delete[]max;
	}
};
