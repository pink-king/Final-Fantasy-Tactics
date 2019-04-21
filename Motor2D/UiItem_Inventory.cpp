#include "UiItem_Inventory.h"
#include "j1Gui.h"
#include "UiItem.h"
#include "PlayerEntityManager.h"
#include "j1EntityFactory.h"
#include "j1Fonts.h"
#include "j1Render.h"




UiItem_Inventory::UiItem_Inventory(UiItem* const parent) :UiItem(parent)
{
	this->parent = parent;
	this->guiType = GUI_TYPES::INVENTORY;
	App->scene->tab_inventory = App->gui->AddImage({ startingPos.x + 41, startingPos.y + 285 }, &tab_image, parent);
}



bool UiItem_Inventory::LoadElements()
{

	// - - - - - - - - - - - - - - - - - - - - - - - - - -  equipped objects 


	if (!App->entityFactory->player->equipedObjects.empty())
	{
		std::vector<LootEntity*>::iterator iter = App->entityFactory->player->equipedObjects.begin();

		for (; iter != App->entityFactory->player->equipedObjects.end(); ++iter)
		{
			if (!(*iter)->MyDescription->spawnedInventoryImage)
			{
				iPoint destPos = {};

				if ((*iter)->character == App->entityFactory->player->selectedCharacterEntity)  // Only load the selected character current items
				{
					// current weapon, armor and head have a target position each
					switch ((*iter)->GetObjectType())
					{
					case OBJECT_TYPE::WEAPON_OBJECT:
						destPos.x = startingPos.x + initialPositionsOffsets.currentWeapon.x;
						destPos.y = startingPos.y + initialPositionsOffsets.currentWeapon.y;

						(*iter)->MyDescription->myLootItemIsEquipped.weapon = true;
						break;
					case OBJECT_TYPE::ARMOR_OBJECT:
						destPos.x = startingPos.x + initialPositionsOffsets.currentArmor.x;
						destPos.y = startingPos.y + initialPositionsOffsets.currentArmor.y;

						(*iter)->MyDescription->myLootItemIsEquipped.armor = true;
						break;
					case OBJECT_TYPE::HEAD_OBJECT:
						destPos.x = startingPos.x + initialPositionsOffsets.currentHead.x;
						destPos.y = startingPos.y + initialPositionsOffsets.currentHead.y;

						(*iter)->MyDescription->myLootItemIsEquipped.head = true;
						break;


					}

					// create the icon image directly in that desired slot (aka position) 

					(*iter)->MyDescription->panelWithButton->section = App->scene->lootPanelRectNoButton;
					(*iter)->MyDescription->iconImageInventory = App->gui->AddSpecialImage(destPos, &(*iter)->MyDescription->iconImage->section, this, (*iter)->entityTex, (*iter)->MyDescription);
					(*iter)->MyDescription->iconImageInventory->printFromLoot = true;
					(*iter)->MyDescription->spawnedInventoryImage = true;


					(*iter)->MyDescription->myLootItemIsEquipped.state = ACTIVE;   // lastly put the image as active (we will need it later) 

				}


			}
		}
	}



	// - - - - - - - - - - - - - - - - - - - - - - - - - -  bag objects 


	if (!App->entityFactory->player->bagObjects.empty())
	{
		int i = 0;
		int j = 0;
		std::vector<LootEntity*>::iterator iter = App->entityFactory->player->bagObjects.begin();


		for (; iter != App->entityFactory->player->bagObjects.end(); ++iter)
		{
			if (!(*iter)->MyDescription->spawnedInventoryImage)
			{
				iPoint position(0, 0);

				if (i == 5)
				{
					i = 0;
					j++;
				}

				if (i == 0)  // first
				{

					position = { (startingPos.x + 56), (startingPos.y + 302 + j * boxSeparation.y) };


				}
				else   // the rest of elements in the row
				{
					position = { (startingPos.x + 56 + i * boxSeparation.x), (startingPos.y + 302) + j * boxSeparation.y };
				}


				(*iter)->MyDescription->panelWithButton->section = App->scene->lootPanelRectNoButton;
				(*iter)->MyDescription->iconImageInventory = App->gui->AddSpecialImage(position, &(*iter)->MyDescription->iconImage->section, this, (*iter)->entityTex, (*iter)->MyDescription);
				(*iter)->MyDescription->iconImageInventory->printFromLoot = true;
				(*iter)->MyDescription->spawnedInventoryImage = true;

				i++;
			}


		}

	}


	// TODO: Consumables: with dpad consum consumable, already define the icons in xml, just keep a a variable that holds the number of potions of each type


	return true;
}

/*
void UiItem_Inventory::DoLogicSelected(LootEntity * ent, bool doIt)
{

	App->gui->selected_object = ent->MyDescription->iconImageInventory;
	ent->MyDescription->iconImageInventory->tabbed = true;
	ent->MyDescription->iconImageInventory->state = HOVER;

	if(doIt)
	ent->MyDescription->HideAllElements(false);
	else
		ent->MyDescription->HideAllElements(true);

}*/

