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
#include "SceneTitle.h"

#include "Defs.h"
#include "Log.h"

#include "SDL_mixer/include/SDL_mixer.h"

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
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;

		if (SString(itemNode.attribute("type").as_string()) == SString("Checkpoint")) {

			checkpoints.emplace_back(item);

		}

	}

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

	playerUI = app->tex->Load("Assets/UI/PlayerUI.png");
	essenceUI = app->tex->Load("Assets/UI/EssenceUI.png");
	timeUI = app->tex->Load("Assets/UI/TimeUI.png");

	pauseScreen = app->tex->Load("Assets/Textures/PauseScreen.png");

	pause = false;

	resume = app->tex->Load("Assets/UI/Resume.png");
	Resume = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, resume, "", { 394,290,233,49 }, this);
	Resume->state = GuiControlState::DISABLED;

	settings = app->tex->Load("Assets/UI/Settings.png");
	Settings = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, settings, "", { 365,382,288,49 }, this);
	Settings->state = GuiControlState::DISABLED;

	backToTitle = app->tex->Load("Assets/UI/BackToTitle.png");
	BackToTitle = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, backToTitle, "", { 288,478,440,49 }, this);
	BackToTitle->state = GuiControlState::DISABLED;

	exit = app->tex->Load("Assets/UI/Exit.png");
	Exit = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, exit, "", { 438,569,145,49 }, this);
	Exit->state = GuiControlState::DISABLED;

	back = app->tex->Load("Assets/UI/Back.png");
	Back = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, back, "", { 704,616,112,59 }, this);
	Back->state = GuiControlState::DISABLED;

	slider = app->tex->Load("Assets/UI/Slider.png");

	SliderMusic = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 7, slider, "", { 621,216,30,59 }, this);
	SliderMusic->state = GuiControlState::DISABLED;

	SliderFX = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 8, slider, "", { 621,371,30,59 }, this);
	SliderFX->state = GuiControlState::DISABLED;

	checkBox = app->tex->Load("Assets/UI/CheckBox.png");

	CheckBoxFullscreen = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, checkBox, "", { 701,468,48,47 }, this);
	CheckBoxFullscreen->state = GuiControlState::DISABLED;

	CheckBoxVsync = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 10, checkBox, "", { 610,566,48,47 }, this);
	CheckBoxVsync->state = GuiControlState::DISABLED;

	OptionsTex = app->tex->Load("Assets/Textures/OptionsScreenFromPause.png");

	showSettings = false;

	startCounting = true;

	itemsCollected = 0;

	OpenPause = app->audio->LoadFx("Assets/Audio/Fx/OpenPause.wav");
	ClosePause = app->audio->LoadFx("Assets/Audio/Fx/ClosePause.wav");

	checkpointIterator = 1;

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	OPTICK_EVENT();
	if (enableMusic) {

		app->audio->PlayMusic("Assets/Audio/Music/Level1_Music.ogg", 0);
		enableMusic = false;

	}

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	OPTICK_EVENT();
	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)

	if (startCounting) {

		timeElapsed.Start();
		startCounting = false;

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

		app->scene->player->hitsTaken = 0;
		app->render->camera.x = 0;
		app->render->camera.y = -192;

		app->scene->player->isAlive = true;

		app->scene->player->pbody->body->SetTransform({ PIXEL_TO_METERS(50),PIXEL_TO_METERS(896) }, 0);

	}

	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {

		// Start from second level

	}

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {

		app->scene->player->hitsTaken = 0;
		app->render->camera.x = 0;
		app->render->camera.y = -192;

		app->scene->player->isAlive = true;

		app->scene->player->pbody->body->SetTransform({ PIXEL_TO_METERS(50),PIXEL_TO_METERS(896) }, 0);

	}

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {

		app->SaveGameRequest();

		app->sceneTitle->Continue_->state = GuiControlState::NORMAL;

		app->sceneTitle->continueEnabled = true;

	}
		
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {

		if (!app->physics->debug) app->physics->debug = true;
		else app->physics->debug = false;

	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {

		app->scene->player->hitsTaken = 0;
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

		app->fadeToBlack->Fade(this, (Module*)app->sceneEnding, 0);

	}

	if (BackToTitle->state == GuiControlState::PRESSED) {

		pause = false;

		if (Resume->state != GuiControlState::DISABLED) Resume->state = GuiControlState::DISABLED;
		if (Settings->state != GuiControlState::DISABLED) Settings->state = GuiControlState::DISABLED;
		if (BackToTitle->state != GuiControlState::DISABLED) BackToTitle->state = GuiControlState::DISABLED;
		if (Exit->state != GuiControlState::DISABLED) Exit->state = GuiControlState::DISABLED;

		enableMusic = true;

		app->fadeToBlack->Fade(this, (Module*)app->sceneTitle, 0);

	}

	currentAnimation->Update();

	SDL_Rect elysianRect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(Elysian, 150, 177, &elysianRect);

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || Resume->state == GuiControlState::PRESSED) {

		if (pause) {

			pause = false;
			app->audio->PlayFx(ClosePause);
		}
		else {

			pause = true;
			app->audio->PlayFx(OpenPause);
		}

		showSettings = false;

	}

	if (Settings->state == GuiControlState::PRESSED) {

		showSettings = true;
		Back->state = GuiControlState::NORMAL;

	}

	if (Back->state == GuiControlState::PRESSED) {
		showSettings = false;
	}

	app->sceneTitle->SliderMusic->bounds.x = SliderMusic->bounds.x;
	app->sceneTitle->SliderMusic->posx = SliderMusic->posx;

	app->sceneTitle->SliderFX->bounds.x = SliderFX->bounds.x;
	app->sceneTitle->SliderFX->posx = SliderFX->posx;

	Mix_VolumeMusic((SliderMusic->bounds.x - 324) * (128 - 0) / (674 - 324) + 0);
	
	for (int i = 0; i < app->audio->fx.Count(); i++) {

		Mix_VolumeChunk(app->audio->fx.At(i)->data,(SliderFX->bounds.x - 324) * (128 - 0) / (674 - 324) + 0);

	}

	if (app->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN) {

		if (checkpointIterator == 0) checkpointIterator = 1;
		else checkpointIterator = 0;

		int x = checkpoints.at(checkpointIterator)->parameters.attribute("x").as_int();
		int y = checkpoints.at(checkpointIterator)->parameters.attribute("y").as_int();

		app->scene->player->pbody->body->SetTransform({ PIXEL_TO_METERS(x),PIXEL_TO_METERS(y) }, 0);

	}

	if (CheckBoxFullscreen->crossed) {

		app->sceneTitle->CheckBoxFullscreen->crossed = true;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		SDL_RenderSetLogicalSize(app->render->renderer, 1024, 768);

	}

	if (!CheckBoxFullscreen->crossed) {

		app->sceneTitle->CheckBoxFullscreen->crossed = false;
		SDL_SetWindowFullscreen(app->win->window, 0);

	}

	if (CheckBoxVsync->crossed) {

		/*SDL_DestroyRenderer(app->render->renderer);

		Uint32 flags = SDL_RENDERER_ACCELERATED;

		flags |= SDL_RENDERER_PRESENTVSYNC;

		app->render->renderer = SDL_CreateRenderer(app->win->window, -1, flags);*/

		app->sceneTitle->CheckBoxVsync->crossed = true;
		SDL_GL_SetSwapInterval(1);

	}

	if (!CheckBoxVsync->crossed) {

		app->sceneTitle->CheckBoxVsync->crossed = false;
		SDL_GL_SetSwapInterval(0);

	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	OPTICK_EVENT();
	bool ret = true;

	SDL_Rect rect = { 0,0 + 191 * player->hitsTaken,479,191 };
	app->render->DrawTexture(playerUI,-app->render->camera.x + 20, -app->render->camera.y - 23,&rect);

	app->render->DrawTexture(essenceUI, -app->render->camera.x + 565, -app->render->camera.y + 23);

	app->render->DrawTexture(timeUI, -app->render->camera.x + 797, -app->render->camera.y + 24);

	if (timeElapsed.ReadSec() < 10) {

		app->render->DrawText(std::to_string((int)(timeElapsed.ReadSec())), 910, 40, 41, 71, { 255,255,255 });

	}
	else if (timeElapsed.ReadSec() >= 10 && timeElapsed.ReadSec() < 100) {

		app->render->DrawText(std::to_string((int)(timeElapsed.ReadSec())), 880, 40, 71, 71, { 255,255,255 });

	}
	else {

		app->render->DrawText(std::to_string((int)(timeElapsed.ReadSec())), 870, 40, 101, 71, { 255,255,255 });

	}

	if (itemsCollected < 10) {

		app->render->DrawText(std::to_string((int)(itemsCollected)), 680, 40, 41, 71, { 255,255,255 });

	}
	else {

		app->render->DrawText(std::to_string((int)(itemsCollected)), 650, 40, 71, 71, { 255,255,255 });

	}
	
	if (pause) {

		app->render->DrawTexture(pauseScreen, -app->render->camera.x, -app->render->camera.y);

		if (Resume->state == GuiControlState::DISABLED) Resume->state = GuiControlState::NORMAL;
		if (Settings->state == GuiControlState::DISABLED) Settings->state = GuiControlState::NORMAL;
		if (BackToTitle->state == GuiControlState::DISABLED) BackToTitle->state = GuiControlState::NORMAL;
		if (Exit->state == GuiControlState::DISABLED) Exit->state = GuiControlState::NORMAL;

		Resume->Draw(app->render);
		Settings->Draw(app->render);
		BackToTitle->Draw(app->render);
		Exit->Draw(app->render);

		if (showSettings) {

			if (Back->state == GuiControlState::DISABLED) Back->state = GuiControlState::NORMAL;
			if (SliderMusic->state == GuiControlState::DISABLED) SliderMusic->state = GuiControlState::NORMAL;
			if (SliderFX->state == GuiControlState::DISABLED) SliderFX->state = GuiControlState::NORMAL;
			if (CheckBoxFullscreen->state == GuiControlState::DISABLED) CheckBoxFullscreen->state = GuiControlState::NORMAL;
			if (CheckBoxVsync->state == GuiControlState::DISABLED) CheckBoxVsync->state = GuiControlState::NORMAL;

			if (Resume->state != GuiControlState::DISABLED) Resume->state = GuiControlState::DISABLED;
			
			if (Settings->state != GuiControlState::DISABLED) Settings->state = GuiControlState::DISABLED;
			if (BackToTitle->state != GuiControlState::DISABLED) BackToTitle->state = GuiControlState::DISABLED;
			if (Exit->state != GuiControlState::DISABLED) Exit->state = GuiControlState::DISABLED;

			app->render->DrawTexture(OptionsTex, -app->render->camera.x, -app->render->camera.y);

			Back->Draw(app->render);
			SliderMusic->Draw(app->render);
			SliderFX->Draw(app->render);
			CheckBoxFullscreen->Draw(app->render);
			CheckBoxVsync->Draw(app->render);

			/*if (app->win->configWindow.child("fullscreen").attribute("value").as_bool() == false) {
				app->win->configWindow.child("fullscreen").append_attribute("value") = "true";
			}*/

		}

		if (!showSettings) {

			if (Resume->state == GuiControlState::DISABLED) Resume->state = GuiControlState::NORMAL;
			if (Settings->state == GuiControlState::DISABLED) Settings->state = GuiControlState::NORMAL;
			if (BackToTitle->state == GuiControlState::DISABLED) BackToTitle->state = GuiControlState::NORMAL;
			if (Exit->state == GuiControlState::DISABLED) Exit->state = GuiControlState::NORMAL;

			if (Back->state != GuiControlState::DISABLED) Back->state = GuiControlState::DISABLED;
			if (SliderMusic->state != GuiControlState::DISABLED) SliderMusic->state = GuiControlState::DISABLED;
			if (SliderFX->state != GuiControlState::DISABLED) SliderFX->state = GuiControlState::DISABLED;
			if (CheckBoxFullscreen->state != GuiControlState::DISABLED) CheckBoxFullscreen->state = GuiControlState::DISABLED;
			if (CheckBoxVsync->state != GuiControlState::DISABLED) CheckBoxVsync->state = GuiControlState::DISABLED;

		}

	}

	if (!pause) {

		if (Resume->state != GuiControlState::DISABLED) Resume->state = GuiControlState::DISABLED;
		if (Settings->state != GuiControlState::DISABLED) Settings->state = GuiControlState::DISABLED;
		if (BackToTitle->state != GuiControlState::DISABLED) BackToTitle->state = GuiControlState::DISABLED;
		if (Exit->state != GuiControlState::DISABLED) Exit->state = GuiControlState::DISABLED;

	}

	if (Exit->state == GuiControlState::PRESSED || app->input->controllers[0].buttons[SDL_CONTROLLER_BUTTON_BACK] == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
