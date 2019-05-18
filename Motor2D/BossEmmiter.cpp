#include "BossEmmiter.h"
#include "j1EntityFactory.h"
#include "j1Render.h"

BossEmmiter::BossEmmiter(fPoint pos, const j1Entity * owner, uint timeLife)
	:Projectile(pos, { 0.F,0.F }, 0u, owner, "EmmiterArrows", PROJECTILE_TYPE::EMMITER)
{
	SetPivot(150, 112);
	size.create(300, 225);

	position -= pivot;

	engine.seed(rd());

	lifeTimer.Start();
	createArrowsTimer.Start();
	dieTimer.Start();
	//currentAnimation = &anim;

	rang.x = 100;
	rang.y = 40;

	lifeTime = timeLife;
	createArrowsSpeed = 75u;
	dieTime = 6u;

	constantHeigth = App->render->camera->h;
	/*App->audio->PlayFx(App->entityFactory->strech_Shoot, 0);*/
}

BossEmmiter::~BossEmmiter()
{

}

bool BossEmmiter::PreUpdate()
{
	if (lifeTimer.ReadSec() > lifeTime)
	{
		to_explode = true;
	}

	return true;
}

bool BossEmmiter::Update(float dt)
{
	if (!to_explode)
	{
		if (createArrowsTimer.Read() > createArrowsSpeed)
		{
			CreateArrow();
			createArrowsTimer.Start();
		}
	}
	return true;
}

bool BossEmmiter::PostUpdate()
{
	if (to_explode)
	{
		if (dieTime < dieTimer.ReadSec())
		{
			to_delete = true;
		}

	}
	return true;
}

void BossEmmiter::CreateArrow()
{
	float posY = RandomValue(-rang.y, rang.y);
	posY += position.y + size.y / 2;

	float posX = RandomValue(-rang.x, rang.x);
	posX += position.x + size.x / 2;


	App->entityFactory->CreateArrow({ posX, posY - 350 }, { posX, posY + 100 }, 200, App->entityFactory->player->GetShara(), PROJECTILE_TYPE::EMMITER_ARROWS, 2);

}

float BossEmmiter::RandomValue(int min, int max)
{
	std::uniform_int_distribution<int> range(min, max);
	return range(rd);

}