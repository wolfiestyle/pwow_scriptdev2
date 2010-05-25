#ifndef DEF_TRIAL_OF_THE_CRUSADER_H
#define DEF_TRIAL_OF_THE_CRUSADER_H

// this appends the difficulty suffix for each mode, so all of them must be defined in enum
// so you declare:      enum { SPELL_TEST_N10, SPELL_TEST_N25, SPELL_TEST_H10, SPELL_TEST_H25 };
// to cast the spell:   DoCast(target, DIFFICULTY(SPELL_TEST));
#define DIFF_SELECT(N10, N25, H10, H25) (m_bIs10Man ? (m_bIsHeroic ? H10 : N10) : (m_bIsHeroic ? H25 : N25))
#define DIFFICULTY(SP)      DIFF_SELECT(SP##_N10, SP##_N25, SP##_H10, SP##_H25)
#define HEROIC(SP)          (m_bIsHeroic ? SP##_H : SP##_N)

#define GET_CREATURE(C)     (m_pInstance ? m_creature->GetMap()->GetCreature(m_pInstance->GetData64(C)) : NULL)
#define GET_GAMEOBJECT(G)   (m_pInstance ? m_creature->GetMap()->GetGameObject(m_pInstance->GetData64(G)) : NULL)

// schedules an event
#define RESCHEDULE_EVENT(EV) Events.RescheduleEvent(EVENT_##EV, TIMER_##EV);

#define IS_HORDE            (m_pInstance ? m_pInstance->GetData(DATA_FACTION) == HORDE: false)

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
    // faction champions of alliance
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
    TYPE_BARRETT_RAMSAY         = 22,

    TYPE_VARIAN_WYRM            = 23,
    TYPE_GARROSH_HELLSCREAM     = 24,
    TYPE_TIRION_FORDRING        = 25,
    TYPE_LICH_KING              = 26,

    TYPE_COLISEUM_FLOOR         = 40,
    TYPE_ANUBARAK_CHEST         = 41,
    TYPE_FACTION_CHAMPION_CHEST = 42,
    TYPE_ENTRANCE_DOOR          = 43,

    DATA_FACTION                = 50,

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

    NPC_BARRETT_RAMSAY          = 34816,
    NPC_VARIAN_WYRM             = 34990,
    NPC_GARROSH_HELLSCREAM      = 34995,
    NPC_TIRION_FORDRING         = 34996,
    NPC_LICH_KING               = 35877, // used for start of anub'arak encounter

    GO_COLISEUM_FLOOR           = 195527,
    GO_ANUBARAK_CHEST           = 195665,
    GO_CHAMPIONS_CHEST_N10      = 195631,
    GO_CHAMPIONS_CHEST_N25      = 195632,
    GO_CHAMPIONS_CHEST_H10      = 195633,
    GO_CHAMPIONS_CHEST_H25      = 195635,
    GO_ENTRANCE_DOOR            = 195589,

    SPELL_SUNWELL_RADIANCE      = 45769, // workaround used to make unscripted bosses a bit harder
};

//---------------------------------------------------------

// helper for the script register process
template <typename T>
CreatureAI* toc_GetAI(Creature *pCreature)
{
    return new T(pCreature);
}

#define REGISTER_SCRIPT(SC) \
    newscript = new Script; \
    newscript->Name = #SC; \
    newscript->GetAI = &toc_GetAI<SC##AI>; \
    newscript->RegisterSelf();

// get data_id from creature
uint32 toc_GetType(Creature *pCreature);

// basic start point for bosses
struct MANGOS_DLL_DECL boss_trial_of_the_crusaderAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;
    bool m_bIs10Man;
    uint32 const m_uiBossEncounterId;
    EventMap Events;

    boss_trial_of_the_crusaderAI(Creature*);

    void Reset();
};

#endif //DEF_TRIAL_OF_THE_CRUSADER_H
