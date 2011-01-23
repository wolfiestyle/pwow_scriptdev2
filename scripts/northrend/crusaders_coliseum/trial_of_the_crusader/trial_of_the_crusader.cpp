#include "precompiled.h"
#include "trial_of_the_crusader.h"

/* TODO: merge this (actually used in npc_barrett_ramsey)
enum
{
    SAY_TIRION_RAID_INTRO_LONG          = -1649000,
    SAY_RAID_TRIALS_INTRO               = -1649001,
    // Northrend Beasts
    SAY_TIRION_BEAST_1                  = -1649002,
    SAY_VARIAN_BEAST_1                  = -1649003,
    SAY_GARROSH_BEAST_1                 = -1649004,
    SAY_TIRION_BEAST_2                  = -1649005,
    SAY_TIRION_BEAST_3                  = -1649006,
    SAY_TIRION_BEAST_SLAY               = -1649007,
    SAY_TIRION_BEAST_WIPE               = -1649008,

    // Jaraxxus Encounter
    SAY_TIRION_JARAXXUS_INTRO_1         = -1649009,
    SAY_WILFRED_JARAXXUS_INTRO_1        = -1649010,
    SAY_WILFRED_JARAXXUS_INTRO_2        = -1649011,
    SAY_WILFRED_JARAXXUS_INTRO_3        = -1649012,
    SAY_JARAXXUS_JARAXXAS_INTRO_1       = -1649013,
    SAY_WILFRED_DEATH                   = -1649014,
    SAY_TIRION_JARAXXUS_INTRO_2         = -1649015,
    SAY_TIRION_JARAXXUS_EXIT_1          = -1649016,
    SAY_GARROSH_JARAXXUS_EXIT_1         = -1649017,
    SAY_VARIAN_JARAXXUS_SLAY            = -1649018,
    SAY_TIRION_JARAXXUS_EXIT_2          = -1649019,

    // Faction-Champions
    SAY_TIRION_PVP_INTRO_1              = -1649020,
    SAY_GARROSH_PVP_A_INTRO_1           = -1649021,
    SAY_VARIAN_PVP_H_INTRO_1            = -1649022,
    SAY_TIRION_PVP_INTRO_2              = -1649023,
    SAY_VARIAN_PVP_A_INTRO_2            = -1649024,
    SAY_GARROSH_PVP_H_INTRO_2           = -1649025,
    SAY_VARIAN_PVP_A_WIN                = -1649026,
    SAY_GARROSH_PVP_H_WIN               = -1649027,
    SAY_TIRION_PVP_WIN                  = -1649028,

    // Twin Valkyrs
    SAY_TIRION_TWINS_INTRO              = -1649029,
    SAY_RAID_INTRO_SHORT                = -1649030,
    SAY_VARIAN_TWINS_A_WIN              = -1649031,
    SAY_GARROSH_TWINS_H_WIN             = -1649032,
    SAY_TIRION_TWINS_WIN                = -1649033,

    // Anub'Arak Encounter
    SAY_LKING_ANUB_INTRO_1              = -1649034,
    SAY_TIRION_ABUN_INTRO_1             = -1649035,
    SAY_LKING_ANUB_INTRO_2              = -1649036,
    SAY_LKING_ANUB_INTRO_3              = -1649037,
    SAY_ANUB_ANUB_INTRO_1               = -1649038,

    NPC_GORMOK                          = 34796,
    NPC_JARAXXUS                        = 34780,

    GOSSIP_ITEM_START_EVENT1            = -3649000
};
*/

typedef UNORDERED_MAP<uint32 /*entry*/, uint32 /*data_id*/> EntryTypeMap;

