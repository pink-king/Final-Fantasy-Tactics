#include "Enemy.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1EntityFactory.h"
#include "j1PathFinding.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "WaveManager.h"
#include <ctime>
#include <random>
Enemy::Enemy(iPoint position, uint movementSpeed, uint detectionRange, uint attackRange, uint baseDamage, float attackSpeed, bool dummy, ENTITY_TYPE entityType, const char* name) 
 	: speed(movementSpeed), detectionRange(detectionRange), baseDamage(baseDamage), attackRange(attackRange), dummy(dummy), attackSpeed(attackSpeed), j1Entity(entityType, position.x, position.y, name)
{
	debugSubtile = App->entityFactory->debugsubtileTex;

	// Intial orientation random
	pointingDir = 1 + std::rand() % 8;
	currentAnimation = &idle[pointingDir]; 
	CheckRenderFlip();

	this->attackPerS = 1.F / attackSpeed;


	if (this->type != ENTITY_TYPE::ENEMY_DUMMY)
	this->lifeBar = App->gui->AddHealthBarToEnemy(&App->gui->enemyLifeBarInfo.dynamicSection, type::enemy, this, App->scene->inGamePanel);


	//App->audio->PlayFx(App->entityFactory->enemySpawn, 0);
}

Enemy::~Enemy()
{
	App->attackManager->DestroyAllMyCurrentAttacks(this);

	if (inWave)
	{
		std::vector<Enemy*>::iterator iter = App->entityFactory->waveManager->alive.begin();

		for (; iter != App->entityFactory->waveManager->alive.end(); iter++)
		{
			if ((*iter) == this)
			{
				break;
			}
		}

		App->entityFactory->waveManager->alive.erase(iter);
		// Probably here will change the label of remaining enemies in the wave? 
		LOG("Enemies remaining: %i", App->entityFactory->waveManager->alive.size());
	}

	memset(idle, 0, sizeof(idle));
	memset(run, 0, sizeof(run));
	memset(basicAttack, 0, sizeof(basicAttack));


	if (!App->cleaningUp)    // When closing the App, Gui cpp already deletes the healthbar before this. Prevent invalid accesses
	{
		if (this->type != ENTITY_TYPE::ENEMY_DUMMY)
		{
			if (lifeBar != nullptr)
			{
				lifeBar->deliever = nullptr;
				lifeBar->dynamicImage->to_delete = true;          // deleted in uitemcpp draw
				lifeBar->to_delete = true;
				lifeBar->skull->to_delete = true;
			}
		}
		LOG("parent enemy bye");
	}
}

bool Enemy::SearchNewPath()
{
	bool ret = false;
	if (path_to_follow.empty() == false)
		FreeMyReservedAdjacents();

	path_to_follow.clear();

	iPoint thisTile = App->map->WorldToMap((int)GetPivotPos().x, (int)GetPivotPos().y);
	iPoint playerTile = App->map->WorldToMap((int)App->entityFactory->player->GetPivotPos().x, (int)App->entityFactory->player->GetPivotPos().y);

	if (thisTile.DistanceManhattan(playerTile) > 1) // The enemy doesnt collapse with the player
	{
		if (App->pathfinding->CreatePath(thisTile, playerTile) > 0)
		{
			path_to_follow = *App->pathfinding->GetLastPath();
			if (path_to_follow.size() > 1)
				path_to_follow.erase(path_to_follow.begin());		// Enemy doesnt go to the center of his initial tile

			if (path_to_follow.size() > 1)
				path_to_follow.pop_back();							// Enemy doesnt eat the player, stays at 1 tile
				//path_to_follow.pop_back();
			ret = (path_to_follow.size() > 0);
		}
		//else LOG("Could not create path correctly");
	}

	return ret;
}

bool Enemy::SearchNewSubPath(bool ignoringColl)		// Default -> path avoids other enemies
{
	bool ret = false;
	if (path_to_follow.empty() == false)
		FreeMyReservedAdjacents();

	path_to_follow.clear();
	iPoint thisTile = App->map->WorldToSubtileMap((int)GetPivotPos().x, (int)GetPivotPos().y);
	iPoint playerTile = App->entityFactory->player->GetSubtilePos();

	if (thisTile.DistanceManhattan(playerTile) > 1) // The enemy doesnt collapse with the player
	{
		if (!ignoringColl)
		{
			if (App->pathfinding->CreateSubtilePath(thisTile, playerTile) > 0)
			{
				path_to_follow = *App->pathfinding->GetLastPath();
				if (path_to_follow.size() > 1)
					path_to_follow.erase(path_to_follow.begin());		// Enemy doesnt go to the center of his initial tile

				if (path_to_follow.size() > 1)
					path_to_follow.pop_back();							// Enemy doesnt eat the player, stays at 1 tile

				iPoint adj = path_to_follow.back();
				App->entityFactory->ReserveAdjacent(adj);
				ret = (path_to_follow.size() > 0);
			}
			else LOG("Could not create path correctly");
		}
		else if(App->pathfinding->CreateSubtilePath(thisTile, playerTile, true) > 0)
			{
				path_to_follow = *App->pathfinding->GetLastPath();
				if (path_to_follow.size() > 1)
					path_to_follow.erase(path_to_follow.begin());		// Enemy doesnt go to the center of his initial tile

				if (path_to_follow.size() > 1)
					path_to_follow.pop_back();							// Enemy doesnt eat the player, stays at 1 tile

				iPoint adj = path_to_follow.back();
				App->entityFactory->ReserveAdjacent(adj);
				ret = (path_to_follow.size() > 0);
			}
		else LOG("Could not create path correctly");
	}

	return ret;
}

