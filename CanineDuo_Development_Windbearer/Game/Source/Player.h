#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "SDL/include/SDL.h"
#include "Timer.h"
#include "Physics.h"

struct SDL_Texture;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	PhysBody* pbody;
	bool godMode;
	int maxJumps;
	bool idleDirection;
	bool isAlive;
	bool isJumping;

	uint jumpFx = 0;

	b2Vec2 velocity;

	// Dash

	void Dash();

	float dashingTime = 200;
	float dashForce = 20.0f;
	float timeCanDash = 1000;
	bool isDashing;
	bool canDash;

	Timer dashingTimer;
	Timer dashCooldown;

	int hitsTaken;
	uint essenceCollectedFx;
	uint lifeCollectedFx;

	uint hitFx;

	uint EndSound;

private:

	//L02: DONE 1: Declare player parameters
	SDL_Texture* texture;
	const char* texturePath;
	float speed;
	float impulse;

	// L07 TODO 5: Add physics to the player - declare a Physics body

	int width = 32;
	int height = 32;

	// Animations

	Animation* currentAnimation;

	Animation idle_right;
	Animation blink_right;
	Animation walk_right;
	Animation run_right;
	Animation duck_right;
	Animation jump_right;
	Animation teleport_right;
	Animation die_right;
	Animation fight_right;

	Animation idle_left;
	Animation blink_left;
	Animation walk_left;
	Animation run_left;
	Animation duck_left;
	Animation jump_left;
	Animation teleport_left;
	Animation die_left;
	Animation fight_left;

};

#endif // __PLAYER_H__