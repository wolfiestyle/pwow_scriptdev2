#include "precompiled.h"

static char const PortalErrorStr[] = "Mall portal requires 1 vote per day to enable it. Visit voting.phoenix-wow.com";

bool GOUse_mall_portal(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->isGameMaster() || pPlayer->HasVoted())
        pPlayer->TeleportTo(35, -98.015, 149.835, -40.382, 3.093);
    else
    {
        WorldPacket data(SMSG_NOTIFICATION, sizeof(PortalErrorStr));
        data << PortalErrorStr;
        pPlayer->SendDirectMessage(&data);
    }

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
