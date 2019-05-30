#include "SaveTrigger.h"
#include "j1DialogSystem.h"
#include "j1Render.h"
#include "j1EntityFactory.h"
SaveTrigger::SaveTrigger(float posx, float posy) :Trigger(TRIGGER_TYPE::SAVE, posx, posy, "saveTrigger")
{
	entityTex = App->entityFactory->campFireTex;
	idle.PushBack({ 0,0,48,48 });
	idle.PushBack({ 48,0,48,48 });
	idle.PushBack({ 96,0,48,48 });
	idle.PushBack({ 144,0,48,48 });
	idle.PushBack({ 192,0,48,48 });
	idle.PushBack({ 0,48,48,48 });
	idle.PushBack({ 48,48,48,48 });
	idle.PushBack({ 96,48,48,48 });
	idle.speed = 10.F;
	idle.loop = true;

	nSubtiles = 1;
	SetPivot(24, 42);
	size.create(48, 48);
	AssignInSubtiles(nSubtiles);
	currentAnim = &idle;
}

SaveTrigger::~SaveTrigger()
{
	DeleteFromSubtiles(nSubtiles);
}

bool SaveTrigger::Update(float dt)
{
	
	return true;
}

void SaveTrigger::Draw()
{
	if (currentAnim != nullptr)
		App->render->Blit(entityTex, position.x, position.y, &currentAnim->GetCurrentFrame(), 1.0F);

	if (App->scene->debugSubtiles)
		DebugTrigger();
}

bool SaveTrigger::CleanUp()
{
	return true;
}

bool SaveTrigger::Save(pugi::xml_node &) const
{
	
	return true;
}

bool SaveTrigger::DoTriggerAction()
{
	if (App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN )
	{
		isActive = true;
		App->dialog->SetCurrentDialog("SAVEGAME");
	}
	return true;
}
