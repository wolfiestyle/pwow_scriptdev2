/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_HALLS_OF_REFLECTION_H
#define DEF_HALLS_OF_REFLECTION_H

#define GET_CREATURE(C)     (InstanceVar<uint64>(C, m_pInstance).getObject<Creature>())
#define GET_GAMEOBJECT(G)   (InstanceVar<uint64>(G, m_pInstance).getObject<GameObject>())

#define DIFF_SELECT(N, H)   (m_bIsHeroic ? H : N)

// schedules a periodic event
#define SCHEDULE_EVENT(EV, ...)   Events.ScheduleEvent(EVENT_##EV, TIMER_##EV, TIMER_##EV, ## __VA_ARGS__)
#define SCHEDULE_EVENT_R(EV, ...) Events.ScheduleEventInRange(EVENT_##EV, TIMER_##EV, TIMER_##EV, ## __VA_ARGS__)

#define IS_HORDE            (m_pInstance ? m_pInstance->GetData(DATA_FACTION) == HORDE : false)

enum
{
    MAX_ENCOUNTER           = 3,

    TYPE_FALRIC             = 0,
    TYPE_MARWYN             = 1,
    TYPE_LICH_KING          = 2,

    // instance objects

    // total count of guid values stored
    DATA_GUID_MAX,

    // instance variables
    DATA_FACTION = 100,

    // creature entries
    NPC_FALRIC              = 38112,
    NPC_MARWYN              = 38113,
    NPC_LICH_KING           = 37226,

    // gameobject entries

    // achievement criteria IDs

    // misc
    FACTION_NEUTRAL         = 7,
    FACTION_HOSTILE         = 14,
    FACTION_FRIENDLY        = 35,
};

//---------------------------------------------------------

namespace hor {

// helper for the script register process
template <typename T>
CreatureAI* GetAI(Creature *pCreature)
{
    return new T(pCreature);
}

#define REGISTER_SCRIPT(SC) \
    newscript = new Script; \
    newscript->Name = #SC; \
    newscript->GetAI = &hor::GetAI<SC##AI>; \
    newscript->RegisterSelf();

// get data_id from Creature or GameObject
uint32 GetType(Creature*);
uint32 GetType(GameObject*);

// check if group killed all previous bosses in progression order
bool MeetsRequirementsForBoss(InstanceData* instance, uint32 boss_id);

} // namespace hor

// basic start point for bosses
struct MANGOS_DLL_DECL boss_halls_of_reflectionAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;
    InstanceVar<uint32> m_BossEncounter;

    boss_halls_of_reflectionAI(Creature*);

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

#endif // DEF_HALLS_OF_REFLECTION_H