bool Enemy::SearchPathToSubtile(const iPoint& goal)
{
	bool ret = false;

	if (path_to_follow.empty() == false)
		FreeMyReservedAdjacents();

	path_to_follow.clear();

	if (imOnSubtile.DistanceManhattan(goal) > 1)
	{
		if (App->pathfinding->CreateSubtilePath(imOnSubtile, goal) > 0)
		{
			path_to_follow = *App->pathfinding->GetLastPath();
			//if (path_to_follow.size() > 1)
			//	path_to_follow.erase(path_to_follow.begin());		// Enemy doesnt go to the center of his initial tile

			//if (path_to_follow.size() > 1)
			//	path_to_follow.pop_back();							// Enemy doesnt eat the player, stays at 1 tile

			/*iPoint adj = path_to_follow.back();
			App->entityFactory->ReserveAdjacent(adj);*/
			ret = (path_to_follow.size() > 0);
		}
	}
	return ret;
}

int Enemy::GetRandomValue(const int& min, const int& max) const
{
	static std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(min, max);
	float dice_roll = distribution(generator);
	return dice_roll;
}

bool Enemy::isInDetectionRange() const
{
	
	iPoint playerPos = App->entityFactory->player->GetTilePos(); 
	return (GetTilePos().DistanceManhattan(playerPos) < detectionRange);
}

bool Enemy::isInAttackRange() const
{
	return (GetSubtilePos().DistanceTo(App->entityFactory->player->GetSubtilePos()) <= attackRange);
}

bool Enemy::isNextPosFree(iPoint futurePos)
{
	iPoint onSubtilePosTemp = App->map->WorldToSubtileMap(futurePos.x, futurePos.y);

	return !(onSubtilePosTemp != previousSubtilePos && !App->entityFactory->isThisSubtileEnemyFree(onSubtilePosTemp));
}

bool Enemy::CheckFuturePos(float dt) const
{
	fPoint tempPos = GetPivotPos() + velocity * dt * speed;
	iPoint subtileTemp = App->map->WorldToSubtileMap(tempPos.x, tempPos.y);

	return !(subtileTemp != previousSubtilePos && !App->entityFactory->isThisSubtileEnemyFree(subtileTemp));
}

bool Enemy::isOnDestiny() const
{
	return GetPivotPos().DistanceTo(currentDestiny.Return_fPoint()) < 5;
}
void Enemy::FreeMyReservedAdjacents()
{
	std::vector<iPoint>::iterator item = path_to_follow.begin();
	for (; item != path_to_follow.end(); ++item)
	{
		if (App->entityFactory->isThisSubtileReserved(*item))
		{
			App->entityFactory->FreeAdjacent(*item);
			break;
		}
	}
}

iPoint Enemy::SearchNeighbourSubtile() const
{
	fPoint goal = position + velocity * speed * App->GetDt();
	iPoint goalSubtile = App->map->WorldToSubtileMap(goal.ReturniPoint().x, goal.ReturniPoint().y);
	iPoint neighbours[4];
	neighbours[0] = goalSubtile + iPoint(1, 0);
	neighbours[1] = goalSubtile + iPoint(0, 1);
	neighbours[2] = goalSubtile + iPoint(-1, 0);
	neighbours[3] = goalSubtile + iPoint(0, -1);
	uint i = 0;
	for (i; i <= 3; ++i)
	{
		if (App->entityFactory->isThisSubtileEnemyFree(neighbours[i]))
		{
			// This is the first free

			return neighbours[i];
			//SearchPathToSubtile(neighbours[i]);
			break;
		}
	}

	return iPoint(0, 0);

}

void Enemy::SetNewDirection()
{
	velocity = currentDestiny.Return_fPoint() - GetPivotPos();
	velocity.Normalize();
	SetLookingTo(currentDestiny.Return_fPoint());

	currentAnimation = &run[pointingDir];
}

