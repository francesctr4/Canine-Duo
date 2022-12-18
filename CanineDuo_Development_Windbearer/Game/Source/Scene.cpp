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
#include "Pathfinding.h"

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

	configFile = config;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	/*for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}*/

	//L02: DONE 3: Instantiate the player using the entity manager

	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	player->parameters = config.child("player");

	// Instantiate the enemies using the entity manager

	for (pugi::xml_node enemyNode = config.child("enemy"); enemyNode; enemyNode = enemyNode.next_sibling("enemy"))
	{
		Enemy* enemy = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY);
		enemy->parameters = enemyNode;

		enemies.Add(enemy);

	}

	for (int i = 0; i < 4; i++) {

		ElysianIdle.PushBack({ 100 * (0 + i), 54 * 0, 100, 54 });

	}
	ElysianIdle.loop = true;
	ElysianIdle.speed = 0.025f;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	
	// L03: DONE: Load map
	bool retLoad = app->map->Load();

	// L12 Create walkability map
	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}

	if (app->map->mapData.type == MapTypes::MAPTYPE_ORTHOGONAL) {

		// Texture to highligh mouse position 
		mouseTileTex = app->tex->Load("Assets/Maps/path_square.png");

		// Texture to show path origin 
		originTex = app->tex->Load("Assets/Maps/x_square.png");
	}

	parallax1 = app->tex->Load("Assets/Maps/Background_day1.png");
	parallax2 = app->tex->Load("Assets/Maps/Background_day2.png");
	parallax3 = app->tex->Load("Assets/Maps/Background_day3.png");
	parallax4 = app->tex->Load("Assets/Maps/Background_day4.png");

	playerTile = iPoint(0, 0);

	enableMusic = true;

	start = 0;

	stop = 0;

	Elysian = app->tex->Load("Assets/Textures/Elysian_Idle.png");

	currentAnimation = &ElysianIdle;

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

	app->render->DrawTexture(parallax1, 0, 16);
	app->render->DrawTexture(parallax2, 0, 30, nullptr, 1.1f);
	app->render->DrawTexture(parallax3, 0, 44, nullptr, 1.2f);
	app->render->DrawTexture(parallax4, 0, 58, nullptr, 1.3f);

	app->render->DrawTexture(parallax1, 1280, 16);
	app->render->DrawTexture(parallax2, 1280, 30, nullptr, 1.1f);
	app->render->DrawTexture(parallax3, 1280, 44, nullptr, 1.2f);
	app->render->DrawTexture(parallax4, 1280, 58, nullptr, 1.3f);

	app->map->Draw();

	if (!app->scene->player->isAlive) {

		stop = SDL_GetTicks();

	}
	else {

		start = SDL_GetTicks();

	}

	stop = SDL_GetTicks();

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

	// L08: DONE 3: Test World to map method

	//int mouseX, mouseY;
	//app->input->GetMousePosition(mouseX, mouseY);

	//iPoint mouseTile = iPoint(0, 0);

	//if (app->map->mapData.type == MapTypes::MAPTYPE_ORTHOGONAL) {
	//	mouseTile = app->map->WorldToMap(mouseX - app->render->camera.x,
	//		mouseY - app->render->camera.y);
	//}

	////Convert again the tile coordinates to world coordinates to render the texture of the tile
	//iPoint highlightedTileWorld = app->map->MapToWorld(mouseTile.x, mouseTile.y);
	//if (app->physics->debug) app->render->DrawTexture(mouseTileTex, highlightedTileWorld.x, highlightedTileWorld.y);

	////Test compute path function
	//if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	//{
	//	if (originSelected == true)
	//	{
	//		app->pathfinding->CreatePath(origin, mouseTile);
	//		originSelected = false;
	//	}
	//	else
	//	{
	//		origin = mouseTile;
	//		originSelected = true;
	//		app->pathfinding->ClearLastPath();
	//	}
	//}

	int playerX, playerY;
	player->pbody->GetPosition(playerX, playerY);

	if (app->map->mapData.type == MapTypes::MAPTYPE_ORTHOGONAL) {

		playerTile = app->map->WorldToMap(playerX, playerY);

	}

	//Convert again the tile coordinates to world coordinates to render the texture of the tile
	iPoint highlightedTileWorld = app->map->MapToWorld(playerTile.x, playerTile.y);
	if (app->physics->debug) app->render->DrawTexture(mouseTileTex, highlightedTileWorld.x, highlightedTileWorld.y);

	// L12: Get the latest calculated path and draw
	//const DynArray<iPoint>* path = app->pathfinding->GetLastPath();
	//for (uint i = 0; i < path->Count(); ++i)
	//{
	//	iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
	//	if (app->physics->debug) app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
	//}

	// L12: Debug pathfinding
	iPoint originScreen = app->map->MapToWorld(origin.x, origin.y);
	if (app->physics->debug) app->render->DrawTexture(originTex, originScreen.x, originScreen.y);

	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN || app->input->controllers[0].buttons[SDL_CONTROLLER_BUTTON_START] == KEY_DOWN || stop - start > 1000) {

		enableMusic = true;

		app->scene->player->isAlive = true;

		app->scene->player->pbody->body->SetTransform({ PIXEL_TO_METERS(50),PIXEL_TO_METERS(896) }, 0);

		app->fadeToBlack->Fade(this, (Module*)app->sceneEnding, 0);

	}

	currentAnimation->Update();

	SDL_Rect elysianRect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(Elysian, 150, 177, &elysianRect);

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->controllers[0].buttons[SDL_CONTROLLER_BUTTON_BACK] == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
