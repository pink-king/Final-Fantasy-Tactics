#ifndef UIITEM_LABEL_H
#define UIITEM_LABEL_H

#include "UiItem.h"
#include "j1PerfTimer.h"
#include <string>
#include "p2Point.h"
#include "SDL_ttf/include/SDL_ttf.h"

class UiItem_Label : public UiItem
{
public:
	UiItem_Label(std::string text, SDL_Color color, TTF_Font * font, p2Point<int> position, UiItem*const parent);
	UiItem_Label(std::string text, SDL_Color color, TTF_Font * font, bool typewriter, p2Point<int> position, UiItem*const parent);
	/*bool ChangeTextureHover(const std::string * textHover, const SDL_Color* color, const TTF_Font* font);
	bool ChangeTextureIdle(const std::string * textHover, const SDL_Color* color, const TTF_Font* font);*/
	bool ChangeTextureIdle(std::string textIdle, const SDL_Color* color, const TTF_Font* font);
	void Draw(const float& dt) override;
	//bool Update(float dt) override;
	bool TypeWriter();
protected:
	std::string text;
	std::string typewriter_text;
	//std::string text_aux;
	bool timer_typewriter = false;
	bool spawn_typewriter = false;
	int counter_typewriter = 0;
	bool right_text = false;
	bool left_text = false;
	SDL_Color color;
	TTF_Font* font = nullptr;
	SDL_Texture* texture = nullptr;
	j1PerfTimer typewriter_time;


public:  
	//bool hide = false;

};
#endif
