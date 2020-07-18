//
// Created by efarhan on 26.01.20.
//

#include <gl/shape.h>
#include <mathematics/trigo.h>

#include "engine/engine.h"

namespace neko::gl
{
void RenderQuad::Init()
{
	Vec2f vertices[4] = {
		Vec2f(0.5f, 0.5f) * size_ + Vec2f(offset_), // top right
		Vec2f(0.5f, -0.5f) * size_ + Vec2f(offset_), // bottom right
		Vec2f(-0.5f, -0.5f) * size_ + Vec2f(offset_), // bottom left
		Vec2f(-0.5f, 0.5f) * size_ + Vec2f(offset_) // top left
	};

	Vec2f texCoords[4] = {
		Vec2f(1.0f, 1.0f), // top right
		Vec2f(1.0f, 0.0f), // bottom right
		Vec2f(0.0f, 0.0f), // bottom left
		Vec2f(0.0f, 1.0f), // bottom left
	};

	Vec3f normals[4] = {
		Vec3f::back,
		Vec3f::back,
		Vec3f::back,
		Vec3f::back
	};

	std::array<Vec3f, 4> tangent{};

	{
		const Vec3f edge1 = Vec3f(vertices[1] - vertices[0]);
		const Vec3f edge2 = Vec3f(vertices[2] - vertices[0]);
		const Vec2f deltaUV1 = texCoords[1] - texCoords[0];
		const Vec2f deltaUV2 = texCoords[2] - texCoords[0];

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		tangent[0].x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent[0].y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent[0].z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	}
	std::fill(tangent.begin() + 1, tangent.end(), tangent[0]);

	unsigned int indices[6] = {
		// note that we start from 0!
		0, 1, 3, // first triangle
		1, 2, 3 // second triangle
	};

	//Initialize the ebo program
	glGenBuffers(4, &vbo[0]);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
	// 1. bind Vertex Array Object
	glBindVertexArray(vao);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), (void*)0);
	glEnableVertexAttribArray(0);
	//bind texture coords data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	// bind normals data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);
	glEnableVertexAttribArray(2);
	// bind tangent data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tangent), &tangent[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);
	glEnableVertexAttribArray(3);
	//bind ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void RenderQuad::Draw() const
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void RenderQuad::Destroy()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(4, &vbo[0]);
	glDeleteBuffers(1, &ebo);
}

