#ifndef UIITEM_BAR_H
#define UIITEM_BAR_H

#include "UiItem.h"
#include "p2Point.h"
#include "j1Textures.h"

class UiItem_Image;
class UiItem_Label;

class UiItem_Checkbox :public UiItem
{
protected:

	UiItem_Image* panel = nullptr;
	UiItem_Image* box = nullptr; 
	UiItem_Label* labe = nullptr; 
	UiItem_Image* tick = nullptr;
	
	SDL_Rect panel_section;   // we will care about the x size. The y size will be calculated procedurally 
	SDL_Rect box_section;

	void DoLogicClicked(bool do_slide = false);   // clicking logic matters the most
	/*void DoLogicHovered(bool do_slide = false);
	void DoLogicAbandoned(bool slidable = false);*/  // TODO: add anim change 

public:
	UiItem_Checkbox(iPoint position, const SDL_Rect* section, const SDL_Rect* box_section, UiItem*const parent); // checkbox does not have parent? or the empty obj?? 
	~UiItem_Checkbox() {}
	void Draw(const float& dt);



};


#endif // UIITEM_BAR_H
