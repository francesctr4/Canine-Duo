#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "SceneEnding.h"
#include "EntityManager.h"
#include "Map.h"
#include "Physics.h"
#include "FadeToBlack.h"

#include "Defs.h"
#include "Log.h"

SceneEnding::SceneEnding(bool startEnabled) : Module(startEnabled)
{
	name.Create("sceneEnding");
}

// Destructor
SceneEnding::~SceneEnding()
{}

// Called before render is available
bool SceneEnding::Awake(pugi::xml_node& config)
{
	LOG("Loading SceneLogo");
	bool ret = true;


	return ret;
}

// Called before the first frame
bool SceneEnding::Start()
{
	
	endingScreen = app->tex->Load("Assets/Textures/EndingScreen.png");

	enableMusic = true;

	return true;
}

// Called each loop iteration
bool SceneEnding::PreUpdate()
{
	app->entityManager->Disable();

	app->physics->debug = false;

	if (enableMusic) {

		app->audio->PlayMusic("Assets/Audio/Music/NoMusic.ogg", 0);
		enableMusic = false;

	}

	return true;
}

// Called each loop iteration
bool SceneEnding::Update(float dt)
{

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN || app->input->controllers[0].buttons[SDL_CONTROLLER_BUTTON_START] == KEY_DOWN) {

		enableMusic = true;
		app->fadeToBlack->Fade(this, (Module*)app->sceneLogo, 0);

	}

	return true;
}

// Called each loop iteration
bool SceneEnding::PostUpdate()
{
	bool ret = true;

	app->render->camera.x = 0;
	app->render->camera.y = -192;

	app->render->DrawTexture(endingScreen, 60, 340);

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->controllers[0].buttons[SDL_CONTROLLER_BUTTON_BACK] == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool SceneEnding::CleanUp()
{
	LOG("Freeing scene");

	return true;
}