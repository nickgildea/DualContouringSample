#ifndef		HAS_MESH_H_BEEN_INCLUDED
#define		HAS_MESH_H_BEEN_INCLUDED

#include <vector>

#include <GL\glew.h>
#include <SDL_opengl.h>
#include <glm\glm.hpp>

// ----------------------------------------------------------------------------

struct MeshVertex
{
	MeshVertex(const glm::vec3& _xyz, const glm::vec3& _normal)
		: xyz(_xyz)
		, normal(_normal)
	{
	}

	glm::vec3		xyz, normal;
};

typedef std::vector<MeshVertex> VertexBuffer;
typedef std::vector<int> IndexBuffer;

// ----------------------------------------------------------------------------

class Mesh
{
public:

	Mesh()
		: vertexArrayObj_(0)
		, vertexBuffer_(0)
		, indexBuffer_(0)
		, numIndices_(0)
	{
	}

	void initialise();
	void uploadData(const VertexBuffer& vertices, const IndexBuffer& indices);
	void destroy();

	GLuint			vertexArrayObj_, vertexBuffer_, indexBuffer_;
	int				numIndices_;
};

// ----------------------------------------------------------------------------

#endif	//	HAS_MESH_H_BEEN_INCLUDED
