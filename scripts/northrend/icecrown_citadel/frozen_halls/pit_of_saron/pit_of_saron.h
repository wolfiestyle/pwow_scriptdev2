#ifndef DEF_PIT_OF_SARON_H
#define DEF_PIT_OF_SARON_H

#define GET_CREATURE(C)     (InstanceVar<uint64>(C, m_pInstance).getObject<Creature>())
#define GET_GAMEOBJECT(G)   (InstanceVar<uint64>(G, m_pInstance).getObject<GameObject>())

#define DIFF_SELECT(N, H)   (m_bIsHeroic ? H : N)

// schedules a periodic event
#define SCHEDULE_EVENT(EV, ...)   Events.ScheduleEvent(EVENT_##EV, TIMER_##EV, TIMER_##EV, ## __VA_ARGS__)
#define SCHEDULE_EVENT_R(EV, ...) Events.ScheduleEventInRange(EVENT_##EV, TIMER_##EV, TIMER_##EV, ## __VA_ARGS__)

#define IS_HORDE            (m_pInstance ? m_pInstance->GetData(DATA_FACTION) == HORDE : false)

enum
{
    MAX_ENCOUNTER           = 4,

    TYPE_GARFROST           = 0,
    TYPE_ICK                = 1,
    TYPE_KRICK              = 2,
    TYPE_TYRANNUS           = 3,

    // instance objects

    // total count of guid values stored
    DATA_GUID_MAX,

    // instance variables
    DATA_FACTION = 100,

    // creature entries
    NPC_GARFROST            = 36494,
    NPC_ICK                 = 36476,
    NPC_KRICK               = 36477,
    NPC_TYRANNUS            = 36658,

    // gameobject entries

    // achievement criteria IDs

    // misc
    FACTION_NEUTRAL         = 7,
    FACTION_HOSTILE         = 14,
    FACTION_FRIENDLY        = 35,
};

//---------------------------------------------------------

namespace pos {

// helper for the script register process
template <typename T>
CreatureAI* GetAI(Creature *pCreature)
{
    return new T(pCreature);
}

#define REGISTER_SCRIPT(SC) \
    newscript = new Script; \
    newscript->Name = #SC; \
    newscript->GetAI = &pos::GetAI<SC##AI>; \
    newscript->RegisterSelf();

// get data_id from Creature or GameObject
uint32 GetType(Creature*);
uint32 GetType(GameObject*);

// check if group killed all previous bosses in progression order
bool MeetsRequirementsForBoss(InstanceData* instance, uint32 boss_id);

} // namespace pos

// basic start point for bosses
struct MANGOS_DLL_DECL boss_pit_of_saronAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance *m_pInstance;
    Difficulty m_Difficulty;
    bool m_bIsHeroic;
    InstanceVar<uint32> m_BossEncounter;

    boss_pit_of_saronAI(Creature*);

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
};

#endif // DEF_PIT_OF_SARON_H
