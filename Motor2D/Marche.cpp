#include "Marche.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1BuffManager.h"
#include "j1Entity.h"
//test buff
#include "j1Input.h"
#include "j1EntityFactory.h"
#include "j1Window.h"

Marche::Marche(int posX, int posY): PlayerEntity(posX,posY)
{
	character = characterName::MARCHE;
	name.assign("Marche");

	// TODO: import from xml
	spritesheet = App->tex->Load("textures/characters/marche/marche_run_test.png");
	entityTex = spritesheet;

	// IDLE

	idle[(int)facingDirection::N].PushBack({ 129,8,32,49 });
	idle[(int)facingDirection::S].PushBack({ 0,8,32,49 });
	idle[(int)facingDirection::E].PushBack({ 64,9,32,49 });
	idle[(int)facingDirection::W].PushBack({ 64,9,32,49 }); // same as E but flipped - 4
	idle[(int)facingDirection::NE].PushBack({ 97,8,32,49 });
	idle[(int)facingDirection::SE].PushBack({ 32,8,32,49 }); 
	idle[(int)facingDirection::SW].PushBack({ 32,8,32,49 }); // same as SE but flipped - 3
	idle[(int)facingDirection::NW].PushBack({ 97,8,32,49 }); // same as NE but flipped - 7
	// one frame doesnt need speed
	//idle[(int)facingDirection::N].speed = 10.0f;

	// RUN

	run[(int)facingDirection::N].PushBack({ 0,196,32,45 });
	run[(int)facingDirection::N].PushBack({ 33,196,32,45 });
	run[(int)facingDirection::N].PushBack({ 64,196,32,45 });
	run[(int)facingDirection::N].PushBack({ 95,196,32,45 });
	run[(int)facingDirection::N].PushBack({ 128,196,32,45 });
	run[(int)facingDirection::N].PushBack({ 160,196,32,45 });
	run[(int)facingDirection::N].speed = 10.0f;

	run[(int)facingDirection::S].PushBack({ 0,74,32,45 });
	run[(int)facingDirection::S].PushBack({ 33,74,32,45 });
	run[(int)facingDirection::S].PushBack({ 64,74,32,45 });
	run[(int)facingDirection::S].PushBack({ 95,74,32,45 });
	run[(int)facingDirection::S].PushBack({ 128,74,32,45 });
	run[(int)facingDirection::S].PushBack({ 160,74,32,45 });
	run[(int)facingDirection::S].speed = 10.0f;

	run[(int)facingDirection::E].PushBack({ 0,136,32,40 });
	run[(int)facingDirection::E].PushBack({ 33,136,32,40 });
	run[(int)facingDirection::E].PushBack({ 64,136,32,40 });
	run[(int)facingDirection::E].PushBack({ 95,136,32,40 });
	run[(int)facingDirection::E].PushBack({ 128,136,32,40 });
	run[(int)facingDirection::E].PushBack({ 160,136,32,40 });
	run[(int)facingDirection::E].speed = 10.0f;

	run[(int)facingDirection::W].PushBack({ 0,136,32,40 });
	run[(int)facingDirection::W].PushBack({ 33,136,32,40 });
	run[(int)facingDirection::W].PushBack({ 64,136,32,40 });
	run[(int)facingDirection::W].PushBack({ 95,136,32,40 });
	run[(int)facingDirection::W].PushBack({ 128,136,32,40 });
	run[(int)facingDirection::W].PushBack({ 160,136,32,40 });
	run[(int)facingDirection::W].speed = 10.0f;

	run[(int)facingDirection::SE].PushBack({ 0,261,32,45 });
	run[(int)facingDirection::SE].PushBack({ 33,261,32,45 });
	run[(int)facingDirection::SE].PushBack({ 64,261,32,45 });
	run[(int)facingDirection::SE].PushBack({ 95,261,32,45 });
	run[(int)facingDirection::SE].PushBack({ 128,261,32,45 });
	run[(int)facingDirection::SE].PushBack({ 160,261,32,45 });
	run[(int)facingDirection::SE].speed = 10.0f;

	run[(int)facingDirection::SW].PushBack({ 0,261,32,45 });
	run[(int)facingDirection::SW].PushBack({ 33,261,32,45 });
	run[(int)facingDirection::SW].PushBack({ 64,261,32,45 });
	run[(int)facingDirection::SW].PushBack({ 95,261,32,45 });
	run[(int)facingDirection::SW].PushBack({ 128,261,32,45 });
	run[(int)facingDirection::SW].PushBack({ 160,261,32,45 });
	run[(int)facingDirection::SW].speed = 10.0f;

	run[(int)facingDirection::NE].PushBack({ 0,327,32,45 });
	run[(int)facingDirection::NE].PushBack({ 33,327,32,45 });
	run[(int)facingDirection::NE].PushBack({ 64,327,32,45 });
	run[(int)facingDirection::NE].PushBack({ 95,327,32,45 });
	run[(int)facingDirection::NE].PushBack({ 128,327,32,45 });
	run[(int)facingDirection::NE].PushBack({ 160,327,32,45 });
	run[(int)facingDirection::NE].speed = 10.0f;

	run[(int)facingDirection::NW].PushBack({ 0,327,32,45 });
	run[(int)facingDirection::NW].PushBack({ 33,327,32,45 });
	run[(int)facingDirection::NW].PushBack({ 64,327,32,45 });
	run[(int)facingDirection::NW].PushBack({ 95,327,32,45 });
	run[(int)facingDirection::NW].PushBack({ 128,327,32,45 });
	run[(int)facingDirection::NW].PushBack({ 160,327,32,45 });
	run[(int)facingDirection::NW].speed = 10.0f;

	currentAnimation = &run[(int)facingDirection::E];

	previousPos = position;
}

