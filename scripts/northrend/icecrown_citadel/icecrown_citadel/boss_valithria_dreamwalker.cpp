/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: boss_valithria
SD%Complete: 90%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_CORRUPTION            = 70904,
    SPELL_DREAMWALKER_RAGE      = 71189,
    SPELL_EMERALD_VIGOR         = 70873,
    SPELL_DREAM_SLIP            = 71196,
    SPELL_DREAM_PORTAL_PRE      = 71304,
    SPELL_NIGHTMARE_PORTAL_PRE  = 71986,
    SPELL_NIGHTMARE_PORTAL_VIS  = 71994,
    SPELL_DREAM_PORTAL_VIS      = 70763,
    SPELL_ROT_WORM_SPAWNER      = 70675,
    SPELL_DREAM_STATE           = 70766,
    SPELL_DREAM_CLOUD_VIS       = 70876,
    SPELL_NIGHTMARE_CLOUD_VIS   = 71939,
    SPELL_TWISTING_NIGHTMARES   = 71941,
    SPELL_SUMMON_DREAM_PORTAL   = 71301,
    SPELL_SUMMON_NIGHTMARE_PORT = 71977,
    //SPELL_NIGHTMARES            = 71946, // trigered from nightmare cloud
    // Blazing Skeletons
    SPELL_FIREBALL              = 70754,
    SPELL_LAY_WASTE             = 69325,
    // Risen Archmages
    SPELL_FROSTBOLT_VOLLEY      = 70759,
    SPELL_MANA_VOID             = 71179,
    SPELL_COLUMN_OF_FROST_SUM   = 70704,
    // Column of Frost
    SPELL_COLUMN_OF_FROST_DMG   = 70702,
    SPELL_COLUMN_OF_FROST_VIS   = 70715,
    // Mana Void
    SPELL_MANA_VOID_PASSIVE     = 71085,
    // Blistering Zombies
    SPELL_ACID_BURST            = 70744,
    SPELL_CORROSION_PASSIVE     = 70749,
    // Gluttonous Abominations
    SPELL_GUT_SPRAY             = 71283,
    SPELL_ROT_WORM              = 70675,
    // Rot Worms
    SPELL_ROT_PASSIVE           = 72962,
    SPELL_ROT_SPAWN             = 70668,
    // Suppressers
    SPELL_SUPPRESSION           = 70588,

    // Kill credit
    SPELL_ACHIEVEMENT_CHECK     = 72706,
};

enum Adds
{
    NPC_DREAM_CLOUD             = 37985,
    NPC_NIGHTMARE_CLOUD         = 38421,
    NPC_DREAM_PORTAL_PRE        = 38186,
    NPC_NIGHTMARE_PORTAL_PRE    = 38429,
    NPC_DREAM_PORTAL            = 37945,
    NPC_NIGHTMARE_PORTAL        = 38430,
    NPC_BLAZING_SKELETON        = 36791,
    NPC_BLISTERING_ZOMBIE       = 37934,
    NPC_RISEN_ARCHMAGE          = 37868,
    NPC_GLUTTONOUS_ABOMINATION  = 37886,
    NPC_ROT_WORM                = 37907,
    NPC_SUPPRESSER              = 37863,
    NPC_LICH_KING_VOICE         = 28765,
    NPC_GREEN_DRAGON_COMBAT_TRIGGER = 38752,
    NPC_MANA_VOID               = 38068,
    NPC_COLUMN_OF_FROST         = 37918,
};

enum Events
{
    EVENT_INIT_SCRIPT = 50, // high enough to avoid definition/event collisions with different scripts
    EVENT_BEGIN_FIGHT,
    EVENT_COMBAT_CHECK,
    EVENT_SUMMON_PORTALS,
    EVENT_DESPAWN,
    EVENT_SUMMON_RISEN,
    EVENT_SUMMON_BLAZING,
    EVENT_SUMMON_SUPPRESSERS,
    EVENT_SUMMON_ABOMS,
    EVENT_SUMMON_ZOMBIE,
    // Blazing Skeletons
    EVENT_FIREBALL,
    EVENT_LAY_WASTE,
    // Risen Archmages
    EVENT_FROSTBOLT,
    EVENT_MANA_VOID,
    EVENT_COLUMN_OF_FROST,
    // Column of Frost
    EVENT_BLAST,    
    // Gluttonous Abomination
    EVENT_GUT_SPRAY,
    // Blistering Zombie and lesser adds
    EVENT_DIE,
};

