/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_OBSIDIAN_SANCTUM_H
#define DEF_OBSIDIAN_SANCTUM_H

#define GET_CREATURE(C) (m_pInstance ? m_creature->GetMap()->GetCreature(m_pInstance->GetData64(C)) : NULL)
#define DIFFICULTY(SP)  (m_bIsRegularMode ? SP : SP##_H)

enum
{
    MAX_ENCOUNTER               = 1,

    TYPE_SARTHARION_EVENT       = 1,

    DATA_SARTHARION             = 10,
    DATA_TENEBRON               = 11,
    DATA_SHADRON                = 12,
    DATA_VESPERON               = 13,
    DATA_DRAKES                 = 14,

    NPC_SARTHARION              = 28860,
    NPC_TENEBRON                = 30452,
    NPC_SHADRON                 = 30451,
    NPC_VESPERON                = 30449,
    GO_TWILIGHT_PORTAL          = 193988,

    // Achievement criterias
    CRIT_TWILIGHT_ASSIST_N      = 7328,  
    CRIT_TWILIGHT_ASSIST_H      = 7331,
    CRIT_TWILIGHT_DUO_N         = 7329,
    CRTT_TWILIGHT_DUO_H         = 7332,
    CRIT_TWILIGHT_ZONE_N        = 7330,
    CRIT_TWILIGHT_ZONE_H        = 7333,
};

//---------------------------------------------------------

// helper for the script register process
template <typename T>
CreatureAI* os_GetAI(Creature *pCreature)
{
    return new T(pCreature);
}

#define REGISTER_SCRIPT(SC) \
    newscript = new Script; \
    newscript->Name = #SC; \
    newscript->GetAI = &os_GetAI<SC##AI>; \
    newscript->RegisterSelf();

#endif
