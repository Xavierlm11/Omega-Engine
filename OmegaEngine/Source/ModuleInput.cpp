#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleImport.h"

#include <iostream>

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_DROPFILE:
			{

				
				/////////////[BEFORE]
				//char* dropped_filedir;
				std::string dropped_filedir_s = e.drop.file;
				std::string fileName_s = dropped_filedir_s.substr(dropped_filedir_s.find_last_of('\\') + 1);
				std::string extension_s = fileName_s.substr(fileName_s.find_last_of('.') + 1);
				std::string assetsPath_s = "Assets/";
				
				const char* fileName = fileName_s.c_str();

				std::string finalAssetPath = assetsPath_s + fileName;
				
				LOG("PATH: %s", finalAssetPath.c_str());
				
				if (extension_s == "png") {
					if (App->editor->GOIndex > -1 && App->editor->GOIndex < App->scene->ListGO.size()) {
						if (App->scene->ListGO[App->editor->GOIndex] != nullptr) {
							//App->scene->ListGO[App->editor->GOIndex]->GOmat->CmMat->texture_id = App->imp->ImportTexture(finalPath.c_str());
							
						}
					}
					App->imp->LoadFile(e.drop.file, Resource::Types::MATERIAL);
					
				}
				else if (extension_s == "fbx")
				{
					//App->scene->LoadCustom(dropped_filedir.c_str(), &App->scene->models[0]->meshes);
					
					/*if()
					App->scene->LoadCustom(dropped_filedir.c_str(), &App->scene);*/
					//App->scene->LoadCustomObj(dropped_filedir_s.c_str(),fileName_s.c_str());
					App->imp->LoadFile(e.drop.file, Resource::Types::MODEL);
					App->fileSystem->ImportFileToAssets(dropped_filedir_s.c_str());

					std::string libraryPath_s = "Library/";
					std::string finalLibraryPath = libraryPath_s + fileName;

					uint size = 0;
					char* buffer;
					//MeshImporter::Save();
					App->fileSystem->SaveFile(finalLibraryPath, buffer, size);
				}


				break;
			}

			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
					App->renderer3D->OnResize(0, 0, e.window.data1, e.window.data2);//e.window.data1, e.window.data2);
				}
				break;
			}

			case SDL_QUIT:
				quit = true;
				break;
			

		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP||App->editor->close==true)
	{
		
		App->editor->close = false;
		CallClose = true;


	}

	if(close)return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}
