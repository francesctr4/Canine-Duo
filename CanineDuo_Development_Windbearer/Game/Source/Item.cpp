#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "SceneTitle.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("item");
}

Item::~Item() {}

bool Item::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	if (SString(parameters.attribute("type").as_string()) == SString("Essence"))
		type = ItemType::ESSENCE;
	if (SString(parameters.attribute("type").as_string()) == SString("LifeCrystal"))
		type = ItemType::LIFECRYSTAL;

	if (SString(parameters.attribute("type").as_string()) == SString("Checkpoint")) {

		type = ItemType::CHECKPOINT;

		if (SString(parameters.attribute("state").as_string()) == SString("Disabled")) {
			
			checkpointState = CheckpointStates::DISABLED;

		}

		if (SString(parameters.attribute("state").as_string()) == SString("Activated")) {
			
			checkpointState = CheckpointStates::ACTIVATED;

		}

		audioPath = parameters.attribute("audiopath").as_string();

	}

	checkpointFx = app->audio->LoadFx(audioPath);

	return true;
}

bool Item::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	// L07 TODO 4: Add a physics to an item - initialize the physics body

	int width = 32;
	int height = 32;

	if (type == ItemType::ESSENCE) {
		pbody = app->physics->CreateCircleSensor(position.x + width / 2, position.y + height / 2, 12, bodyType::KINEMATIC, ColliderType::ITEM_ESSENCE);
	}
	
	if (type == ItemType::LIFECRYSTAL) {
		pbody = app->physics->CreateCircleSensor(position.x + width / 2, position.y + height / 2, 14, bodyType::KINEMATIC, ColliderType::ITEM_LIFECRYSTAL);
	}

	if (type == ItemType::CHECKPOINT) {
		pbody = app->physics->CreateRectangleSensor(position.x, position.y, 58,51, bodyType::KINEMATIC, ColliderType::ITEM_CHECKPOINT);
	}

	pbody->listener = this;

	return true;
}

bool Item::Update()
{
	// L07 TODO 4: Add a physics to an item - update the position of the object from the physics.  
	
	b2Transform transform = pbody->body->GetTransform();
	b2Vec2 pos = transform.p;

	if (type == ItemType::ESSENCE) {

		position.x = METERS_TO_PIXELS(pos.x) - 16;
		position.y = METERS_TO_PIXELS(pos.y) - 18;

		app->render->DrawTexture(texture, position.x, position.y);

	}

	if (type == ItemType::LIFECRYSTAL) {

		position.x = METERS_TO_PIXELS(pos.x) - 15;
		position.y = METERS_TO_PIXELS(pos.y) - 15;

		app->render->DrawTexture(texture, position.x, position.y);

	}

	if (type == ItemType::CHECKPOINT) {

		position.x = METERS_TO_PIXELS(pos.x) - 29;
		position.y = METERS_TO_PIXELS(pos.y) - 29;

		SDL_Rect rect;

		if (checkpointState == CheckpointStates::DISABLED) rect = { 0,0,58,51 };
		if (checkpointState == CheckpointStates::ACTIVATED) rect = { 58,0,58,51 };

		app->render->DrawTexture(texture, position.x, position.y, &rect);

	}
	
	return true;
}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::ITEM_ESSENCE:
		LOG("Collision ITEM");

		break;
	case ColliderType::ITEM_LIFECRYSTAL:
		LOG("Collision ITEM");

		break;
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");

		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");

		break;

	case ColliderType::PLAYER:
		LOG("Collision PLAYER");

		if (type == ItemType::ESSENCE || type == ItemType::LIFECRYSTAL) {

			Disable();
			pbody->body->DestroyFixture(pbody->body->GetFixtureList());

		}

		if (type == ItemType::CHECKPOINT) {

			if (checkpointState == CheckpointStates::DISABLED) app->audio->PlayFx(checkpointFx);

			checkpointState = CheckpointStates::ACTIVATED;

			app->SaveGameRequest();

			app->sceneTitle->Continue_->state = GuiControlState::NORMAL;

			app->sceneTitle->continueEnabled = true;

		}

		break;

	}

}