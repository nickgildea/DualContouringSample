#include "mesh.h"

#include <stdio.h>

// ----------------------------------------------------------------------------

void Mesh::initialise()
{
	glGenVertexArrays(1, &vertexArrayObj_);
	glGenBuffers(1, &vertexBuffer_);
	glGenBuffers(1, &indexBuffer_);

	glBindVertexArray(vertexArrayObj_);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), 0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)(sizeof(glm::vec3) * 1));

	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

void Mesh::uploadData(const VertexBuffer& vertices, const IndexBuffer& indices)
{
	if (vertices.empty() || indices.empty())
	{
		return;
	}

	glBindVertexArray(vertexArrayObj_);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	numIndices_ = indices.size();

	printf("Mesh: %d vertices %d triangles\n", vertices.size(), indices.size() / 3);

	glBindVertexArray(0);
}

// ----------------------------------------------------------------------------

void Mesh::destroy()
{
	glDeleteBuffers(1, &indexBuffer_);
	glDeleteBuffers(1, &vertexBuffer_);
	glDeleteVertexArrays(1, &vertexArrayObj_);
}

