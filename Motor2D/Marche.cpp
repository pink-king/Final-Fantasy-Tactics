
#include "Marche.h"
#include "j1Textures.h"

Marche::Marche()
{
	character = characterName::MARCHE;
	name.assign("Marche");

	// TODO: import from xml
	spritesheet = App->tex->Load("textures/characters/marche/marche_run_test.png");
	entityTex = spritesheet;

	run[(int)facingDirection::E].PushBack({ 0,136,32,40 });
	run[(int)facingDirection::E].PushBack({ 33,136,32,40 });
	run[(int)facingDirection::E].PushBack({ 64,136,32,40 });
	run[(int)facingDirection::E].PushBack({ 95,136,32,40 });
	run[(int)facingDirection::E].PushBack({ 128,136,32,40 });
	run[(int)facingDirection::E].PushBack({ 160,136,32,40 });
	run[(int)facingDirection::E].speed = 10.0f;

	currentAnimation = &run[(int)facingDirection::E];
}

Marche::~Marche()
{
	//if (collider.collider != nullptr)
	//	collider.collider->to_delete = true;
}

bool Marche::Start()
{
	return true;
}

bool Marche::PreUpdate()
{
	return true;
}

bool Marche::Update(float dt)
{
	InputMovement(dt);

	return true;
}

//bool Marche::PostUpdate()
//{
//	return true;
//}
