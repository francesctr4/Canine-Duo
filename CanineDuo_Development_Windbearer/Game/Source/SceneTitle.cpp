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
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer/include/SDL_mixer.h"

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
	MenuScreen = app->tex->Load("Assets/Textures/MenuScreen.png");
	OptionsTex = app->tex->Load("Assets/Textures/OptionsScreen.png");
	CreditsTex = app->tex->Load("Assets/Textures/CreditsScreen.png");

	char lookupTable[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,“”’?!_#$%()+-/:;<=>[*]^{|}~ ©" };

	WindjammersFont = app->fonts->Load("Assets/Fonts/WindjammersFont.png", lookupTable, 1);

	enableMusic = true;

	// UI

	play = app->tex->Load("Assets/UI/Play.png");

	Play = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, play, "", {427,375,163,49}, this);

	continue_ = app->tex->Load("Assets/UI/Continue.png");
	Continue_ = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, continue_, "", { 360,452,301,49 }, this);
	Continue_->state = GuiControlState::DISABLED;

	settings = app->tex->Load("Assets/UI/Settings.png");

	Settings = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, settings, "", { 365,530,288,49 }, this);

	credits = app->tex->Load("Assets/UI/Credits.png");

	Credits = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, credits, "", { 388,607,248,49 }, this);

	exit = app->tex->Load("Assets/UI/Exit.png");

	Exit = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, exit, "", { 438,679,145,49 }, this);

	back = app->tex->Load("Assets/UI/Back.png");
	Back = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, back, "", { 704,619,112,59 }, this);
	Back->state = GuiControlState::DISABLED;

	slider = app->tex->Load("Assets/UI/Slider.png");

	SliderMusic = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 7, slider, "", { 621,220,30,59 }, this);
	SliderMusic->state = GuiControlState::DISABLED;

	SliderFX = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 8, slider, "", { 621,375,30,59 }, this);
	SliderFX->state = GuiControlState::DISABLED;

	checkBox = app->tex->Load("Assets/UI/CheckBox.png");

	CheckBoxFullscreen = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 9, checkBox, "", { 701,472,48,47 }, this);
	CheckBoxFullscreen->state = GuiControlState::DISABLED;

	CheckBoxVsync = (GuiCheckBox*)app->guiManager->CreateGuiControl(GuiControlType::CHECKBOX, 10, checkBox, "", { 610,570,48,47 }, this);
	CheckBoxVsync->state = GuiControlState::DISABLED;

	BackCredits = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, back, "", { 704,619,112,59 }, this);
	BackCredits->state = GuiControlState::DISABLED;

	showSettings = false;
	showCredits = false;
	continueEnabled = false;

	OpenPause = app->audio->LoadFx("Assets/Audio/Fx/OpenPause.wav");
	ClosePause = app->audio->LoadFx("Assets/Audio/Fx/ClosePause.wav");

	return true;
}

// Called each loop iteration
bool SceneTitle::PreUpdate()
{
	OPTICK_EVENT();
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
	OPTICK_EVENT();
	if (Play->state == GuiControlState::PRESSED) {

		enableMusic = true;

		app->scene->player->isAlive = true;

		app->scene->player->pbody->body->SetTransform({ PIXEL_TO_METERS(50),PIXEL_TO_METERS(896) }, 0);

		app->fadeToBlack->Fade(this, (Module*)app->scene, 0);

	}

	if (Continue_->state == GuiControlState::PRESSED) {

		enableMusic = true;

		app->scene->player->isAlive = true;

		app->scene->player->pbody->body->SetTransform({ PIXEL_TO_METERS(app->entityManager->playerX),PIXEL_TO_METERS(app->entityManager->playerY)}, 0);

		app->fadeToBlack->Fade(this, (Module*)app->scene, 0);

	}

	if (Settings->state == GuiControlState::PRESSED) {

		showSettings = true;
		app->audio->PlayFx(OpenPause);
		Back->state = GuiControlState::NORMAL;

	}

	if (Back->state == GuiControlState::PRESSED) {
		showSettings = false;
		app->audio->PlayFx(ClosePause);
	}

	if (Credits->state == GuiControlState::PRESSED) {

		showCredits = true;
		app->audio->PlayFx(OpenPause);
		BackCredits->state = GuiControlState::NORMAL;

	}

	if (BackCredits->state == GuiControlState::PRESSED) {

		showCredits = false;
		app->audio->PlayFx(ClosePause);

	}

	if (CheckBoxFullscreen->crossed) {

		app->scene->CheckBoxFullscreen->crossed = true;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		SDL_RenderSetLogicalSize(app->render->renderer,1024,768);

	}

	if (!CheckBoxFullscreen->crossed) {

		app->scene->CheckBoxFullscreen->crossed = false;
		SDL_SetWindowFullscreen(app->win->window, 0);

	}

	if (CheckBoxVsync->crossed) {

		/*SDL_DestroyRenderer(app->render->renderer);

		Uint32 flags = SDL_RENDERER_ACCELERATED;

		flags |= SDL_RENDERER_PRESENTVSYNC;

		app->render->renderer = SDL_CreateRenderer(app->win->window, -1, flags);*/

		app->scene->CheckBoxVsync->crossed = true;
		SDL_GL_SetSwapInterval(1);

	}

	if (!CheckBoxVsync->crossed) {

		app->scene->CheckBoxVsync->crossed = false;
		SDL_GL_SetSwapInterval(0);

	}

	app->scene->SliderMusic->bounds.x = SliderMusic->bounds.x;
	app->scene->SliderMusic->posx = SliderMusic->posx;
	
	app->scene->SliderFX->bounds.x = SliderFX->bounds.x;
	app->scene->SliderFX->posx = SliderFX->posx;

	Mix_VolumeMusic((SliderMusic->bounds.x - 324) * (128 - 0) / (674 - 324) + 0);

	for (int i = 0; i < app->audio->fx.Count(); i++) {

		Mix_VolumeChunk(app->audio->fx.At(i)->data, (SliderFX->bounds.x - 324) * (128 - 0) / (674 - 324) + 0);

	}

	app->scene->startCounting = true;

	return true;
}

