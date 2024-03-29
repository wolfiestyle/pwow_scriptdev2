/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ICECROWN_CITADEL_H
#define DEF_ICECROWN_CITADEL_H

#define GET_CREATURE(C)     (InstanceVar<uint64>(C, m_pInstance).getObject<Creature>())
#define GET_GAMEOBJECT(G)   (InstanceVar<uint64>(G, m_pInstance).getObject<GameObject>())

#define DIFF_SELECT(N10, N25, H10, H25) (m_bIs10Man ? (m_bIsHeroic ? H10 : N10) : (m_bIsHeroic ? H25 : N25))

// schedules a periodic event
#define SCHEDULE_EVENT(EV, ...)   Events.ScheduleEvent(EVENT_##EV, TIMER_##EV, TIMER_##EV, ## __VA_ARGS__)
#define SCHEDULE_EVENT_R(EV, ...) Events.ScheduleEventInRange(EVENT_##EV, TIMER_##EV, TIMER_##EV, ## __VA_ARGS__)

#define IS_HORDE            (m_pInstance ? m_pInstance->GetData(DATA_FACTION) == HORDE : false)

enum
{
    MAX_ENCOUNTER           = 15,

    // The Lower Spire
    TYPE_MARROWGAR          = 0,
    TYPE_DEATHWHISPER       = 1,
    TYPE_GUNSHIP_BATTLE     = 2,
    TYPE_SAURFANG           = 3,
    // The Plagueworks
    TYPE_FESTERGUT          = 4,
    TYPE_ROTFACE            = 5,
    TYPE_PUTRICIDE          = 6,
    // The Crimson Hall
    TYPE_VALANAR            = 7,
    TYPE_KELESETH           = 8,
    TYPE_TALDARAM           = 9,
    TYPE_LANATHEL           = 10,
    // The Frostwing Halls
    TYPE_SVALNA             = 11,
    TYPE_VALITHRIA          = 12,
    TYPE_SINDRAGOSA         = 13,
    // The Frozen Throne
    TYPE_LICH_KING          = 14,

    // instance objects
    DATA_MARROWGAR_DOOR_ENTRANCE,
    DATA_MARROWGAR_DOOR_1,
    DATA_MARROWGAR_DOOR_2,
    DATA_DEATHWHISPER_ELEV,
    DATA_SAURFANG_CHEST,
    DATA_SAURFANG_DOOR,
    DATA_FROSTWING_DOOR_ENTRANCE,
    DATA_VALITHRIA_DOOR_ENTRANCE,
    DATA_VALITHRIA_DOOR_LEFT_1,
    DATA_VALITHRIA_DOOR_LEFT_2,
    DATA_VALITHRIA_DOOR_RIGHT_1,
    DATA_VALITHRIA_DOOR_RIGHT_2,
    DATA_VALITHRIA_DOOR_EXIT,
    DATA_VALITHRIA_CHEST,
    DATA_GREEN_TUBES,
    DATA_ORANGE_TUBES,
    DATA_ROTFACE_DOOR,
    DATA_FESTERGUT_DOOR,
    DATA_PUTRICIDE_UPPER_DOOR,
    DATA_PUTRICIDE_LOWER_DOOR,
    DATA_PUTRICIDE_LOWER_DOOR_ORANGE,
    DATA_PUTRICIDE_LOWER_DOOR_GREEN,
    DATA_BLOODWING_DOOR,
    DATA_BLOOD_PRINCE_DOOR,
    DATA_LANATHEL_DOOR_LEFT,
    DATA_LANATHEL_DOOR_RIGHT,
    DATA_SINDRAGOSA_DOOR,
    DATA_SINDRAGOSA_EXIT_DOOR,
    DATA_SINDRAGOSA_EXIT_DOOR2,
    DATA_LICHKING_PLATFORM,
    DATA_LICHKING_PRECIPICE,    // DNT Background
    DATA_LICHKING_SPIKE1,
    DATA_LICHKING_SPIKE2,
    DATA_LICHKING_SPIKE3,
    DATA_LICHKING_SPIKE4,
    DATA_LICHKING_EDGE_WARNING,
    DATA_LICHKING_FROSTY_WIND,
    DATA_LICHKING_FROSTY_EDGE,