enum Says
{
    SAY_VALITHRIA_AGGRO         = -1301000,
    SAY_VALITHRIA_PORTALS       = -1301001,
    SAY_VALITHRIA_75            = -1301002,
    SAY_VALITHRIA_25            = -1301003,
    SAY_VALITHRIA_FAIL          = -1301004,
    SAY_VALITHRIA_SLAY_GOOD     = -1301005,
    SAY_VALITHRIA_BERSERK       = -1301006,
    SAY_VALITHRIA_SLAY_BAD      = -1301007,
    SAY_VALITHRIA_WIN           = -1301008,
    SAY_LICH_KING_GREEN_DRAGON  = -1301009,

};

// Following timers are gotten from video-research
static const uint32 BlazingTimers[] = {50, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10,  5,  5,  5,  5, 5};
static const uint32 AbomTimers[]    = {20, 60, 55, 50, 50, 50, 45, 45, 45, 40, 40, 40, 35, 35, 35, 30, 30, 30, 25, 25, 25};

static const float locations[4][2] =
{
    {4241.56f, 2424.29f}, //L1
    {4241.56f, 2546.83f}, //R1
    {4165.97f, 2424.29f}, //L2
    {4165.97f, 2546.83f}, //R2
};

#define ZOMBIE_SUMMON_TIMER     30*IN_MILLISECONDS
#define RISEN_SUMMON_TIMER      30*IN_MILLISECONDS
#define SUPPRESSER_SUMMON_TIMER 60*IN_MILLISECONDS
#define PORTAL_TIMER            46.5*IN_MILLISECONDS
#define TIMER_COMBAT_CHECK      3*IN_MILLISECONDS
#define MAX_SPAWN_POSITIONS     8 // we spawn 2 per point
#define SUMMON_PORTAL_RADIUS    30.0f

