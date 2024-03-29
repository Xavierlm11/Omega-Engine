#include "Globals.h"
#include "Glew/include/glew.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "SDL/include/SDL_opengl.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#pragma comment (lib, "Source/External/Glew/libx86/glew32.lib")

#ifdef _DEBUG
#pragma comment( lib, "Source/External/MathGeoLib/libx86/MGLDebug/MathGeoLib.lib")
#else
#pragma comment (lib, "Source/External/MathGeoLib/libx86/MGLRelease/MathGeoLib.lib")
#endif

#include "CCamera.h"
#include "Primitive.h"


ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	context = nullptr;
	mode = RenderMode::NORMAL;
	buffersLoaded = false;
	hasLoadedMesh = false;

	num_vertices = 0;
	my_id = 0;

	my_indices = 0;
	elementbuffer = 0;
	num_indices = 0;
	vertexbuffer = 0;

	checkersID = 0;

	houseTexID = 0;

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)0;
			checkerImage[i][j][1] = (GLubyte)0;
			checkerImage[i][j][2] = (GLubyte)0;
			checkerImage[i][j][3] = (GLubyte)0;
		}
	}
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	//Glew library
	GLenum err = glewInit();
	LOG("Using Glew %s", glewGetString(GLEW_VERSION));

	//Current hardware and driver capabilities
	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));


	if(ret == true)
	{
		
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		


		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		lights[0].Active(true);

		LoadTextureBuffers();
		//houseTexID = App->imp->ImportTexture("Assets/Baker_house.png");

		
		OnResize(0,0, App->window->winWidth, App->window->winHeight );
		
	}
	
	

	return ret;
}

