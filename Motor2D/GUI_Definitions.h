#ifndef GUI_DEFINITIONS_H
#define GUI_DEFINITIONS_H

#include "UiItem_CooldownClock.h"

/// GUI CPP 

// - - - - - - - the following two methods are only meant for loading the xml data - - - - - - - 
struct coolDownClockData
{
	iPoint position;
	SDL_Rect section;
	std::string type;
};

struct theClocks
{

	coolDownClockData ability1, ability2, ulti, potion;

};

// - - - - - - - the following two methods are meant for checking created clocks according to type and player - - - - - - - 

struct createdClocks {
	bool special1 = false;
	bool special2 = false;
	bool ulti = false;
	bool potion = false;
};


struct clockOwners {
	createdClocks Marche, Ritz, Shara;
};

// - - - - - - - - - - - - - this method is meant to contain all clocks for a single player - - - - - - - - - - - - // 



struct myClocks {
	UiItem_CooldownClock* special1 = nullptr;
	UiItem_CooldownClock* special2 = nullptr;
	UiItem_CooldownClock* ulti = nullptr;
	UiItem_CooldownClock* potion = nullptr;
};


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - // 

// health bar info 

struct enemyHealthBarInfo
{
	SDL_Rect staticSection; 
	SDL_Rect dynamicSection;
	SDL_Rect damageSection;
};


struct bossLifeBarInfo
{
	SDL_Rect staticSection;
	SDL_Rect dynamicSection;
};


class GUI_Definitions
{


};


#endif 