static EntryTypeMap const CreatureEntryToType = map_initializer<EntryTypeMap>
    (NPC_GORMOK,                TYPE_GORMOK)
    (NPC_ACIDMAW,               TYPE_ACIDMAW)
    (NPC_DREADSCALE,            TYPE_DREADSCALE)
    (NPC_ICEHOWL,               TYPE_ICEHOWL)
    (NPC_JARAXXUS,              TYPE_JARAXXUS)
    (NPC_TYRIUS_DUSKBLADE,      TYPE_TYRIUS_DUSKBLADE)
    (NPC_KAVINA_GROVESONG,      TYPE_KAVINA_GROVESONG)
    (NPC_MELADOR_VALESTRIDER,   TYPE_MELADOR_VALESTRIDER)
    (NPC_ALYSSIA_MOONSTALKER,   TYPE_ALYSSIA_MOONSTALKER)
    (NPC_NOOZLE_WHIZZLESTICK,   TYPE_NOOZLE_WHIZZLESTICK)
    (NPC_VELNAA,                TYPE_VELNAA)
    (NPC_BAELNOR_LIGHTBEARER,   TYPE_BAELNOR_LIGHTBEARER)
    (NPC_ANTHAR_FORGEMENDER,    TYPE_ANTHAR_FORGEMENDER)
    (NPC_BRIENNA_NIGHTFELL,     TYPE_BRIENNA_NIGHTFELL)
    (NPC_IRIETH_SHADOWSTEP,     TYPE_IRIETH_SHADOWSTEP)
    (NPC_SHAABAD,               TYPE_SHAABAD)
    (NPC_SAAMUL,                TYPE_SAAMUL)
    (NPC_SERISSA_GRIMDABBLER,   TYPE_SERISSA_GRIMDABBLER)
    (NPC_SHOCUUL,               TYPE_SHOCUUL)
    (NPC_GORGRIM_SHADOWCLEAVE,  TYPE_GORGRIM_SHADOWCLEAVE)
    (NPC_BIRANA_STORMHOOF,      TYPE_BIRANA_STORMHOOF)
    (NPC_ERIN_MISTHOOF,         TYPE_ERIN_MISTHOOF)
    (NPC_RUJKAH,                TYPE_RUJKAH)
    (NPC_GINSELLE_BLIGHTSLINGER,TYPE_GINSELLE_BLIGHTSLINGER)
    (NPC_LIANDRA_SUNCALLER,     TYPE_LIANDRA_SUNCALLER)
    (NPC_MALITHAS_BRIGHTBLADE,  TYPE_MALITHAS_BRIGHTBLADE)
    (NPC_CAIPHUS_THE_STERN,     TYPE_CAIPHUS_THE_STERN)
    (NPC_VIVIENNE_BLACKWHISPER, TYPE_VIVIENNE_BLACKWHISPER)
    (NPC_MAZDINAH,              TYPE_MAZDINAH)
    (NPC_BROLN_STOUTHORN,       TYPE_BROLN_STOUTHORN)
    (NPC_THRAKGAR,              TYPE_THRAKGAR)
    (NPC_HARKZOG,               TYPE_HARKZOG)
    (NPC_NARRHOK_STEELBREAKER,  TYPE_NARRHOK_STEELBREAKER)
    (NPC_FJOLA_LIGHTBANE,       TYPE_FJOLA_LIGHTBANE)
    (NPC_EYDIS_DARKBANE,        TYPE_EYDIS_DARKBANE)
    (NPC_ANUBARAK,              TYPE_ANUBARAK)
    (NPC_BARRETT_RAMSEY,        TYPE_BARRETT_RAMSEY)
    (NPC_VARIAN_WYRM,           TYPE_VARIAN_WYRM)
    (NPC_GARROSH_HELLSCREAM,    TYPE_GARROSH_HELLSCREAM)
    (NPC_TIRION_FORDRING,       TYPE_TIRION_FORDRING)
    (NPC_LICH_KING,             TYPE_LICH_KING)
    (NPC_WILFRED_FIZZLEBANG,    TYPE_FIZZLEBANG)
    (NPC_PURPLE_RUNE,           TYPE_PURPLE_RUNE)
    (NPC_PORTAL_TARGET,         TYPE_PORTAL_TARGET);

static EntryTypeMap const GameObjectEntryToType = map_initializer<EntryTypeMap>
    (GO_COLISEUM_FLOOR,         TYPE_COLISEUM_FLOOR)
    (GO_ANUBARAK_CHEST_10H_0,   TYPE_ANUBARAK_CHEST_FAIL)
    (GO_ANUBARAK_CHEST_10H_25,  TYPE_ANUBARAK_CHEST_25)
    (GO_ANUBARAK_CHEST_10H_45,  TYPE_ANUBARAK_CHEST_45)
    (GO_ANUBARAK_CHEST_10H_50,  TYPE_ANUBARAK_CHEST_50)
    (GO_ANUBARAK_CHEST_25H_0,   TYPE_ANUBARAK_CHEST_FAIL)
    (GO_ANUBARAK_CHEST_25H_25,  TYPE_ANUBARAK_CHEST_25)
    (GO_ANUBARAK_CHEST_25H_45,  TYPE_ANUBARAK_CHEST_45)
    (GO_ANUBARAK_CHEST_25H_50,  TYPE_ANUBARAK_CHEST_50)
    (GO_CHAMPIONS_CHEST_N10,    TYPE_FACTION_CHAMPION_CHEST)
    (GO_CHAMPIONS_CHEST_N25,    TYPE_FACTION_CHAMPION_CHEST)
    (GO_CHAMPIONS_CHEST_H10,    TYPE_FACTION_CHAMPION_CHEST)
    (GO_CHAMPIONS_CHEST_H25,    TYPE_FACTION_CHAMPION_CHEST)
    (GO_ENTRANCE_DOOR,          TYPE_ENTRANCE_DOOR)
    (GO_MAIN_GATE,              TYPE_MAIN_GATE);

namespace toc {

uint32 GetType(Creature *pCreature)
{
    return map_find(CreatureEntryToType, pCreature->GetEntry(), DATA_GUID_MAX);
}

uint32 GetType(GameObject *pGO)
{
    return map_find(GameObjectEntryToType, pGO->GetEntry(), DATA_GUID_MAX);
}

} // namespace toc

boss_trial_of_the_crusaderAI::boss_trial_of_the_crusaderAI(Creature* pCreature):
    ScriptedAI(pCreature),
    ScriptEventInterface(pCreature),
    m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
    m_Difficulty(pCreature->GetMap()->GetDifficulty()),
    m_bIsHeroic(m_Difficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_Difficulty == RAID_DIFFICULTY_25MAN_HEROIC),
    m_bIs10Man(m_Difficulty == RAID_DIFFICULTY_10MAN_NORMAL || m_Difficulty == RAID_DIFFICULTY_10MAN_HEROIC),
    m_BossEncounter(toc::GetType(pCreature), m_pInstance)
{
    m_BossEncounter = NOT_STARTED;
}

void boss_trial_of_the_crusaderAI::Reset()
{
    Events.Reset();
    m_BossEncounter = NOT_STARTED;
    if (Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSEY))
        if (ScriptedAI *barrettAI = dynamic_cast<ScriptedAI*>(barrett->AI()))
            barrettAI->Reset();
}