void UiItem_Inventory::Draw(const float& dt)
{
	/*SDL_Rect draw = { 0 };

	if (!drawTest)
	{
		for (int i = 0; i < MAX_Weapons_plus_Equipment; ++i)
		{
			draw.x = initialPositions.weaponsAndEquipment.x + i*boxSize;


			App->render->DrawQuad();
		}
	}*/


	//App->gui->ApplyTabBetweenSimilar(true); 



}


void UiItem_Inventory::De_______Equip(LootEntity * callback)
{
	iPoint destPos = {};


	// 1) Check that both the item that wants to be equipped and the already equipped one belong to the current player

	if (callback->character == App->entityFactory->player->selectedCharacterEntity)
	{
		std::vector<LootEntity*>::iterator iter = App->entityFactory->player->equipedObjects.begin();

		for (; iter != App->entityFactory->player->equipedObjects.end(); ++iter)
		{
			if ((*iter)->character == App->entityFactory->player->selectedCharacterEntity)       // Search only for the selected character's current items
			{
				if ((*iter)->MyDescription->myLootItemIsEquipped.state == ACTIVE)
				{
					if ((*iter)->GetObjectType() == OBJECT_TYPE::WEAPON_OBJECT && (*iter)->MyDescription->myLootItemIsEquipped.weapon)
					{
						//App->entityFactory->player->DesequipItem((*iter));
						(*iter)->MyDescription->myLootItemIsEquipped.weapon = false;
						(*iter)->MyDescription->myLootItemIsEquipped.state = INACTIVE;

						// 2) The de-equipped item position will be at the grid, where the new item is (spwapping positions) 
						(*iter)->MyDescription->iconImageInventory->hitBox.x = callback->MyDescription->iconImageInventory->hitBox.x;
						(*iter)->MyDescription->iconImageInventory->hitBox.y = callback->MyDescription->iconImageInventory->hitBox.y;
					}
					else if ((*iter)->GetObjectType() == OBJECT_TYPE::ARMOR_OBJECT && (*iter)->MyDescription->myLootItemIsEquipped.armor)
					{
						//App->entityFactory->player->DesequipItem((*iter));
						(*iter)->MyDescription->myLootItemIsEquipped.armor = false;
						(*iter)->MyDescription->myLootItemIsEquipped.state = INACTIVE;

						// 2) The de-equipped item position will be at the grid, where the new item is (spwapping positions) 
						(*iter)->MyDescription->iconImageInventory->hitBox.x = callback->MyDescription->iconImageInventory->hitBox.x;
						(*iter)->MyDescription->iconImageInventory->hitBox.y = callback->MyDescription->iconImageInventory->hitBox.y;
					}
					else if ((*iter)->GetObjectType() == OBJECT_TYPE::HEAD_OBJECT && (*iter)->MyDescription->myLootItemIsEquipped.head)
					{
						//App->entityFactory->player->DesequipItem((*iter));
						(*iter)->MyDescription->myLootItemIsEquipped.head = false;
						(*iter)->MyDescription->myLootItemIsEquipped.state = INACTIVE;

						// 2) The de-equipped item position will be at the grid, where the new item is (spwapping positions) 
						(*iter)->MyDescription->iconImageInventory->hitBox.x = callback->MyDescription->iconImageInventory->hitBox.x;
						(*iter)->MyDescription->iconImageInventory->hitBox.y = callback->MyDescription->iconImageInventory->hitBox.y;
					}
				}

			}
		}

		// 3) THEN equip the new item 

		App->entityFactory->player->EquipItem(callback);

		switch (callback->GetObjectType())
		{
		case OBJECT_TYPE::WEAPON_OBJECT:
			callback->MyDescription->myLootItemIsEquipped.weapon = true;
			callback->MyDescription->iconImageInventory->hitBox.x = startingPos.x + initialPositionsOffsets.currentWeapon.x;
			callback->MyDescription->iconImageInventory->hitBox.y = startingPos.y + initialPositionsOffsets.currentWeapon.y;

			break;
		case OBJECT_TYPE::ARMOR_OBJECT:
			callback->MyDescription->myLootItemIsEquipped.armor = true;
			callback->MyDescription->iconImageInventory->hitBox.x = startingPos.x + initialPositionsOffsets.currentArmor.x;
			callback->MyDescription->iconImageInventory->hitBox.y = startingPos.y + initialPositionsOffsets.currentArmor.y;

			break;

		case OBJECT_TYPE::HEAD_OBJECT:
			callback->MyDescription->myLootItemIsEquipped.head = true;
			callback->MyDescription->iconImageInventory->hitBox.x = startingPos.x + initialPositionsOffsets.currentHead.x;
			callback->MyDescription->iconImageInventory->hitBox.y = startingPos.y + initialPositionsOffsets.currentHead.y;

			break;

		}

		callback->MyDescription->myLootItemIsEquipped.state = ACTIVE;


	}

}






