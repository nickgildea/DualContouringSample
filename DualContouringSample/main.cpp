#include <stdio.h>
#include <stdlib.h>

#include <GL\glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm\glm.hpp>

#include "glsl_program.h"
#include "mesh.h"
#include "octree.h"

// ----------------------------------------------------------------------------

void HandleMouseMove(const SDL_MouseMotionEvent& e, float& rotateXAxis, float& rotateYAxis)
{
	if (e.state & SDL_BUTTON_LMASK)
	{
		rotateXAxis += (float)e.yrel * 0.5f;
		rotateYAxis += (float)e.xrel * 0.5f;

		rotateXAxis = glm::min(80.f, rotateXAxis);
		rotateXAxis = glm::max(-80.f, rotateXAxis);
	}
}

// ----------------------------------------------------------------------------

void HandleMouseWheel(const SDL_MouseWheelEvent& e, float& distance)
{
	distance += -e.y * 10.f;
}

// ----------------------------------------------------------------------------

void HandleKeyPress(const SDL_KeyboardEvent& e, bool& drawWireframe, bool& refreshMesh)
{
	if (e.type != SDL_KEYUP)
	{
		return;
	}

	switch (e.keysym.sym)
	{
	case SDLK_F1:
		drawWireframe = !drawWireframe;
		break;

	case SDLK_F2:
		refreshMesh = true;
		break;
	}
}

// ----------------------------------------------------------------------------

void DrawFrame(GLSLProgram& program, Mesh& mesh, const glm::vec3& pos, const glm::vec3& fwd, bool drawWireframe)
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::mat4 projection = glm::perspective(60.f, 16.f/9.f, 0.1f, 500.f);
	glm::mat4 modelview = glm::lookAt(pos + fwd, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

	glUseProgram(program.getId());
	program.setUniform("MVP", projection * modelview);
	program.setUniformInt("useUniformColour", 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(mesh.vertexArrayObj_);
	glDrawElements(GL_TRIANGLES, mesh.numIndices_, GL_UNSIGNED_INT, (void*)0);

	if (drawWireframe)
	{
		glm::mat4 wireframe = glm::translate(glm::mat4(1), 0.02f * -fwd);

		program.setUniform("MVP", projection * modelview * wireframe);
		program.setUniformInt("useUniformColour", 1);
		program.setUniform("colour", glm::vec4(1));
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, mesh.numIndices_, GL_UNSIGNED_INT, (void*)0);
	}

	glUseProgram(0);
}

// ----------------------------------------------------------------------------

int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("Error: SDL_Init failed: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow("DualContourStarter", 100, 100, 1280, 720, SDL_WINDOW_OPENGL);
	if (!window)
	{
		printf("Error: SDL_CreateWindow failed: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (!context)
	{
		printf("Error: SDL_GL_CreateContext failed: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_GL_MakeCurrent(window, context);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		printf("Error: glewInit failed\n");
		return EXIT_FAILURE;
	}

	glViewport(0, 0, 1280, 720);

	// getting an (suprious?) error from glew here, just ignore
	glGetError();

	printf("----------------------------------------------------------------\n");
	printf("The controls are:\n");
	printf("	- hold left mouse and drag to rotate the view\n");
	printf("	- use the mouse wheel to zoom in/out\n");
	printf("	- press F1 to render a wireframe\n");
	printf("	- press F2 to regenerate the octree using a new error threshold (and generate a new mesh)\n");
	printf("----------------------------------------------------------------\n");
	printf("\n\n");

	printf("OpenGL version: %s\n", glGetString(GL_VERSION));
	printf("OpenGL shading version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	GLSLProgram program;
	if (!program.initialise() ||
		!program.compileShader(ShaderType_Vertex, "shader.vert") ||
		!program.compileShader(ShaderType_Fragment, "shader.frag") ||
		!program.link())
	{
		printf("Error: failed to create GLSL program\n");
		return EXIT_FAILURE;
	}

	const int MAX_THRESHOLDS = 5;
	const float THRESHOLDS[MAX_THRESHOLDS] = { -1.f, 0.1f, 1.f, 10.f, 50.f };
	int thresholdIndex = -1;

	Mesh mesh;
	mesh.initialise();

	OctreeNode* root = nullptr;
	Uint32 lastFrameTime = 0;

	// octreeSize must be a power of two!
	const int octreeSize = 64;

	float rotateX = -60.f, rotateY = 0.f;
	float distance = 100.f;
	bool drawWireframe = false;
	bool refreshMesh = true;

	bool quit = false;
	while (!quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;

			case SDL_MOUSEMOTION:
				HandleMouseMove(event.motion, rotateX, rotateY);
				break;

			case SDL_MOUSEWHEEL:
				HandleMouseWheel(event.wheel, distance);
				break;

			case SDL_KEYUP:
				HandleKeyPress(event.key, drawWireframe, refreshMesh);
				break;
			}
		}

		if (refreshMesh)
		{
			refreshMesh = false;
			thresholdIndex = (thresholdIndex + 1) % MAX_THRESHOLDS;
			printf("Generating mesh with error threshold=%.1f...\n", THRESHOLDS[thresholdIndex]);
			auto startTime = SDL_GetTicks();

			VertexBuffer vertices;
			IndexBuffer indices;

			root = BuildOctree(glm::ivec3(-octreeSize / 2), octreeSize, THRESHOLDS[thresholdIndex]);
			GenerateMeshFromOctree(root, vertices, indices);
			mesh.uploadData(vertices, indices);
			printf("Generated mesh (%.1fs)\n\n", (float)(SDL_GetTicks() - startTime) / 1000.f);
		}

		if ((SDL_GetTicks() - lastFrameTime) < 33)
		{
			continue;
		}

		lastFrameTime = SDL_GetTicks();

		// calculate the forward vector and then use that find the camera position
		glm::vec3 dir(0.f, 0.f, 1.f);
		dir = glm::rotateX(dir, rotateX);
		dir = glm::rotateY(dir, rotateY);

		glm::vec3 position = dir * distance;

		DrawFrame(program, mesh, position, -dir, drawWireframe);

		SDL_GL_SwapWindow(window);
	}

	DestroyOctree(root);
	mesh.destroy();

	SDL_GL_DeleteContext(context);
	SDL_Quit();

	return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------

