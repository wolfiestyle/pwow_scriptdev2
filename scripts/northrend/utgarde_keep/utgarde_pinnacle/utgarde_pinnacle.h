/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_UTGARDE_PINNACLE_H
#define DEF_UTGARDE_PINNACLE_H

enum
{
    MAX_ENCOUNTER                   = 4,

    TYPE_SVALA                      = 0,
    TYPE_GORTOK                     = 1,
    TYPE_SKADI                      = 2,
    TYPE_YMIRON                     = 3,

    DATA_ACHIEVEMENT_SVALA          = 4,
    DATA_ACHIEVEMENT_SKADI_MY_GIRL  = 5,

    DATA_SKADI                      = 12,
    DATA_GRAUF                      = 13,

    GO_STASIS_GENERATOR             = 188593,
    GO_DOOR_SKADI                   = 192173,

    NPC_FURBOLG                     = 26684,
    NPC_WORGEN                      = 26683,
    NPC_JORMUNGAR                   = 26685,
    NPC_RHINO                       = 26686,
    NPC_GORTOK                      = 26687,

    NPC_GRAUF                       = 26893,
    NPC_SKADI                       = 26693,

    // All these criterias are defined as CRITERIA_REQUIREMENT_INSTANCE_SCRIPT (18)
    CRITERIA_THE_INCREDIBLE_HULK    = 7322,
    CRITERIA_MY_GIRL                = 7595,

    TIMED_ACHIEVEMENT_EVENT_SKADI   = 17726,                // DBC value
};

#endif
