#version 330

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

uniform mat4 MVP;
smooth out vec3 vertexColour;
smooth out vec3 vertexNormal;

void main()
{
	vertexColour = vec3(0.7f, 0.f, 0.f);
	vertexNormal = normal;

	gl_Position = MVP * vec4(position, 1);
}