// Called each loop iteration
bool SceneTitle::PostUpdate()
{
	OPTICK_EVENT();
	bool ret = true;

	app->render->camera.x = 0;
	app->render->camera.y = -192;

	app->render->DrawTexture(MenuScreen, 0, 192);

	//app->render->DrawTexture(MenuBackground, 0, 192);

	//app->render->DrawTexture(WindbearerLogo, 120, 440);

	app->guiManager->Draw();

	if (showCredits) {

		if (BackCredits->state == GuiControlState::DISABLED) BackCredits->state = GuiControlState::NORMAL;

		if (Play->state != GuiControlState::DISABLED) Play->state = GuiControlState::DISABLED;
		if (Continue_->state != GuiControlState::DISABLED) Continue_->state = GuiControlState::DISABLED;
		if (Settings->state != GuiControlState::DISABLED) Settings->state = GuiControlState::DISABLED;
		if (Credits->state != GuiControlState::DISABLED) Credits->state = GuiControlState::DISABLED;
		if (Exit->state != GuiControlState::DISABLED) Exit->state = GuiControlState::DISABLED;

		app->render->DrawTexture(CreditsTex, 0, 192);

		BackCredits->Draw(app->render);

	}

	if (!showCredits) {

		if (Play->state == GuiControlState::DISABLED) Play->state = GuiControlState::NORMAL;
		if (Continue_->state == GuiControlState::DISABLED && continueEnabled) Continue_->state = GuiControlState::NORMAL;
		if (Settings->state == GuiControlState::DISABLED) Settings->state = GuiControlState::NORMAL;
		if (Credits->state == GuiControlState::DISABLED) Credits->state = GuiControlState::NORMAL;
		if (Exit->state == GuiControlState::DISABLED) Exit->state = GuiControlState::NORMAL;

		if (BackCredits->state != GuiControlState::DISABLED) BackCredits->state = GuiControlState::DISABLED;

	}

	if (showSettings) {

		if (Back->state == GuiControlState::DISABLED) Back->state = GuiControlState::NORMAL;
		if (SliderMusic->state == GuiControlState::DISABLED) SliderMusic->state = GuiControlState::NORMAL;
		if (SliderFX->state == GuiControlState::DISABLED) SliderFX->state = GuiControlState::NORMAL;
		if (CheckBoxFullscreen->state == GuiControlState::DISABLED) CheckBoxFullscreen->state = GuiControlState::NORMAL;
		if (CheckBoxVsync->state == GuiControlState::DISABLED) CheckBoxVsync->state = GuiControlState::NORMAL;

		if (Play->state != GuiControlState::DISABLED) Play->state = GuiControlState::DISABLED;
		if (Continue_->state != GuiControlState::DISABLED) Continue_->state = GuiControlState::DISABLED;
		if (Settings->state != GuiControlState::DISABLED) Settings->state = GuiControlState::DISABLED;
		if (Credits->state != GuiControlState::DISABLED) Credits->state = GuiControlState::DISABLED;
		if (Exit->state != GuiControlState::DISABLED) Exit->state = GuiControlState::DISABLED;
		
		app->render->DrawTexture(OptionsTex, 0, 192);

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

		if (Play->state == GuiControlState::DISABLED) Play->state = GuiControlState::NORMAL;
		if (Continue_->state == GuiControlState::DISABLED && continueEnabled) Continue_->state = GuiControlState::NORMAL;
		if (Settings->state == GuiControlState::DISABLED) Settings->state = GuiControlState::NORMAL;
		if (Credits->state == GuiControlState::DISABLED) Credits->state = GuiControlState::NORMAL;
		if (Exit->state == GuiControlState::DISABLED) Exit->state = GuiControlState::NORMAL;

		if (Back->state != GuiControlState::DISABLED) Back->state = GuiControlState::DISABLED;
		if (SliderMusic->state != GuiControlState::DISABLED) SliderMusic->state = GuiControlState::DISABLED;
		if (SliderFX->state != GuiControlState::DISABLED) SliderFX->state = GuiControlState::DISABLED;
		if (CheckBoxFullscreen->state != GuiControlState::DISABLED) CheckBoxFullscreen->state = GuiControlState::DISABLED;
		if (CheckBoxVsync->state != GuiControlState::DISABLED) CheckBoxVsync->state = GuiControlState::DISABLED;

	}

	app->scene->player->hitsTaken = 0;

	if (Exit->state == GuiControlState::PRESSED || app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || app->input->controllers[0].buttons[SDL_CONTROLLER_BUTTON_BACK] == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool SceneTitle::CleanUp()
{
	LOG("Freeing scene");

	return true;
}