/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_TOC_H
#define DEF_TOC_H

#define DIFFICULTY(SP)  (m_bIsRegularMode ? SP : SP##_H)
#define GET_CREATURE(C) (m_pInstance ? m_creature->GetMap()->GetCreature(m_pInstance->GetData64(C)) : NULL)

enum
{
    MAX_ENCOUNTER               = 3,
    MAX_CHAMPIONS               = 3,
    MAX_MEMORY                  = 25,

    TYPE_GRAND_CHAMPIONS        = 0,
    TYPE_ARGENT_CHALLENGE       = 1,
    TYPE_BLACK_KNIGHT           = 2,

    DATA_CHAMPION_1             = 3,
    DATA_CHAMPION_2             = 4,
    DATA_CHAMPION_3             = 5,
    DATA_BLACK_KNIGHT           = 6,
    DATA_BLACK_KNIGHT_MINION    = 7,
    DATA_TOC5_ANNOUNCER         = 8,
    DATA_JAEREN                 = 9,
    DATA_ARELAS                 = 10,
    DATA_CHAMPIONID_1           = 11,
    DATA_CHAMPIONID_2           = 12,
    DATA_CHAMPIONID_3           = 13,
    DATA_MEMORY                 = 14,
    DATA_ARGENT_CHALLENGER      = 15,

    DATA_CHAMPION_BEGIN         = DATA_CHAMPION_1,
    DATA_CHAMPION_END           = DATA_CHAMPION_3,

    NPC_JACOB                   = 34705,
    NPC_AMBROSE                 = 34702,
    NPC_COLOSOS                 = 34701,
    NPC_JAELYNE                 = 34657,
    NPC_LANA                    = 34703,
    NPC_MOKRA                   = 35572,
    NPC_ERESSEA                 = 35569,
    NPC_RUNOK                   = 35571,
    NPC_ZULTORE                 = 35570,
    NPC_VISCERI                 = 35617,
    NPC_EADRIC                  = 35119,
    NPC_PALETRESS               = 34928,
    NPC_BLACK_KNIGHT            = 35451,
    NPC_RISEN_JAEREN            = 35545,
    NPC_RISEN_ARELAS            = 35564,
    NPC_JAEREN                  = 35004,
    NPC_ARELAS                  = 35005,

    MEMORY_ALGALON              = 35052,
    MEMORY_ARCHIMONDE           = 35041,
    MEMORY_CHROMAGGUS           = 35033,
    MEMORY_CYANIGOSA            = 35046,
    MEMORY_DELRISSA             = 35043,
    MEMORY_ECK                  = 35047,
    MEMORY_ENTROPIUS            = 35044,
    MEMORY_GRUUL                = 35039,
    MEMORY_HAKKAR               = 35034,
    MEMORY_HEIGAN               = 35049,
    MEMORY_HEROD                = 35030,
    MEMORY_HOGGER               = 34942,
    MEMORY_IGNIS                = 35050,
    MEMORY_ILLIDAN              = 35042,
    MEMORY_INGVAR               = 35045,
    MEMORY_KALITHRESH           = 35037,
    MEMORY_LUCIFRON             = 35031,
    MEMORY_MALCHEZAAR           = 35038,
    MEMORY_MUTANUS              = 35029,
    MEMORY_ONYXIA               = 35048,
    MEMORY_THUNDERAAN           = 35032,
    MEMORY_VANCLEEF             = 35028,
    MEMORY_VASHJ                = 35040,
    MEMORY_VEKNILASH            = 35036,
    MEMORY_VEZAX                = 35051,

    GO_CHAMPIONS_LOOT           = 195709,
    GO_CHAMPIONS_LOOT_H         = 195710,
    GO_EADRIC_LOOT              = 195374,
    GO_EADRIC_LOOT_H            = 195375,
    GO_PALETRESS_LOOT           = 195323,
    GO_PALETRESS_LOOT_H         = 195324
};

// base class for all ToC5 bosses
struct MANGOS_DLL_DECL boss_trial_of_the_championAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;

    boss_trial_of_the_championAI(Creature *pCreature);

    Player* SelectRandomPlayer();
};

// helper for the script register process
template <typename T>
CreatureAI* toc5_GetAI(Creature *pCreature)
{
    return new T(pCreature);
}

#define REGISTER_SCRIPT(SC) \
    NewScript = new Script; \
    NewScript->Name = #SC; \
    NewScript->GetAI = &toc5_GetAI<SC##AI>; \
    NewScript->RegisterSelf();

#endif
