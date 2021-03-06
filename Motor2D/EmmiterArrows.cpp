#include "EmmiterArrows.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1EntityFactory.h"
#include "j1AttackManager.h"
#include "j1Render.h"
#include "j1PathFinding.h"
#include "p2Defs.h"
#include "j1ParticlesClassic.h"

EmmiterArrows::EmmiterArrows(fPoint pos, fPoint destination, uint speed, const j1Entity* owner, uint timeLife)
	: Projectile(pos, destination, speed, owner, "Arrow", PROJECTILE_TYPE::EMMITER_ARROWS), timeLife(timeLife)
{
	// TODO SFX arrow throwing

	entityTex = App->entityFactory->arrowsTexture;

	anim.PushBack({ 0,48,64,16 });
	anim.PushBack({ 64,48,64,16 });
	anim.PushBack({ 128,48,64,16 });
	anim.PushBack({ 192,48,64,16 });
	anim.PushBack({ 256,48,64,16 });
	anim.PushBack({ 320,48,64,16 });
	anim.PushBack({ 384,48,64,16 });
	anim.PushBack({ 448,48,64,16 });
	anim.PushBack({ 0,64,64,16 });
	anim.PushBack({ 64,64,64,16 });
	anim.speed = 10.F;

	currentAnimation = &anim;

	SetPivot(132, 12);
	size.create(64, 16);

	damage = true;
	App->audio->PlayFx(App->scene->SharaUltimateWoosh, 0);
	// Important for aiming offset
	SetInitially();
	
}

EmmiterArrows::~EmmiterArrows()
{
	LOG("Bye arrow!");
}

bool EmmiterArrows::PreUpdate()
{

	
	return true;
}

bool EmmiterArrows::Update(float dt)
{
	if (to_explode) {
		to_delete = true;
	}
	else {
		Move(dt);
	}
	return true;
}

bool EmmiterArrows::Move(float dt)
{
	if (position.y + size.y <= destination.y)
	{
		position += direction * speed * dt;
		willExplode = true;
		//iPoint drawRectified;
		//drawRectified.x = position.x + size.x * 0.5f;
		//drawRectified.y = position.y + (size.y * 0.5f) * 2;

		//// flip particles pseudo randomly
		//SDL_RendererFlip renderFlip = SDL_RendererFlip::SDL_FLIP_NONE;

		//if (rand() % 2 == 0)
		//{
		//	renderFlip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
		//}

		//iPoint fire01Pivot = { 8, 48 };
		//drawRectified -= fire01Pivot;
		////App->particles->AddParticle(App->particles->fire02, drawRectified.x, drawRectified.y, { 0,0 }, 0u, renderFlip);
	}
	else
	{
		if (willExplode)
		{
			App->audio->PlayFx(App->scene->emitter_explodeFire, 0);
			willExplode = false;
		}
		if (damage)
		{
			iPoint newCords = App->map->SubTileMapToWorld(GetSubtilePos().x, GetSubtilePos().y);
			App->attackManager->AddPropagationAttack(App->entityFactory->player->GetShara(), App->map->WorldToSubtileMap(newCords.x, newCords.y - 90), propagationType::BFS,
				damageType::DIRECT, ELEMENTAL_TYPE::FIRE_ELEMENT, 30, 2, 120, false);
			App->attackManager->AddPropagationAttack(App->entityFactory->player->GetShara(), App->map->WorldToSubtileMap(newCords.x, newCords.y - 90), propagationType::BFS, 
				damageType::INTIME, ELEMENTAL_TYPE::FIRE_ELEMENT, 30, 4, 200, true);
			timeLifeTimer.Start();
			damage = false;

			iPoint drawRectified;
			drawRectified.x = newCords.x;
			drawRectified.y = newCords.y - 90;

			// flip particles pseudo randomly
			SDL_RendererFlip renderFlip = SDL_RendererFlip::SDL_FLIP_NONE;

			if (rand() % 2 == 0)
			{
				renderFlip = SDL_RendererFlip::SDL_FLIP_HORIZONTAL;
			}

			iPoint fireBlastPivot = { 85, 390 };
			drawRectified -= fireBlastPivot;
			App->particles->AddParticle(App->particles->fireBlast, drawRectified.x, drawRectified.y, { 0,0 }, 0u, renderFlip);

			App->camera2D->AddTrauma(6.5f / 100.f);
			App->input->DoGamePadRumble(0.2f, 100);
		}
		
		
		if (timeLife < timeLifeTimer.ReadSec())
			to_explode = true;
		
	}
	return true;
}


bool EmmiterArrows::CleanUp()
{
	return true;
}

