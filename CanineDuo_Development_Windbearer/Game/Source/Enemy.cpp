#include "Enemy.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "PathFinding.h"
#include "Map.h"

#include <iostream>

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("Enemy");

}

Enemy::~Enemy() {

}

bool Enemy::Awake() {

	if (SString(parameters.attribute("type").as_string()) == SString("walking"))
		type = EnemyType::WALKING;
	if (SString(parameters.attribute("type").as_string()) == SString("flying"))
		type = EnemyType::FLYING;

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();

	initialPos.x = parameters.attribute("x").as_int();
	initialPos.y = parameters.attribute("y").as_int();

	texturePath = parameters.attribute("texturepath").as_string();

	for (int i = 0; i < 9; i++) {

		idleRight_walkingEnemy.PushBack({ spriteWidth_walkingEnemy * (0 + i), spriteHeight_walkingEnemy * 0, spriteWidth_walkingEnemy, spriteHeight_walkingEnemy });

	}
	idleRight_walkingEnemy.loop = true;
	idleRight_walkingEnemy.speed = 0.1f;

	for (int i = 0; i < 6; i++) {

		attackRight_walkingEnemy.PushBack({ spriteWidth_walkingEnemy * (0 + i), spriteHeight_walkingEnemy * 2, spriteWidth_walkingEnemy, spriteHeight_walkingEnemy });

	}
	attackRight_walkingEnemy.loop = true;
	attackRight_walkingEnemy.speed = 0.075f;

	for (int i = 8; i >= 0; i--) {

		idleLeft_walkingEnemy.PushBack({ spriteWidth_walkingEnemy * (0 + i), spriteHeight_walkingEnemy * 5, spriteWidth_walkingEnemy, spriteHeight_walkingEnemy });

	}
	idleLeft_walkingEnemy.loop = true;
	idleLeft_walkingEnemy.speed = 0.1f;

	for (int i = 8; i >= 3; i--) {

		attackLeft_walkingEnemy.PushBack({ spriteWidth_walkingEnemy * (0 + i), spriteHeight_walkingEnemy * 7, spriteWidth_walkingEnemy, spriteHeight_walkingEnemy });

	}
	attackLeft_walkingEnemy.loop = true;
	attackLeft_walkingEnemy.speed = 0.075f;

	for (int i = 0; i < 7; i++) {

		dieRight_walkingEnemy.PushBack({ spriteWidth_walkingEnemy * (0 + i), spriteHeight_walkingEnemy * 4, spriteWidth_walkingEnemy, spriteHeight_walkingEnemy });

	}
	dieRight_walkingEnemy.loop = false;
	dieRight_walkingEnemy.speed = 0.1f;

	for (int i = 8; i >= 2; i--) {

		dieLeft_walkingEnemy.PushBack({ spriteWidth_walkingEnemy * (0 + i), spriteHeight_walkingEnemy * 9, spriteWidth_walkingEnemy, spriteHeight_walkingEnemy });

	}
	dieLeft_walkingEnemy.loop = false;
	dieLeft_walkingEnemy.speed = 0.1f;

	for (int i = 0; i < 8; i++) {

		idleRight_flyingEnemy.PushBack({ spriteWidth_flyingEnemy * (0 + i), spriteHeight_flyingEnemy * 0, spriteWidth_flyingEnemy, spriteHeight_flyingEnemy });

	}
	idleRight_flyingEnemy.loop = true;
	idleRight_flyingEnemy.speed = 0.1f;

	for (int i = 0; i < 8; i++) {

		attackRight_flyingEnemy.PushBack({ spriteWidth_flyingEnemy * (0 + i), spriteHeight_flyingEnemy * 2, spriteWidth_flyingEnemy, spriteHeight_flyingEnemy });

	}
	attackRight_flyingEnemy.loop = true;
	attackRight_flyingEnemy.speed = 0.075f;

	for (int i = 7; i >= 0; i--) {

		idleLeft_flyingEnemy.PushBack({ spriteWidth_flyingEnemy * (0 + i), spriteHeight_flyingEnemy * 7, spriteWidth_flyingEnemy, spriteHeight_flyingEnemy });

	}
	idleLeft_flyingEnemy.loop = true;
	idleLeft_flyingEnemy.speed = 0.1f;

	for (int i = 7; i >= 0; i--) {

		attackLeft_flyingEnemy.PushBack({ spriteWidth_flyingEnemy * (0 + i), spriteHeight_flyingEnemy * 9, spriteWidth_flyingEnemy, spriteHeight_flyingEnemy });

	}
	attackLeft_flyingEnemy.loop = true;
	attackLeft_flyingEnemy.speed = 0.075f;

	//for (int i = 0; i < 4; i++) {

	//	hitRight_flyingEnemy.PushBack({ spriteWidth_flyingEnemy * (0 + i), spriteHeight_flyingEnemy * 4, spriteWidth_flyingEnemy, spriteHeight_flyingEnemy });

	//}
	//hitRight_flyingEnemy.loop = false;
	//hitRight_flyingEnemy.speed = 0.1f;

	/*for (int i = 7; i >= 4; i--) {

		hitLeft_flyingEnemy.PushBack({ spriteWidth_flyingEnemy * (0 + i), spriteHeight_flyingEnemy * 11, spriteWidth_flyingEnemy, spriteHeight_flyingEnemy });

	}
	hitLeft_flyingEnemy.loop = false;
	hitLeft_flyingEnemy.speed = 0.1f;*/

	for (int i = 0; i < 8; i++) {

		dieRight_flyingEnemy.PushBack({ spriteWidth_flyingEnemy * (0 + i), spriteHeight_flyingEnemy * 5, spriteWidth_flyingEnemy, spriteHeight_flyingEnemy });

	}
	dieRight_flyingEnemy.loop = false;
	dieRight_flyingEnemy.speed = 0.1f;

	for (int i = 7; i >= 0; i--) {

		dieLeft_flyingEnemy.PushBack({ spriteWidth_flyingEnemy * (0 + i), spriteHeight_flyingEnemy * 12, spriteWidth_flyingEnemy, spriteHeight_flyingEnemy });

	}
	dieLeft_flyingEnemy.loop = false;
	dieLeft_flyingEnemy.speed = 0.1f;

	return true;
}

