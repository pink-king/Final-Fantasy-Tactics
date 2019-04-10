#ifndef  _CONSUMABLELOOT_H__
#define _CONSUMABLELOOT_H__

#include "LootEntity.h"

enum class CONSUMABLE_TYPE
{
	
	POTION,
	NO_CONSUMABLE
};
class Consumable : public LootEntity
{

public:
	Consumable(int posX, int posY);
	~Consumable();

	bool Update(float dt);
	
	void ChooseConsumable();
	void SetConsumable();
	
	//void ConstructConsumable();
public:
	CONSUMABLE_TYPE  consumableType;

	int chance;
};
#endif // ! _CONSUMABLE_LOOT_H__