struct MANGOS_DLL_DECL boss_valithriaAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    std::bitset<5> m_abSays;
    bool m_bHeroicPortalsSummoned;
    bool m_bScriptStarted;
    uint32 m_uiPortalsUsed;
    uint32 m_uiPortalsSummoned;

    boss_valithriaAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature),
        m_bHeroicPortalsSummoned(false),
        m_bScriptStarted(false),
        m_uiPortalsUsed(0),
        m_uiPortalsSummoned(0)
    {
        pCreature->SetHealthPercent(50.0f);
        pCreature->CastSpell(pCreature, SPELL_CORRUPTION, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        DoStartNoMovement(pCreature);
        SetCombatMovement(false);
    }

    void Reset()
    {
        m_abSays.reset();
        DoScriptText(SAY_VALITHRIA_BERSERK, m_creature);
        SummonMgr.UnsummonAll(); // she only summons the combat trigger which despawns when he receives the broadcasted message "Win" or "wipe"
        m_bHeroicPortalsSummoned = false;

        m_creature->SetHealthPercent(50.0f);
        m_creature->CastSpell(m_creature, SPELL_CORRUPTION, false);
        m_bScriptStarted = false;
        m_uiPortalsUsed     = 0;
        m_uiPortalsSummoned = 0;

        if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_ENTRANCE))
            Door->SetGoState(GO_STATE_ACTIVE);
        if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_LEFT_1))
            Door->SetGoState(GO_STATE_READY);
        if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_RIGHT_1))
            Door->SetGoState(GO_STATE_READY);
        if (!m_bIs10Man)
        {
            if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_LEFT_2))
                Door->SetGoState(GO_STATE_READY);
            if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_RIGHT_2))
                Door->SetGoState(GO_STATE_READY);
        }
        SendEncounterUnit(ENCOUNTER_FRAME_REMOVE);

        boss_icecrown_citadelAI::Reset();
    }

    void AchieveCheck(Unit* pCaller)
    {
        if (pCaller->GetTypeId() == TYPEID_UNIT)
            m_uiPortalsUsed++;
    }

    void Aggro(Unit* pWho)
    {
        //m_BossEncounter = IN_PROGRESS;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER && !m_abSays[0] &&
            pWho->isTargetableForAttack() && m_creature->IsWithinDistInMap(pWho, 65.0f) &&
            icc::MeetsRequirementsForBoss(m_pInstance, TYPE_VALITHRIA))
        {
            Unit* LK = m_creature->SummonCreature(NPC_LICH_KING_VOICE,
                    m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0,
                    TEMPSUMMON_TIMED_DESPAWN, 5000);
            LK->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            LK->SetDisplayId(11686);
            DoScriptText(SAY_LICH_KING_GREEN_DRAGON, LK);
            m_abSays[0] = true;

            if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_ENTRANCE)) //LK talk -> Doors Close and players enter in combat
                Door->SetGoState(GO_STATE_READY);

            Events.RescheduleEvent(EVENT_INIT_SCRIPT, 3500);
            Events.RescheduleEvent(EVENT_BEGIN_FIGHT, 12500);
            Events.RescheduleEvent(EVENT_COMBAT_CHECK, 13*IN_MILLISECONDS, TIMER_COMBAT_CHECK);
            m_BossEncounter = IN_PROGRESS;
            SendEncounterUnit(ENCOUNTER_FRAME_ADD);
        }
    }

    void JustDied(Unit* pKiller)
    {
        m_bScriptStarted = false;
        DoScriptText(SAY_VALITHRIA_FAIL, m_creature);
        BroadcastEventToEntry(NPC_GREEN_DRAGON_COMBAT_TRIGGER, EVENT_DESPAWN, 5*IN_MILLISECONDS, 100.0f);
        if (m_BossEncounter != DONE)
        {
            m_BossEncounter = NOT_STARTED;
            m_creature->Respawn();
        }
        SendEncounterUnit(ENCOUNTER_FRAME_REMOVE);
    }

    void JustSummoned(Creature* pSummon)
    {
        SummonMgr.AddSummonToList(pSummon->GetObjectGuid());

        switch (pSummon->GetEntry())
        {
            case NPC_DREAM_PORTAL:
            case NPC_NIGHTMARE_PORTAL:
            case NPC_DREAM_PORTAL_PRE:
            case NPC_NIGHTMARE_PORTAL_PRE:
                pSummon->setFaction(FACTION_FRIENDLY);
                break;
            default:
                pSummon->SetInCombatWithZone();
                break;
        }
    }

    void SummonedCreatureJustDied(Creature* pSummon)
    {
        SummonMgr.RemoveSummonFromList(pSummon->GetObjectGuid());

        if (roll_chance_i(15))
            DoScriptText(SAY_VALITHRIA_SLAY_BAD, m_creature);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_BossEncounter != IN_PROGRESS)
            return;

        m_creature->SetTargetGuid(ObjectGuid());
        if (m_bScriptStarted && !m_abSays[2] && m_creature->GetHealthPercent() <= 25.0f)
        {
            DoScriptText(SAY_VALITHRIA_25, m_creature);
            m_abSays[2] = true;
        }

        if (m_bScriptStarted && !m_abSays[3] && m_creature->GetHealthPercent() >= 75.0f)
        {
            DoScriptText(SAY_VALITHRIA_75, m_creature);
            m_abSays[3] = true;
        }

        if (m_bScriptStarted && !m_abSays[4] && m_creature->GetHealthPercent() >= 99.7f)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_CORRUPTION);
            DoScriptText(SAY_VALITHRIA_WIN, m_creature);
            if (m_uiPortalsUsed == m_uiPortalsSummoned && m_uiPortalsSummoned > 0)  // Portal Jokey
                m_pInstance->SetData(DATA_ACHIEVEMENT_CHECK_VALITHRIA, 1);
            m_creature->CastSpell(m_creature, SPELL_DREAMWALKER_RAGE, false);
            if (Unit* pTrigger = SummonMgr.GetFirstFoundSummonWithId(NPC_GREEN_DRAGON_COMBAT_TRIGGER))
                pTrigger->CastSpell(pTrigger, SPELL_ACHIEVEMENT_CHECK, false);
            m_abSays[4] = true;
            Events.Reset();
            Events.ScheduleEvent(EVENT_DESPAWN, 5*IN_MILLISECONDS);
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_INIT_SCRIPT:
                    //spawn a combat trigger to keep track of players
                    SummonMgr.SummonCreature(NPC_GREEN_DRAGON_COMBAT_TRIGGER, m_creature->GetPositionX(), m_creature->GetPositionY(),
                        m_creature->GetPositionZ()+3.0f, 3.175f, TEMPSUMMON_MANUAL_DESPAWN, 20000);

                    Events.RescheduleEvent(EVENT_SUMMON_PORTALS, 41500); // first portal timer (taken from DBM database)
                    m_creature->SetHealthPercent(50.0f);
                    m_bScriptStarted = true;
                    m_pInstance->SetData(DATA_ACHIEVEMENT_CHECK_VALITHRIA, 0);

                    if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_LEFT_1))
                        Door->SetGoState(GO_STATE_ACTIVE);
                    if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_RIGHT_1))
                        Door->SetGoState(GO_STATE_ACTIVE);
                    if (!m_bIs10Man)
                    {
                        if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_LEFT_2))
                            Door->SetGoState(GO_STATE_ACTIVE);
                        if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_RIGHT_2))
                            Door->SetGoState(GO_STATE_ACTIVE);
                    }
                    break;
                case EVENT_BEGIN_FIGHT:
                    if (!m_abSays[1])
                    {
                        DoScriptText(SAY_VALITHRIA_AGGRO, m_creature);
                        m_abSays[1] = true;
                    }
                    break;
                case EVENT_COMBAT_CHECK:
                    if (Unit* CombatTrigger = SummonMgr.GetFirstFoundSummonWithId(NPC_GREEN_DRAGON_COMBAT_TRIGGER))
                        if (CombatTrigger->getThreatManager().getThreatList().size() == 0)
                        {
                            CombatTrigger->DealDamage(CombatTrigger, CombatTrigger->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                            Reset();
                        }
                    break;
                case EVENT_SUMMON_PORTALS:
                    if (!m_bIsHeroic)
                        DoScriptText(SAY_VALITHRIA_PORTALS, m_creature);

                    for (uint32 i = m_bIs10Man ? 3 : 6; i; i--)
                    {
                        float angle = frand(0.0f, m_bIs10Man ? M_PI_F/2.0f : 2.0f*M_PI_F);  // just a cone in front / all around
                        float x = SUMMON_PORTAL_RADIUS * cos(angle - M_PI_F / 4.0f) + m_creature->GetPositionX();
                        float y = SUMMON_PORTAL_RADIUS * sin(angle - M_PI_F / 4.0f) + m_creature->GetPositionY();
                        float z = m_creature->GetPositionZ() + 1.0f;
                        m_creature->CastSpell(x, y, z, m_bIsHeroic ? SPELL_SUMMON_NIGHTMARE_PORT : SPELL_SUMMON_DREAM_PORTAL, true);
                        m_uiPortalsSummoned++;
                    }
                    if (!m_bIsHeroic)
                        SummonMgr.UnsummonAllWithId(NPC_DREAM_CLOUD); // remove leftovers

                    if (!m_bHeroicPortalsSummoned)
                        for (uint32 i = MAX_SPAWN_POSITIONS; i ; i--)
                        {
                            float x, y, z;
                            m_creature->GetPosition(x, y, z);
                            z = 383.0f + frand(0.0f, 5.0f);
                            x += (SUMMON_PORTAL_RADIUS - 5.0f)*cos(i*(2*M_PI/MAX_SPAWN_POSITIONS));
                            y += (SUMMON_PORTAL_RADIUS - 5.0f)*sin(i*(2*M_PI/MAX_SPAWN_POSITIONS));
                            for (uint32 j = 2 ; j ; j--)
                            {
                                Creature* pSumm = SummonMgr.SummonCreature(m_bIsHeroic ? NPC_NIGHTMARE_CLOUD : NPC_DREAM_CLOUD, 
                                    x + frand(-5.0f, 5.0f), y + frand(-5.0f, 5.0f) ,z + frand(-5.0f, 5.0f), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 10*MINUTE*IN_MILLISECONDS);
                                pSumm->SetPhaseMask(16, true);
                            }
                        }
                    Events.RescheduleEvent(EVENT_SUMMON_PORTALS, PORTAL_TIMER);
                    if (m_bIsHeroic && !m_bHeroicPortalsSummoned) // on heroic, we dont resummon the clouds inside the "dream world"
                        m_bHeroicPortalsSummoned = true;
                    break;
                case EVENT_DESPAWN:
                    SummonMgr.UnsummonAll();
                    m_creature->SetPhaseMask(16, true);
                    m_BossEncounter = DONE;
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                default:
                    break;
            }
    }
};

