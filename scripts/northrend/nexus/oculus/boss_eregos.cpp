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
SDName: Boss Ley-Guardian Eregos
SD%Complete: 0
SDComment:
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "def_oculus.h"
#include "Vehicle.h"

enum Says
{
    //SAY_SPAWN               = -1578030, // used from Belgaristrasz script
    SAY_AGGRO               = -1578031,
    SAY_ARCANE_SHIELD       = -1578032, // unknown relation
    SAY_FIRE_SHIELD         = -1578033, // unknown relation
    SAY_NATURE_SHIELD       = -1578034, // unknown relation
    SAY_FRENZY              = -1578035,
    SAY_KILL_1              = -1578036,
    SAY_KILL_2              = -1578037,
    SAY_KILL_3              = -1578038,
    SAY_DEATH               = -1578039,
};

enum Spells
{
    SPELL_ARCANE_BARRAGE_N          = 50804,
    SPELL_ARCANE_BARRAGE_H          = 59381,

    SPELL_ARCANE_VOLLEY_N           = 51153,
    SPELL_ARCANE_VOLLEY_H           = 59382,

    SPELL_ENRAGED_ASSAULT           = 51170,    // frenzy

    //SPELL_SUMMON_DRAKE              = 51175,    // to use or not to use... summons at ground position
    SPELL_PLANAR_SHIFT              = 51162,

    SPELL_PLANAR_ANOMALIES          = 57959,
    SPELL_PLANAR_ANOMALIES_SUMMON   = 57963, // need spell script on m_creature 27656
    SPELL_PLANAR_ANOMALIES_VISUAL   = 57971,
    SPELL_PLANAR_ANOMALIES_DMG      = 57976,
};

enum NPCs
{
    NPC_PLANAR_ANOMALY      = 30879,
    NPC_DRAGON              = 28276,
};

enum Events
{
    EVENT_ARCANE_BARRAGE = 1,
    EVENT_ARCANE_VOLLEY,
    EVENT_PLANAR_SHIFT,
    EVENT_ENRAGED_ASSAULT,
    EVENT_PLANAR_ANOMALITIES,
    EVENT_SUMMON_DRAKES,
    EVENT_DRAKE_COLOR_CHECK,
};

#define EMOTE_PLANAR_SHIFT  "Anomalies form as Ley-Guardian Eregos shifts into the Astral Plane!"

#define TIMER_ARCANE_BARRAGE        8*IN_MILLISECONDS,  8*IN_MILLISECONDS
#define TIMER_ARCANE_VOLLEY         15*IN_MILLISECONDS, 15*IN_MILLISECONDS
#define TIMER_PLANAR_SHIFT          30*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_ENRAGED_ASSAULT       40*IN_MILLISECONDS, 40*IN_MILLISECONDS
#define TIMER_SUMMON_DRAKES         30*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_COLOR_CHECK           2*IN_MILLISECONDS,  2*IN_MILLISECONDS

// just to prevent lag
#define MAX_DRAGON_COUNT 15

