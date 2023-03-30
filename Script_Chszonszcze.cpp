#include "Script_Chszonszcze.h"

#include "util\Memory.h"
#include "util\Logging.h"
#include "util\Hook.h"
#include "util/Util.h"

#include "Script.h"

mCFunctionHook Hook_OnTouch;
void OnTouch(eCEntity * TouchedEntity, eCContactIterator & ci) {
	Entity This = Entity(Hook_OnTouch.GetSelf<eCDynamicEntity *>());
	Entity Target = Entity(TouchedEntity);
	Entity Player = Entity::GetPlayer();

	if (This != Target && 
		This == Player && 
		!Player.NPC.IsTransformed() && 
		Target.NPC.IsValid())
	{
		gESpecies TargetSpecies = Target.NPC.Species;
		if (TargetSpecies == gESpecies::gESpecies_Meatbug ||
			TargetSpecies == gESpecies::gESpecies_Snake ||
			TargetSpecies == gESpecies::gESpecies_Rabbit ||
			TargetSpecies == gESpecies::gESpecies_Lizard)
		{
			/*This is not really needed, I think---------------------------------------------*/
			gCScriptAdmin & ScriptAdmin = GetScriptAdmin();

			Target.DamageReceiver.AccessProperty<PSDamageReceiver::PropertyDamageAmount>() = 0;
			Target.NPC.SetLastAttacker(Target.NPC.GetLastAttacker());
			Target.NPC.SetCurrentAttacker(This);
			ScriptAdmin.CallScriptFromScript("SetHitPoints", &Target, &None, 0);
			/*-------------------------------------------------------------------------------*/

			Target.Routine.FullStop();
			Target.Routine.SetTask("ZS_RagDollDead");
		}
	}

	Hook_OnTouch.GetOriginalFunction(&OnTouch)(TouchedEntity, ci);
}

gSScriptInit & GetScriptInit()
{
	static gSScriptInit s_ScriptInit;
	return s_ScriptInit;
}

extern "C" __declspec(dllexport)
gSScriptInit const * GE_STDCALL ScriptInit(void)
{
	GetScriptAdmin().LoadScriptDLL("Script_Game.dll");

	Hook_OnTouch.Hook(PROC_Engine("?OnTouch@eCDynamicEntity@@UAEXPAVeCEntity@@AAVeCContactIterator@@@Z"), &OnTouch, mCBaseHook::mEHookType_ThisCall);

	return &GetScriptInit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		::DisableThreadLibraryCalls(hModule);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
