#include "BreakableAsset.h"
#include "j1Render.h"
#include "j1EntityFactory.h"
#include "j1PathFinding.h"

BreakableAsset::BreakableAsset(const iPoint & pos, BreakableType type, bool isBroken) : isBroken(isBroken), breakableType(type), j1Entity(ENTITY_TYPE::BREAKABLE_ASSET,pos.x,pos.y,"jar")
{
	switch (type)
	{
		// Rects -9 in Y to adjust to tile's center
	case BreakableType::JAR:
		idle.PushBack({ 384, 693, 64, 64 });

		breaking.PushBack({ 448, 693, 64, 64 });
		breaking.PushBack({ 512, 693, 64, 64 });
		breaking.PushBack({ 576, 693, 64, 64 });
		breaking.PushBack({ 640, 693, 64, 64 });
		breaking.speed = 10.f;
		breaking.loop = false; 

		break;

	case BreakableType::JARFULL:
		idle.PushBack({ 384, 435, 64,64 });

		breaking.PushBack({ 512, 693, 64, 64 });
		breaking.PushBack({ 576, 693, 64, 64 });
		breaking.PushBack({ 640, 693, 64, 64 });
		breaking.speed = 10.f;
		breaking.loop = false;
		break;

	case BreakableType::BARREL:
		idle.PushBack({ 384, 757, 64, 64 });
							  
		breaking.PushBack({ 448, 757, 64, 64 });
		breaking.PushBack({ 512, 757, 64, 64 });
		breaking.PushBack({ 576, 757, 64, 64 });  
		breaking.PushBack({ 640, 757, 64, 64 });
		breaking.speed = 10.f;
		breaking.loop = false;
		break;

	default:
		LOG("Not Correct Breakable Asset Constructed");
		break;
	}
	
	if (isBroken)
	{
		breaking.SetCurrentFrame(breaking.GetSize());
		currentAnimation = &breaking;
	}
	else
		currentAnimation = &idle; 


	pivot.create(32, 50);
	size.create(64, 64);


	this->life = 1;
	// When this entity is created, the Walkability Map is not created yet
	//	App->pathfinding->ActivateTile(App->map->WorldToMap(GetPivotPos().x, GetPivotPos().y));
}

BreakableAsset::~BreakableAsset()
{
	App->audio->PlayFx(App->scene->jar_breakSFX, 0);
}



bool BreakableAsset::PreUpdate()
{

	//if (!isAssigned)	// I have to do this the pretty way: booleans! :D
	//{
	//	App->pathfinding->ActivateTile(App->map->WorldToMap(GetPivotPos().x, GetPivotPos().y));
	//	isAssigned = true;
	//}

	return true;
}

bool BreakableAsset::Update(float dt)
{

	if (to_die && !isBroken)
		Break();

	if (isBroken && PlaySFX)
	{
		App->audio->PlayFx(App->scene->jar_breakSFX, 0);
		PlaySFX = false;
	}
	return true;
}

void BreakableAsset::Draw()
{

	if (currentAnimation != nullptr)
	{
		if (entityTex != nullptr) // if we have any specific linked texture
		{
			App->render->Blit(entityTex, position.x, position.y, &currentAnimation->GetCurrentFrame(), 1.0F, flip);
		}
		else
			App->render->Blit(App->entityFactory->assetsAtlasTex, position.x, position.y, &currentAnimation->GetCurrentFrame());
	}
	else App->render->Blit(App->entityFactory->assetsAtlasTex, position.x, position.y, &drawAtlasRect);

}

bool BreakableAsset::Load(pugi::xml_node &node)
{
	return true;
}

bool BreakableAsset::Save(pugi::xml_node &node) const
{
	node.append_attribute("type") = (int)breakableType;
	node.append_attribute("posX") = (int)position.x;
	node.append_attribute("posY") = (int)position.y;
	node.append_attribute("broken") = isBroken;
	return true;
}

void BreakableAsset::Break()
{
	isBroken = true;
	currentAnimation = &breaking;
	App->entityFactory->RepeatAmountofConsumables(4, GetPivotPos(), OBJECT_TYPE::GOLD); 
	App->entityFactory->RepeatAmountofConsumables(1, GetPivotPos(), OBJECT_TYPE::POTIONS);

	// TODO: Add SFX

	//App->pathfinding->DeactivateTile(App->map->WorldToMap(GetPivotPos().x, GetPivotPos().y));
}
