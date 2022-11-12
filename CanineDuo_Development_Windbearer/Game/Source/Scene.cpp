#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "EntityManager.h"
#include "Map.h"
#include "Physics.h"
#include "FadeToBlack.h"
#include "Fonts.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene(bool startEnabled) : Module(startEnabled)
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	//L02: DONE 3: Instantiate the player using the entity manager
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//img = app->tex->Load("Assets/Textures/test.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");
	
	parallax1 = app->tex->Load("Assets/Maps/Background_day1.png");
	parallax2 = app->tex->Load("Assets/Maps/Background_day2.png");
	parallax3 = app->tex->Load("Assets/Maps/Background_day3.png");
	parallax4 = app->tex->Load("Assets/Maps/Background_day4.png");

	// L03: DONE: Load map

	app->map->Load();

	// L04: DONE 7: Set the window title with map/tileset info
	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	app->win->SetTitle(title.GetString());

	enableMusic = true;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{

	if (enableMusic) {

		app->audio->PlayMusic("Assets/Audio/Music/Level1_Music.ogg", 0);
		enableMusic = false;

	}

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)

	if (!app->entityManager->IsEnabled()) {

		app->entityManager->Enable();

	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {

		app->render->camera.x = 0;
		app->render->camera.y = -192;

		app->scene->player->isAlive = true;

		app->scene->player->pbody->body->SetTransform({ PIXEL_TO_METERS(50),PIXEL_TO_METERS(896) }, 0);

	}

	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {

		// Start from second level

	}

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {

		app->render->camera.x = 0;
		app->render->camera.y = -192;

		app->scene->player->isAlive = true;

		app->scene->player->pbody->body->SetTransform({ PIXEL_TO_METERS(50),PIXEL_TO_METERS(896) }, 0);

	}

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		app->SaveGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	//if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) {

	//	if (!app->entityManager->IsEnabled()) app->entityManager->Enable();
	//	else app->entityManager->Disable();

	//}

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {

		if (!app->physics->debug) app->physics->debug = true;
		else app->physics->debug = false;

	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {

		app->scene->player->isAlive = true;

		if (!app->scene->player->godMode) app->scene->player->godMode = true;
		else app->scene->player->godMode = false;

	}

	//if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	//	app->render->camera.y += 1;

	//if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	//	app->render->camera.y -= 1;

	//if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	//	app->render->camera.x += 1;

	//if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	//	app->render->camera.x -= 1;

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {

		enableMusic = true;
		app->fadeToBlack->Fade(this, (Module*)app->sceneEnding, dt);

	}

	app->render->DrawTexture(parallax1, 0, 16);
	app->render->DrawTexture(parallax2, 0, 30, nullptr, 1.1f);
	app->render->DrawTexture(parallax3, 0, 44, nullptr, 1.2f);
	app->render->DrawTexture(parallax4, 0, 58, nullptr, 1.3f);

	app->render->DrawTexture(parallax1, 1280, 16);
	app->render->DrawTexture(parallax2, 1280, 30, nullptr, 1.1f);
	app->render->DrawTexture(parallax3, 1280, 44, nullptr, 1.2f);
	app->render->DrawTexture(parallax4, 1280, 58, nullptr, 1.3f);

	app->map->Draw();

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