bool GossipHello_mob_valithria_portal(Player *pPlayer, Creature* pCreature)
{
    pPlayer->CastSpell(pPlayer, SPELL_DREAM_STATE, true);
    if (pCreature->GetMap()->IsDungeon())
        if (ScriptedInstance *m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
            if (Creature *pValithria = GET_CREATURE(TYPE_VALITHRIA))
                if (boss_valithriaAI *valithriaAI = dynamic_cast<boss_valithriaAI*>(pValithria->AI()))
                    valithriaAI->AchieveCheck(pCreature);
    DespawnCreature(pCreature);
    return true;
};

struct MANGOS_DLL_DECL mob_green_dragon_combat_triggerAI: public Scripted_NoMovementAI
{
    EventManager Events;
    SummonManager SummonMgr;
    uint32 m_uiAbomWaveCount;
    uint32 m_uiBlazingWaveCount;
    uint32 m_uiSuppresorCount;
    ScriptedInstance* m_pInstance;
    bool m_bIs10man;

    mob_green_dragon_combat_triggerAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        SummonMgr(pCreature),
        m_uiAbomWaveCount(0),
        m_uiBlazingWaveCount(0),
        m_uiSuppresorCount(0),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_bIs10man(m_pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || m_pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
    {
        pCreature->SetLevel(83);
        pCreature->setFaction(FACTION_HOSTILE);
        pCreature->SetInCombatWithZone();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        Events.ScheduleEvent(EVENT_SUMMON_RISEN, RISEN_SUMMON_TIMER + urand(5, 6)*IN_MILLISECONDS, RISEN_SUMMON_TIMER);
        Events.ScheduleEvent(EVENT_SUMMON_SUPPRESSERS, 23*IN_MILLISECONDS, SUPPRESSER_SUMMON_TIMER);
        Events.ScheduleEvent(EVENT_SUMMON_ZOMBIE, ZOMBIE_SUMMON_TIMER, ZOMBIE_SUMMON_TIMER);
        Events.ScheduleEvent(EVENT_SUMMON_ABOMS, AbomTimers[m_uiAbomWaveCount++]*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_SUMMON_BLAZING, BlazingTimers[m_uiBlazingWaveCount++]*IN_MILLISECONDS);
    }

    void Reset()
    {
        Events.Reset();
        SummonMgr.UnsummonAll();
        m_uiAbomWaveCount = 0;
        m_uiBlazingWaveCount = 0;
        m_uiSuppresorCount = 0;
    }

    void SummonedCreatureJustDied(Creature* pSummon)
    {
        SummonMgr.RemoveSummonFromList(pSummon->GetObjectGuid());

        if (roll_chance_i(10))  //10% chance of having her say stuff for killing adds
            if (Creature* Valithria = GET_CREATURE(TYPE_VALITHRIA))
                DoScriptText(SAY_VALITHRIA_SLAY_BAD, Valithria);

        if (pSummon->GetEntry() == NPC_SUPPRESSER && m_uiSuppresorCount > 0)
            --m_uiSuppresorCount;
    }

    void JustSummoned(Creature* pSummon)
    {
        pSummon->SetInCombatWithZone();
    }

    void JustDied(Unit* pSlayer)
    {
        SummonMgr.UnsummonAll();
    }

    void SummonAdd(uint32 entry)
    {
        uint32 loc = urand(0, m_bIs10man ? 1 : 3);
        SummonMgr.SummonCreature(entry, locations[loc][0], locations[loc][1], m_creature->GetPositionZ()+1.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 6*IN_MILLISECONDS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() ||  !m_creature->getVictim())
            return;

        if (m_pInstance->GetData(TYPE_VALITHRIA) == DONE) // apparently dead mobs dont sent events
        {
            SummonMgr.UnsummonAll();
            DespawnCreature(m_creature);
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_SUMMON_ZOMBIE:
                    SummonAdd(NPC_BLISTERING_ZOMBIE);
                    break;
                case EVENT_SUMMON_SUPPRESSERS:
                {
                    uint32 max_count = m_bIs10man ? 5 : 7;
                    if (m_uiSuppresorCount < max_count)
                    {
                        for (uint32 count = max_count - m_uiSuppresorCount; count; --count)
                            SummonAdd(NPC_SUPPRESSER);
                        m_uiSuppresorCount = max_count;
                    }
                    break;
                }
                case EVENT_SUMMON_RISEN:
                    SummonAdd(NPC_RISEN_ARCHMAGE);
                    break;
                case EVENT_SUMMON_ABOMS:
                    SummonAdd(NPC_GLUTTONOUS_ABOMINATION);
                    Events.ScheduleEvent(EVENT_SUMMON_ABOMS, AbomTimers[m_uiAbomWaveCount]*IN_MILLISECONDS);
                    if (m_uiAbomWaveCount < sizeof(AbomTimers)/sizeof(uint32) - 1)
                        m_uiAbomWaveCount++;
                    break;
                case EVENT_SUMMON_BLAZING:
                    SummonAdd(NPC_BLAZING_SKELETON);
                    Events.ScheduleEvent(EVENT_SUMMON_BLAZING, BlazingTimers[m_uiBlazingWaveCount]*IN_MILLISECONDS);
                    if (m_uiBlazingWaveCount < sizeof(BlazingTimers)/sizeof(uint32) - 1)
                        m_uiBlazingWaveCount++;
                    break;
                case EVENT_DESPAWN:
                    SummonMgr.UnsummonAll();
                    DespawnCreature(m_creature);
                    break;
                default:
                    break;
            }
    }
};

struct MANGOS_DLL_DECL mob_valithria_pre_portalAI: public ScriptedAI
{
    uint32 m_uiTimer;

    mob_valithria_pre_portalAI(Creature *pCreature):
        ScriptedAI(pCreature),
        m_uiTimer(15*IN_MILLISECONDS)
    {
        pCreature->CastSpell(pCreature, pCreature->GetEntry() == NPC_DREAM_PORTAL_PRE ? SPELL_DREAM_PORTAL_PRE : SPELL_NIGHTMARE_PORTAL_PRE, false);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Reset() {}

    void JustSummoned(Creature* pSummon)
    {
        pSummon->CastSpell(pSummon, pSummon->GetEntry() == NPC_DREAM_PORTAL ? SPELL_DREAM_PORTAL_VIS : SPELL_NIGHTMARE_PORTAL_VIS, false);
        pSummon->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiTimer <= uiDiff)
        {
            m_creature->SummonCreature(m_creature->GetEntry() == NPC_DREAM_PORTAL_PRE ? NPC_DREAM_PORTAL : NPC_NIGHTMARE_PORTAL,
                    m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(),
                    TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS);
            DespawnCreature(m_creature);
        }
        else
            m_uiTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_valithria_cloudAI: public ScriptedAI
{
    bool m_bIsUsed;

    mob_valithria_cloudAI(Creature *pCreature):
        ScriptedAI(pCreature),
        m_bIsUsed(false)
    {
        Reset();
    }

    void Reset()
    {
        m_creature->setFaction(FACTION_HOSTILE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        m_bIsUsed = false;
        m_creature->CastSpell(m_creature, m_creature->GetEntry()== NPC_DREAM_CLOUD ? SPELL_DREAM_CLOUD_VIS : SPELL_NIGHTMARE_CLOUD_VIS, false);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (m_bIsUsed || !pWho || pWho->GetTypeId() != TYPEID_PLAYER || !pWho->isTargetableForAttack() || !m_creature->IsWithinDistInMap(pWho, 2.0f))
            return;

        switch (m_creature->GetEntry())
        {
            case NPC_DREAM_CLOUD:
                DoCast(m_creature, SPELL_EMERALD_VIGOR, false);
                m_bIsUsed = true;
                break;
            case NPC_NIGHTMARE_CLOUD:
                DoCast(m_creature, SPELL_TWISTING_NIGHTMARES, false);
                m_bIsUsed = true;
                break;
            default:
                return;
        }
        m_creature->ForcedDespawn(1500);
    }

    void UpdateAI(uint32 const uiDiff)
    {
    }
};

#define FIREBALL_TIMER      10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define LAY_WASTE_TIMER     20*IN_MILLISECONDS, 40*IN_MILLISECONDS
#define FROSTBOLT_TIMER     10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define MANA_VOID_TIMER     25*IN_MILLISECONDS, 45*IN_MILLISECONDS
#define FROST_COLUMN_TIMER  25*IN_MILLISECONDS, 45*IN_MILLISECONDS
#define GUT_SPRAY_TIMER     15*IN_MILLISECONDS, 30*IN_MILLISECONDS

struct MANGOS_DLL_DECL mob_valithria_addAI: public ScriptedAI
{
    EventManager Events;
    SummonManager SummonMgr;
    ScriptedInstance* m_pInstance;

    mob_valithria_addAI(Creature* pCreature):
        ScriptedAI(pCreature),
        SummonMgr(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
    {
        switch(pCreature->GetEntry())
        {
           case NPC_BLISTERING_ZOMBIE:
               pCreature->CastSpell(pCreature, SPELL_CORROSION_PASSIVE, false);
               break;
           case NPC_ROT_WORM:
               pCreature->CastSpell(pCreature, SPELL_ROT_PASSIVE, false);
               pCreature->CastSpell(pCreature, SPELL_ROT_SPAWN, false);
               break;
           case NPC_SUPPRESSER:
               if (Creature* Valithria = GET_CREATURE(TYPE_VALITHRIA))
               {
                   pCreature->AddThreat(Valithria, 1000000.0f);
                   //pCreature->GetMotionMaster()->MoveChase(Valithria, 15.0f);
               }
               break;
           default:
               break;
        }
    }
    
    void Reset()
    {
        SummonMgr.UnsummonAll();
        DespawnCreature(m_creature);
    }

    void SummonedCreatureJustDied(Creature* pSummon)
    {
        SummonMgr.RemoveSummonFromList(pSummon->GetObjectGuid());
        if (roll_chance_i(10)) //10% chance of having her say stuff for killing adds
            if (Creature* Valithria = GET_CREATURE(TYPE_VALITHRIA))
                DoScriptText(SAY_VALITHRIA_SLAY_BAD, Valithria);
    }

    void JustSummoned(Creature* pSummon)
    {
        SummonMgr.AddSummonToList(pSummon->GetObjectGuid());

        switch (m_creature->GetEntry())
        {
            case NPC_MANA_VOID:
            case NPC_COLUMN_OF_FROST:
                break;
            default:
                pSummon->SetInCombatWithZone();
                break;
        }
    }

    void KilledUnit(Unit* pKilled)
    {
        if (pKilled->GetTypeId() != TYPEID_PLAYER)
            return;
        if (roll_chance_i(50)) //50% chance of having her say stuff for killing players
            if (Creature* Valithria = GET_CREATURE(TYPE_VALITHRIA))
                DoScriptText(SAY_VALITHRIA_SLAY_GOOD, Valithria);
    }

    void Aggro(Unit* pWho)
    {
        switch (m_creature->GetEntry())
        {
            case NPC_BLAZING_SKELETON:
                Events.ScheduleEventInRange(EVENT_FIREBALL, 0, 0, FIREBALL_TIMER);
                Events.ScheduleEventInRange(EVENT_LAY_WASTE, LAY_WASTE_TIMER, LAY_WASTE_TIMER);
                break;
            case NPC_RISEN_ARCHMAGE:
                Events.ScheduleEventInRange(EVENT_FROSTBOLT, FROSTBOLT_TIMER, FROSTBOLT_TIMER);
                Events.ScheduleEventInRange(EVENT_MANA_VOID, MANA_VOID_TIMER, MANA_VOID_TIMER);
                Events.ScheduleEventInRange(EVENT_COLUMN_OF_FROST, FROST_COLUMN_TIMER, FROST_COLUMN_TIMER);
                break;
            case NPC_GLUTTONOUS_ABOMINATION:
                Events.ScheduleEventInRange(EVENT_GUT_SPRAY, GUT_SPRAY_TIMER, GUT_SPRAY_TIMER);
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() ||  !m_creature->getVictim())
            return;

        if (m_creature->GetEntry() == NPC_BLISTERING_ZOMBIE && m_creature->GetHealthPercent() < 10.0f)
        {
            DoCastSpellIfCan(m_creature, SPELL_ACID_BURST);
            Events.ScheduleEvent(EVENT_DIE, 2*IN_MILLISECONDS);
        }
    
        if (m_creature->GetEntry() == NPC_SUPPRESSER)
            if (Creature* Valithria = GET_CREATURE(TYPE_VALITHRIA))
                if (m_creature->IsWithinDist(Valithria, 20.0f))
                {
                    m_creature->SetTargetGuid(Valithria->GetObjectGuid());
                    m_creature->CastSpell(Valithria, SPELL_SUPPRESSION, true);
                }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_FIREBALL:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL);
                    break;
                case EVENT_LAY_WASTE:
                    DoCastSpellIfCan(m_creature, SPELL_LAY_WASTE);
                    break;
                case EVENT_FROSTBOLT:
                    DoCastSpellIfCan(m_creature, SPELL_FROSTBOLT_VOLLEY);
                    break;
                case EVENT_MANA_VOID:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        SummonMgr.SummonCreatureAt(pTarget, NPC_MANA_VOID);
                    break;
                case EVENT_COLUMN_OF_FROST:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        SummonMgr.SummonCreatureAt(pTarget, NPC_COLUMN_OF_FROST);
                    break;
                case EVENT_GUT_SPRAY:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_GUT_SPRAY);
                    break;
                case EVENT_DIE:
                    m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                    break;
                default:
                    break;
            }

        if (m_creature->GetEntry() != NPC_SUPPRESSER)
            DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pSlayer)
    {
        if (m_creature->GetEntry() == NPC_GLUTTONOUS_ABOMINATION)   // since npc's cant casts spells on death... i make the killer cast the spell (summon-worm spell)
            pSlayer->CastSpell(pSlayer, SPELL_ROT_WORM, true);      // TODO: make abom cast the spell
    }

};

struct MANGOS_DLL_DECL mob_valithria_add_nmAI: public Scripted_NoMovementAI
{
    EventManager Events;
    ScriptedInstance* m_pInstance;

    mob_valithria_add_nmAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
    {
        switch(pCreature->GetEntry())
        {
           case NPC_MANA_VOID:
               pCreature->SetDisplayId(11686);
               pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
               pCreature->CastSpell(pCreature, SPELL_MANA_VOID_PASSIVE, false);
               Events.ScheduleEvent(EVENT_DIE, 30*IN_MILLISECONDS);
               break;
           case NPC_COLUMN_OF_FROST:
               pCreature->SetDisplayId(11686);
               pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
               pCreature->CastSpell(pCreature, SPELL_COLUMN_OF_FROST_VIS, false);
               Events.ScheduleEvent(EVENT_BLAST, 2*IN_MILLISECONDS);
               break;
           default:
               break;
        }
    }

    void Reset() {}

    void KilledUnit(Unit* pKilled)
    {
        if (pKilled->GetTypeId() != TYPEID_PLAYER)
            return;
        if (roll_chance_i(50)) //50% chance of having her say stuff for killing players
            if (Creature* Valithria = GET_CREATURE(TYPE_VALITHRIA))
                DoScriptText(SAY_VALITHRIA_SLAY_GOOD, Valithria);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        m_creature->SetTargetGuid(ObjectGuid());    // prevent void/column from targetting ppl or moving

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BLAST:
                    DoCastSpellIfCan(m_creature, SPELL_COLUMN_OF_FROST_DMG);
                    Events.ScheduleEvent(EVENT_DIE, 5*IN_MILLISECONDS);
                    break;
                case EVENT_DIE:
                    DespawnCreature(m_creature);
                    break;
                default:
                    break;
            }
    }
};

void AddSC_boss_valithria_dreamwalker()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_valithria);
    REGISTER_SCRIPT(mob_valithria_cloud);
    REGISTER_SCRIPT(mob_valithria_pre_portal);
    REGISTER_SCRIPT(mob_green_dragon_combat_trigger);
    REGISTER_SCRIPT(mob_valithria_add);
    REGISTER_SCRIPT(mob_valithria_add_nm);

    newscript = new Script;
    newscript->Name = "mob_valithria_portal";
    newscript->pGossipHello = &GossipHello_mob_valithria_portal;
    newscript->RegisterSelf();
}
