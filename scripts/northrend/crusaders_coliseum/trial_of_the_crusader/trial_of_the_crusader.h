#ifndef DEF_TRIAL_OF_THE_CRUSADER_H
#define DEF_TRIAL_OF_THE_CRUSADER_H

#define DIFF_SELECT(N10, N25, H10, H25) (m_bIs10Man ? (m_bIsHeroic ? H10 : N10) : (m_bIsHeroic ? H25 : N25))

#define GET_CREATURE(C)     (InstanceVar<uint64>(C, m_pInstance).getObject<Creature>())
#define GET_GAMEOBJECT(G)   (InstanceVar<uint64>(G, m_pInstance).getObject<GameObject>())

// schedules a periodic event
#define SCHEDULE_EVENT(EV, ...)   Events.ScheduleEvent(EVENT_##EV, TIMER_##EV, TIMER_##EV, ## __VA_ARGS__)
#define SCHEDULE_EVENT_R(EV, ...) Events.ScheduleEventInRange(EVENT_##EV, TIMER_##EV, TIMER_##EV, ## __VA_ARGS__)

#define IS_HORDE            (m_pInstance ? m_pInstance->GetData(DATA_FACTION) == HORDE : false)

#define DESPAWN_CREATURE \
    do { if (m_creature->isTemporarySummon()) static_cast<TemporarySummon*>(m_creature)->UnSummon(); else m_creature->ForcedDespawn(); } while (0)

enum
{
    MAX_ENCOUNTER               = 22,
    FACTION_CHAMPION_START      = 5,
    FACTION_CHAMPION_END        = 18,

    // beasts of northerend
    TYPE_GORMOK                 = 0,
    TYPE_ACIDMAW                = 1,
    TYPE_DREADSCALE             = 2,
    TYPE_ICEHOWL                = 3,
    // jaraxxus
    TYPE_JARAXXUS               = 4,
    // alliance faction champions
    TYPE_TYRIUS_DUSKBLADE       = 5,
    TYPE_KAVINA_GROVESONG       = 6,
    TYPE_MELADOR_VALESTRIDER    = 7,
    TYPE_ALYSSIA_MOONSTALKER    = 8,
    TYPE_NOOZLE_WHIZZLESTICK    = 9,
    TYPE_VELNAA                 = 10,
    TYPE_BAELNOR_LIGHTBEARER    = 11,
    TYPE_ANTHAR_FORGEMENDER     = 12,
    TYPE_BRIENNA_NIGHTFELL      = 13,
    TYPE_IRIETH_SHADOWSTEP      = 14,
    TYPE_SAAMUL                 = 15,
    TYPE_SHAABAD                = 16,
    TYPE_SERISSA_GRIMDABBLER    = 17,
    TYPE_SHOCUUL                = 18,
    // horde faction champions
    TYPE_GORGRIM_SHADOWCLEAVE   = 5,
    TYPE_BIRANA_STORMHOOF       = 6,
    TYPE_ERIN_MISTHOOF          = 7,
    TYPE_RUJKAH                 = 8,
    TYPE_GINSELLE_BLIGHTSLINGER = 9,
    TYPE_LIANDRA_SUNCALLER      = 10,
    TYPE_MALITHAS_BRIGHTBLADE   = 11,
    TYPE_CAIPHUS_THE_STERN      = 12,
    TYPE_VIVIENNE_BLACKWHISPER  = 13,
    TYPE_MAZDINAH               = 14,
    TYPE_THRAKGAR               = 15,
    TYPE_BROLN_STOUTHORN        = 16,
    TYPE_HARKZOG                = 17,
    TYPE_NARRHOK_STEELBREAKER   = 18,
    // twin valkyr
    TYPE_FJOLA_LIGHTBANE        = 19,
    TYPE_EYDIS_DARKBANE         = 20,
    // 'suprise' boss
    TYPE_ANUBARAK               = 21,
    // controller guy
    TYPE_BARRETT_RAMSEY,
    // spectators
    TYPE_VARIAN_WYRM,
    TYPE_GARROSH_HELLSCREAM,
    TYPE_TIRION_FORDRING,
    // used in intros/outros
    TYPE_LICH_KING,
    TYPE_FIZZLEBANG,
    TYPE_PURPLE_RUNE,
    TYPE_PORTAL_TARGET,
    // instance objects
    TYPE_COLISEUM_FLOOR,
    TYPE_ANUBARAK_CHEST_FAIL,
    TYPE_ANUBARAK_CHEST_25,
    TYPE_ANUBARAK_CHEST_45,
    TYPE_ANUBARAK_CHEST_50,
    TYPE_FACTION_CHAMPION_CHEST,
    TYPE_ENTRANCE_DOOR,
    TYPE_MAIN_GATE,
    // total count of guid values stored
    DATA_GUID_MAX,