void ModuleRenderer3D::LoadTextureBuffers() {

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	//glBindTexture(GL_TEXTURE_2D, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &checkersID);
	glBindTexture(GL_TEXTURE_2D, checkersID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	glBindTexture(GL_TEXTURE_2D, 0);
	//
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		BindCamBuffer(App->camera->ScnCam);

		

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		
		
		/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(c.r, c.g, c.b, c.a);*/

		
	}
	

	// light 0 on cam pos
	float3 camPos = App->camera->ScnCam->cameraFrustum.pos;
	lights[0].SetPos(camPos.x, camPos.y, camPos.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Update(float dt)
{
	

	

	return UPDATE_CONTINUE;
}

void ModuleRenderer3D::DrawMesh(MeshData* mesh) {

	
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	

	GoRender();

	
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (MainCam != nullptr)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glLoadIdentity();
		BindCamBuffer(MainCam);
		GoRenderGame();

		App->physics->DrawWorldGame();

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	App->editor->Draw();

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

void ModuleRenderer3D::GoRender()
{
	PrimPlane p(0, 0, 0, 0);
	p.axis = true;
	p.Render();
	switch (mode)
	{
	case RenderMode::NONE:

		break;
	case RenderMode::NORMAL:
		
		
		for (int i = 0; i <meshlist.size(); i++)
		{
			if (meshlist[i] != nullptr)
			{

				meshlist[i]->MeshRenderer();

			}
		}
	//	meshlist.clear();
		//DrawCube();
		break;
	case RenderMode::CHECKERS:
	
		for (int i = 0; i < meshlist.size(); i++)
		{
			if (meshlist[i] != nullptr)
			{

				meshlist[i]->MeshRenderer();

			}
		}
		//meshlist.clear();
		//DrawCube();
		break;
	case RenderMode::WIREFRAME:
		
		for (int i = 0; i < meshlist.size(); i++)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (meshlist[i] != nullptr)
			{

				meshlist[i]->MeshRenderer();

			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	//	meshlist.clear();
		//Draw();
		break;
	}
}

void ModuleRenderer3D::GoRenderGame()
{
	PrimPlane p(0, 0, 0, 0);
	p.axis = true;
	p.Render();
	switch (mode)
	{
	case RenderMode::NONE:

		break;
	case RenderMode::NORMAL:
		
		
		for (int i = 0; i <meshlist.size(); i++)
		{
			if (meshlist[i] != nullptr)
			{

				meshlist[i]->MeshRenderer();

			}
		}
		meshlist.clear();
		//DrawCube();
		break;
	case RenderMode::CHECKERS:
	
		for (int i = 0; i < meshlist.size(); i++)
		{
			if (meshlist[i] != nullptr)
			{

				meshlist[i]->MeshRenderer();

			}
		}
		meshlist.clear();
		//DrawCube();
		break;
	case RenderMode::WIREFRAME:
		
		for (int i = 0; i < meshlist.size(); i++)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (meshlist[i] != nullptr)
			{

				meshlist[i]->MeshRenderer();

			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		meshlist.clear();
		//Draw();
		break;
	}
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	for (uint i = 0; i < meshlist.size(); ++i)
	{
		delete meshlist[i];
		meshlist[i] = nullptr;
	}
	meshlist.clear();
	return true;
}


void ModuleRenderer3D::Draw() {

}

GLuint ModuleRenderer3D::GetBuffCam()
{
	return checkersID;
}

void ModuleRenderer3D::BindCamBuffer(CCamera* _CCam)
{
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(_CCam->GetProjectionMatrixOpenGL());
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(_CCam->GetViewMatrixOpenGL());

	//glBindFramebuffer(GL_FRAMEBUFFER, _CCam->GetFrameBuffer());
	glBindFramebuffer(GL_FRAMEBUFFER, _CCam->NewFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Color c ;
	glClearColor(c.r, c.g, c.b, c.a);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ModuleRenderer3D::OnResize(int x, int y, int width, int height)
{
		App->window->winWidth = width;
		App->window->winHeight = height;

		glViewport(x, y, width, height);

		if (App->camera!=nullptr)
		{
			if (App->camera->ScnCam != nullptr)
				App->camera->ScnCam->LoadBuffer(width, height);
		}
		if (App->renderer3D != nullptr)
		{
			if (App->renderer3D->MainCam != nullptr)
				App->renderer3D->MainCam->LoadBuffer(width, height);
		}

}


void ModuleRenderer3D::DrawArrayCube()
{
	uint my_id = 0;
	float vertices[] = {
		//Vertices according to faces
		-1.0f,-1.0f,-1.0f, // triángulo 1 : comienza
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triángulo 1 : termina
		1.0f, 1.0f,-1.0f, // triángulo 2 : comienza
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triángulo 2 : termina
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};
	
	glGenBuffers(1, (GLuint*)&(my_id));
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertices, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	// … bind and use other buffers
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableClientState(GL_VERTEX_ARRAY);
}
void ModuleRenderer3D::DrawArrayIndexCube()
{
	uint my_id = 0;
	GLfloat vertices2[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
						1, 1, 1,   1,-1, 1,   1,-1,-1,   1, 1,-1,   // v0,v3,v4,v5 (right)
						1, 1, 1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,   // v0,v5,v6,v1 (top)
					   -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,   // v1,v6,v7,v2 (left)
					   -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,   // v7,v4,v3,v2 (bottom)
						1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1 };
	GLubyte indices[] = { 0,1,2, 2,3,0,   // 36 of indices
					 0,3,4, 4,5,0,
					 0,5,6, 6,1,0,
					 1,6,7, 7,2,1,
					 7,4,3, 3,2,7,
					 4,7,6, 6,5,4 };


	glGenBuffers(1, (GLuint*)&(my_id));
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertices2, GL_STATIC_DRAW);

	uint my_indices = 0;
	glGenBuffers(1, (GLuint*)&(my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);


	// activate and specify pointer to vertex array
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices2);

	// draw a cube
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);

	// deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);
	
}
void ModuleRenderer3D::DrawDirectCube()
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 10.f, 0.f);
	glEnd();
	glLineWidth(1.0f);

	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
									  // Top face (y = 1.0f)
									  // Define vertices in counter-clockwise (CCW) order with normal pointing out
	glColor3f(0.0f, 1.0f, 0.0f);     // Green
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	// Bottom face (y = -1.0f)
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	// Front face  (z = 1.0f)
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	// Back face (z = -1.0f)
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	// Left face (x = -1.0f)
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	// Right face (x = 1.0f)
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();  // End of drawing color-cube
	//glRotatef(190.f, 1.0f, 1.0f, 0.0f);
}
