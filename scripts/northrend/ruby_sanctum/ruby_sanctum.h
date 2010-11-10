#ifndef DEF_RUBY_SANCTUM_H
#define DEF_RUBY_SANCTUM_H

#define GET_CREATURE(C)     (InstanceVar<uint64>(C, m_pInstance).getObject<Creature>())
#define GET_GAMEOBJECT(G)   (InstanceVar<uint64>(G, m_pInstance).getObject<GameObject>())

#define DIFF_SELECT(N10, N25, H10, H25) (m_bIs10Man ? (m_bIsHeroic ? H10 : N10) : (m_bIsHeroic ? H25 : N25))

// schedules a periodic event
#define SCHEDULE_EVENT(EV, ...)   Events.ScheduleEvent(EVENT_##EV, TIMER_##EV, TIMER_##EV, ## __VA_ARGS__)
#define SCHEDULE_EVENT_R(EV, ...) Events.ScheduleEventInRange(EVENT_##EV, TIMER_##EV, TIMER_##EV, ## __VA_ARGS__)

enum{
    MAX_ENCOUNTER           = 4,

    // Bosses
    TYPE_BALTHARUS          = 1,
    TYPE_SAVIANA            = 2, 
    TYPE_ZARITHRIAN         = 3,
    TYPE_HALION             = 4,

    //Gameobjects
    DATA_HALION_FLAME_RING  = 5,
    DATA_FIRE_FIELD         = 6,
    DATA_FLAME_WALLS        = 7,
    DATA_BURNING_TREE_1     = 8,
    DATA_BURNING_TREE_2     = 9,
    DATA_BURNING_TREE_3     = 10,
    DATA_BURNING_TREE_4     = 11,

    // total count of guid values stored
    DATA_GUID_MAX,

    // NPC entries
    NPC_SAVIANA_RAGEFIRE    = 39747,
    NPC_BALTHARUS           = 39751, // original one
    NPC_BALTHARUS_CLONE     = 39899,
    NPC_ZARITHRIAN          = 39746,
    NPC_SANCTUM_GUARD_XERESTRASZA = 40429,
    NPC_HALION_PHYSICAL     = 39863, // only this form counts towards the achievement
    NPC_HALION_TWILIGHT     = 40142,
    NPC_HALION_CONTROLLER   = 40146,

    // GOs
    GO_HALION_FLAME_RING    = 203007, // Halion
    GO_FIRE_FIELD           = 203005, // Baltharus
    GO_FIRE_WALLS           = 203006, // Zarithrian

    GO_BURNING_TREE_1       = 203034,
    GO_BURNING_TREE_2       = 203035,
    GO_BURNING_TREE_3       = 203036,
    GO_BURNING_TREE_4       = 203037,
};
//---------------------------------------------------------

namespace rs {

// helper for the script register process
template <typename T>
CreatureAI* GetAI(Creature *pCreature)
{
    return new T(pCreature);
}

#define REGISTER_SCRIPT(SC) \
    newscript = new Script; \
    newscript->Name = #SC; \
    newscript->GetAI = &rs::GetAI<SC##AI>; \
    newscript->RegisterSelf();

// get data_id from Creature or GameObject
uint32 GetType(Creature*);
uint32 GetType(GameObject*);

// check if group killed all previous bosses in progression order
bool MeetsRequirementsForBoss(InstanceData* instance, uint32 boss_id);

} // namespace rs

// basic start point for bosses
struct MANGOS_DLL_DECL boss_ruby_sanctumAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance *m_pInstance;
    Difficulty m_Difficulty;
    bool m_bIsHeroic :1;
    bool m_bIs10Man :1;
    InstanceVar<uint32> m_BossEncounter;

    boss_ruby_sanctumAI(Creature*);

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

#endif// DEF_RUBY_SANCTUM_H
