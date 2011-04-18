/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

enum
{
    MAX_ENCOUNTER           = 4,

    TYPE_DRAKOS             = 0,
    TYPE_VAROS              = 1,
    TYPE_UROM               = 2,
    TYPE_EREGOS             = 3,

    DATA_ROBOTS,
    DATA_SPOTLIGHT,
    DATA_CACHE_OF_EREGOS,
    DATA_ETERNOS,
    DATA_VERDISA,
    DATA_BELGARISTRASZ,
    DATA_RUBY_VOID,
    DATA_EMERALD_VOID,
    DATA_AMBER_VOID,

    NPC_ROBOT           = 27641,
    NPC_BALGAR_IMAGE    = 28012,
    NPC_DRAKOS          = 27654,
    NPC_VAROS           = 27447,
    NPC_UROM            = 27655,
    NPC_EREGOS          = 27656,

    NPC_RUBY_DRAKE      = 27756,
    NPC_EMERALD_DRAKE   = 27692,
    NPC_AMBER_DRAKE     = 27755,

    NPC_ETERNOS         = 27659,
    NPC_VERDISA         = 27657,
    NPC_BALGARISTRASZ   = 27658,

    GO_DRAGON_CAGE_DOOR = 193995,
    GO_EREGOS_CACHE     = 191349,
    GO_EREGOS_CACHE_H   = 193603,
    GO_SPOTLIGHT        = 191351,

    BELGAR_TEXT_0       = 13267,
    BELGAR_TEXT_1       = 13268,
    BELGAR_TEXT_2       = 13269,

    WORLD_STATE_ROBOTS          = 3524,
    WORLD_STATE_ROBOTS_COUNT    = 3486,

    MESSAGE_DRAKOS      = 1000,
    MESSAGE_VAROS       = 1001,
    MESSAGE_UROM        = 1002,

    CRITERIA_MAKE_IT_COUNT          = 7145,
    MAKE_IT_COUNT_START             = 18153,

    // individual per-player check
    CRITERIA_DRAKE_RIDER_RUBY       = 7179,
    CRITERIA_DRAKE_RIDER_EMERALD    = 7178,
    CRITERIA_DRAKE_RIDER_AMBER      = 7177,

    // raidwide check
    CRITERIA_RUBY_VOID              = 7323,
    CRITERIA_EMERALD_VOID           = 7324,
    CRITERIA_AMBER_VOID             = 7325,
};
