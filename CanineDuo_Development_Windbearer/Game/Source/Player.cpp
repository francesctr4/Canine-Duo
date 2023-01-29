#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include <iostream>

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");

}

Player::~Player() {

}

bool Player::Awake() {

	//L02: DONE 1: Initialize Player parameters
	//L02: DONE 5: Get Player parameters from XML

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	speed = parameters.attribute("speed").as_int();
	impulse = parameters.attribute("impulse").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	// RIGHT ANIMATIONS

	for (int i = 0; i < 8; i++) {

		idle_right.PushBack({ 32 * (0 + i), 32 * 0, 32, 32 });

	}
	idle_right.loop = true;
	idle_right.speed = 0.04f;

	for (int i = 0; i < 8; i++) {

		run_right.PushBack({ 32 * (0 + i), 32 * 3, 32, 32 });

	}
	run_right.loop = true;
	run_right.speed = 0.1f;

	for (int i = 0; i < 8; i++) {

		jump_right.PushBack({ 32 * (0 + i), 32 * 5, 32, 32 });

	}
	jump_right.loop = false;
	jump_right.speed = 0.1f;

	for (int i = 0; i < 8; i++) {

		die_right.PushBack({ 32 * (0 + i), 32 * 7, 32, 32 });

	}
	die_right.loop = false;
	die_right.speed = 0.1f;

	// LEFT ANIMATIONS

	for (int i = 7; i >= 0; i--) {

		idle_left.PushBack({ 32 * (0 + i), 32 * 9, 32, 32 });

	}
	idle_left.loop = true;
	idle_left.speed = 0.04f;

	for (int i = 7; i >= 0; i--) {

		run_left.PushBack({ 32 * (0 + i), 32 * 12, 32, 32 });

	}
	run_left.loop = true;
	run_left.speed = 0.1f;

	for (int i = 7; i >= 0; i--) {

		jump_left.PushBack({ 32 * (0 + i), 32 * 14, 32, 32 });

	}
	jump_left.loop = false;
	jump_left.speed = 0.1f;

	for (int i = 7; i >= 0; i--) {

		die_left.PushBack({ 32 * (0 + i), 32 * 16, 32, 32 });

	}
	die_left.loop = false;
	die_left.speed = 0.1f;

	godMode = false;

	return true;
}

bool Player::Start() {

	//initilize textures

	texture = app->tex->Load(texturePath);

	// L07 TODO 5: Add physics to the player - initialize physics body

	if (!godMode) {

		pbody = app->physics->CreateCircle(position.x + 16, position.y + 16, 16, bodyType::DYNAMIC);

		pbody->listener = this;

		pbody->ctype = ColliderType::PLAYER;

	}

	maxJumps = 0;

	isAlive = true;

	idleDirection = false;

	currentAnimation = &idle_right;

	isJumping = false;

	jumpFx = app->audio->LoadFx("Assets/Audio/Fx/JumpFx.wav");

	isDashing = false;
	canDash = true;

	hitsTaken = 0;

	essenceCollectedFx = app->audio->LoadFx("Assets/Audio/Fx/Essence.wav");
	lifeCollectedFx = app->audio->LoadFx("Assets/Audio/Fx/LifeCrystal.wav");

	hitFx = app->audio->LoadFx("Assets/Audio/Fx/Hit.wav");

	EndSound = app->audio->LoadFx("Assets/Audio/Fx/EndSound.wav");

	return true;
}

