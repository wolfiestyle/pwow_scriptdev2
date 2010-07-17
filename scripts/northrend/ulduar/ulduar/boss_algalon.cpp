/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss Algalon the Observer
SD%Complete: 10%
SDComment:
SDCategory: Ulduar
EndScriptData */

// NOT DONE: 
//Big Bang (entire sequence(timer,spell,black hole hiding plyrs etc.)), 
//'star effect' at beginning, remove at end

#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
    // Algalon
    SPELL_ASCEND_TO_HEAVENS = 64487,  // oneshots raid, equivalent to berserk (after 8 mins)
    SPELL_BERSERK           = 47008,  // after 6 mins
    SPELL_QUANTUM_STRIKE    = 64395,
    SPELL_QUANTUM_STRIKE_H  = 64592,
    SPELL_PHASE_PUNCH       = 64412,
    SPELL_COSMIC_SMASH      = 62304,  // need to find correct way of doing this
    SPELL_COSMIC_SMASH_H    = 64597,

    // Collapsing Star
    BLACK_HOLE_EXPLOSION    = 64122,
    BLACK_HOLE_EXPLOSION_H  = 65108,

    // Black Hole
    SPELL_BLACK_HOLE_VISUAL = 62003,  // explosion effect
    SPELL_SUMMON_UNLEASHED_DARK_MATTER = 64450
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_ASCEND_TO_HEAVENS,
    EVENT_QUANTUM_STRIKE,
    EVENT_PHASE_PUNCH,
    EVENT_COSMIC_SMASH,
    EVENT_COSMIC_SMASH_EFFECT,
    EVENT_SUMMON_STARS, //(only phase 1)
    EVENT_SUMMON_CONSTELLATION
};

enum SayTexts
{
    SAY_ENTER_1         = -1300000,
    SAY_ENTER_2         = -1300001,
    SAY_ENTER_3         = -1300002,

    SAY_FIRST_AGGRO     = -1300003,
    SAY_AGGRO           = -1300004,

    SAY_SUMMON_STARS    = -1300005,
    SAY_BIG_BANG_1      = -1300006,
    SAY_BIG_BANG_2      = -1300007,

    SAY_START_PHASE_2   = -1300008,

    SAY_KILLED_PLAYER_1 = -1300009,
    SAY_KILLED_PLAYER_2 = -1300010,

    SAY_BERSERK         = -1300011,

    SAY_DEFEATED_1      = -1300012,
    SAY_DEFEATED_2      = -1300013,
    SAY_DEFEATED_3      = -1300014,
    SAY_DEFEATED_4      = -1300015,
    SAY_DEFEATED_5      = -1300016,

    SAY_END_1           = -1300017,
    SAY_END_2           = -1300018,
    SAY_END_3           = -1300019,

    EMOTE_COSMIC_SMASH  = -1300020,
    EMOTE_BIG_BANG      = -1300021,
    EMOTE_BERSERK       = -1300022
};

enum SayEvents
{
    SAYEVENT_END1,
    SAYEVENT_END2,
    SAYEVENT_END3,

    SAYEVENT_ENTER1,
    SAYEVENT_ENTER2,
    SAYEVENT_ENTER3,

    SAYEVENT_DEFEATED1,
    SAYEVENT_DEFEATED2,
    SAYEVENT_DEFEATED3,
    SAYEVENT_DEFEATED4,
    SAYEVENT_DEFEATED5
};

enum AddIds
{
    NPC_COLLAPSING_STAR         = 32955,
    NPC_BLACK_HOLE              = 32953,
    NPC_LIVING_CONSTELLATION    = 33052,
    NPC_UNLEASHED_DARK_MATTER   = 34097
};

enum Phases
{
    PHASE_NOTSTARTED,
    PHASE_1,
    PHASE_2,
    PHASE_DEAD,
    PHASE_STARTED_NOCOMBAT,
    PHASE_DESPAWN
};

enum SummonLimits
{
    MAX_STARS = 4,
    MAX_CONSTELLATIONS = 10
};

#define BERSERK_TIMER               6*MINUTE*IN_MILLISECONDS
#define QUANTUM_STRIKE_TIMER        urand(3, 5)*IN_MILLISECONDS
#define PHASE_PUNCH_TIMER           urand(14, 15)*IN_MILLISECONDS
#define ASCEND_TO_HEAVENS_TIMER     8*MINUTE*IN_MILLISECONDS
#define DESPAWN_TIMER               HOUR*IN_MILLISECONDS
#define COSMIC_SMASH_TIMER          25*IN_MILLISECONDS
#define SUMMON_STARS_TIMER          15*IN_MILLISECONDS
#define SUMMON_CONSTELLATION_TIMER  urand(50, 60)*IN_MILLISECONDS
#define DARK_MATTER_TIMER           30*IN_MILLISECONDS

