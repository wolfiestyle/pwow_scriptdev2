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
//Cosmic Smash (what spell it is), 
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
    SPELL_COSMIC_SMASH      = 62301,  // need to find correct way of doing this
    SPELL_COSMIC_SMASH_H    = 64598,

    // Collapsing Star
    BLACK_HOLE_EXPLOSION    = 64122,
    BLACK_HOLE_EXPLOSION_H  = 65108,

    // Black Hole
    SPELL_SUMMON_UNLEASHED_DARK_MATTER = 64450
};

enum Events
{
    EVENT_BERSERK,
    EVENT_ASCEND_TO_HEAVENS,
    EVENT_QUANTUM_STRIKE,
    EVENT_PHASE_PUNCH,
    EVENT_COSMIC_SMASH,
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

#define BERSERK_TIMER               6*MINUTE*IN_MILISECONDS
#define QUANTUM_STRIKE_TIMER        urand(3, 5)*IN_MILISECONDS
#define PHASE_PUNCH_TIMER           urand(14, 15)*IN_MILISECONDS
#define ASCEND_TO_HEAVENS_TIMER     8*MINUTE*IN_MILISECONDS
#define DESPAWN_TIMER               HOUR*IN_MILISECONDS
#define COSMIC_SMASH_TIMER          25*IN_MILISECONDS
#define SUMMON_STARS_TIMER          15*IN_MILISECONDS
#define SUMMON_CONSTELLATION_TIMER  urand(50, 60)*IN_MILISECONDS
#define DARK_MATTER_TIMER           30*IN_MILISECONDS

struct MANGOS_DLL_DECL boss_algalonAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    EventMap SayEvents;      //for speeches that encompass > 1 line (eg- despawn, killed)
    uint32 m_uiDespawnTimer; //not using EventMap b/c needed in all phases, and can not be affected by EventMap::Reset()
    bool IsFirstTime;
    bool HasSaidBeginningStuff;
    bool IsPhase2;
    bool CombatStopped;

