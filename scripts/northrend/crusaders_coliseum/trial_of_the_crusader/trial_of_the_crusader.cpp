#include "precompiled.h"
#include "trial_of_the_crusader.h"

typedef UNORDERED_MAP<uint32 /*entry*/, uint32 /*data_id*/> EntryTypeMap;

static EntryTypeMap const CreatureEntryToType = map_initializer<uint32, uint32>
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
    (NPC_BARRETT_RAMSAY,        TYPE_BARRETT_RAMSAY)
    (NPC_VARIAN_WYRM,           TYPE_VARIAN_WYRM)
    (NPC_GARROSH_HELLSCREAM,    TYPE_GARROSH_HELLSCREAM)
    (NPC_TIRION_FORDRING,       TYPE_TIRION_FORDRING)
    (NPC_LICH_KING,             TYPE_LICH_KING)
    (NPC_WILFRED_FIZZLEBANG,    TYPE_FIZZLEBANG)
    (NPC_PURPLE_RUNE,           TYPE_PURPLE_RUNE)
    (NPC_PORTAL_TARGET,         TYPE_PORTAL_TARGET);

static EntryTypeMap const GameObjectEntryToType = map_initializer<uint32, uint32>
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

template <>
uint32 GetType<Creature>(Creature *pCreature)
{
    EntryTypeMap::const_iterator it = CreatureEntryToType.find(pCreature->GetEntry());
    return it != CreatureEntryToType.end() ? it->second : 0;
}

template <>
uint32 GetType<GameObject>(GameObject *pGO)
{
    EntryTypeMap::const_iterator it = GameObjectEntryToType.find(pGO->GetEntry());
    return it != GameObjectEntryToType.end() ? it->second : 0;
}

void GetRandomPointInCircle(float& x, float& y, float max_rad, float cx, float cy)
{
    float ang = 2*M_PI * rand_norm();
    float rad = max_rad * sqrt(rand_norm());
    x = cx + rad * cos(ang);
    y = cy + rad * sin(ang);
}

} // namespace toc

boss_trial_of_the_crusaderAI::boss_trial_of_the_crusaderAI(Creature* pCreature):
    ScriptedAI(pCreature),
    m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
    m_BossEncounter(toc::GetType<Creature>(pCreature), m_pInstance)
{
    Difficulty diff = pCreature->GetMap()->GetDifficulty();
    m_bIsHeroic = diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC;
    m_bIs10Man = diff == RAID_DIFFICULTY_10MAN_NORMAL || diff == RAID_DIFFICULTY_10MAN_HEROIC;
    m_BossEncounter = NOT_STARTED;
    Events.Reset();
}

void boss_trial_of_the_crusaderAI::Reset()
{
    m_BossEncounter = NOT_STARTED;
    if (Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSAY))
        if(ScriptedAI *barrettAI = dynamic_cast<ScriptedAI*>(barrett->AI()))
            barrettAI->Reset();
}

void boss_trial_of_the_crusaderAI::KilledUnit(Unit* pWho)
{
    if (m_pInstance && pWho && pWho->GetTypeId() == TYPEID_PLAYER)
        m_pInstance->SetData(DATA_IMMORTAL, 0);
}
