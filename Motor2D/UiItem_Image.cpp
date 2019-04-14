#include "UiItem_Image.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1LootManager.h"

UiItem_Image::UiItem_Image(iPoint position, const SDL_Rect* section, UiItem*const parent, bool isPanel) : UiItem(position, parent)
{
	this->section = *section;
	this->guiType = GUI_TYPES::IMAGE;


	this->hitBox.w = section->w;
	this->hitBox.h = section->h;

	if (isPanel == 1)
	{
		this->isPanel = true;
	}

	// the parent
	this->parent = parent;



	this->hitBox.x = position.x;
	this->hitBox.y = position.y;

}

void UiItem_Image::Draw(const float& dt)
{

	
	if (!hide)
	{
		// TODO: don't blit the icon in the loot item description using the GUI atlas, but instead the Loot atlas 

		if (!printFromLoot)
		{
			App->render->BlitGui(App->gui->GetAtlas(), hitBox.x, hitBox.y, &this->section, 0.0F, 1.0f, 0.0f, resizedRect);
		}
		else
		{
			// TODO: check that this works
			App->render->BlitGui(App->lootManager->LootTexture, hitBox.x, hitBox.y, &this->section, 0.0F);
		}
		
	}
}