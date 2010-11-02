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
SD%Complete: 100%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
    GOSSIP_TEXT_ID      = 15221,
};

static uint32 const TeleportSpells[TP_MAX] =
{
    70781,
    70856,
    70857,
    70858,
    70859,
    70861,
    70860,
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
        pPlayer->CastSpell(pPlayer, TeleportSpells[i], false);
    }

    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

bool AT_scourge_transporter_trigger(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (!pPlayer || pPlayer->isGameMaster())    // prevent GMs from activating transporters
        return false;

    ScriptedInstance* m_pInstance = dynamic_cast<ScriptedInstance*>(pPlayer->GetInstanceData());
    if (!m_pInstance)
        return false;

    std::bitset<32> tpData = m_pInstance->GetData(DATA_TP_UNLOCKED);

    switch (pAt->id)
    {
        case 5736:  // Light's Hammer
            if (!tpData[TP_LIGHTS_HAMMER])
            {
                tpData[TP_LIGHTS_HAMMER] = true;
                m_pInstance->SetData(DATA_TP_UNLOCKED, tpData.to_ulong());
                return true;
            }
            break;
        case 5649:  // Upper Spire
            // trigger enabled only if Saurfang is dead + someone actually gets there
            if (!tpData[TP_UPPER_SPIRE] && m_pInstance->GetData(TYPE_SAURFANG) == DONE)
            {
                tpData[TP_UPPER_SPIRE] = true;
                m_pInstance->SetData(DATA_TP_UNLOCKED, tpData.to_ulong());
                pPlayer->SummonCreature(NPC_CROK_SCOURGEBANE, 4199.25f,  2769.26f, 351.06f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 25*IN_MILLISECONDS);
                return true;
            }
            break;
        case 5604:  // Sindragosa's Lair
            if (!tpData[TP_SINDRAGOSA_LAIR] && m_pInstance->GetData(TYPE_VALITHRIA) == DONE)
            {
                tpData[TP_SINDRAGOSA_LAIR] = true;
                m_pInstance->SetData(DATA_TP_UNLOCKED, tpData.to_ulong());
                return true;
            }
            break;
        case 5718:  // LK Teleporter
            if (!tpData[TP_FROZEN_THRONE])
            {
                if (m_pInstance->GetData(TYPE_SAURFANG) == DONE &&
                    m_pInstance->GetData(TYPE_PUTRICIDE) == DONE &&
                    m_pInstance->GetData(TYPE_LANATHEL) == DONE &&
                    m_pInstance->GetData(TYPE_SINDRAGOSA) == DONE)
                {
                    tpData[TP_FROZEN_THRONE] = true;
                    m_pInstance->SetData(DATA_TP_UNLOCKED, tpData.to_ulong());
                    pPlayer->CastSpell(pPlayer, TeleportSpells[TP_FROZEN_THRONE], false);
                }
            }
            else
                pPlayer->CastSpell(pPlayer, TeleportSpells[TP_FROZEN_THRONE], false);
            break;
        default:
            break;
    }
    return false;
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