struct MANGOS_DLL_DECL boss_eregosAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    EventManager Events;
    SummonManager SummonMgr;

    bool m_bIsRegularMode;
    bool m_bIsMove;

    bool m_bFirstPlanar;
    bool m_bSecondPlanar;

    uint32 m_uiRed;
    uint32 m_uiGreen;
    uint32 m_uiYellow;

    uint64 CacheOfEregosGUID;
    uint64 SpotlightGUID;

    boss_eregosAI(Creature* pCreature) : ScriptedAI(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        Events.Reset();
        SummonMgr.UnsummonAll();

        m_bIsMove = false;
        m_bFirstPlanar = false;
        m_bSecondPlanar = false;

        m_uiRed = 0;
        m_uiGreen = 0;
        m_uiYellow = 0;

        if (m_pInstance)
        {
            m_pInstance->SetData(DATA_RUBY_VOID, 0);
            m_pInstance->SetData(DATA_EMERALD_VOID, 0);
            m_pInstance->SetData(DATA_AMBER_VOID, 0);
            if (m_creature->isAlive())
                m_pInstance->SetData(TYPE_EREGOS, NOT_STARTED);
        }

        CacheOfEregosGUID = 0;
        SpotlightGUID = 0;
    }

    void SummonedCreatureJustDied(Creature* pSumm)
    {
        if (!pSumm)
            return;
        SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void Aggro(Unit* who)
    {
        m_creature->SetInCombatWithZone();
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_EREGOS, IN_PROGRESS);

        Events.ScheduleEvent(EVENT_ARCANE_BARRAGE, TIMER_ARCANE_BARRAGE);
        Events.ScheduleEvent(EVENT_ARCANE_VOLLEY, TIMER_ARCANE_VOLLEY);
        Events.ScheduleEvent(EVENT_SUMMON_DRAKES, TIMER_SUMMON_DRAKES);

        if (!m_bIsRegularMode)
            Events.ScheduleEvent(EVENT_DRAKE_COLOR_CHECK, TIMER_COLOR_CHECK);

        Events.ScheduleEvent(EVENT_SUMMON_DRAKES, TIMER_SUMMON_DRAKES);
    }

    void CheckDrakeColors()
    {
        Map* map = m_creature->GetMap();
        if(map && map->IsDungeon())
        {
            Map::PlayerList const &PlayerList = map->GetPlayers();

            if(PlayerList.isEmpty())
                return;

            for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if(Player* pPlayer = i->getSource())
                    if (VehicleKit* pVeh = pPlayer->GetVehicle())
                        if (Unit* pDrake = pVeh->GetBase())
                        {
                            switch(pDrake->GetEntry())
                            {
                                case NPC_RUBY_DRAKE*10:
                                    m_uiRed++;
                                    break;
                                case NPC_EMERALD_DRAKE*10:
                                    m_uiGreen++;
                                    break;
                                case NPC_AMBER_DRAKE*10:
                                    m_uiYellow++;
                                    break;
                            }
                        }
            }
            if (m_pInstance)
            {
                if (m_uiRed)
                    m_pInstance->SetData(DATA_RUBY_VOID, m_uiRed);
                if (m_uiGreen)
                    m_pInstance->SetData(DATA_EMERALD_VOID, m_uiGreen);
                if (m_uiYellow)
                    m_pInstance->SetData(DATA_AMBER_VOID, m_uiYellow);
            }
        }
    }

    void KilledUnit(Unit *victim)
    {
        uint8 uiText = urand(0, 2);
        switch (uiText)
        {
           case 0:
               DoScriptText(SAY_KILL_1, m_creature);
               break;
           case 1:
               DoScriptText(SAY_KILL_2, m_creature);
               break;
           case 2:
               DoScriptText(SAY_KILL_3, m_creature);
               break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_EREGOS, DONE);
            SpotlightGUID = m_pInstance->GetData64(DATA_SPOTLIGHT);
            if (SpotlightGUID)
                m_pInstance->DoRespawnGameObject(SpotlightGUID);

            CacheOfEregosGUID = m_pInstance->GetData64(DATA_CACHE_OF_EREGOS);
            if (CacheOfEregosGUID)
                m_pInstance->DoRespawnGameObject(CacheOfEregosGUID);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bFirstPlanar && !m_bIsRegularMode && m_creature->GetHealthPercent() < 50.0f)
        {
            m_bFirstPlanar = true;
            Events.ScheduleEvent(EVENT_PLANAR_SHIFT, 0);
        }

        if (!m_bSecondPlanar && !m_bIsRegularMode &&  m_creature->GetHealthPercent() < 25.0f)
        {
            m_bSecondPlanar = true;
            Events.ScheduleEvent(EVENT_PLANAR_SHIFT, 0);
        }

        if(m_creature->GetDistance2d(m_creature->getVictim()) > 35.0f && !m_bIsMove)
        {
           m_bIsMove = true;
           SetCombatMovement(m_bIsMove);
           if(Unit* pTarget = m_creature->getVictim())
              m_creature->GetMotionMaster()->MoveChase(pTarget);
        }

        if(m_creature->GetDistance2d(m_creature->getVictim()) < 20.0f && m_bIsMove)
        {
           m_bIsMove = false;
           SetCombatMovement(m_bIsMove);
           m_creature->GetMotionMaster()->Clear(false);
           m_creature->GetMotionMaster()->MoveIdle();
           m_creature->StopMoving();
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
            case EVENT_DRAKE_COLOR_CHECK:
                CheckDrakeColors();
                break;
            case EVENT_ARCANE_BARRAGE:
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ARCANE_BARRAGE_N : SPELL_ARCANE_BARRAGE_H);
                break;
            case EVENT_ARCANE_VOLLEY:
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ARCANE_VOLLEY_N : SPELL_ARCANE_VOLLEY_H);
                break;
            case EVENT_PLANAR_SHIFT:
                m_creature->MonsterTextEmote(EMOTE_PLANAR_SHIFT, m_creature, true);
                Events.DelayEvents(18*IN_MILLISECONDS);
                m_creature->InterruptNonMeleeSpells(false);
                DoCast(m_creature,SPELL_PLANAR_SHIFT, false);
                Events.ScheduleEvent(EVENT_ENRAGED_ASSAULT, 19*IN_MILLISECONDS);
                Events.ScheduleEvent(EVENT_PLANAR_ANOMALITIES, 2*IN_MILLISECONDS);
                break;
            case EVENT_ENRAGED_ASSAULT:
                DoCast(m_creature,SPELL_ENRAGED_ASSAULT, true);
                break;
            case EVENT_PLANAR_ANOMALITIES:
                DoCast(m_creature,SPELL_PLANAR_ANOMALIES, true);
                break;
            case EVENT_SUMMON_DRAKES:
                if (SummonMgr.GetSummonCount(NPC_DRAGON) > MAX_DRAGON_COUNT)
                    break;
                for ( int i = 0 ; i < 3 ; i++ )
                {
                    //DoCast(m_creature, SPELL_SUMMON_DRAKE, true); // summoned @ ground position
                    Unit* pSumm =  SummonMgr.SummonCreature(NPC_DRAGON, m_creature->GetPositionX() - 5 + urand(0, 10), m_creature->GetPositionY() - 5 + urand(0, 10),
                        m_creature->GetPositionZ() - 5 + urand(0, 10), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    if (Unit* pPlayer = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        if (pPlayer->GetVehicle())
                            if (Unit* pDrake = pPlayer->GetVehicle()->GetBase())
                            {
                                pSumm->AddThreat(pDrake, 10000.0f);
                                pSumm->GetMotionMaster()->MoveIdle();
                                pSumm->StopMoving();
                                pSumm->GetMotionMaster()->MoveChase(pDrake, 20.0f);
                            }
                    }
                }
                break;
            }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_eregos(Creature* pCreature)
{
    return new boss_eregosAI(pCreature);
}

