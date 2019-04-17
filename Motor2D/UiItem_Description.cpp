#include "UiItem_Description.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "j1Window.h"

UiItem_Description::UiItem_Description(iPoint position, std::string itemName, const SDL_Rect* panelRect, const SDL_Rect* iconRect, float Value, EquipmentStatType variableType, UiItem*const parent) : UiItem(position, parent)
{
	this->descrType = descriptionType::EQUIPMENT; 
	this->parent = parent; 

	// TODO

}


UiItem_Description::UiItem_Description(iPoint position, std::string itemName, const SDL_Rect* panelRect, const SDL_Rect* iconRect, float Attack, float resistance, UiItem*const parent) : UiItem(position, parent)
{

	this->descrType = descriptionType::WEAPON;
	this->parent = parent;

	panelWithButton = App->gui->AddImage(iPoint(0, 0), panelRect, this);
	panelWithButton->useCamera = false; 

	if (itemName.empty())
	{
		itemName = "Item has no name"; 
	}

    name = App->gui->AddLabel(itemName, { 155, 126, 186, 255 }, App->font->openSansBold18, iPoint(0, 0), this);
	name->useCamera = false;
	

	std::string resString("Resistance: "); 
	resString.append(std::to_string((int)resistance));
	resistanceLabel = App->gui->AddLabel(resString, { 0, 0, 0, 255 }, App->font->openSansBold18, iPoint(0, 0), this);
	
	resistanceLabel->useCamera = false;

	std::string dmgString("Damage: ");
	dmgString.append(std::to_string((int)Attack));
    damageLabel = App->gui->AddLabel(dmgString, { 0, 0, 0, 255 }, App->font->openSansBold18, iPoint(0, 0), this);
	
	damageLabel->useCamera = false;


	// the icon image is created after creating description in loot spawning
	


	

	

}


void UiItem_Description::HideAllElements(bool hide)
{
		this->iconImage->hide = hide;
		this->panelWithButton->hide = hide;
		this->name->hide = hide;
		this->hide = hide;

	   
		if (this->descrType == descriptionType::WEAPON)
		{
			this->damageLabel->hide = hide; 
			this->resistanceLabel->hide = hide;

		}
		else if(this->descrType == descriptionType::EQUIPMENT)
		{

		}


}

void UiItem_Description::RepositionAllElements(iPoint referencePanelPosition)
{

	this->panelWithButton->hitBox.x = referencePanelPosition.x;
	this->panelWithButton->hitBox.y = referencePanelPosition.y;

	this->iconImage->hitBox.x = referencePanelPosition.x;
	this->iconImage->hitBox.y = referencePanelPosition.y;

	this->name->hitBox.x = referencePanelPosition.x + 20; 
	this->name->hitBox.y = referencePanelPosition.y + 40;

	if (this->descrType == descriptionType::WEAPON)
	{
		this->damageLabel->hitBox.x = referencePanelPosition.x + 90;
		this->damageLabel->hitBox.y = referencePanelPosition.y + 70;

		this->resistanceLabel->hitBox.x = referencePanelPosition.x + 90;
		this->resistanceLabel->hitBox.y = referencePanelPosition.y + 100;

	}
	else if (this->descrType == descriptionType::EQUIPMENT)
	{

	}




	// switch description type and reposition everything

}


/*
void UiItem_Description::DeleteEverything()
{

	this->iconImage->to_delete = true;
	this->panelWithButton->to_delete = true;
	this->name->to_delete = true;

	if (this->descrType == descriptionType::WEAPON)
	{
		this->damageLabel->to_delete = true;
		this->resistanceLabel->to_delete = true;

	}
	else if (this->descrType == descriptionType::EQUIPMENT)
	{

	}

	this->to_delete = true;

}*/
