#pragma once

#include "Entity.h"
#include "Point.h"
#include "Animation.h"
#include "DynArray.h"
#include "Timer.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

enum class EnemyState
{
	IDLE,
	ROAMING,
	ATTACKING,
	DYING
};

enum class EnemyType
{
	FLYING,
	WALKING,

};

class Enemy : public Entity
{
public:

	Enemy();

	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update();

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	void Death();

public:

	EnemyState state;
	EnemyType type;
	iPoint origin;
	const DynArray<iPoint>* path;
	Timer pathTimer;
	int pathIterator;

	Uint32 start;
	Uint32 stop;
	bool enemyDirection;
	bool isAlive;

	iPoint initialPos;

	PhysBody* pbody;
	PhysBody* enemySensor;

private:

	SDL_Texture* texture;
	const char* texturePath;
	

	// Animations

	int spriteWidth_walkingEnemy = 96;
	int spriteHeight_walkingEnemy = 64;

	int spriteWidth_flyingEnemy = 60;
	int spriteHeight_flyingEnemy = 50;

	Animation* currentAnimation;

	Animation idleRight_walkingEnemy;
	Animation attackRight_walkingEnemy;
	Animation idleLeft_walkingEnemy;
	Animation attackLeft_walkingEnemy;
	Animation dieRight_walkingEnemy;
	Animation dieLeft_walkingEnemy;

	Animation idleRight_flyingEnemy;
	Animation attackRight_flyingEnemy;
	Animation idleLeft_flyingEnemy;
	Animation attackLeft_flyingEnemy;
	//Animation hitRight_flyingEnemy;
	//Animation hitLeft_flyingEnemy;
	Animation dieRight_flyingEnemy;
	Animation dieLeft_flyingEnemy;

};