#include "precompiled.h"
#include "pit_of_saron.h"

typedef UNORDERED_MAP<uint32 /*entry*/, uint32 /*data_id*/> EntryTypeMap;

static EntryTypeMap const CreatureEntryToType = map_initializer<EntryTypeMap>
    (NPC_GARFROST,          TYPE_GARFROST)
    (NPC_ICK,               TYPE_ICK)
    (NPC_KRICK,             TYPE_KRICK)
    (NPC_TYRANNUS,          TYPE_TYRANNUS)
    ;

static EntryTypeMap const GameObjectEntryToType = map_initializer<EntryTypeMap>
    (0, DATA_GUID_MAX)  // remove this when adding first entry
    ;

namespace pos {

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
        case TYPE_GARFROST:
        case TYPE_ICK:
        case TYPE_KRICK:
            return true;
        case TYPE_TYRANNUS:
            return BOSS_DONE(TYPE_GARFROST) && BOSS_DONE(TYPE_ICK) && BOSS_DONE(TYPE_KRICK);
        default:    // unknown boss
            return true;
    }
}

#undef BOSS_DONE

} // namespace pos

boss_pit_of_saronAI::boss_pit_of_saronAI(Creature* pCreature):
    ScriptedAI(pCreature),
    ScriptEventInterface(pCreature),
    m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
    m_bIsHeroic(!pCreature->GetMap()->IsRegularDifficulty()),
    m_BossEncounter(pos::GetType(pCreature), m_pInstance)
{
    if (m_creature->isAlive())
        m_BossEncounter = NOT_STARTED;
}

void boss_pit_of_saronAI::Reset()
{
    Events.Reset();
    if (m_creature->isAlive())
        m_BossEncounter = NOT_STARTED;
}

bool boss_pit_of_saronAI::IsOutOfCombatArea() const
{
    if (m_creature->GetMapId() != 658)  // invalid check outside PoS
        return false;

    float z = m_creature->GetPositionZ();
    uint32 areaId = m_creature->GetAreaId();

    switch (m_creature->GetEntry())
    {
        case NPC_GARFROST:
        case NPC_ICK:
        case NPC_KRICK:
        case NPC_TYRANNUS:
        default:
            return false;
    }
}

bool boss_pit_of_saronAI::OutOfCombatAreaCheck()
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

bool boss_pit_of_saronAI::InstanceProgressionCheck()
{
    if (!pos::MeetsRequirementsForBoss(m_pInstance, m_BossEncounter.getDataId()))
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

void boss_pit_of_saronAI::RemoveEncounterAuras(int32 spellId_1, int32 spellId_2, int32 spellId_3, int32 spellId_4)
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