    // total count of guid values stored
    DATA_GUID_MAX,

    // instance variables
    DATA_FACTION = 100,
    DATA_TP_UNLOCKED,
    DATA_ACHIEVEMENT_COUNTER_SAURFANG,
    DATA_ACHIEVEMENT_CHECK_VALITHRIA,
    DATA_ACHIEVEMENT_CHECK_MARROWGAR,

    // creature entries
    NPC_MARROWGAR           = 36612,
    NPC_DEATHWHISPER        = 36855,
    NPC_SAURFANG            = 37813,
    NPC_FESTERGUT           = 36626,
    NPC_ROTFACE             = 36627,
    NPC_PUTRICIDE           = 36678,
    NPC_VALANAR             = 37970,
    NPC_KELESETH            = 37972,
    NPC_TALDARAM            = 37973,
    NPC_LANATHEL            = 37955,
    NPC_SVALNA              = 37126,
    NPC_VALITHRIA           = 36789,
    NPC_SINDRAGOSA          = 36853,
    NPC_LICH_KING           = 36597,
    NPC_GRONDEL             = 37124,
    NPC_RUPERT              = 37125,
    NPC_ARNATH              = 37122,
    NPC_BRANDON             = 37123,
    NPC_CROK_SCOURGEBANE    = 37129,
    NPC_LANATHEL_FAKE       = 38004,

    // gameobject entries
    GO_MARROWGAR_ENTRANCE   = 201857,
    GO_MARROWGAR_DOOR_1     = 201911,
    GO_MARROWGAR_DOOR_2     = 201910,
    GO_DEATHWHISPER_ELEV    = 202220,
    GO_SAURFANG_CHEST_N10   = 202239,
    GO_SAURFANG_CHEST_N25   = 202240,
    GO_SAURFANG_CHEST_H10   = 202238,
    GO_SAURFANG_CHEST_H25   = 202241,
    GO_SAURFANG_DOOR        = 201825,
    GO_FROSTWING_DOOR_ENTRANCE = 201919,
    GO_VALITHRIA_DOOR_ENTRANCE = 201375,
    GO_VALITHRIA_DOOR_EXIT  = 201374,
    GO_VALITHRIA_DOOR_L1    = 201381,
    GO_VALITHRIA_DOOR_L2    = 201380,
    GO_VALITHRIA_DOOR_R1    = 201382,
    GO_VALITHRIA_DOOR_R2    = 201383,
    GO_VALITHRIA_CHEST_N10  = 201959,
    GO_VALITHRIA_CHEST_N25  = 202339,
    GO_VALITHRIA_CHEST_H10  = 202338,
    GO_VALITHRIA_CHEST_H25  = 202340,
    GO_GREEN_TUBES          = 201618,
    GO_ORANGE_TUBES         = 201617,
    GO_ROTFACE_DOOR         = 201370,
    GO_FESTERGUT_DOOR       = 201371,
    GO_PUTRICIDE_UPPER_DOOR = 201372,
    GO_PUTRICIDE_LOWER_DOOR = 201612,
    GO_PUTRICIDE_LOWER_DOOR_ORANGE  = 201613,
    GO_PUTRICIDE_LOWER_DOOR_GREEN   = 201614,
    GO_BLOODWING_DOOR       = 201920,
    GO_BLOOD_PRINCE_DOOR    = 201376,
    GO_LANATHEL_DOOR_LEFT   = 201378,
    GO_LANATHEL_DOOR_RIGHT  = 201377,
    GO_SINDRAGOSA_DOOR      = 201373,
    GO_SINDRAGOSA_EXIT_DOOR = 201369,
    GO_SINDRAGOSA_EXIT_DOOR2 = 201379,
    GO_LICHKING_PLATFORM    = 202161,
    GO_LICHKING_PRECIPICE   = 202078,
    GO_LICHKING_SPIKE1      = 202141,
    GO_LICHKING_SPIKE2      = 202142,
    GO_LICHKING_SPIKE3      = 202143,
    GO_LICHKING_SPIKE4      = 202144,
    GO_LICHKING_EDGE_WARNING = 202190,
    GO_LICHKING_FROSTY_WIND = 202188,
    GO_LICHKING_FROSTY_EDGE = 202189,
    GO_TP_LIGHTS_HAMMER     = 202242,
    GO_TP_ORATORY           = 202243,
    GO_TP_RAMPART_OF_SKULLS = 202244,
    GO_TP_DEATHBRINGER_RISE = 202245,
    GO_TP_UPPER_SPIRE       = 202235,
    GO_TP_SINDRAGOSA_LAIR   = 202246,
    GO_TP_FROZEN_THRONE     = 202223,

