#include "precompiled.h"
#include "icecrown_citadel.h"
#include "TemporarySummon.h"

typedef UNORDERED_MAP<uint32 /*entry*/, uint32 /*data_id*/> EntryTypeMap;

static EntryTypeMap const CreatureEntryToType = map_initializer<uint32, uint32>
    (NPC_MARROWGAR,         TYPE_MARROWGAR)
    (NPC_DEATHWHISPER,      TYPE_DEATHWHISPER)
    (NPC_SAURFANG,          TYPE_SAURFANG)
    (NPC_FESTERGUT,         TYPE_FESTERGUT)
    (NPC_ROTFACE,           TYPE_ROTFACE)
    (NPC_PUTRICIDE,         TYPE_PUTRICIDE)
    (NPC_VALANAR,           TYPE_VALANAR)
    (NPC_KELESETH,          TYPE_KELESETH)
    (NPC_TALDARAM,          TYPE_TALDARAM)
    (NPC_LANATHEL,          TYPE_LANATHEL)
    (NPC_VALITHRIA,         TYPE_VALITHRIA)
    (NPC_SINDRAGOSA,        TYPE_SINDRAGOSA)
    (NPC_LICH_KING,         TYPE_LICH_KING);

static EntryTypeMap const GameObjectEntryToType = map_initializer<uint32, uint32>
    (GO_MARROWGAR_DOOR_1,   DATA_MARROWGAR_DOOR_1)
    (GO_MARROWGAR_DOOR_2,   DATA_MARROWGAR_DOOR_2)
    (GO_DEATHWHISPER_ELEV,  DATA_DEATHWHISPER_ELEV)
    (GO_SAURFANG_CHEST_N10, DATA_SAURFANG_CHEST)
    (GO_SAURFANG_CHEST_N25, DATA_SAURFANG_CHEST)
    (GO_SAURFANG_CHEST_H10, DATA_SAURFANG_CHEST)
    (GO_SAURFANG_CHEST_H25, DATA_SAURFANG_CHEST)
    (GO_SAURFANG_DOOR,      DATA_SAURFANG_DOOR);

namespace icc {

uint32 GetType(Creature *pCreature)
{
    return map_find(CreatureEntryToType, pCreature->GetEntry(), DATA_MAX);
}

uint32 GetType(GameObject *pGO)
{
    return map_find(GameObjectEntryToType, pGO->GetEntry(), DATA_MAX);
}

} // namespace icc

boss_icecrown_citadelAI::boss_icecrown_citadelAI(Creature* pCreature):
    ScriptedAI(pCreature),
    m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
    m_BossEncounter(icc::GetType(pCreature), m_pInstance)
{
    Difficulty diff = pCreature->GetMap()->GetDifficulty();
    m_bIsHeroic = diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC;
    m_bIs10Man = diff == RAID_DIFFICULTY_10MAN_NORMAL || diff == RAID_DIFFICULTY_10MAN_HEROIC;
    m_BossEncounter = NOT_STARTED;
}

void boss_icecrown_citadelAI::Reset()
{
    Events.Reset();
    m_BossEncounter = NOT_STARTED;
}

SummonManager::SummonManager(Creature *pCreature):
    m_creature(pCreature)
{
}

void SummonManager::AddSummonToList(ObjectGuid const& guid)
{
    m_Summons.push_back(guid);
}

void SummonManager::RemoveSummonFromList(ObjectGuid const& guid)
{
    // could have used remove(guid) but we know they're unique so it wastes time searching after unique entry removed
    for (SummonContainer::iterator i = m_Summons.begin(); i != m_Summons.end(); ++i)
        if (*i == guid)
        {
            m_Summons.erase(i);
            break;
        }
}

Creature* SummonManager::SummonCreature(uint32 Id, float x, float y, float z, float ang, TempSummonType type, uint32 SummonTimer)
{
    Creature *pSummon = m_creature->SummonCreature(Id, x, y, z, ang, type, SummonTimer);
    if (pSummon)
        AddSummonToList(pSummon->GetObjectGuid());
    return pSummon;
}

void SummonManager::SummonCreatures(uint32 Id, float x, float y, float z, uint32 number, float ang, TempSummonType type, uint32 SummonTimer)
{
    for (; number; --number)
        SummonCreature(Id, x, y, z, ang, type, SummonTimer);
}

Creature* SummonManager::SummonCreatureAt(WorldObject* target, uint32 Id, TempSummonType type, uint32 SummonTimer, float dx, float dy, float dz, float dang)
{
    return SummonCreature(Id, target->GetPositionX()+dx, target->GetPositionY()+dy, target->GetPositionZ()+dz, target->GetOrientation()+dang, type, SummonTimer);
}

void SummonManager::SummonCreaturesAt(WorldObject* target, uint32 Id, uint32 number, TempSummonType type, uint32 SummonTimer, float dx, float dy, float dz, float dang)
{
    for (; number; --number)
        SummonCreatureAt(target, Id, type, SummonTimer, dx, dy, dz, dang);
}

void SummonManager::UnsummonCreature(Creature *pSummon)
{
    if (!pSummon)
        return;
    if (pSummon->isTemporarySummon())
    {
        RemoveSummonFromList(pSummon->GetObjectGuid());
        static_cast<TemporarySummon*>(pSummon)->UnSummon();
    }
    else    // called for non-summoned unit
        pSummon->ForcedDespawn();
}

void SummonManager::UnsummonByGuid(ObjectGuid const& guid)
{
    if (Creature *pSummon = m_creature->GetMap()->GetCreature(guid))
        UnsummonCreature(pSummon);
}

void SummonManager::UnsummonAllWithId(uint32 Id)
{
    for (SummonContainer::iterator i = m_Summons.begin(); i != m_Summons.end(); )
    {
        if (i->GetEntry() == Id)
        {
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                static_cast<TemporarySummon*>(pSummon)->UnSummon();
            i = m_Summons.erase(i);
        }
        else
            ++i;
    }
}

void SummonManager::UnsummonAll()
{
    for (SummonContainer::const_iterator i = m_Summons.begin(); i != m_Summons.end(); ++i)
        if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
            static_cast<TemporarySummon*>(pSummon)->UnSummon();

    m_Summons.clear();
}
