#ifndef _SAVETRIGGER_H_
#define _SAVETRIGGER_H_

#include "Trigger.h"


class SaveTrigger : public Trigger
{
public:
	SaveTrigger(float posx, float posy);
	~SaveTrigger();

	bool Update(float dt);

	void Draw() override;

	bool CleanUp();

	bool Save(pugi::xml_node&) const;
	bool DoTriggerAction() override;
private:
	Animation idle;
	Animation* currentAnim = nullptr;
	Animation saveSign;
	Animation sign;

	SDL_Texture* signTex = nullptr;
	bool isActive = false;
};


#endif