void RenderQuad::SetValues(const Vec2f& newSize, const Vec3f& newOffset)
{
	size_ = newSize;
	offset_ = newOffset;

	Vec2f vertices[4] = {
		Vec2f(0.5f, 0.5f) * size_ + Vec2f(offset_), // top right
		Vec2f(0.5f, -0.5f) * size_ + Vec2f(offset_), // bottom right
		Vec2f(-0.5f, -0.5f) * size_ + Vec2f(offset_), // bottom left
		Vec2f(-0.5f, 0.5f) * size_ + Vec2f(offset_) // top left
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}

void RenderQuad::SetSize(const Vec2f& newSize)
{
	size_ = newSize;

	Vec2f vertices[4] = {
		Vec2f(0.5f, 0.5f) * size_ + Vec2f(offset_), // top right
		Vec2f(0.5f, -0.5f) * size_ + Vec2f(offset_), // bottom right
		Vec2f(-0.5f, -0.5f) * size_ + Vec2f(offset_), // bottom left
		Vec2f(-0.5f, 0.5f) * size_ + Vec2f(offset_) // top left
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}

void RenderQuad::SetOffset(const Vec3f& newOffset)
{
	offset_ = newOffset;

	Vec2f vertices[4] = {
		Vec2f(0.5f, 0.5f) * size_ + Vec2f(offset_), // top right
		Vec2f(0.5f, -0.5f) * size_ + Vec2f(offset_), // bottom right
		Vec2f(-0.5f, -0.5f) * size_ + Vec2f(offset_), // bottom left
		Vec2f(-0.5f, 0.5f) * size_ + Vec2f(offset_) // top left
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}

void RenderCuboid::Init()
{
	Vec3f position[36] =
	{
		//Right face 
		Vec3f(0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Right
		Vec3f(0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Right
		Vec3f(0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Left
		Vec3f(0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Right
		Vec3f(0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Left
		Vec3f(0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Left
		//Left face                 
		Vec3f(-0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Right
		Vec3f(-0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Left
		Vec3f(-0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Left
		Vec3f(-0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Left
		//Top face                  
		Vec3f(0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Right
		Vec3f(0.5f, 0.5f, -0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Left
		Vec3f(0.5f, 0.5f, -0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, 0.5f, -0.5f) * size_ + offset_, //Bottom Left
		Vec3f(-0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Left
		//Bottom face               
		Vec3f(0.5f, -0.5f, 0.5f) * size_ + offset_, //Top Right
		Vec3f(-0.5f, -0.5f, 0.5f) * size_ + offset_, //Top Left
		Vec3f(0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Right
		Vec3f(0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, -0.5f, 0.5f) * size_ + offset_, //Top Left
		Vec3f(-0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Left
		//Front face                
		Vec3f(0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Right
		Vec3f(-0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Left
		Vec3f(0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Right
		Vec3f(0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Left
		Vec3f(-0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Left
		//Back face
		Vec3f(0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Right
		Vec3f(0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Left
		Vec3f(0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Left
		Vec3f(-0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Left
	};
	Vec2f texCoords[36] = {
		//Right Face
		Vec2f(1.0f, 1.0f), //Top Right
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(0.0f, 1.0f), //Top Left
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(0.0f, 0.0f), //Bottom Left
		Vec2f(0.0f, 1.0f), //Top Left
		//Left Face
		Vec2f(1.0f, 1.0f), //Top Right
		Vec2f(0.0f, 1.0f), //Top Left
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(0.0f, 1.0f), //Top Left
		Vec2f(0.0f, 0.0f), //Bottom Left
		//Top Face
		Vec2f(1.0f, 1.0f), //Top Right
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(0.0f, 1.0f), //Top Left
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(0.0f, 0.0f), //Bottom Left
		Vec2f(0.0f, 1.0f), //Top Left
		//Bottom Face
		Vec2f(1.0f, 1.0f), //Top Right
		Vec2f(0.0f, 1.0f), //Top Left
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(0.0f, 1.0f), //Top Left
		Vec2f(0.0f, 0.0f), //Bottom Left
		//Front Face
		Vec2f(1.0f, 1.0f), //Top Right
		Vec2f(0.0f, 1.0f), //Top Left
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(0.0f, 1.0f), //Top Left
		Vec2f(0.0f, 0.0f), //Bottom Left
		//Back Face
		Vec2f(1.0f, 1.0f), //Top Right
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(0.0f, 1.0f), //Top Left
		Vec2f(1.0f, 0.0f), //Bottom Right
		Vec2f(0.0f, 0.0f), //Bottom Left
		Vec2f(0.0f, 1.0f), //Top Left
	};

	Vec3f normals[36] =
	{
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),

		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),

		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),

		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),

		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 1.0f),

		Vec3f(0.0f, 0.0f, -1.0f),
		Vec3f(0.0f, 0.0f, -1.0f),
		Vec3f(0.0f, 0.0f, -1.0f),
		Vec3f(0.0f, 0.0f, -1.0f),
		Vec3f(0.0f, 0.0f, -1.0f),
		Vec3f(0.0f, 0.0f, -1.0f),
	};

	Vec3f tangent[36]{};
	for (int i = 0; i < 36; i += 3)
	{
		const Vec3f edge1 = position[i + 1] - position[i];
		const Vec3f edge2 = position[i + 2] - position[i];
		const Vec2f deltaUV1 = texCoords[i + 1] - texCoords[i];
		const Vec2f deltaUV2 = texCoords[i + 2] - texCoords[i];

		const float f = 1.0f / (deltaUV1.u * deltaUV2.v - deltaUV2.u * deltaUV1.v);
		tangent[i].x = f * (deltaUV2.v * edge1.x - deltaUV1.v * edge2.x);
		tangent[i].y = f * (deltaUV2.v * edge1.y - deltaUV1.v * edge2.y);
		tangent[i].z = f * (deltaUV2.v * edge1.z - deltaUV1.v * edge2.z);
		tangent[i + 1] = tangent[i];
		tangent[i + 2] = tangent[i];
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(4, &vbo[0]);

	glBindVertexArray(vao);
	// position attribute
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(1);
	// normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(2);
	//tangent attribute
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tangent), tangent, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(3);
}

void RenderCuboid::InitInstanced(const Vec3f& positions, const int count)
{
	Init();

	glBindVertexArray(vao);

	//Instances Positions
	glGenBuffers(1, &instanceVbo);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f) * count, &positions, GL_STATIC_DRAW);

	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)0);
	glVertexAttribDivisor(5, 1);
	glEnableVertexAttribArray(5);
}

void RenderCuboid::Draw() const
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RenderCuboid::DrawInstanced(const unsigned count) const
{
	glBindVertexArray(vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, count);
}

void RenderCuboid::Destroy()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(4, &vbo[0]);
	//glDeleteBuffers(2, &ebo);
}

void RenderCubeMap::Init()
{
	Vec3f position[36] =
	{
		//Right face 
		Vec3f(0.5f, 0.5f, 0.5f), //Top Right
		Vec3f(0.5f, -0.5f, 0.5f), //Bottom Right
		Vec3f(0.5f, 0.5f, -0.5f), //Top Left
		Vec3f(0.5f, -0.5f, 0.5f), //Bottom Right
		Vec3f(0.5f, -0.5f, -0.5f), //Bottom Left
		Vec3f(0.5f, 0.5f, -0.5f), //Top Left
		//Left face                 
		Vec3f(-0.5f, 0.5f, 0.5f), //Top Right
		Vec3f(-0.5f, 0.5f, -0.5f), //Top Left
		Vec3f(-0.5f, -0.5f, 0.5f), //Bottom Right
		Vec3f(-0.5f, -0.5f, 0.5f), //Bottom Right
		Vec3f(-0.5f, 0.5f, -0.5f), //Top Left
		Vec3f(-0.5f, -0.5f, -0.5f), //Bottom Left
		//Top face                  
		Vec3f(0.5f, 0.5f, 0.5f), //Top Right
		Vec3f(0.5f, 0.5f, -0.5f), //Bottom Right
		Vec3f(-0.5f, 0.5f, 0.5f), //Top Left
		Vec3f(0.5f, 0.5f, -0.5f), //Bottom Right
		Vec3f(-0.5f, 0.5f, -0.5f), //Bottom Left
		Vec3f(-0.5f, 0.5f, 0.5f), //Top Left
		//Bottom face               
		Vec3f(0.5f, -0.5f, 0.5f), //Top Right
		Vec3f(-0.5f, -0.5f, 0.5f), //Top Left
		Vec3f(0.5f, -0.5f, -0.5f), //Bottom Right
		Vec3f(0.5f, -0.5f, -0.5f), //Bottom Right
		Vec3f(-0.5f, -0.5f, 0.5f), //Top Left
		Vec3f(-0.5f, -0.5f, -0.5f), //Bottom Left
		//Front face                
		Vec3f(0.5f, 0.5f, 0.5f), //Top Right
		Vec3f(-0.5f, 0.5f, 0.5f), //Top Left
		Vec3f(0.5f, -0.5f, 0.5f), //Bottom Right
		Vec3f(0.5f, -0.5f, 0.5f), //Bottom Right
		Vec3f(-0.5f, 0.5f, 0.5f), //Top Left
		Vec3f(-0.5f, -0.5f, 0.5f), //Bottom Left
		//Back face
		Vec3f(0.5f, 0.5f, -0.5f), //Top Right
		Vec3f(0.5f, -0.5f, -0.5f), //Bottom Right
		Vec3f(-0.5f, 0.5f, -0.5f), //Top Left
		Vec3f(0.5f, -0.5f, -0.5f), //Bottom Right
		Vec3f(-0.5f, -0.5f, -0.5f), //Bottom Left
		Vec3f(-0.5f, 0.5f, -0.5f), //Top Left
	};

	Vec3f normals[36] =
	{
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),

		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),
		Vec3f(-1.0f, 0.0f, 0.0f),

		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),

		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),
		Vec3f(0.0f, -1.0f, 0.0f),

		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 1.0f),
		Vec3f(0.0f, 0.0f, 1.0f),

		Vec3f(0.0f, 0.0f, -1.0f),
		Vec3f(0.0f, 0.0f, -1.0f),
		Vec3f(0.0f, 0.0f, -1.0f),
		Vec3f(0.0f, 0.0f, -1.0f),
		Vec3f(0.0f, 0.0f, -1.0f),
		Vec3f(0.0f, 0.0f, -1.0f),
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(2, &vbo[0]);

	glBindVertexArray(vao);
	//Positions
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), nullptr);
	glEnableVertexAttribArray(0);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), nullptr);
	glEnableVertexAttribArray(1);

	const auto& config = BasicEngine::GetInstance()->config;
	shader_.LoadFromFile(
		config.dataRootPath + "shaders/skybox.vert",
		config.dataRootPath + "shaders/skybox.frag"
		);
}

void RenderCubeMap::Draw(const Mat4f& view, const Mat4f& projection) const
{
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_FRONT);
	shader_.Bind();
	shader_.SetMat4("view", Mat4f(view.ToMat3()));
	shader_.SetMat4("projection", projection);
	shader_.SetInt("skybox", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);
}

void RenderCubeMap::Destroy()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, &vbo[0]);
	shader_.Destroy();
	glDeleteTextures(1, &texture_);
}

void RenderWireFrameCuboid::Init()
{
	Vec3f position[36] =
	{
		//Right face 
		Vec3f(0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Right
		Vec3f(0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Right

		Vec3f(0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Left
		Vec3f(0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Left

		Vec3f(0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Right
		Vec3f(0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Left

		Vec3f(0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Left
		Vec3f(0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Right

		//Left face                 
		Vec3f(-0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Right
		Vec3f(-0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Left

		Vec3f(-0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Left

		Vec3f(-0.5f, -0.5f, 0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Right

		Vec3f(-0.5f, 0.5f, -0.5f) * size_ + offset_, //Top Left
		Vec3f(-0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Left

		//Top face                  
		Vec3f(0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Right
		Vec3f(-0.5f, 0.5f, 0.5f) * size_ + offset_, //Top Left
		Vec3f(0.5f, 0.5f, -0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, 0.5f, -0.5f) * size_ + offset_, //Bottom Left
		//Bottom face               
		Vec3f(0.5f, -0.5f, 0.5f) * size_ + offset_, //Top Right
		Vec3f(-0.5f, -0.5f, 0.5f) * size_ + offset_, //Top Left
		Vec3f(0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Right
		Vec3f(-0.5f, -0.5f, -0.5f) * size_ + offset_, //Bottom Left
	};
	glGenVertexArrays(1, &vao);
	glGenBuffers(4, &vbo[0]);

	glBindVertexArray(vao);
	// position attribute
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
}

void RenderWireFrameCuboid::Draw() const
{
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 24);
}

void RenderWireFrameCuboid::Destroy()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo[0]);
}

void RenderCircle::Init()
{
	Vec2f vertices[resolution + 2];
	Vec2f texCoords[resolution + 2];
	vertices[0] = Vec2f(offset_);
	texCoords[0] = Vec2f::one / 2.0f;

	for (size_t i = 1; i < resolution + 1; i++)
	{
		Vec2f vertex = Vec2f::up * radius_;
		auto angle = degree_t(360.0f / resolution * static_cast<float>(i - 1));
		vertex = vertex.Rotate(angle);
		vertices[i] = vertex;

		texCoords[i] = Vec2f::one / 2.0f + Vec2f::one / 2.0f * Vec2f(Sin(angle), Cos(angle));
	}
	vertices[resolution + 1] = vertices[1];
	texCoords[resolution + 1] = texCoords[resolution + 1];

	//Initialize the ebo program
	glGenBuffers(2, &vbo[0]);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);
	// 1. bind Vertex Array Object
	glBindVertexArray(vao);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), (void*)nullptr);
	glEnableVertexAttribArray(0);
	//bind texture coords data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), (void*)nullptr);
}

void RenderCircle::Draw() const
{
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, resolution + 2);
}

void RenderCircle::Destroy()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, &vbo[0]);
}

void RenderLine3d::Init()
{
	Vec3f position[2] =
	{
		Vec3f(0.0f, 0.0f, 0.0f) * offset_, //Left
		Vec3f(1.0f, 1.0f, 1.0f) * relativeEndPos_ + offset_, //Right
	};

	float linePos[2] =
	{
		0,
		1
	};
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, &vbo[0]);
	// 1. bind Vertex Array Object
	glBindVertexArray(vao);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3f), (void*)nullptr);
	glEnableVertexAttribArray(0);
	//bind texture coords data
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(linePos), linePos, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)nullptr);

	glEnableVertexAttribArray(1);
}

void RenderLine3d::Draw() const
{
	glBindVertexArray(vao);
	glDrawArrays(GL_LINES, 0, 2);
}

void RenderLine3d::Destroy()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, &vbo[0]);
}

RenderSphere::RenderSphere(Vec3f offset, float radius, size_t segment)
	: neko::RenderSphere(offset, radius),
	  segment_(segment)
{
}

void RenderSphere::Init()
{
	glCheckError();
	glGenVertexArrays(1, &vao);

	glGenBuffers(1, &vbo[0]);
	glGenBuffers(1, &ebo);

	std::vector<Vec3f> positions;
	positions.reserve((segment_ + 1) * (segment_ + 1));
	std::vector<Vec2f> uv;
	uv.reserve((segment_ + 1) * (segment_ + 1));
	std::vector<Vec3f> normals;
	normals.reserve((segment_ + 1) * (segment_ + 1));
	std::vector<Vec3f> tangent;
	tangent.resize((segment_ + 1) * (segment_ + 1));
	std::vector<unsigned int> indices;
	indices.reserve((segment_ + 1) * segment_);
	for (unsigned int y = 0; y <= segment_; ++y)
	{
		for (unsigned int x = 0; x <= segment_; ++x)
		{
			float xSegment = static_cast<float>(x) / static_cast<float>(segment_);
			float ySegment = static_cast<float>(y) / static_cast<float>(segment_);
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			positions.emplace_back(xPos, yPos, zPos);
			uv.emplace_back(xSegment, ySegment);
			normals.emplace_back(xPos, yPos, zPos);
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < segment_; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (unsigned int x = 0; x <= segment_; ++x)
			{
				indices.push_back(y * (segment_ + 1) + x);
				indices.push_back((y + 1) * (segment_ + 1) + x);
			}
		}
		else
		{
			for (int x = segment_; x >= 0; --x)
			{
				indices.push_back((y + 1) * (segment_ + 1) + x);
				indices.push_back(y * (segment_ + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	indexCount_ = indices.size();
	for (size_t i = 0; i < indexCount_ - 2; i++)
	{
		const Vec3f edge1 = positions[indices[i + 1]] - positions[indices[i]];
		const Vec3f edge2 = positions[indices[i + 2]] - positions[indices[i]];
		const Vec2f deltaUV1 = uv[indices[i + 1]] - uv[indices[i]];
		const Vec2f deltaUV2 = uv[indices[i + 2]] - uv[indices[i]];

		const float f = 1.0f / (deltaUV1.u * deltaUV2.v - deltaUV2.u * deltaUV1.v);
		tangent[indices[i]].x = f * (deltaUV2.v * edge1.x - deltaUV1.v * edge2.x);
		tangent[indices[i]].y = f * (deltaUV2.v * edge1.y - deltaUV1.v * edge2.y);
		tangent[indices[i]].z = f * (deltaUV2.v * edge1.z - deltaUV1.v * edge2.z);
	}

	std::vector<float> data;
	data.reserve(
		positions.size() * sizeof(Vec3f) + uv.size() * sizeof(Vec2f) + normals.size() * sizeof(Vec3f
		));
	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
		if (!uv.empty())
		{
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}
		if (!normals.empty())
		{
			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}
		if (!tangent.empty())
		{
			data.push_back(tangent[i].x);
			data.push_back(tangent[i].y);
			data.push_back(tangent[i].z);
		}
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	             indices.size() * sizeof(unsigned int),
	             &indices[0],
	             GL_STATIC_DRAW);
	const auto stride = (3 + 2 + 3 + 3) * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
	glBindVertexArray(0);
	glCheckError();
}

void RenderSphere::Draw() const
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount_, GL_UNSIGNED_INT, 0);
}

void RenderSphere::Destroy()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo[0]);
	glDeleteBuffers(1, &ebo);
}
}
