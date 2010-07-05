#ifndef DEF_ICECROWN_CITADEL_H
#define DEF_ICECROWN_CITADEL_H

#define GET_CREATURE(C)     (InstanceVar<uint64>(C, m_pInstance).getObject<Creature>())
#define GET_GAMEOBJECT(G)   (InstanceVar<uint64>(G, m_pInstance).getObject<GameObject>())

#define DIFF_SELECT(N10, N25, H10, H25) (m_bIs10Man ? (m_bIsHeroic ? H10 : N10) : (m_bIsHeroic ? H25 : N25))

// schedules an event
#define RESCHEDULE_EVENT(EV) Events.RescheduleEvent(EVENT_##EV, TIMER_##EV);

#define IS_HORDE            (m_pInstance ? m_pInstance->GetData(DATA_FACTION) == HORDE : false)

enum
{
    MAX_ENCOUNTER           = 14,

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
    TYPE_VALITHRIA          = 11,
    TYPE_SINDRAGOSA         = 12,
    // The Frozen Throne
    TYPE_LICH_KING          = 13,

    // instance objects
    DATA_MARROWGAR_DOOR_1,
    DATA_MARROWGAR_DOOR_2,
    DATA_DEATHWHISPER_ELEV,
    DATA_SAURFANG_CHEST,
    DATA_SAURFANG_DOOR,

    // total count of guid values stored
    DATA_MAX,

    // instance variables
    DATA_FACTION,

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
    NPC_VALITHRIA           = 36789,
    NPC_SINDRAGOSA          = 36853,
    NPC_LICH_KING           = 36597,

    // gameobject entries
    GO_MARROWGAR_DOOR_1     = 201911,
    GO_MARROWGAR_DOOR_2     = 201910,
    GO_DEATHWHISPER_ELEV    = 202220,
    GO_SAURFANG_CHEST_N10   = 202239,
    GO_SAURFANG_CHEST_N25   = 202240,
    GO_SAURFANG_CHEST_H10   = 202238,
    GO_SAURFANG_CHEST_H25   = 202241,
    GO_SAURFANG_DOOR        = 201825,
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

} // namespace icc

// basic start point for bosses
struct MANGOS_DLL_DECL boss_icecrown_citadelAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    Difficulty m_Difficulty;
    bool m_bIsHeroic :1;
    bool m_bIs10Man :1;
    InstanceVar<uint32> m_BossEncounter;
    EventMap Events;

    boss_icecrown_citadelAI(Creature*);

    void Reset();
};

#endif // DEF_ICECROWN_CITADEL_H
