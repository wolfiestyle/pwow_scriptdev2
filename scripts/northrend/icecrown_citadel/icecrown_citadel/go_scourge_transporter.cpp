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

enum
{
    GOSSIP_TEXT_ID      = 15221,
};

static float const TeleportCoords[TP_MAX][4] =
{
    {-17.0711f, 2211.47f, 30.0546f, 0.0f},
    { -503.62f, 2211.47f, 62.8235f, 3.13002f},
    {-615.145f, 2211.47f, 199.972f, 6.27553f},
    {-549.131f, 2211.29f, 539.291f, 6.26765f},
    { 4199.35f, 2769.42f, 350.977f, 0.0f},
    { 4356.58f, 2565.75f, 220.402f, -1.5708f},
    //{ 4356.93f, 2769.41f, 355.955f, -2.35619f},
    { 529.3f,   -2124.7f, 1041.0f,  3.14159f} // this dest location for Frozen Throne teleport
};

static char const* const GossipStrings[TP_MAX] =
{
    "Light's Hammer",
    "Oratory of the Damned",
    "Rampart of Skulls",
    "Deathbringer's Rise",
    "Upper Spire",
    "Sindragosa's Lair",
    "Frozen Throne"
};

bool GOHello_scourge_transporter(Player *pPlayer, GameObject *pGo)
{
    ScriptedInstance *m_pInstance = dynamic_cast<ScriptedInstance*>(pGo->GetInstanceData());
    if (!m_pInstance)
        return false;
    bool skipCheck = pPlayer->isGameMaster();  // allow GM to teleport anywhere
    std::bitset<32> tpData = m_pInstance->GetData(DATA_TP_UNLOCKED);

    for (uint32 i = 0; i < TP_MAX; ++i)
        if (skipCheck || tpData[i])
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GossipStrings[i], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + i);

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

bool AT_scourge_transporter_trigger(Player* pPlayer, AreaTriggerEntry *pAt)
{
    if (pPlayer && !pPlayer->isGameMaster()) // prevent GMs from activating transporters
    {
        ScriptedInstance* m_pInstance = dynamic_cast<ScriptedInstance*>(pPlayer->GetInstanceData());

        if (!m_pInstance)
            return false;

        std::bitset<32> tpData = m_pInstance->GetData(DATA_TP_UNLOCKED);

        switch(pAt->id)
        {
            case 5736:  // Light's Hammer
                if (!tpData[TP_LIGHTS_HAMMER])
                {
                    m_pInstance->SetData(DATA_TP_UNLOCKED, m_pInstance->GetData(DATA_TP_UNLOCKED) | (1 << TP_LIGHTS_HAMMER));
                    return false;
                } else 
                    return false;
                break;
            case 5649:  // Upper Spire
                if (!tpData[TP_UPPER_SPIRE])
                {
                    if (m_pInstance->GetData(TYPE_SAURFANG) == DONE) // trigger enabled only if Saurfang is dead + someone actually gets there
                    {
                        pPlayer->SummonCreature(NPC_CROK_SCOURGEBANE, 4199.25f,  2769.26f,   351.06f, 0.0f,TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                        m_pInstance->SetData(DATA_TP_UNLOCKED, m_pInstance->GetData(DATA_TP_UNLOCKED) | (1 << TP_UPPER_SPIRE));
                        return false;
                    }
                    else
                        return false;  // we wont handle this event cause player shouldnt be here yet ;<
                } else 
                    return false;
                break;
            case 5604:  // 
                if (!tpData[TP_SINDRAGOSA_LAIR])
                {
                    if(m_pInstance->GetData(TYPE_VALITHRIA) == DONE)
                    {
                        m_pInstance->SetData(DATA_TP_UNLOCKED, m_pInstance->GetData(DATA_TP_UNLOCKED) | (1 << TP_SINDRAGOSA_LAIR));
                        return false;
                    }
                    else
                        return false;  // we wont handle this event cause player shouldnt be here yet ;<
                }
                break;
            case 5718:  // LK Teleporter
                if (!tpData[TP_FROZEN_THRONE])
                {
                    for (uint32 i = 0; i < MAX_ENCOUNTER -1; i++)
                    {
                        if (m_pInstance->GetData(i) != DONE)
                            return false;
                    }
                    m_pInstance->SetData(DATA_TP_UNLOCKED, m_pInstance->GetData(DATA_TP_UNLOCKED) | (1 << TP_FROZEN_THRONE));
                    pPlayer->TeleportTo(pPlayer->GetMapId(), TeleportCoords[TP_FROZEN_THRONE][0], TeleportCoords[TP_FROZEN_THRONE][1], TeleportCoords[TP_FROZEN_THRONE][2], TeleportCoords[TP_FROZEN_THRONE][3], 0);
                    return false;
                } 
                else
                {
                    pPlayer->TeleportTo(pPlayer->GetMapId(), TeleportCoords[TP_FROZEN_THRONE][0], TeleportCoords[TP_FROZEN_THRONE][1], TeleportCoords[TP_FROZEN_THRONE][2], TeleportCoords[TP_FROZEN_THRONE][3], 0);
                    return false;
                }
                break;
        }
        return false;
    }
    else 
    {
        //pPlayer->MonsterSay("GM's have no control under the Lich King's citadel!", 0, 0);
        return false;
    }
};

void AddSC_scourge_transporter()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "go_scourge_transporter";
    newscript->pGOGossipHello = &GOHello_scourge_transporter;
    newscript->pGOGossipSelect = &GOSelect_scourge_transporter;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_scourge_transporter";
    newscript->pAreaTrigger = &AT_scourge_transporter_trigger;
    newscript->RegisterSelf();
}
