/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: go_scourge_transporter
SD%Complete: 0%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum TeleportLocations
{
    TP_LIGHTS_HAMMER,
    TP_ORATORY,
    TP_RAMPART_OF_SKULLS,
    TP_DEATHBRINGER_RISE,
    TP_UPPER_SPIRE,
    TP_SINDRAGOSA_LAIR,
    TP_FROZEN_THRONE,
    // values count
    TP_MAX
};

enum
{
    GOSSIP_TEXT_ID      = 15221,
};

typedef UNORDERED_MAP<uint32, TeleportLocations> EntryLocationMap;

static EntryLocationMap const EntryToLocation = map_initializer<EntryLocationMap>
    (GO_TP_LIGHTS_HAMMER,       TP_LIGHTS_HAMMER)
    (GO_TP_ORATORY,             TP_ORATORY)
    (GO_TP_RAMPART_OF_SKULLS,   TP_RAMPART_OF_SKULLS)
    (GO_TP_DEATHBRINGER_RISE,   TP_DEATHBRINGER_RISE)
    (GO_TP_UPPER_SPIRE,         TP_UPPER_SPIRE)
    (GO_TP_SINDRAGOSA_LAIR,     TP_SINDRAGOSA_LAIR)
    (GO_TP_FROZEN_THRONE,       TP_FROZEN_THRONE);

static float const TeleportCoords[TP_MAX][4] =
{
    {-17.0711f, 2211.47f, 30.0546f, 0.0f},
    { -503.62f, 2211.47f, 62.8235f, 3.13002f},
    {-615.145f, 2211.47f, 199.972f, 6.27553f},
    {-549.131f, 2211.29f, 539.291f, 6.26765f},
    { 4199.35f, 2769.42f, 350.977f, 3.14159f},
    { 4356.58f, 2565.75f, 220.402f, -1.5708f},
    { 4356.93f, 2769.41f, 355.955f, -2.35619f}
};

#define G_LIGHTS_HAMMER     "Light's Hammer"
#define G_ORATORY           "Oratory of the Damned"
#define G_RAMPART_OF_SKULLS "Rampart of Skulls"
#define G_DEATHBRINGER_RISE "Deathbringer's Rise"
#define G_UPPER_SPIRE       "Upper Spire"
#define G_SINDRAGOSA_LAIR   "Sindragosa's Lair"
#define G_FROZEN_THRONE     "Frozen Throne"

bool GOHello_scourge_transporter(Player *pPlayer, GameObject *pGo)
{
    ScriptedInstance *m_pInstance = dynamic_cast<ScriptedInstance*>(pGo->GetInstanceData());
    if (!m_pInstance)
        return false;
    bool skipCheck = pPlayer->isGameMaster();  // allow GM to teleport anywhere
    std::bitset<32> tpData = m_pInstance->GetData(DATA_TP_UNLOCKED);

    // TODO: destinations should be enabled by AreaTrigger, using on GO click for now
    TeleportLocations selectedLoc = map_find(EntryToLocation, pGo->GetEntry(), TP_MAX);
    if (selectedLoc < TP_MAX)
    {
        tpData[selectedLoc] = true;
        m_pInstance->SetData(DATA_TP_UNLOCKED, tpData.to_ulong());
    }

    if (skipCheck || tpData[TP_LIGHTS_HAMMER])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_LIGHTS_HAMMER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + TP_LIGHTS_HAMMER);
    if (skipCheck || tpData[TP_ORATORY])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_ORATORY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + TP_ORATORY);
    if (skipCheck || tpData[TP_RAMPART_OF_SKULLS])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_RAMPART_OF_SKULLS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + TP_RAMPART_OF_SKULLS);
    if (skipCheck || tpData[TP_DEATHBRINGER_RISE])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_DEATHBRINGER_RISE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + TP_DEATHBRINGER_RISE);
    if (skipCheck || tpData[TP_UPPER_SPIRE])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_UPPER_SPIRE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + TP_UPPER_SPIRE);
    if (skipCheck || tpData[TP_SINDRAGOSA_LAIR])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_SINDRAGOSA_LAIR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + TP_SINDRAGOSA_LAIR);
    /* FIXME: not sure how to do this one
    if (skipCheck || tpData[TP_FROZEN_THRONE])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_FROZEN_THRONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + TP_FROZEN_THRONE);
    */

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID, pGo->GetGUID());

    return true;
}

bool GOSelect_scourge_transporter(Player* pPlayer, GameObject* pGo, uint32 uiSender, uint32 uiAction)
{
    if (uiAction >= GOSSIP_ACTION_INFO_DEF && uiAction < GOSSIP_ACTION_INFO_DEF + TP_MAX)
    {
        uint32 i = uiAction - GOSSIP_ACTION_INFO_DEF;
        pPlayer->TeleportTo(pPlayer->GetMapId(), TeleportCoords[i][0], TeleportCoords[i][1], TeleportCoords[i][2], TeleportCoords[i][3], 0);
    }

    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

void AddSC_scourge_transporter()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "go_scourge_transporter";
    newscript->pGOGossipHello = &GOHello_scourge_transporter;
    newscript->pGOGossipSelect = &GOSelect_scourge_transporter;
    newscript->RegisterSelf();
}
