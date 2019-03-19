#include "UiItem_Image.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Scene.h"

UiItem_Image::UiItem_Image(iPoint position, const SDL_Rect* section) : UiItem(position)
{
	this->section = *section;
}

void UiItem_Image::Draw(const float& dt)
{
	App->render->Blit(App->gui->GetAtlas(), hitBox.x, hitBox.y, &this->section, 0.0F, SDL_FLIP_NONE);
}