typedef std::multimap<uint32 /*entry*/, uint64 /*guid*/> GuidMap;
typedef std::pair<GuidMap::iterator, GuidMap::iterator> GuidMapRange;

struct MANGOS_DLL_DECL boss_algalonAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;
    EventManager SayEvents;  //for speeches that encompass > 1 line (eg- despawn, killed)
    uint32 m_uiDespawnTimer; //not using EventManager b/c needed in all phases, and can not be affected by EventManager::Reset()
    bool IsFirstTime;
    bool HasSaidBeginningStuff;
    bool IsPhase2;
    bool CombatStopped;

    GuidMap CurrSummons;    //generalized summon storage

    boss_algalonAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_ALGALON),
        m_uiDespawnTimer(DESPAWN_TIMER),
        IsFirstTime(true),
        HasSaidBeginningStuff(false)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        UnSummonAllCreatures();
        SayEvents.Reset();
        events.Reset();
        IsPhase2 = false;
        CombatStopped = false;
        if(!IsFirstTime)
            events.SetPhase(PHASE_STARTED_NOCOMBAT);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    /*void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }*/

    void KilledUnit(Unit* victim)
    {
        if(victim->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER_1 : SAY_KILLED_PLAYER_2, m_creature);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(!HasSaidBeginningStuff && who && who->GetTypeId() == TYPEID_PLAYER)
        {
            SayEvents.Reset();
            SayEvents.ScheduleEvent(SAYEVENT_ENTER1, 0);
            SayEvents.ScheduleEvent(SAYEVENT_ENTER2, 8*IN_MILLISECONDS);
            SayEvents.ScheduleEvent(SAYEVENT_ENTER3, (8+6)*IN_MILLISECONDS);
            HasSaidBeginningStuff = true;
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if(uiDamage > m_creature->GetHealth() && m_pInstance->GetData(m_uiBossEncounterId) == IN_PROGRESS)
        {
            uiDamage = 0;
            if (m_pInstance)
                m_pInstance->SetData(m_uiBossEncounterId, DONE);

            DoStopAttack();
            CombatStopped = true;
            m_creature->RemoveAllAuras();
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->setFaction(35);

            UnSummonAllCreatures();

            SayEvents.Reset();
            SayEvents.ScheduleEvent(SAYEVENT_DEFEATED1, 0);
            SayEvents.ScheduleEvent(SAYEVENT_DEFEATED2, (40)*IN_MILLISECONDS);
            SayEvents.ScheduleEvent(SAYEVENT_DEFEATED3, (40+15)*IN_MILLISECONDS);
            SayEvents.ScheduleEvent(SAYEVENT_DEFEATED4, (40+15+10)*IN_MILLISECONDS);
            SayEvents.ScheduleEvent(SAYEVENT_DEFEATED5, (40+15+10+10)*IN_MILLISECONDS);
        }
    }

    void Aggro(Unit* pWho)
    {
        events.Reset();
        events.RescheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        events.RescheduleEvent(EVENT_QUANTUM_STRIKE, QUANTUM_STRIKE_TIMER);
        events.RescheduleEvent(EVENT_PHASE_PUNCH, PHASE_PUNCH_TIMER);
        events.RescheduleEvent(EVENT_ASCEND_TO_HEAVENS, ASCEND_TO_HEAVENS_TIMER);
        events.RescheduleEvent(EVENT_COSMIC_SMASH, COSMIC_SMASH_TIMER);
        events.RescheduleEvent(EVENT_SUMMON_STARS, SUMMON_STARS_TIMER);
        events.RescheduleEvent(EVENT_SUMMON_CONSTELLATION, SUMMON_CONSTELLATION_TIMER);

        if (IsFirstTime)
        {
            DoScriptText(SAY_FIRST_AGGRO, m_creature);
            IsFirstTime = false;
        }
        else
            DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void JustSummoned(Creature *pSummon)
    {
        if (!pSummon)
            return;
        CurrSummons.insert(std::make_pair(pSummon->GetEntry(), pSummon->GetGUID()));
        if (pSummon->GetEntry() == NPC_LIVING_CONSTELLATION)
            pSummon->SetInCombatWithZone();
    }

    void SummonedCreatureJustDied(Creature *pSummon)
    {
        if (!pSummon)
            return;
        GuidMapRange range = CurrSummons.equal_range(pSummon->GetEntry());
        if (range.first == range.second)
            return;
        uint64 guid = pSummon->GetGUID();
        for (GuidMap::iterator i = range.first; i != range.second; ++i)
            if (i->second == guid)
            {
                CurrSummons.erase(i);
                break;
            }
    }

    void SummonedCreatureDespawn(Creature *pSummon)
    {
        SummonedCreatureJustDied(pSummon);
    }

    void GetRandomPointInCircle(float max_rad, float &x, float &y)
    {
        float ang = 2*M_PI * rand_norm();
        float rad = max_rad * sqrt(rand_norm());
        x = rad * cos(ang);
        y = rad * sin(ang);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //despawn after 1 hour timer
        if(!IsFirstTime)
        {
            if(m_uiDespawnTimer < uiDiff)
            {
                IsFirstTime = true;
                DoStopAttack();
                CombatStopped = true;
                m_creature->RemoveAllAuras();
                m_creature->setFaction(35); //friendly
                events.Reset();

                //schedule says
                SayEvents.Reset();
                SayEvents.ScheduleEvent(SAYEVENT_END1, 0);
                SayEvents.ScheduleEvent(SAYEVENT_END2, (10)*IN_MILLISECONDS);
                SayEvents.ScheduleEvent(SAYEVENT_END3, (10+8)*IN_MILLISECONDS);
            }
            else
                m_uiDespawnTimer -= uiDiff;
        }

        SayEvents.Update(uiDiff);
        while (uint32 eventId = SayEvents.ExecuteEvent())
        {
            switch (eventId)
                {
                case SAYEVENT_END1:
                    DoScriptText(SAY_END_1, m_creature);
                    break;
                case SAYEVENT_END2:
                    DoScriptText(SAY_END_2, m_creature);
                    break;
                case SAYEVENT_END3:
                    DoScriptText(SAY_END_3, m_creature);
                    m_creature->SetVisibility(VISIBILITY_OFF);
                    EnterEvadeMode();
                    break;

                case SAYEVENT_ENTER1:
                    DoScriptText(SAY_ENTER_1, m_creature);
                    break;
                case SAYEVENT_ENTER2:
                    DoScriptText(SAY_ENTER_2, m_creature);
                    break;
                case SAYEVENT_ENTER3:
                    DoScriptText(SAY_ENTER_3, m_creature);
                    HasSaidBeginningStuff = true;
                    break;

                case SAYEVENT_DEFEATED1:
                    DoScriptText(SAY_DEFEATED_1, m_creature);
                    break;
                case SAYEVENT_DEFEATED2:
                    DoScriptText(SAY_DEFEATED_2, m_creature);
                    break;
                case SAYEVENT_DEFEATED3:
                    DoScriptText(SAY_DEFEATED_3, m_creature);
                    break;
                case SAYEVENT_DEFEATED4:
                    DoScriptText(SAY_DEFEATED_4, m_creature);
                    break;
                case SAYEVENT_DEFEATED5:
                    DoScriptText(SAY_DEFEATED_5, m_creature);
                    m_creature->SetVisibility(VISIBILITY_OFF);
                    EnterEvadeMode();
                    break;

                default:
                    break;
            }
        }

        if (CombatStopped || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (IsOutOfCombatArea(m_creature))
        {
            EnterEvadeMode();
            return;
        }

        //check for phase 2 at 20% health
        if(!IsPhase2)
        {
            if(m_creature->GetHealthPercent() < 20.0f)
            {
                IsPhase2 = true;
                DoScriptText(SAY_START_PHASE_2, m_creature);
                events.CancelEvent(EVENT_SUMMON_STARS);
                events.CancelEvent(EVENT_SUMMON_CONSTELLATION);
                UnSummonAllCreatures();

                //4 black holes in square pattern
                const float pos[4][2] = {{10,0}, {-10,0}, {0,10}, {0,-10}};
                for (int i = 0; i < 4; i++)
                    DoSpawnCreature(NPC_BLACK_HOLE, pos[i][0], pos[i][1], 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 1000);

                DoScriptText(SAY_SUMMON_STARS, m_creature);
                events.ScheduleEvent(EVENT_SUMMON_STARS,SUMMON_STARS_TIMER);
            }
        }

        //switch targets to next top aggro if phase punch stacks >= 5
        if (Unit *TopAggro = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
        {
            Aura *aur = TopAggro->GetAura(SPELL_PHASE_PUNCH, EFFECT_INDEX_0);
            if (aur && aur->GetStackAmount() >= 5)
                if (Unit *NextTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1))
                    m_creature->Attack(NextTarget, true);
        }

        events.Update(uiDiff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoCast(m_creature, SPELL_BERSERK);
                    //events.ScheduleEvent(EVENT_BERSERK, 10*IN_MILLISECONDS);
                    DoScriptText(SAY_BERSERK, m_creature);
                    DoScriptText(EMOTE_BERSERK, m_creature);
                    break;
                case EVENT_QUANTUM_STRIKE:
                    DoCast(m_creature->getVictim(), HEROIC(SPELL_QUANTUM_STRIKE, SPELL_QUANTUM_STRIKE_H));
                    events.ScheduleEvent(EVENT_QUANTUM_STRIKE, QUANTUM_STRIKE_TIMER);
                    break;
                case EVENT_PHASE_PUNCH:
                    DoCast(m_creature->getVictim(), SPELL_PHASE_PUNCH);
                    events.ScheduleEvent(EVENT_PHASE_PUNCH, PHASE_PUNCH_TIMER);
                    break;
                case EVENT_ASCEND_TO_HEAVENS:
                    DoCast(m_creature->getVictim(), SPELL_ASCEND_TO_HEAVENS);
                    events.ScheduleEvent(EVENT_ASCEND_TO_HEAVENS, 5*IN_MILLISECONDS); //just in case they survive... hehehe
                    break;
                case EVENT_SUMMON_STARS:
                    if(!IsPhase2)
                    {
                        size_t n_stars = CurrSummons.count(NPC_COLLAPSING_STAR);
                        if (n_stars > MAX_STARS) //shouldnt happen
                            break;
                        for(size_t i = 0; i < MAX_STARS-n_stars; i++) //respawn missing stars
                        {
                            float x, y;
                            GetRandomPointInCircle(30.0f, x, y);
                            DoSpawnCreature(NPC_COLLAPSING_STAR, x, y, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 1000);
                        }
                        if (MAX_STARS-n_stars > 0) //summoned at least one
                            DoScriptText(SAY_SUMMON_STARS, m_creature);
                        events.ScheduleEvent(EVENT_SUMMON_STARS, SUMMON_STARS_TIMER);
                    }
                    break;
                case EVENT_SUMMON_CONSTELLATION:
                    if (CurrSummons.count(NPC_LIVING_CONSTELLATION) > MAX_CONSTELLATIONS) //just to avoid infinite summons
                        break;
                    for(int i=0; i<3; i++)
                    {
                        float x, y;
                        GetRandomPointInCircle(30.0f, x, y);
                        DoSpawnCreature(NPC_LIVING_CONSTELLATION, x, y, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 1000);
                    }
                    events.ScheduleEvent(EVENT_SUMMON_CONSTELLATION, SUMMON_CONSTELLATION_TIMER);
                    break;
                case EVENT_COSMIC_SMASH:
                    //TODO: mark destination point (don't know the spell)
                    DoScriptText(EMOTE_COSMIC_SMASH, m_creature);
                    events.ScheduleEvent(EVENT_COSMIC_SMASH_EFFECT, 2*IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_COSMIC_SMASH, COSMIC_SMASH_TIMER);
                    break;
                case EVENT_COSMIC_SMASH_EFFECT:
                    for (int i = 0; i < HEROIC(1, 3); i++)
                    {
                        float x, y;
                        GetRandomPointInCircle(30.0f, x, y);
                        m_creature->CastSpell(m_creature->GetPositionX()+x, m_creature->GetPositionY()+y, m_creature->GetPositionZ()+0.5f,
                                HEROIC(SPELL_COSMIC_SMASH, SPELL_COSMIC_SMASH_H), true);
                    }
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    void UnSummonAllCreatures()
    {
        for (GuidMap::const_iterator i = CurrSummons.begin(); i != CurrSummons.end(); ++i)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(i->second))
                pSummon->ForcedDespawn();
        CurrSummons.clear();

        //not summoned by Algalon himself
        std::list<Creature*> DarkMatter;
        GetCreatureListWithEntryInGrid(DarkMatter, m_creature, NPC_UNLEASHED_DARK_MATTER, 100.0f);
        for (std::list<Creature*>::const_iterator i = DarkMatter.begin(); i!= DarkMatter.end(); ++i)
            (*i)->ForcedDespawn();
    }

    void CreateBlackHole(Creature* Star)
    {
        if (Creature *BlackHole = m_creature->SummonCreature(NPC_BLACK_HOLE, Star->GetPositionX(), Star->GetPositionY(), Star->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 1000))
            Star->ForcedDespawn();
    }

    void CancelBlackHole(Creature* Constellation, Creature* BlackHole)
    {
        Constellation->ForcedDespawn();
        BlackHole->ForcedDespawn();
    }
};

CreatureAI* GetAI_boss_algalon(Creature* pCreature)
{
    return new boss_algalonAI(pCreature);
}

struct MANGOS_DLL_DECL mob_collapsing_starAI : public Scripted_NoMovementAI
{
    Creature* Algalon;
    boss_algalonAI* AlgalonAI;
    bool m_bIsRegularMode;
    uint32 HealthDecrease;
    uint32 HealthTimer;
    ScriptedInstance* m_pInstance;

    mob_collapsing_starAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        AlgalonAI(NULL)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Algalon = GET_CREATURE(TYPE_ALGALON);
        if (Algalon)
            AlgalonAI = dynamic_cast<boss_algalonAI*>(Algalon->AI());
        Reset();
    }

    void Reset()
    {
        m_creature->SetConfused(true);
        HealthDecrease = m_creature->GetMaxHealth() / 100;
        HealthTimer = 1*IN_MILLISECONDS;
        m_creature->SetInCombatWithZone();
    }

    void OnDeath()
    {
        DoCast(m_creature, HEROIC(BLACK_HOLE_EXPLOSION, BLACK_HOLE_EXPLOSION_H), true);
        if (AlgalonAI)
            AlgalonAI->CreateBlackHole(m_creature);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage >= m_creature->GetHealth())
            OnDeath();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(HealthTimer < uiDiff)
        {
            if(m_creature->GetHealth() < HealthDecrease)
            {
                OnDeath();
                m_creature->SetHealth(0);
                m_creature->setDeathState(JUST_DIED);
            }
            else
                m_creature->SetHealth(m_creature->GetHealth() - HealthDecrease);
            HealthTimer = 1*IN_MILLISECONDS;
        }
        else
            HealthTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_collapsing_starAI(Creature* pCreature)
{
    return new mob_collapsing_starAI(pCreature);
}

struct MANGOS_DLL_DECL mob_black_holeAI : public ScriptedAI
{
    Creature* Algalon;
    boss_algalonAI* AlgalonAI;
    bool m_bIsRegularMode;
    bool IsPhase2;
    uint32 UnleashedDarkMatterTimer;
    ScriptedInstance* m_pInstance;

    mob_black_holeAI(Creature* pCreature):
        ScriptedAI(pCreature),
        AlgalonAI(NULL)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Algalon = GET_CREATURE(TYPE_ALGALON);
        if (Algalon)
            AlgalonAI = dynamic_cast<boss_algalonAI*>(Algalon->AI());
        Reset();
    }

    void Reset()
    {
        IsPhase2 = AlgalonAI ? AlgalonAI->IsPhase2 : false;
        UnleashedDarkMatterTimer = 10*IN_MILLISECONDS;
        DoCast(m_creature, SPELL_BLACK_HOLE_VISUAL, true);
    }

    void JustSummoned(Creature *pSummon)
    {
        if (pSummon)
            pSummon->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (IsPhase2)
        {
            if(UnleashedDarkMatterTimer < uiDiff)
            {
                DoCast(m_creature, SPELL_SUMMON_UNLEASHED_DARK_MATTER, true);
                UnleashedDarkMatterTimer = DARK_MATTER_TIMER;
            }
            else
                UnleashedDarkMatterTimer -= uiDiff;
        }
        else if (AlgalonAI)
        {
            GuidMapRange constellations = AlgalonAI->CurrSummons.equal_range(NPC_LIVING_CONSTELLATION);
            for (GuidMap::const_iterator i = constellations.first; i != constellations.second; ++i)
            {
                Creature *who = m_creature->GetMap()->GetCreature(i->second);
                if(who && m_creature->IsWithinDistInMap(who, 3.0f))
                {
                    AlgalonAI->CancelBlackHole(who, m_creature);
                    break;
                }
            }
        }
    }
}; //dont we just love these large end bracket sequences at the end of functions? --> python solves that not using brackets at all.

CreatureAI* GetAI_mob_black_holeAI(Creature* pCreature)
{
    return new mob_black_holeAI(pCreature);
}

void AddSC_boss_algalon()
{
    Script *NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_algalon";
    NewScript->GetAI = &GetAI_boss_algalon;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_collapsing_star";
    NewScript->GetAI = &GetAI_mob_collapsing_starAI;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_black_hole";
    NewScript->GetAI = &GetAI_mob_black_holeAI;
    NewScript->RegisterSelf();
}
