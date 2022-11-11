#pragma once

#include "Module.h"
#include "Player.h"
#include "Item.h"

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

private:

	SDL_Texture* WindbearerLogo;
	SDL_Texture* MenuBackground;
	
};