    // instance variables
    DATA_FACTION,
    DATA_ACHIEVEMENT_COUNTER,
    DATA_ATTEMPT_COUNTER,
    DATA_IN_TALK_PHASE,
    DATA_IMMORTAL,

    NPC_GORMOK                  = 34796, // beasts of northerend
    NPC_ACIDMAW                 = 35144,
    NPC_DREADSCALE              = 34799,
    NPC_ICEHOWL                 = 34797,
    NPC_JARAXXUS                = 34780,
    NPC_TYRIUS_DUSKBLADE        = 34461, // faction champions of alliance
    NPC_KAVINA_GROVESONG        = 34460,
    NPC_MELADOR_VALESTRIDER     = 34469,
    NPC_ALYSSIA_MOONSTALKER     = 34467,
    NPC_NOOZLE_WHIZZLESTICK     = 34468,
    NPC_VELNAA                  = 34465,
    NPC_BAELNOR_LIGHTBEARER     = 34471,
    NPC_ANTHAR_FORGEMENDER      = 34466,
    NPC_BRIENNA_NIGHTFELL       = 34473,
    NPC_IRIETH_SHADOWSTEP       = 34472,
    NPC_SHAABAD                 = 34463,
    NPC_SAAMUL                  = 34470,
    NPC_SERISSA_GRIMDABBLER     = 34474,
    NPC_SHOCUUL                 = 34475,
    NPC_GORGRIM_SHADOWCLEAVE    = 34458, // horde faction champions
    NPC_BIRANA_STORMHOOF        = 34451,
    NPC_ERIN_MISTHOOF           = 34459,
    NPC_RUJKAH                  = 34448,
    NPC_GINSELLE_BLIGHTSLINGER  = 34449,
    NPC_LIANDRA_SUNCALLER       = 34445,
    NPC_MALITHAS_BRIGHTBLADE    = 34456,
    NPC_CAIPHUS_THE_STERN       = 34447,
    NPC_VIVIENNE_BLACKWHISPER   = 34441,
    NPC_MAZDINAH                = 34454,
    NPC_BROLN_STOUTHORN         = 34455,
    NPC_THRAKGAR                = 34444,
    NPC_HARKZOG                 = 34450,
    NPC_NARRHOK_STEELBREAKER    = 34453,
    NPC_FJOLA_LIGHTBANE         = 34497, // twin valkyr
    NPC_EYDIS_DARKBANE          = 34496,
    NPC_ANUBARAK                = 34564, // 'suprise' boss

    NPC_BARRETT_RAMSEY          = 34816,
    NPC_VARIAN_WYRM             = 34990,
    NPC_GARROSH_HELLSCREAM      = 34995,
    NPC_TIRION_FORDRING         = 34996,
    NPC_LICH_KING               = 35877, // used for start of anub'arak encounter
    NPC_WILFRED_FIZZLEBANG      = 35458,
    NPC_PURPLE_RUNE             = 35651,
    NPC_PORTAL_TARGET           = 17965, // for "summoning" jaraxxus

    TRIBUTE_TO_SKILL_CRITERIA_10H1      = 12344,
    TRIBUTE_TO_SKILL_CRITERIA_10H2      = 12345,
    TRIBUTE_TO_SKILL_CRITERIA_10H3      = 12346,

