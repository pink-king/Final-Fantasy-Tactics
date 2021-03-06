#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "j1Entity.h"
#include "p2Point.h"

#define DESTRUCTIONRANGE 550


enum class PROJECTILE_TYPE
{
	BASIC_ARROW,
	CONTAGIOUS_ARROW,
	FIRE_ARROW,
	MAGIC_BOLT,
	DEATH_CIRCLE,
	EMMITER_ARROWS,
	EMMITER,
	MEDUSA,
	TORNADO,
	EARTH_SHAKER,
	ENEMY_ARROW,
	GOLEM_ARROW,
	BOSS_EMMITER,
	BOSS_EMMITER_ARROWS,
	WHISPER_OF_ICE,
	NO_ARROW
};

class Projectile : public j1Entity
{
public:
	Projectile(fPoint pos, fPoint destination, uint speed, const j1Entity* owner, const char* name, PROJECTILE_TYPE type);
	~Projectile(); 

	void SetInitially();
	bool PostUpdate() override;		// Checks if its too far from the player and deletes it
	void SetNewDirection(const fPoint& newdir);		
	float SetMyAngleRotation(const fPoint& direction);	// Useful for render rotations
	bool TooFarAway() const;
	bool OnCollisionWithEnemy(const int& radius = 0) const;
	bool OnCollisionWithWall(const int& radius = 0) const;
	bool OnCollisionWithPlayer(const int& radius = 0) const;
	bool ReachedDestiny() const; // Needs to take a look to the initial direction (vertical cases)
	void Draw() override;

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	const j1Entity* GetOwnerEntity() const;

protected: 

	Animation anim; 
	const j1Entity* owner = nullptr;
	SDL_Texture* debugSubtile = nullptr;
	fPoint destination = { 0, 0 };
	fPoint direction = { 0,0 };
	uint speed = 0;
	float angle = 0.F;
	bool to_explode = false;
	bool willExplode = true;
	PROJECTILE_TYPE type;
};

#endif
