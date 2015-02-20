#ifndef		HAS_OCTREE_H_BEEN_INCLUDED
#define		HAS_OCTREE_H_BEEN_INCLUDED

#include "qef.h"
#include "mesh.h"

#include <glm/glm.hpp>
using glm::vec3;
using glm::ivec3;

// ----------------------------------------------------------------------------

enum OctreeNodeType
{
	Node_None,
	Node_Internal,
	Node_Psuedo,
	Node_Leaf,
};

// ----------------------------------------------------------------------------

struct OctreeDrawInfo 
{
	OctreeDrawInfo()
		: index(-1)
		, corners(0)
	{
	}

	int				index;
	int				corners;
	vec3			position;
	vec3			averageNormal;
	QEF				qef;
};

// ----------------------------------------------------------------------------

class OctreeNode
{
public:

	OctreeNode()
		: type(Node_None)
		, min(0, 0, 0)
		, size(0)
		, drawInfo(nullptr)
	{
		for (int i = 0; i < 8; i++)
		{
			children[i] = nullptr;
		}
	}

	OctreeNode(const OctreeNodeType _type)
		: type(_type)
		, min(0, 0, 0)
		, size(0)
		, drawInfo(nullptr)
	{
		for (int i = 0; i < 8; i++)
		{
			children[i] = nullptr;
		}
	}

	OctreeNodeType	type;
	ivec3			min;
	int				size;
	OctreeNode*		children[8];
	OctreeDrawInfo*	drawInfo;
};

// ----------------------------------------------------------------------------

OctreeNode* BuildOctree(const ivec3& min, const int size, const float threshold);
void DestroyOctree(OctreeNode* node);
void GenerateMeshFromOctree(OctreeNode* node, VertexBuffer& vertexBuffer, IndexBuffer& indexBuffer);

// ----------------------------------------------------------------------------

#endif	// HAS_OCTREE_H_BEEN_INCLUDED

