/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ULDUAR_H
#define DEF_ULDUAR_H

#define HEROIC(N, H) (m_bIsRegularMode ? N : H)
#define GET_CREATURE(C) (m_pInstance ? m_creature->GetMap()->GetCreature(m_pInstance->GetData64(C)) : NULL)

enum
{
    MAX_ENCOUNTER               = 14,

    TYPE_LEVIATHAN              = 0,
    TYPE_IGNIS                  = 1,
    TYPE_RAZORSCALE             = 2,
    TYPE_XT002                  = 3,
    TYPE_ASSEMBLY               = 4,
    TYPE_KOLOGARN               = 5,
    TYPE_AURIAYA                = 6,
    TYPE_MIMIRON                = 7,
    TYPE_HODIR                  = 8,
    TYPE_THORIM                 = 9,
    TYPE_FREYA                  = 10,
    TYPE_VEZAX                  = 11,
    TYPE_YOGGSARON              = 12,
    TYPE_ALGALON                = 13,

    DATA_STEELBREAKER           = 20,
    DATA_MOLGEIM                = 21,
    DATA_BRUNDIR                = 22,
    DATA_BRAIN_OF_YOGGSARON     = 23,
    DATA_SARA                   = 24,
	DATA_ILLUSION               = 25,
    DATA_LEFT_ARM               = 30,
    DATA_RIGHT_ARM              = 31,
    DATA_SANCTUM_SENTRY         = 40,
    DATA_SANCTUM_SENTRY2        = 41,
    DATA_SANCTUM_SENTRY3        = 42,
    DATA_SANCTUM_SENTRY4        = 43,
    DATA_KOLOGARN_CHEST         = 50,
    DATA_FREYA_CHEST            = 51,
    DATA_HODIR_CHEST            = 52,
    DATA_HODIR_CHEST_HARD       = 53,
    DATA_MIMIRON_CHEST          = 54,
    DATA_THORIM_CHEST           = 55,
    DATA_ALGALON_CHEST          = 56,
    DATA_ASSEMBLY_DOOR          = 60,
    DATA_HODIR_DOOR             = 61,
    DATA_VEZAX_DOOR             = 62,
    DATA_BOSS_ENGAGED           = 70,
    DATA_BOSS_DONE              = 71,
    DATA_GUARDIANS_CHOSEN       = 72,

    NPC_LEVIATHAN               = 33113,
    NPC_IGNIS                   = 33118,
    NPC_RAZORSCALE              = 33186,
    NPC_XT002                   = 33293,
    NPC_STEELBREAKER            = 32867,
    NPC_MOLGEIM                 = 32927,
    NPC_BRUNDIR                 = 32857,
    NPC_KOLOGARN                = 32930,
    NPC_LEFT_ARM                = 32933,
    NPC_RIGHT_ARM               = 32934,
    NPC_AURIAYA                 = 33515,
    NPC_SANCTUM_SENTRY          = 34014,
    NPC_MIMIRON                 = 33350,
    NPC_LEVIATHAN_MKII          = 33432,    //mimiron miniboss
    NPC_VX001                   = 33651,    //mimiron miniboss
    NPC_AERIAL_COMMAND_UNIT     = 33670,    //mimiron miniboss
    NPC_HODIR                   = 32845,
    NPC_THORIM                  = 32865,
    NPC_FREYA                   = 32906,
    NPC_VEZAX                   = 33271,
    NPC_YOGGSARON               = 33288,
    NPC_BRAIN_OF_YOGGSARON      = 33890,
    NPC_SARA                    = 33134,
    NPC_ALGALON                 = 32871,

    GO_KOLOGARN_CHEST           = 195046,
    GO_KOLOGARN_CHEST_H         = 195047,
    GO_FREYA_CHEST              = 194324,
    GO_FREYA_CHEST_H            = 194328,
    GO_HODIR_CHEST              = 194307,
    GO_HODIR_CHEST_H            = 194308,
    GO_HODIR_CHEST_HARD         = 194200,
    GO_HODIR_CHEST_HARD_H       = 194201,
    GO_MIMIRON_CHEST            = 194789,
    GO_MIMIRON_CHEST_H          = 194956,
    GO_THORIM_CHEST             = 194312,
    GO_THORIM_CHEST_H           = 194314,
    GO_ALGALON_CHEST            = 194822,
    GO_ALGALON_CHEST_H          = 194821,
    GO_ASSEMBLY_DOOR            = 194556,
    GO_HODIR_DOOR               = 194441,
    GO_VEZAX_DOOR               = 194750
};

// check if boss has been pulled too far from its place
bool IsOutOfCombatArea(Creature*);

#endif
