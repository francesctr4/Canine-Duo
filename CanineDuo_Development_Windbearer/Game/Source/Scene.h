#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Player.h"
#include "Item.h"
#include "Enemy.h"
#include "Animation.h"
#include "List.h"
#include "GuiManager.h"
#include "GuiButton.h"
#include "GuiSlider.h"
#include "GuiCheckBox.h"
#include <vector>

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene(bool startEnabled);

	// Destructor
	virtual ~Scene();

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

	//L02: DONE 3: Declare a Player attribute 
	Player* player;

	List<Enemy*> enemies; 

	bool enableMusic;

	Uint32 start;
	Uint32 stop;

	iPoint playerTile;

	SDL_Texture* mouseTileTex = nullptr;

	pugi::xml_node configFile;


public:

	SDL_Texture* img;

	//SDL_Texture* mouseTileTex = nullptr;
	SDL_Texture* originTex = nullptr;

	// L12: Debug pathfing

	iPoint origin;
	bool originSelected = false;

	SDL_Texture* parallax1;
	SDL_Texture* parallax2;
	SDL_Texture* parallax3;
	SDL_Texture* parallax4;

	SDL_Texture* Elysian;

	Animation ElysianIdle;

	Animation* currentAnimation;

	SDL_Texture* playerUI;
	SDL_Texture* essenceUI;
	SDL_Texture* timeUI;

	SDL_Texture* pauseScreen;
	bool pause;

	SDL_Texture* OptionsTex;
	bool showSettings;

	SDL_Texture* resume;
	GuiButton* Resume;

	SDL_Texture* settings;
	GuiButton* Settings;

	SDL_Texture* backToTitle;
	GuiButton* BackToTitle;

	SDL_Texture* exit;
	GuiButton* Exit;

	// -----------------------

	SDL_Texture* back;
	GuiButton* Back;

	SDL_Texture* slider;
	GuiSlider* SliderMusic;
	GuiSlider* SliderFX;

	SDL_Texture* checkBox;
	GuiCheckBox* CheckBoxFullscreen;
	GuiCheckBox* CheckBoxVsync;

	bool startCounting;
	Timer timeElapsed;

	int itemsCollected;

	uint OpenPause;
	uint ClosePause;

	std::vector<Item*> checkpoints{};

	int checkpointIterator;

};

#endif // __SCENE_H__