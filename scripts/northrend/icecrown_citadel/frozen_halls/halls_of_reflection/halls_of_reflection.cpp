#include "precompiled.h"
#include "halls_of_reflection.h"

typedef UNORDERED_MAP<uint32 /*entry*/, uint32 /*data_id*/> EntryTypeMap;

static EntryTypeMap const CreatureEntryToType = map_initializer<EntryTypeMap>
    (NPC_FALRIC,            TYPE_FALRIC)
    (NPC_MARWYN,            TYPE_MARWYN)
    (NPC_LICH_KING,         TYPE_LICH_KING)
    ;

static EntryTypeMap const GameObjectEntryToType = map_initializer<EntryTypeMap>
    (0, DATA_GUID_MAX)  // remove this when adding first entry
    ;

namespace hor {

uint32 GetType(Creature *pCreature)
{
    return map_find(CreatureEntryToType, pCreature->GetEntry(), DATA_GUID_MAX);
}

uint32 GetType(GameObject *pGO)
{
    return map_find(GameObjectEntryToType, pGO->GetEntry(), DATA_GUID_MAX);
}

#define BOSS_DONE(id)   (instance->GetData(id) == DONE)

bool MeetsRequirementsForBoss(InstanceData* instance, uint32 boss_id)
{
    if (!instance)
        return true;

    switch (boss_id)
    {
        case TYPE_FALRIC:
        case TYPE_MARWYN:
            return true;
        case TYPE_LICH_KING:
            return BOSS_DONE(TYPE_FALRIC) && BOSS_DONE(TYPE_MARWYN);
        default:    // unknown boss
            return true;
    }
}

#undef BOSS_DONE

} // namespace hor

boss_halls_of_reflectionAI::boss_halls_of_reflectionAI(Creature* pCreature):
    ScriptedAI(pCreature),
    ScriptEventInterface(pCreature),
    m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
    m_bIsHeroic(!pCreature->GetMap()->IsRegularDifficulty()),
    m_BossEncounter(hor::GetType(pCreature), m_pInstance)
{
    if (m_creature->isAlive())
        m_BossEncounter = NOT_STARTED;
}

void boss_halls_of_reflectionAI::Reset()
{
    Events.Reset();
    if (m_creature->isAlive())
        m_BossEncounter = NOT_STARTED;
}

bool boss_halls_of_reflectionAI::IsOutOfCombatArea() const
{
    if (m_creature->GetMapId() != 668)  // invalid check outside HoR
        return false;

    float z = m_creature->GetPositionZ();
    uint32 areaId = m_creature->GetAreaId();

    switch (m_creature->GetEntry())
    {
        case NPC_FALRIC:
        case NPC_MARWYN:
        case NPC_LICH_KING:
        default:
            return false;
    }
}

bool boss_halls_of_reflectionAI::OutOfCombatAreaCheck()
{
    if (m_creature->IsInEvadeMode())
        return false;

    if (IsOutOfCombatArea())
    {
        EnterEvadeMode();
        return true;
    }

    return false;
}

bool boss_halls_of_reflectionAI::InstanceProgressionCheck()
{
    if (!hor::MeetsRequirementsForBoss(m_pInstance, m_BossEncounter.getDataId()))
    {
        EnterEvadeMode();
        // teleport offending players outside instance
        std::deque<Player*> players;
        Map::PlayerList const &plist = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = plist.begin(); i != plist.end(); ++i)
        {
            Unit *pPlayer = i->getSource();
            if (pPlayer && pPlayer->GetTypeId() == TYPEID_PLAYER)
                players.push_back(static_cast<Player*>(pPlayer));
        }
        for (std::deque<Player*>::const_iterator i = players.begin(); i != players.end(); ++i)
            (*i)->TeleportToHomebind();
        return true;
    }

    return false;
}

void boss_halls_of_reflectionAI::RemoveEncounterAuras(int32 spellId_1, int32 spellId_2, int32 spellId_3, int32 spellId_4)
{
    Map::PlayerList const &Players = m_creature->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
    {
        Unit *pPlayer = itr->getSource();
        if (!pPlayer)
            continue;

        if (spellId_1 > 0)
            pPlayer->RemoveAurasDueToSpell(spellId_1);
        else if (spellId_1 < 0)
            pPlayer->RemoveAurasByDifficulty(-spellId_1);

        if (spellId_2 > 0)
            pPlayer->RemoveAurasDueToSpell(spellId_2);
        else if (spellId_2 < 0)
            pPlayer->RemoveAurasByDifficulty(-spellId_2);

        if (spellId_3 > 0)
            pPlayer->RemoveAurasDueToSpell(spellId_3);
        else if (spellId_3 < 0)
            pPlayer->RemoveAurasByDifficulty(-spellId_3);

        if (spellId_4 > 0)
            pPlayer->RemoveAurasDueToSpell(spellId_4);
        else if (spellId_4 < 0)
            pPlayer->RemoveAurasByDifficulty(-spellId_4);
    }
}

void AddSC_halls_of_reflection()
{
}
