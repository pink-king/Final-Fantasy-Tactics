#include "j1BuffManager.h"
#include <string.h>

#include "j1Window.h"

j1BuffManager::j1BuffManager()
{
	name.assign("Buff");
}

j1BuffManager::~j1BuffManager()
{
}

bool j1BuffManager::Awake(pugi::xml_node &node)
{
	bool ret = true;

	for (buffNode = node.child("buff"); buffNode && ret; buffNode = buffNode.next_sibling("buff"))
	{
		/*std::string add = buffNode.attribute("type").as_string();
		std::string clas = buffNode.attribute("class").as_string();
		if (add.compare("additive") == 0)
		{
			if(clas.compare("weapon") == 0)
			CreateBuff(BUFF_TYPE::ADDITIVE,OBJECT_TYPE::WEAPON_OBJECT, buffNode.attribute("name").as_string(), buffNode.attribute("character").as_string(), buffNode.attribute("stat").as_string(), buffNode.attribute("value").as_float());
			if (clas.compare("armor") == 0)
				CreateBuff(BUFF_TYPE::ADDITIVE, OBJECT_TYPE::ARMOR_OBJECT, buffNode.attribute("name").as_string(), buffNode.attribute("character").as_string(), buffNode.attribute("stat").as_string(), buffNode.attribute("value").as_float());
			if (clas.compare("head") == 0)
				CreateBuff(BUFF_TYPE::ADDITIVE, OBJECT_TYPE::HEAD_OBJECT, buffNode.attribute("name").as_string(), buffNode.attribute("character").as_string(), buffNode.attribute("stat").as_string(), buffNode.attribute("value").as_float());
		}
		else if (add.compare("multiplicative") == 0)
		{
			if (clas.compare("weapon") == 0)
				CreateBuff(BUFF_TYPE::MULTIPLICATIVE, OBJECT_TYPE::WEAPON_OBJECT, buffNode.attribute("name").as_string(), buffNode.attribute("character").as_string(), buffNode.attribute("stat").as_string(), buffNode.attribute("value").as_float());
			if (clas.compare("armor") == 0)
				CreateBuff(BUFF_TYPE::MULTIPLICATIVE, OBJECT_TYPE::ARMOR_OBJECT, buffNode.attribute("name").as_string(), buffNode.attribute("character").as_string(), buffNode.attribute("stat").as_string(), buffNode.attribute("value").as_float());
			if (clas.compare("head") == 0)
				CreateBuff(BUFF_TYPE::MULTIPLICATIVE, OBJECT_TYPE::HEAD_OBJECT, buffNode.attribute("name").as_string(), buffNode.attribute("character").as_string(), buffNode.attribute("stat").as_string(), buffNode.attribute("value").as_float());
		}*/
	}

	burnedDamagesecond = node.child("timebuff").attribute("burnedInSecond").as_float();
	return ret;
}

bool j1BuffManager::Start()
{
	return true;
}

bool j1BuffManager::Update(float dt)
{
	bool ret = true;

	if (entitiesTimeDamage.size() != 0)
	{
		std::list<j1Entity*>::iterator item = entitiesTimeDamage.begin();
		for (; item != entitiesTimeDamage.end() && ret; ++item)
			
			if (DamageInTime(*item))
				entitiesTimeDamage.remove(*item);
	}

	static char title[30];
	std::string name;
	std::list<Buff*>::iterator item =buffs.begin();
	for (; item != buffs.end(); ++item)
	{
		if ((*item)->GetIfActive())
		{
			name = (*item)->GetName();
			sprintf_s(title, 30, " |  buff: %s", name.data());
			App->win->AddStringToTitle(title);
		}
	}
	App->win->ClearTitle();
	return ret;
}

bool j1BuffManager::CleanUp()
{
	std::list<Buff*>::iterator item = buffs.begin();

	while (item != buffs.end())
	{
		buffs.remove(*item);
		++item;
	}
	buffs.clear();

	std::list<j1Entity*>::iterator item2 = entitiesTimeDamage.begin();

	while (item2 != entitiesTimeDamage.end())
	{
		entitiesTimeDamage.remove(*item2);
		++item2;
	}
	entitiesTimeDamage.clear();

	return true;
}


void j1BuffManager::CreateBuff(BUFF_TYPE type,OBJECT_TYPE clas, std::string name, std::string character, std::string stat, float value)
{
	bool exist = false;
	std::list<Buff*>::iterator item = buffs.begin();
	if(buffs.size() == 0)
		buffs.push_back(new Buff(type, clas, name, character, stat, value, GetNewSourceID()));
	else
	{
		for (; item != buffs.end(); ++item)
		{
			if (name.compare((*item)->GetName()) == 0 && ((*item)->GetCharacter()).compare(character) == 0)
				exist = true;
		}

		if (!exist)
		{
			std::list<Buff*>::iterator item2 = buffs.begin();
			for (; item2 != buffs.end(); ++item2)
			{
				if (clas == (*item2)->GetObjectType())
					buffs.remove(*item2);
			}
			buffs.push_back(new Buff(type, clas, name, character, stat, value, GetNewSourceID()));
		}
	}
}