void Enemy::MoveToCurrDestiny(float dt)
{
	position += velocity * dt * speed;
}

int Enemy::GetPointingDir(float angle)
{

	int numAnims = 8;
		//LOG("angle: %f", angle);
		// divide the semicircle in 4 portions
	float animDistribution = PI / (numAnims * 0.5f); // each increment between portions //two semicircles

	int i = 0;
	if (angle >= 0) // is going right or on bottom semicircle range to left
	{
		// iterate between portions to find a match
		for (float portion = animDistribution * 0.5f; portion <= PI; portion += animDistribution) // increment on portion units
		{
			if (portion >= angle) // if the portion is on angle range
			{
				// return the increment position matching with enumerator direction animation
				// TODO: not the best workaround, instead do with std::map
				/*LOG("bottom semicircle");
				LOG("portion: %i", i);*/
				break;
			}
			++i;
		}
	}
	else if (angle <= 0) // animations relatives to upper semicircle
	{
		i = 0; // the next 4 on the enum direction

		for (float portion = -animDistribution * 0.5f; portion >= -PI; portion -= animDistribution)
		{
			if (i == 1) i = numAnims * 0.5f + 1;
			if (portion <= angle)
			{
				/*LOG("upper semicircle");
				LOG("portion: %i", i);*/
				break;
			}
			++i;
		}
	}

	pointingDir = i;
	if (pointingDir == numAnims) // if max direction
		pointingDir = numAnims - 1; // set to prev

	//LOG("portion: %i", pointingDir);

	return pointingDir;
}

void Enemy::CheckRenderFlip()
{
	if (pointingDir == int(facingDirection::SW) || pointingDir == 4 || pointingDir == 7)
	{
		flip = SDL_FLIP_HORIZONTAL;
	}
	else
		flip = SDL_FLIP_NONE;
}

void Enemy::SetLookingTo(const fPoint& dir)
{
	fPoint aux;
	aux = dir - GetPivotPos();
	aux.Normalize();
	GetPointingDir(atan2f(aux.y, aux.x));
	CheckRenderFlip();
}

void Enemy::DebugPath() const
{
	for (uint i = 0; i < path_to_follow.size(); ++i)
	{
		if (!isSubpathRange) {
			iPoint pos = App->map->MapToWorld(path_to_follow[i].x + 1, path_to_follow[i].y);		// X + 1, Same problem with map
			App->render->DrawQuad({ pos.x, pos.y + (int)(App->map->data.tile_height * 0.5F), 5, 5 }, 255 , 0, 255, 175, true);
			//App->render->Blit(App->pathfinding->debug_texture, pos.x, pos.y);
		}
		else
		{
			iPoint pos = App->map->SubTileMapToWorld(path_to_follow[i].x + 1, path_to_follow[i].y);		// X + 1, Same problem with map
			App->render->DrawQuad({ pos.x, pos.y + (int)(App->map->data.tile_height * 0.5F * 0.5F), 3, 3 }, 255, 0, 255, 175, true);
			//App->render->Blit(App->pathfinding->debug_texture, pos.x, pos.y);
		}

	}

	iPoint subTilePos = GetSubtilePos();
	subTilePos = App->map->SubTileMapToWorld(subTilePos.x, subTilePos.y);
	App->render->Blit(debugSubtile, subTilePos.x, subTilePos.y, NULL);

	// Real subtile? 
	//App->render->Blit(debugSubtile, subTilePos.x - 16, subTilePos.y - 8, NULL);

	/*App->render->DrawQuad({ subTilePos.x, subTilePos.y, 5,5 }, 255, 255, 0, 255, true);
	App->render->DrawIsoQuad({ subTilePos.x, subTilePos.y, 16, 16});*/
}

bool Enemy::Load(pugi::xml_node &)
{
	return true;
}

bool Enemy::Save(pugi::xml_node &node) const
{
	if(type == ENTITY_TYPE::ENEMY_BOMB)
		node.append_attribute("type") = "enemyBomb";
	else if(type == ENTITY_TYPE::ENEMY_TEST)
		node.append_attribute("type") = "enemyTest";

	node.append_attribute("level") = level;
	node.append_attribute("life") = life;
	pugi::xml_node nodeSpeed = node.append_child("position");

	nodeSpeed.append_attribute("x") = position.x;
	nodeSpeed.append_attribute("y") = position.y;

	return true;
}


void Enemy::Draw() 
{
	if(App->scene->debugSubtiles == true)
		DebugPath();

	if (entityTex != nullptr)
	{
		if (currentAnimation != nullptr)
			App->render->Blit(entityTex, position.x, position.y, &currentAnimation->GetCurrentFrame(), 1.0F, flip);
		else
			App->render->Blit(entityTex, position.x, position.y);
	}	
}


