#include "precompiled.h"
#include "ruby_sanctum.h"

typedef UNORDERED_MAP<uint32 /*entry*/, uint32 /*data_id*/> EntryTypeMap;

static EntryTypeMap const CreatureEntryToType = map_initializer<EntryTypeMap>
    (NPC_BALTHARUS,         TYPE_BALTHARUS)
    (NPC_SAVIANA_RAGEFIRE,  TYPE_SAVIANA)
    (NPC_ZARITHRIAN,        TYPE_ZARITHRIAN)
    (NPC_HALION_PHYSICAL,   TYPE_HALION)
    (NPC_SANCTUM_GUARD_XERESTRASZA, DATA_XERESTRASZA)
    (NPC_HALION_CONTROLLER, DATA_HALION_CONTROLLER)
    ;

static EntryTypeMap const GameObjectEntryToType = map_initializer<EntryTypeMap>
    (GO_HALION_FLAME_RING,  DATA_HALION_FLAME_RING)
    (GO_FIRE_WALLS,         DATA_FLAME_WALLS)
    (GO_FIRE_FIELD,         DATA_FIRE_FIELD)
    (GO_BURNING_TREE_1,     DATA_BURNING_TREE_1)
    (GO_BURNING_TREE_2,     DATA_BURNING_TREE_2)
    (GO_BURNING_TREE_3,     DATA_BURNING_TREE_3)
    (GO_BURNING_TREE_4,     DATA_BURNING_TREE_4)
    ;

namespace rs {

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
        case TYPE_HALION:
            return BOSS_DONE(TYPE_BALTHARUS) && BOSS_DONE(TYPE_SAVIANA) && BOSS_DONE(TYPE_ZARITHRIAN);
        default:    // unknown boss
            return true;
    }
}

#undef BOSS_DONE

} // namespace rs

boss_ruby_sanctumAI::boss_ruby_sanctumAI(Creature* pCreature):
    ScriptedAI(pCreature),
    ScriptEventInterface(pCreature),
    m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
    m_Difficulty(pCreature->GetMap()->GetDifficulty()),
    m_bIsHeroic(m_Difficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_Difficulty == RAID_DIFFICULTY_25MAN_HEROIC),
    m_bIs10Man(m_Difficulty == RAID_DIFFICULTY_10MAN_NORMAL || m_Difficulty == RAID_DIFFICULTY_10MAN_HEROIC),
    m_BossEncounter(rs::GetType(pCreature), m_pInstance)
{
    if (m_creature->isAlive())
        m_BossEncounter = NOT_STARTED;
}

void boss_ruby_sanctumAI::Reset()
{
    Events.Reset();
    if (m_creature->isAlive())
        m_BossEncounter = NOT_STARTED;
}

void boss_ruby_sanctumAI::JustDied(Unit* pKiller)
{
    if (rs::MeetsRequirementsForBoss(m_pInstance, TYPE_HALION))
        if (Creature* Controller = GET_CREATURE(DATA_HALION_CONTROLLER))
            SendEventTo(Controller, 1, 0); // on boss death if notifiers are ready, init the spawning of the twilight destroyer
}

bool boss_ruby_sanctumAI::IsOutOfCombatArea() const
{
    if (m_creature->GetMapId() != 724)  // invalid check outside RS
        return false;

    uint32 areaId = m_creature->GetAreaId();

    switch (m_creature->GetEntry())
    {
        case NPC_SAVIANA_RAGEFIRE:
            return !m_creature->IsWithinDist2d(3162.77f, 638.14f, 75.0f);
        case NPC_BALTHARUS:
        case NPC_BALTHARUS_CLONE:
            return !m_creature->IsWithinDist2d(3153.06f, 389.48f, 63.0f);
        case NPC_ZARITHRIAN:
            return !m_creature->IsWithinDist2d(3004.659f, 535.489f, 63.0f);
        case NPC_HALION_PHYSICAL:
        case NPC_HALION_TWILIGHT:
            return !m_creature->IsWithinDist2d(3152.08f, 532.163f, 55.0f);
        default:
            return false;
    }
}

bool boss_ruby_sanctumAI::OutOfCombatAreaCheck()
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

bool boss_ruby_sanctumAI::InstanceProgressionCheck()
{
    if (!rs::MeetsRequirementsForBoss(m_pInstance, m_BossEncounter.getDataId()))
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

void boss_ruby_sanctumAI::RemoveEncounterAuras(int32 spellId_1, int32 spellId_2, int32 spellId_3, int32 spellId_4)
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