struct MANGOS_DLL_DECL npc_planar_anomalyAI : public ScriptedAI
{
    npc_planar_anomalyAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
       Reset();
    }

    uint32 uiPulseTimer;
    uint32 uiDeathTimer;
    bool m_bBlown;

    ObjectGuid m_pTarget;

    void Reset()
    {
        m_pTarget.Clear();
        m_creature->SetDisplayId(11686);
        m_creature->SetObjectScale(2.0f);
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        m_creature->SetSpeedRate(MOVE_RUN, 1.5, true);
        m_creature->setFaction(14);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetInCombatWithZone();
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
            if (VehicleKit* pVeh = pTarget->GetVehicle())
                if (Unit* pDrake = pVeh->GetBase())
                {
                    m_pTarget = pDrake->GetObjectGuid();
                    m_creature->GetMotionMaster()->MoveChase(pDrake);
                    m_creature->AddThreat(pDrake, 100000.0f);
                }
        DoCast(m_creature, SPELL_PLANAR_ANOMALIES_VISUAL, true);
        uiDeathTimer = 20500;
        uiPulseTimer = 19000;
        m_bBlown = false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pTarget.IsEmpty())
        {
            Unit* pTarget = NULL;
            if (Map* pMap = m_creature->GetMap())
            {
                pTarget = pMap->GetUnit(m_pTarget);
            }
            if (pTarget)
            {
                m_creature->GetMotionMaster()->MoveChase(pTarget);
                if (!m_bBlown && m_creature->IsWithinDist(pTarget, 30.0f))
                {
                    m_bBlown = true;
                    uiPulseTimer = 1*IN_MILLISECONDS;
                    uiDeathTimer = 3*IN_MILLISECONDS;
                }
            }

        }
        if (uiPulseTimer < diff)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_PLANAR_ANOMALIES_VISUAL);
            m_creature->CastSpell(m_creature, SPELL_PLANAR_ANOMALIES_DMG, true);
            uiPulseTimer = 6000;
        } else uiPulseTimer -= diff;

        if (uiDeathTimer < diff)
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else uiDeathTimer -= diff;
    }
};

CreatureAI* GetAI_npc_planar_anomaly(Creature* pCreature)
{
    return new npc_planar_anomalyAI (pCreature);
}

void AddSC_boss_eregos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_eregos";
    newscript->GetAI = &GetAI_boss_eregos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_planar_anomaly";
    newscript->GetAI = &GetAI_npc_planar_anomaly;
    newscript->RegisterSelf();
}
