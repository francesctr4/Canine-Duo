#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "SceneTitle.h"
#include "EntityManager.h"
#include "Map.h"
#include "Physics.h"
#include "FadeToBlack.h"
#include "Fonts.h"

#include "Defs.h"
#include "Log.h"

SceneTitle::SceneTitle(bool startEnabled) : Module(startEnabled)
{
	name.Create("sceneTitle");
}

// Destructor
SceneTitle::~SceneTitle()
{}

// Called before render is available
bool SceneTitle::Awake(pugi::xml_node& config)
{
	LOG("Loading SceneTitle");
	bool ret = true;


	return ret;
}

// Called before the first frame
bool SceneTitle::Start()
{

	WindbearerLogo = app->tex->Load("Assets/Textures/WindbearerLogo.png");
	MenuBackground = app->tex->Load("Assets/Textures/MenuBackground.png");

	char lookupTable[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,“”’?!_#$%()+-/:;<=>[*]^{|}~ ©" };

	WindjammersFont = app->fonts->Load("Assets/Fonts/WindjammersFont.png", lookupTable, 1);

	enableMusic = true;

	return true;
}

// Called each loop iteration
bool SceneTitle::PreUpdate()
{

	app->entityManager->Disable();

	if (enableMusic) {

		app->audio->PlayMusic("Assets/Audio/Music/SymphonyOfBorealWind.ogg", 0);
		enableMusic = false;

	}

	return true;
}

// Called each loop iteration
bool SceneTitle::Update(float dt)
{

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {

		enableMusic = true;
		app->fadeToBlack->Fade(this, (Module*)app->scene, dt);

	}

	return true;
}

// Called each loop iteration
bool SceneTitle::PostUpdate()
{
	bool ret = true;

	app->render->camera.x = 0;
	app->render->camera.y = -192;

	app->render->DrawTexture(MenuBackground, 0, 192);

	app->render->DrawTexture(WindbearerLogo, 120, 440);

	//app->render->DrawTexture(WindbearerLogo, 120, 320);

	//app->fonts->BlitText(116 + 400, 180 + 200, WindjammersFont, "SAMPLE TEXT");

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool SceneTitle::CleanUp()
{
	LOG("Freeing scene");

	return true;
}