    std::list<uint64> CurrStars;
    std::list<uint64> CurrConstellations;
    std::list<uint64> CurrBlackHoles;

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
        switch (urand(0, 1))
        {
            case 0:
                DoScriptText(SAY_KILLED_PLAYER_1, m_creature);
                break;
            case 1:
                DoScriptText(SAY_KILLED_PLAYER_2, m_creature);
                break;
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(!HasSaidBeginningStuff && who && who->GetTypeId() == TYPEID_PLAYER)
        {
            SayEvents.Reset();
            SayEvents.ScheduleEvent(SAYEVENT_ENTER1, 0);
            SayEvents.ScheduleEvent(SAYEVENT_ENTER2, 7*IN_MILISECONDS);
            SayEvents.ScheduleEvent(SAYEVENT_ENTER3, (7+5)*IN_MILISECONDS);
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
            SayEvents.ScheduleEvent(SAYEVENT_DEFEATED2, (40)*IN_MILISECONDS);
            SayEvents.ScheduleEvent(SAYEVENT_DEFEATED3, (40+15)*IN_MILISECONDS);
            SayEvents.ScheduleEvent(SAYEVENT_DEFEATED4, (40+15+10)*IN_MILISECONDS);
            SayEvents.ScheduleEvent(SAYEVENT_DEFEATED5, (40+15+10+10)*IN_MILISECONDS);
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

    void GetRandomPointInCircle(float max_rad, float &x, float &y)
    {
        float ang = 2*M_PI * rand_norm();
        float rad = max_rad * rand_norm();
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
                SayEvents.ScheduleEvent(SAYEVENT_END2, 10*IN_MILISECONDS);
                SayEvents.ScheduleEvent(SAYEVENT_END3, 18*IN_MILISECONDS);
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

        //check for phase 2 at 20% health
        if(!IsPhase2)
        {
            float percent = float(m_creature->GetHealth()) / float(m_creature->GetMaxHealth());
            if(percent < 0.2f)
            {
                IsPhase2 = true;
                DoScriptText(SAY_START_PHASE_2, m_creature);
                events.CancelEvent(EVENT_SUMMON_STARS);
                events.CancelEvent(EVENT_SUMMON_CONSTELLATION);
                UnSummonAllCreatures();

                //4 black holes in square pattern
                Creature* summ = NULL;

                summ = DoSpawnCreature(NPC_BLACK_HOLE,10,0,0,0,TEMPSUMMON_CORPSE_DESPAWN,0);
                if(summ)
                    CurrBlackHoles.push_back(summ->GetGUID());

                summ = DoSpawnCreature(NPC_BLACK_HOLE,-10,0,0,0,TEMPSUMMON_CORPSE_DESPAWN,0);
                if(summ)
                    CurrBlackHoles.push_back(summ->GetGUID());

                summ = DoSpawnCreature(NPC_BLACK_HOLE,0,10,0,0,TEMPSUMMON_CORPSE_DESPAWN,0);
                if(summ)
                    CurrBlackHoles.push_back(summ->GetGUID());

                summ = DoSpawnCreature(NPC_BLACK_HOLE,0,-10,0,0,TEMPSUMMON_CORPSE_DESPAWN,0);
                if(summ)
                    CurrBlackHoles.push_back(summ->GetGUID());

                DoScriptText(SAY_SUMMON_STARS, m_creature);
                events.ScheduleEvent(EVENT_SUMMON_STARS,SUMMON_STARS_TIMER);
            }
        }

        //switch targets to next top aggro if phase punch stacks >= 5
        if (Unit *TopAggro = SelectUnit(SELECT_TARGET_TOPAGGRO, 0))
        {
            Aura *aur = TopAggro->GetAura(SPELL_PHASE_PUNCH, 0);
            if (aur && aur->GetStackAmount() >= 5)
                if (Unit *NextTarget = SelectUnit(SELECT_TARGET_TOPAGGRO, 1))
                    m_creature->Attack(NextTarget, true);
        }

        events.Update(uiDiff);
        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoCast(m_creature, SPELL_BERSERK);
                    //events.ScheduleEvent(EVENT_BERSERK, 10*IN_MILISECONDS);
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
                    events.ScheduleEvent(EVENT_ASCEND_TO_HEAVENS, 5*IN_MILISECONDS); //just in case they survive... hehehe
                    break;
                case EVENT_SUMMON_STARS:
                    if(!IsPhase2)
                    {
                        for(int i=0; i<4; i++)
                        {
                            float x, y;
                            GetRandomPointInCircle(30.0f, x, y);
                            if(Creature *summ = DoSpawnCreature(NPC_COLLAPSING_STAR, x, y, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 100))
                                CurrStars.push_back(summ->GetGUID());
                        }
                        DoScriptText(SAY_SUMMON_STARS, m_creature);
                        events.ScheduleEvent(EVENT_SUMMON_STARS, SUMMON_STARS_TIMER);
                    }
                    break;
                case EVENT_SUMMON_CONSTELLATION:
                    for(int i=0; i<3; i++)
                    {
                        float x, y;
                        GetRandomPointInCircle(30.0f, x, y);
                        if(Creature *summ = DoSpawnCreature(NPC_LIVING_CONSTELLATION, x, y, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                            CurrConstellations.push_back(summ->GetGUID());
                    }
                    events.ScheduleEvent(EVENT_SUMMON_CONSTELLATION, SUMMON_CONSTELLATION_TIMER);
                    break;
                case EVENT_COSMIC_SMASH:
                    /*DoScriptText(EMOTE_COSMIC_SMASH, m_creature);
                    if(m_bIsRegularMode)  //still no idea what sequence of spells this is
                    {
                        DoCast(SelectUnit(SELECT_TARGET_RANDOM,0), HEROIC(SPELL_COSMIC_SMASH, SPELL_COSMIC_SMASH_H));
                    }
                    else //3 targets in 25 man mode
                    {
                        DoCast(SelectUnit(SELECT_TARGET_RANDOM,0), HEROIC(SPELL_COSMIC_SMASH, SPELL_COSMIC_SMASH_H));
                        DoCast(SelectUnit(SELECT_TARGET_RANDOM,0), HEROIC(SPELL_COSMIC_SMASH, SPELL_COSMIC_SMASH_H));
                        DoCast(SelectUnit(SELECT_TARGET_RANDOM,0), HEROIC(SPELL_COSMIC_SMASH, SPELL_COSMIC_SMASH_H));
                    }
                    events.ScheduleEvent(EVENT_COSMIC_SMASH, COSMIC_SMASH_TIMER);*/
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    void UnSummonAllCreatures()
    {
        for (std::list<uint64>::const_iterator i = CurrStars.begin(); i != CurrStars.end(); i++)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                pSummon->ForcedDespawn();
        CurrStars.clear();
        for (std::list<uint64>::const_iterator i = CurrBlackHoles.begin(); i != CurrBlackHoles.end(); i++)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                pSummon->ForcedDespawn();
        CurrBlackHoles.clear();
        for (std::list<uint64>::const_iterator i = CurrConstellations.begin(); i != CurrConstellations.end(); i++)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                pSummon->ForcedDespawn();
        CurrConstellations.clear();
        
        std::list<Creature*> DarkMatter;
        GetCreatureListWithEntryInGrid(DarkMatter, m_creature, NPC_UNLEASHED_DARK_MATTER, 100.0f);
        for (std::list<Creature*>::const_iterator i = DarkMatter.begin(); i!= DarkMatter.end(); i++)
            (*i)->ForcedDespawn();
    }

    void UnSummonCreature(Creature *pSummon)
    {
        if (!pSummon)
            return;
        uint64 guid = pSummon->GetGUID();
        switch (pSummon->GetEntry())
        {
            case NPC_COLLAPSING_STAR:
                CurrStars.remove(guid);
                break;
            case NPC_BLACK_HOLE:
                CurrBlackHoles.remove(guid);
                break;
            case NPC_LIVING_CONSTELLATION:
                CurrConstellations.remove(guid);
                break;
            default:
                return;
        }
        pSummon->ForcedDespawn();
    }

    void CreateBlackHole(Creature* Star)
    {
        Creature *BlackHole = m_creature->SummonCreature(NPC_BLACK_HOLE, Star->GetPositionX(), Star->GetPositionY(), Star->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
        if (BlackHole)
        {
            CurrBlackHoles.push_back(BlackHole->GetGUID());
            UnSummonCreature(Star);
        }
    }

    void CancelBlackHole(Creature* Constellation, Creature* BlackHole)
    {
        UnSummonCreature(Constellation);
        UnSummonCreature(BlackHole);
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
        HealthTimer = 1*IN_MILISECONDS;
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
            HealthTimer = 1*IN_MILISECONDS;
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
        UnleashedDarkMatterTimer = 10*IN_MILISECONDS;
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
            for(std::list<uint64>::const_iterator i = AlgalonAI->CurrConstellations.begin(); i != AlgalonAI->CurrConstellations.end(); i++)
            {
                Creature *who = m_creature->GetMap()->GetCreature(*i);
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