bool Player::Update()
{
	// L07 TODO 5: Add physics to the player - updated player position using physics
	
	//L02: DONE 4: modify the position of the player using arrow keys and render the texture

	if (!godMode) {

		velocity = { 0, pbody->body->GetLinearVelocity().y };

		if (!app->scene->pause) {

			if (isAlive) {

				if (app->scene->player->position.y > 910) {

					isAlive = false;

				}

				if (!idleDirection) {

					currentAnimation = &idle_right;

					if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {

						currentAnimation = &die_right;
						isAlive = false;

					}

				}
				else {

					currentAnimation = &idle_left;

					if (app->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN) {

						currentAnimation = &die_left;
						isAlive = false;

					}

				}

				if ((app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) /* || app->input->reduce_val(SDL_IsGameController(0), app->input->controllers[0].j1_x, 10000, 2) < 0*/) {

					velocity = { -speed, pbody->body->GetLinearVelocity().y };
					currentAnimation = &run_left;

					idleDirection = true;

				}

				if ((app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) /* || app->input->reduce_val(SDL_IsGameController(0), app->input->controllers[0].j1_x, 10000, 2) > 0*/) {

					velocity = { speed, pbody->body->GetLinearVelocity().y };
					currentAnimation = &run_right;

					idleDirection = false;

				}
				if ((app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) /* || app->input->controllers[0].buttons[SDL_CONTROLLER_BUTTON_A] == KEY_DOWN*/) {

					app->audio->PlayFx(jumpFx);

					if (maxJumps < 2) {

						isJumping = true;

						if (maxJumps < 1) {

							velocity = { pbody->body->GetLinearVelocity().x, -impulse };

						}
						else {

							velocity = { pbody->body->GetLinearVelocity().x, (2 * -impulse / 3) };

						}

						maxJumps++;

					}

				}

				if (isJumping) {

					if (!idleDirection) {

						currentAnimation = &jump_right;

						if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN /* || app->input->controllers[0].buttons[SDL_CONTROLLER_BUTTON_A] == KEY_DOWN*/) {

							currentAnimation->Reset();

						}

					}
					else {

						currentAnimation = &jump_left;

						if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN /* || app->input->controllers[0].buttons[SDL_CONTROLLER_BUTTON_A] == KEY_DOWN*/) {

							currentAnimation->Reset();

						}
					}
				}

				if ((app->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN) /* || app->input->controllers[0].buttons[SDL_CONTROLLER_BUTTON_X] == KEY_DOWN*/) {

					/*app->audio->PlayFx(jumpFx);

					if (!idleDirection) {
						velocity = { impulse * 20, pbody->body->GetLinearVelocity().y };
					}
					else {
						velocity = { -impulse * 20, pbody->body->GetLinearVelocity().y };
					}*/

				}

			}

		}

		pbody->body->SetLinearVelocity(velocity);

		b2Transform transform = pbody->body->GetTransform();
		b2Vec2 pos = transform.p;

		position.x = METERS_TO_PIXELS(pos.x) - 16;
		position.y = METERS_TO_PIXELS(pos.y) - 16;

	}
	else {

		if ((app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) /* || app->input->reduce_val(SDL_IsGameController(0), app->input->controllers[0].j1_y, 10000, 2) < 0*/) {

			position.y -= speed;
			currentAnimation = &idle_right;

		}
		if ((app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) /* || app->input->reduce_val(SDL_IsGameController(0), app->input->controllers[0].j1_y, 10000, 2) > 0*/) {

			position.y += speed;
			currentAnimation = &idle_right;

		}
		if ((app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) /* || app->input->reduce_val(SDL_IsGameController(0), app->input->controllers[0].j1_x, 10000, 2) < 0*/) {

			position.x -= speed;
			currentAnimation = &run_left;

		}
		if ((app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) /* || app->input->reduce_val(SDL_IsGameController(0), app->input->controllers[0].j1_x, 10000, 2) > 0*/) {

			position.x += speed;
			currentAnimation = &run_right;

		}

	}

	if (!isAlive) app->audio->PlayFx(EndSound);

	currentAnimation->Update();

	SDL_Rect playerRect = currentAnimation->GetCurrentFrame();

	app->render->DrawTexture(texture, position.x, position.y, &playerRect);

	return true;
}

bool Player::CleanUp()
{

	return true;
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::ITEM_ESSENCE:
		LOG("Collision ESSENCE");
		// Do something

		app->audio->PlayFx(essenceCollectedFx);
		app->scene->itemsCollected++;

		break;
	case ColliderType::ITEM_LIFECRYSTAL:
		LOG("Collision LIFECRYSTAL");
		// Do something

		app->audio->PlayFx(lifeCollectedFx);
		if (hitsTaken > 0) app->scene->player->hitsTaken--;

		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		maxJumps = 0;
		isJumping = false;
		break;
	case ColliderType::SPIKES:
		LOG("Collision SPIKES");

		if (isAlive) {

			hitsTaken = 3;

			app->audio->PlayFx(hitFx);

			if (!idleDirection) {

				currentAnimation = &die_right;
				isAlive = false;

			}
			else {

				currentAnimation = &die_left;
				isAlive = false;

			}

		}

		break;

	case ColliderType::ENEMY:
		LOG("Collision ENEMY");

		if (hitsTaken <= 2) hitsTaken++;

		if (hitsTaken <= 3) app->audio->PlayFx(hitFx);

		if (isAlive && hitsTaken > 2) {

			if (!idleDirection) {

				currentAnimation = &die_right;
				isAlive = false;

			}
			else {

				currentAnimation = &die_left;
				isAlive = false;

			}

		}

		break;

	case ColliderType::ENEMY_SENSOR:
		LOG("Collision ENEMY_SENSOR");

		break;

	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	}

}

void Player::Dash() {

	std::cout << "DASHING" << std::endl;
	isDashing = true;
	canDash = false;

	if (dashingTimer.ReadMSec() <= dashingTime && isDashing) {

		if (!idleDirection) {

			velocity = { dashForce, pbody->body->GetLinearVelocity().y };

			currentAnimation = &run_left;

		}
		else if (idleDirection) {

			velocity = { -dashForce, pbody->body->GetLinearVelocity().y };

			currentAnimation = &run_right;

		}

	}
	else {

		isDashing = false;

	}

	if (dashCooldown.ReadMSec() >= timeCanDash) {

		canDash = true;

	}

}