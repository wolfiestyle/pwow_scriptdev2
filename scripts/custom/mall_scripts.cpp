#include "precompiled.h"

bool GOUse_mall_portal(Player* pPlayer, GameObject* pGo)
{
    pPlayer->TeleportTo(35, -98.015, 149.835, -40.382, 3.093);

    return true;
}

void AddSC_mall_scripts()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_mall_portal";
    pNewScript->pGOUse = &GOUse_mall_portal;
    pNewScript->RegisterSelf();
}