    TRIBUTE_TO_SKILL_CRITERIA_25H1      = 12338,
    TRIBUTE_TO_SKILL_CRITERIA_25H2      = 12339,
    TRIBUTE_TO_SKILL_CRITERIA_25H3      = 12340,

    TRIBUTE_TO_MAD_SKILL_CRITERIA_10H1  = 12347,
    TRIBUTE_TO_MAD_SKILL_CRITERIA_10H2  = 12348,

    TRIBUTE_TO_MAD_SKILL_CRITERIA_25H1  = 12341,
    TRIBUTE_TO_MAD_SKILL_CRITERIA_25H2  = 12342,

    TRIBUTE_TO_INSANITY_CRITERIA_10H1   = 12349,
    TRIBUTE_TO_INSANITY_CRITERIA_25H1   = 12343,

    TRIBUTE_TO_IMMORTALITY_CRITERIA_25H = 12358,
    TRIBUTE_TO_IMMORTALITY_CRITERIA_25A = 12359,

    REALMFIRST_GRAND_CRUSADER_CRITERIA  = 12350,

    TRIBUTE_TO_DEDICATED_INSANITY       = 12360,

    UPPER_BACK_PAIN_CRITERIA_N10 = 11779,
    UPPER_BACK_PAIN_CRITERIA_N25 = 11780,
    UPPER_BACK_PAIN_CRITERIA_H10 = 11802,
    UPPER_BACK_PAIN_CRITERIA_H25 = 11801,

    THREE_SIXTY_CRITERIA_N10     = 11838,
    THREE_SIXTY_CRITERIA_N25     = 11839,
    THREE_SIXTY_CRITERIA_H10     = 11861,
    THREE_SIXTY_CRITERIA_H25     = 11862,

    GO_COLISEUM_FLOOR           = 195527,
    GO_ANUBARAK_CHEST_10H_0     = 195665,
    GO_ANUBARAK_CHEST_10H_25    = 195666,
    GO_ANUBARAK_CHEST_10H_45    = 195667,
    GO_ANUBARAK_CHEST_10H_50    = 195668,
    GO_ANUBARAK_CHEST_25H_0     = 195669,
    GO_ANUBARAK_CHEST_25H_25    = 195670,
    GO_ANUBARAK_CHEST_25H_45    = 195671,
    GO_ANUBARAK_CHEST_25H_50    = 195672,
    GO_CHAMPIONS_CHEST_N10      = 195631,
    GO_CHAMPIONS_CHEST_N25      = 195632,
    GO_CHAMPIONS_CHEST_H10      = 195633,
    GO_CHAMPIONS_CHEST_H25      = 195635,
    GO_ENTRANCE_DOOR            = 195589,
    GO_MAIN_GATE                = 195647,

    WORLD_STATE_TOTGC                   = 4390,
    WORLD_STATE_TOTGC_ATTEMPT_COUNTER   = 4389,

    FACTION_NEUTRAL             = 7,
    FACTION_HOSTILE             = 14,
};

//---------------------------------------------------------

namespace toc {

// helper for the script register process
template <typename T>
CreatureAI* GetAI(Creature *pCreature)
{
    return new T(pCreature);
}

#define REGISTER_SCRIPT(SC) \
    newscript = new Script; \
    newscript->Name = #SC; \
    newscript->GetAI = &toc::GetAI<SC##AI>; \
    newscript->RegisterSelf();

// get data_id from Creature or GameObject
uint32 GetType(Creature*);
uint32 GetType(GameObject*);

} // namespace toc

// basic start point for bosses
struct MANGOS_DLL_DECL boss_trial_of_the_crusaderAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance *m_pInstance;
    Difficulty m_Difficulty;
    bool m_bIsHeroic :1;
    bool m_bIs10Man :1;
    InstanceVar<uint32> m_BossEncounter;

    boss_trial_of_the_crusaderAI(Creature*);

    void Reset();
    void KilledUnit(Unit*);
};

#endif //DEF_TRIAL_OF_THE_CRUSADER_H
