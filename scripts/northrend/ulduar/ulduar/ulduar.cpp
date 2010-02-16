#include "precompiled.h"
#include "ulduar.h"

bool IsOutOfCombatArea(Creature *pCreature)
{
    float x, y, z;
    pCreature->GetPosition(x, y, z);
    uint32 areaId = pCreature->GetMap()->GetAreaId(x, y, z);

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
