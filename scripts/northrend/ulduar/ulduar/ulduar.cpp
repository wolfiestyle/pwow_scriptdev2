/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: ulduar
SD%Complete: 0%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

/* TODO: merge this, maybe move teleporter code here */
// Teleporter Gossip handled by SD2 because depending on Instance Data
enum TeleporterGossipItems
{
    GOSSIP_ITEM_TELE_BASE_CAMP              = -3603000,
    GOSSIP_ITEM_TELE_FORMATION_GROUNDS      = -3603001,
    GOSSIP_ITEM_TELE_COLOSSAR_FORGE         = -3603002,
    GOSSIP_ITEM_TELE_SCRAPYARD              = -3603003,
    GOSSIP_ITEM_TELE_ANTECHAMBER            = -3603004,
    GOSSIP_ITEM_TELE_WALKWAY                = -3603005,
    GOSSIP_ITEM_TELE_CONSERVATORY           = -3603006,
    GOSSIP_ITEM_TELE_SPARK_IMAGINATION      = -3603007,
    GOSSIP_ITEM_TELE_YOGG_SARON             = -3603008,
};

bool IsOutOfCombatArea(Creature *pCreature)
{
    if (pCreature->GetMapId() != 603)   // invalid check outside Ulduar
        return false;

    float z = pCreature->GetPositionZ();
    uint32 areaId = pCreature->GetAreaId();

    switch (pCreature->GetEntry())
    {
        case NPC_LEVIATHAN:
            return areaId != 4652;  // Formation Grounds
        case NPC_IGNIS:
            return areaId != 4654;  // The Colossal Forge
        case NPC_RAZORSCALE:
            return areaId != 4653;  // Razorscale's Aerie
        case NPC_XT002:
            return areaId != 4655;  // The Scrapyard
        case NPC_STEELBREAKER:
        case NPC_MOLGEIM:
        case NPC_BRUNDIR:
            return !pCreature->IsWithinDist2d(1587.6f, 119.8f, 90.0f);
        case NPC_KOLOGARN:
        case NPC_LEFT_ARM:
        case NPC_RIGHT_ARM:
            return !pCreature->IsWithinDist2d(1799.6f, -24.3f, 70.0f);
        case NPC_AURIAYA:
            return z < 400.0f || !pCreature->IsWithinDist2d(1981.5f, -28.5f, 120.0f);
        case NPC_MIMIRON:
        case NPC_LEVIATHAN_MKII:
        case NPC_VX001:
        case NPC_AERIAL_COMMAND_UNIT:
            return !pCreature->IsWithinDist2d(2744.1f, 2569.1f, 100.0f);
        case NPC_HODIR:
            return !pCreature->IsWithinDist2d(2000.7f, -233.7f, 70.0f);
        case NPC_THORIM:
            return !pCreature->IsWithinDist2d(2135.0f, -303.8f, 90.0f);
        case NPC_FREYA:
            return areaId != 4656;  // The Conservatory of Life
        case NPC_VEZAX:
            return !pCreature->IsWithinDist2d(1841.1f, 133.7f, 90.0f);
        case NPC_YOGGSARON:
            return z > 400.0f || !pCreature->IsWithinDist2d(1981.5f, -28.5f, 100.0f);
        case NPC_ALGALON:
            return !pCreature->IsWithinDist2d(1632.9f, -307.9f, 90.0f);
        default:
            return false;
    }
}

void AddSC_ulduar()
{
}
