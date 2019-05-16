#ifndef _TRIGGER_H_
#define _TRIGGER_H_

#include "j1Entity.h"

enum class TRIGGER_TYPE
{
	PORTAL,
	NO_TRIGGER
};

class Trigger : public j1Entity
{
public:
	Trigger(TRIGGER_TYPE type, float posx, float posy,std::string name);
	~Trigger();

	bool CleanUp();

	virtual bool DoTriggerAction();
	void AssignInSubtiles(int numbreOfSubtile);
public:
	TRIGGER_TYPE type = TRIGGER_TYPE::NO_TRIGGER;
	SDL_Rect rect;

protected:
	int nSubtiles;
};


#endif