void j1BuffManager::RemoveBuff(std::string name)
{
	std::list<Buff*>::iterator item = buffs.begin();
	for (; item != buffs.end(); ++item)
		if(name.compare((*item)->GetCharacter()) != 0)
			buffs.remove((*item));
}

float j1BuffManager::CalculateStat(const j1Entity* ent,float initialDamage, std::string stat)
{

	float totalMult = 0.f;
	for (std::list<Buff*>::iterator iter = buffs.begin(); iter != buffs.end(); ++iter)
	{
		if (ent != nullptr && (*iter)->GetIfActive() && stat.compare((*iter)->GetStat()) == 0 
			&& (ent->name.compare((*iter)->GetCharacter()) == 0 || ent->name.compare("all") == 0))
		{
					if ((*iter)->GetType() == BUFF_TYPE::ADDITIVE)
						initialDamage += (*iter)->GetValue();

					else if ((*iter)->GetType() == BUFF_TYPE::MULTIPLICATIVE)
						totalMult += (*iter)->GetValue();	
		}
	}
	
	return initialDamage * (1 + totalMult);
}

uint j1BuffManager::GetNewSourceID()
{
	return ++lastSourceID;
}

void j1BuffManager::DirectAttack(j1Entity * attacker, j1Entity* defender, float initialDamage, std::string stat)
{
	defender->life -= CalculateStat(attacker, initialDamage, stat) - CalculateStat(attacker, defender->defence, stat);;
	if (defender->life < 0)
		defender->life = 0;
}

void j1BuffManager::CreateBurned(j1Entity* attacker, j1Entity* defender, float damage)
{
	entityStat* newStat = new entityStat(STAT_TYPE::BURNED_STAT, damage);
	newStat->maxDamage = App->buff->CalculateStat(attacker, newStat->maxDamage, "hability") - App->buff->CalculateStat(defender, defender->defence, "defence");
	newStat->count.Start();
	defender->stat.push_back(newStat);
	defender->isBurned = true;
	entitiesTimeDamage.push_back(defender);
}

void j1BuffManager::CreateParalize(j1Entity * attacker, j1Entity * defender)
{
	entityStat* newStat = new entityStat(STAT_TYPE::PARALIZE_STAT, 1);
	newStat->count.Start();
	defender->stat.push_back(newStat);
	defender->isParalize = true;
	entitiesTimeDamage.push_back(defender);
}


void j1BuffManager::ActiveBuff(std::string buffName, std::string character, OBJECT_TYPE clasType)
{
	std::list<Buff*>::iterator item = buffs.begin();
	for (; item != buffs.end(); ++item)
	{
		if (buffName.compare((*item)->GetName()) == 0)
		{
			(*item)->ActiveBuff();
			character.assign((*item)->GetCharacter());
		}
		else
		{
			if (((*item)->GetCharacter()).compare(character) == 0
				&& clasType == (*item)->GetObjectType() && (*item)->GetIfActive())
				buffs.remove(*item);
		}
			
	}
}

void j1BuffManager::DeleteBuff(std::string buffName)
{
	std::list<Buff*>::iterator item = buffs.begin();
	for (; item != buffs.end(); ++item)
	{
		if (buffName.compare((*item)->GetName()) == 0)
			buffs.remove(*item);
	}
}

bool j1BuffManager::DamageInTime(j1Entity* entity)
{ 
	bool ret = false;
	std::list<entityStat*>::iterator item = entity->stat.begin();
	for (; item != entity->stat.end(); ++item)
	{
		switch ((*item)->type)
		{
		case STAT_TYPE::BURNED_STAT:

			if ((*item)->maxDamage + burnedDamagesecond > burnedDamagesecond)
			{
				if ((*item)->count.ReadSec() > 0.5)
				{
					entity->life -= burnedDamagesecond;
					(*item)->maxDamage -= burnedDamagesecond;
					(*item)->count.Start();
				}
			}
			else
			{
				entity->isBurned = false;
				entity->stat.remove(*item);
			}
			break;
		case STAT_TYPE::PARALIZE_STAT:
			if ((*item)->count.ReadSec() > (*item)->maxDamage)
			{
				entity->stat.remove(*item);
				entity->isParalize = false;
			}
			break;
		case STAT_TYPE::NORMAL:
			break;
		default:
			break;
		}
	}
	if (entity->life < 0)
		entity->life = 0;
	if (entity->stat.size() == 0)
		ret = true;

	return ret;
}

//void j1BuffManager::ZoneAttack(j1Entity * attacker, std::vector<j1Entity*> defenders, float initialDamage)
//{
//	float powerAttack = CalculateStat(attacker->name, initialDamage);
//}

float j1BuffManager::GetBurnedDamage()
{
	return burnedTotalDamage;
}