Marche::~Marche()
{
	//if (collider.collider != nullptr)
	//	collider.collider->to_delete = true;
}

bool Marche::Start()
{
	SetPivot(16, 40);
	size.create(32,45);
	return true;
}

bool Marche::PreUpdate()
{
	return true;
}

bool Marche::Update(float dt)
{
	fPoint pivotPos = GetPivotPos();
	LOG("%f,%f", pivot.x, pivot.y);
	iPoint onTilePos = App->map->WorldToMap(pivotPos.x, pivotPos.y);
	LOG("Player pos: %f,%f | Tile pos: %i,%i",position.x, position.y, onTilePos.x, onTilePos.y);
	if (App->pathfinding->IsWalkable(onTilePos))
	{
		previousPos = position;

		
		if (!isParalize)
		{
			currentAnimation->speed = 10.f;
			InputMovement(dt);
		}
		else
		{
			currentAnimation->speed = 0.f;
		}
	}
	else
	{
		position = previousPos;
	}
	//test buff
	j1Entity* j1 = nullptr;
	if (App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_A) == 1 || App->input->GetKey(SDL_SCANCODE_Q) == 1)
	{
		std::vector<j1Entity*>::iterator item2 = App->entityFactory->entities.begin();

		for (; item2 != App->entityFactory->entities.end(); ++item2)
		{
			if ((*item2)->name.compare("whatever") == 0)
			{
				App->buff->DirectAttack(this, *item2, 10, "basic");
			}
		}
	}
	if (App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_B) == 1 || App->input->GetKey(SDL_SCANCODE_W) == 1)
	{
		std::vector<j1Entity*>::iterator item3 = App->entityFactory->entities.begin();

		for (; item3 != App->entityFactory->entities.end(); ++item3)
		{
			if ((*item3)->name.compare("whatever") == 0)
			{
				App->buff->DirectAttack(this, *item3, 15, "hability");
			}
		}
	}
	if ((App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_Y) == 1 || App->input->GetKey(SDL_SCANCODE_E) == 1)
		&& !isBurned)
	{
		entityStat* newStat = new entityStat(STAT_TYPE::BURNED_STAT, life - App->buff->GetBurnedDamage());
		newStat->count.Start();
		stat.push_back(newStat);
		isBurned = true;
		
	}
	if ((App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_X) == 1 || App->input->GetKey(SDL_SCANCODE_R) == 1)
		&& !isParalize)
	{
		entityStat* newStat = new entityStat(STAT_TYPE::PARALIZE_STAT, 0.0f);
		newStat->count.Start();
		stat.push_back(newStat);
		isParalize = true;
	}



	static char title[30];
	sprintf_s(title, 30, " | Marche life: %f", life);
	App->win->SetTitle(title);
	return true;
}

bool Marche::CleanUp()
{
	return true;
}

//bool Marche::PostUpdate()
//{
//	return true;
//}
