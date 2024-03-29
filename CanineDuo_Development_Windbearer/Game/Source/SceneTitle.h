#pragma once

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "GuiManager.h"
#include "GuiButton.h"
#include "GuiSlider.h"
#include "GuiCheckBox.h"

struct SDL_Texture;

class SceneTitle : public Module
{
public:

	SceneTitle(bool startEnabled);

	// Destructor
	virtual ~SceneTitle();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	int WindjammersFont = -1;
	bool enableMusic;

public:

	//SDL_Texture* WindbearerLogo;
	//SDL_Texture* MenuBackground;
	SDL_Texture* MenuScreen;
	SDL_Texture* OptionsTex;
	SDL_Texture* CreditsTex;

	SDL_Texture* play;
	GuiButton* Play;

	SDL_Texture* continue_;
	GuiButton* Continue_;
	bool continueEnabled;

	SDL_Texture* settings;
	GuiButton* Settings;

	bool showSettings;

	SDL_Texture* credits;
	GuiButton* Credits;

	SDL_Texture* exit;
	GuiButton* Exit;

	SDL_Texture* back;
	GuiButton* Back;

	SDL_Texture* slider;
	GuiSlider* SliderMusic;
	GuiSlider* SliderFX;

	SDL_Texture* checkBox;
	GuiCheckBox* CheckBoxFullscreen;
	GuiCheckBox* CheckBoxVsync;

	bool showCredits;
	GuiButton* BackCredits;

	uint OpenPause;
	uint ClosePause;

};