bool Enemy::Start() {

	texture = app->tex->Load(texturePath);

	if (type == EnemyType::WALKING) {

		pbody = app->physics->CreateCircle(position.x + 48, position.y + 32, 20, bodyType::DYNAMIC);
		enemySensor = app->physics->CreateCircleSensor(position.x + 48, position.y + 10, 100, bodyType::KINEMATIC, ColliderType::ENEMY_SENSOR);
		currentAnimation = &idleRight_walkingEnemy;

	}
	
	if (type == EnemyType::FLYING) {

		pbody = app->physics->CreateCircle(position.x + 30, position.y + 25, 16, bodyType::KINEMATIC);
		enemySensor = app->physics->CreateCircleSensor(position.x + 30, position.y + 25, 120, bodyType::KINEMATIC, ColliderType::ENEMY_SENSOR);
		currentAnimation = &idleRight_flyingEnemy;

	}

	//pbody->listener = this;

	enemySensor->listener = this;

	pbody->ctype = ColliderType::ENEMY;

	state = EnemyState::IDLE;

	enemyDirection = true;

	start = 0;

	path = nullptr;

	pathIterator = 1;

	pathTimer.Start();

	isAlive = true;

	return true;
}

bool Enemy::Update()
{
	b2Vec2 velocity = { 0, 0 };

	float32 enemySpeed = 3;

	int enemyX, enemyY;
	pbody->GetPosition(enemyX, enemyY);

	origin = app->map->WorldToMap(enemyX, enemyY);

	app->pathfinding->CreatePath(origin, app->scene->playerTile);

	path = app->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		if (app->physics->debug) app->render->DrawTexture(app->scene->mouseTileTex, pos.x, pos.y);
	}

	if (pathTimer.ReadMSec() > 1500) {

		if (pathIterator < path->Count()) {

			pathIterator++;

		}

		pathTimer.Start();

	}

	if (isAlive) {

		switch (state) {

		case EnemyState::IDLE:

			if (type == EnemyType::WALKING) {

				int x, y;
				pbody->GetPosition(x, y);

				x = x + 8;

				enemySensor->body->SetTransform({ PIXEL_TO_METERS(x),PIXEL_TO_METERS(y) }, 0);

				if (enemyDirection) currentAnimation = &idleRight_walkingEnemy;
				else currentAnimation = &idleLeft_walkingEnemy;

				pathIterator = 1;

				velocity = { 0, pbody->body->GetLinearVelocity().y };

			}

			if (type == EnemyType::FLYING) {

				int x, y;
				pbody->GetPosition(x, y);

				x = x + 8;
				y = y + 4;

				enemySensor->body->SetTransform({ PIXEL_TO_METERS(x),PIXEL_TO_METERS(y) }, 0);

				if (enemyDirection) currentAnimation = &idleRight_flyingEnemy;
				else currentAnimation = &idleLeft_flyingEnemy;

				pathIterator = 1;

			}

			break;

		/*case EnemyState::ROAMING:

			if (start == 0) {

				start = SDL_GetTicks();

			}

			stop = SDL_GetTicks();

			if (stop - start > 3000) {

				if (enemyDirection) enemyDirection = false;
				else enemyDirection = true;
				
				start = SDL_GetTicks();

			}

			if (enemyDirection) {

				velocity = { enemySpeed,0 };

			}
			else {

				velocity = { -enemySpeed,0 };

			}

			break;*/

		case EnemyState::ATTACKING:

			if (type == EnemyType::WALKING) {

				if (pathIterator < path->Count()) {

					if (path->At(pathIterator)->x - path->At(pathIterator - 1)->x > 0) {

						enemyDirection = true;
						velocity = { enemySpeed,pbody->body->GetLinearVelocity().y };

					}

					if (path->At(pathIterator)->x - path->At(pathIterator - 1)->x < 0) {

						enemyDirection = false;
						velocity = { -enemySpeed,pbody->body->GetLinearVelocity().y };

					}

				}

				if (enemyDirection) currentAnimation = &attackRight_walkingEnemy;
				else currentAnimation = &attackLeft_walkingEnemy;

			}

			if (type == EnemyType::FLYING) {

				if (pathIterator < path->Count()) {

					if (path->At(pathIterator)->x - path->At(pathIterator - 1)->x > 0) {

						enemyDirection = true;
						velocity = { enemySpeed,0 };

					}

					if (path->At(pathIterator)->x - path->At(pathIterator - 1)->x < 0) {

						enemyDirection = false;
						velocity = { -enemySpeed,0 };

					}

					if (path->At(pathIterator)->y - path->At(pathIterator - 1)->y > 0) velocity = { 0,enemySpeed };
					if (path->At(pathIterator)->y - path->At(pathIterator - 1)->y < 0) velocity = { 0,-enemySpeed };

					if (enemyDirection) currentAnimation = &attackRight_flyingEnemy;
					else currentAnimation = &attackLeft_flyingEnemy;

				}

			}

			break;

		case EnemyState::DYING:

			if (type == EnemyType::WALKING) {

				if (enemyDirection) currentAnimation = &dieRight_walkingEnemy;
				else currentAnimation = &dieLeft_walkingEnemy;

			}

			if (type == EnemyType::FLYING) {

				if (enemyDirection) currentAnimation = &dieRight_flyingEnemy;
				else currentAnimation = &dieLeft_flyingEnemy;

			}

			isAlive = false;

			break;

		}
	}
	else {
		
		if (currentAnimation->HasFinished()) Death();

	}
	

	if (app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {

		state = EnemyState::DYING;

	}

	if (app->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN || app->scene->stop - app->scene->start > 1000) {

		isAlive = true;
		state = EnemyState::IDLE;
		enemyDirection = true;

		if (type == EnemyType::WALKING) pbody->body->SetTransform({ PIXEL_TO_METERS((initialPos.x + 48)),PIXEL_TO_METERS((initialPos.y + 32)) }, 0);
		if (type == EnemyType::FLYING) pbody->body->SetTransform({ PIXEL_TO_METERS((initialPos.x + 30)),PIXEL_TO_METERS((initialPos.y + 25)) }, 0);

		currentAnimation->Reset();

	}

	pbody->body->SetLinearVelocity(velocity);
	enemySensor->body->SetLinearVelocity(velocity);

	b2Transform transform = pbody->body->GetTransform();
	b2Vec2 pos = transform.p;

	if (type == EnemyType::WALKING) {

		position.x = METERS_TO_PIXELS(pos.x) - 48;
		position.y = METERS_TO_PIXELS(pos.y) - 48;

	}

	if (type == EnemyType::FLYING) {

		position.x = METERS_TO_PIXELS(pos.x) - 30;
		position.y = METERS_TO_PIXELS(pos.y) - 25;

	}

	currentAnimation->Update();

	SDL_Rect enemyRect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(texture, position.x, position.y, &enemyRect);

	return true;
}

bool Enemy::CleanUp()
{

	return true;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");

		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");

		break;

	case ColliderType::PLAYER:
		LOG("Collision PLAYER");

		state = EnemyState::ATTACKING;

		break;

	}

}

void Enemy::OnCollisionEnd(PhysBody* physA, PhysBody* physB) {

	state = EnemyState::IDLE;

}

void Enemy::Death() {

	Disable();
	pbody->body->DestroyFixture(pbody->body->GetFixtureList());
	enemySensor->body->DestroyFixture(enemySensor->body->GetFixtureList());

}