    FACTION_NEUTRAL         = 7,
    FACTION_HOSTILE         = 14,
    FACTION_FRIENDLY        = 35,

    // achievement criteria IDs
    CRITERIA_SAURFANG_MESS_10   = 12778,
    CRITERIA_SAURFANG_MESS_10H  = 13035,
    CRITERIA_SAURFANG_MESS_25   = 13036,
    CRITERIA_SAURFANG_MESS_25H  = 13037,

    CRITERIA_SAURFANG_LK_KILLS_10 = 13334,
    CRITERIA_SAURFANG_LK_KILLS_25 = 13354,

    CRITERIA_PORTAL_JOKEY_10    = 12978,
    CRITERIA_PORTAL_JOKEY_10H   = 12979,
    CRITERIA_PORTAL_JOKEY_25    = 12980,
    CRITERIA_PORTAL_JOKEY_25H   = 12971,

    CRITERIA_VALITHRIA_LK_KILLS_10  = 13335,
    CRITERIA_VALITHRIA_LK_KILLS_25  = 13355,

    CRITERIA_MARROWGAR_BONED_10     = 12775,
    CRITERIA_MARROWGAR_BONED_10H    = 13393,
    CRITERIA_MARROWGAR_BONED_25     = 12962,
    CRITERIA_MARROWGAR_BONED_25H    = 13394,

    // shared between bosses
    MESSAGE_PUTRICIDE           = 1000,
};

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

enum EncounterFrameType
{
    ENCOUNTER_FRAME_ADD     = 0,
    ENCOUNTER_FRAME_REMOVE  = 1,
    ENCOUNTER_FRAME_UNK2    = 2,
    ENCOUNTER_FRAME_UNK3    = 3,
    ENCOUNTER_FRAME_UNK4    = 4,
    ENCOUNTER_FRAME_UNK5    = 5,
    ENCOUNTER_FRAME_UNK6    = 6,
    ENCOUNTER_FRAME_UNK7    = 7,
};

//---------------------------------------------------------

namespace icc {

// helper for the script register process
template <typename T>
CreatureAI* GetAI(Creature *pCreature)
{
    return new T(pCreature);
}

#define REGISTER_SCRIPT(SC) \
    newscript = new Script; \
    newscript->Name = #SC; \
    newscript->GetAI = &icc::GetAI<SC##AI>; \
    newscript->RegisterSelf();

// get data_id from Creature or GameObject
uint32 GetType(Creature*);
uint32 GetType(GameObject*);

// check if group killed all previous bosses in progression order
bool MeetsRequirementsForBoss(InstanceData* instance, uint32 boss_id);

} // namespace icc

// basic start point for bosses
struct MANGOS_DLL_DECL boss_icecrown_citadelAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance *m_pInstance;
    Difficulty m_Difficulty;
    bool m_bIsHeroic :1;
    bool m_bIs10Man :1;
    InstanceVar<uint32> m_BossEncounter;

    boss_icecrown_citadelAI(Creature*);

    void Reset();

    // check if boss has been pulled too far from its place
    bool IsOutOfCombatArea() const;
    // resets boss if pulled out of combat area
    bool OutOfCombatAreaCheck();
    // prevents combat with boss if group doesn't meet requirements
    bool InstanceProgressionCheck();

    // remove auras from all players in map
    // if spellId is negative, removes with spell difficulty
    void RemoveEncounterAuras(int32 spellId_1, int32 spellId_2 = 0, int32 spellId_3 = 0, int32 spellId_4 = 0);

    // for sending boss unit frames
    void SendEncounterUnit(EncounterFrameType type, uint8 param1 = 0, uint8 param2 = 0);
};

#endif // DEF_ICECROWN_CITADEL_H
