/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_rotface
SD%Complete: 0%
SDComment: Slime Spray not working from a target selection problem.
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_SLIME_SPRAY               = 69508,
    SPELL_OOZE_FLOOD                = 69783,
    SPELL_MUTATED_INFECTION         = 69674,

    // putricide
    SPELL_VILE_GAS                  = 72272,

    // little ooze
    SPELL_WEAK_RADIATING_OOZE       = 69750,

    // big ooze
    SPELL_RADIATING_OOZE            = 69760,
    SPELL_UNSTABLE_OOZE_EXPLOSION   = 69839,
    SPELL_UNSTABLE_OOZE             = 69558,

    // both oozes
    SPELL_STICKY_OOZE               = 69774,
    SPELL_STICKY_OOZE_DAMAGE_AURA   = 69776,
};

enum Npcs
{
    NPC_PUDDLE_STALKER      = 37013,    // Ooze flood creator/reciver
    NPC_STICKY_OOZE         = 37006,    // Sticky Ooze damage NPC
    NPC_OOZE_SPRAY_STALKER  = 37986,    // Reciver of Slime Spray
    NPC_BIG_OOZE            = 36899,
    NPC_LITTLE_OOZE         = 36897,
};

enum Says
{
    SAY_PRECIOUS_DEATH              = -1300483,
    SAY_AGGRO                       = -1300484,
    SAY_SLIME_SPRAY                 = -1300485,
    SAY_UNSTABLE_OOZE_EXPLOSION     = -1300486,
    SAY_PUTRICIDE_SLIME_FLOOD1      = -1300487,
    SAY_PUTRICIDE_SLIME_FLOOD2      = -1300488,
    SAY_KILLED_PLAYER1              = -1300489,
    SAY_KILLED_PLAYER2              = -1300490,
    SAY_BERSERK                     = -1300491,     // Unused apparently
    SAY_DEATH1                      = -1300492,
    SAY_PUTRICIDE_DEATH2            = -1300493,
};

enum Events
{
    EVENT_MUTATED_INFECTION = 1,
    EVENT_OOZE_FLOOD,
    EVENT_SLIME_SPRAY,
    EVENT_VILE_GAS,
    
    EVENT_STICKY_OOZE,
    EVENT_DESPAWN_OOZE,
};

#define OOZE_FLOOD_CAST_HEIGHT      373.0f
#define FLOOR_HEIGHT                360.4f
#define OOZE_FLOOD_RECIVE_DISTANCE  34.659f
#define OOZE_FLOOD_CASTER_DISTANCE  45.138f

static const float CenterPosition[2] = {4445.6f, 3137.21f};

#define TIMER_OOZE_FLOOD            20*IN_MILLISECONDS
#define TIMER_MUTATED_INFECTION     13*IN_MILLISECONDS  // The time is estimated to be 13 - 0.25*(Number of infections since start). This estimate is intentionally below certain observed values, just in case.
#define TIMER_SLIME_SPRAY           25*IN_MILLISECONDS
#define TIMER_VILE_GAS              30*IN_MILLISECONDS
#define TIMER_STICKY_OOZE           15*IN_MILLISECONDS
#define TIMER_DESPAWN_OOZE          6*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_rotfaceAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    uint32 NumInfections;
    std::bitset<4> OozeFloodPositionsUsed;

    boss_rotfaceAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(m_creature),
        NumInfections(0)
    {
    }

    void Reset()
    {
        SummonMgr.UnsummonAll();
        boss_icecrown_citadelAI::Reset();
        if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
            Putricide->MonsterMoveWithSpeed(4356.7f, 3265.5f, 384.4f);
    }

    void Aggro(Unit* pWho)
    {
        NumInfections = 0;
        m_BossEncounter = IN_PROGRESS;
        SCHEDULE_EVENT(OOZE_FLOOD);
        //SCHEDULE_EVENT(SLIME_SPRAY);
        if (m_bIsHeroic)
            SCHEDULE_EVENT(VILE_GAS);
        Events.ScheduleEvent(EVENT_MUTATED_INFECTION, TIMER_MUTATED_INFECTION);
        if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
            Putricide->MonsterMoveWithSpeed(4416.1f, 3190.5f, 389.4f);
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        if (pSumm)
            SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void JustSummoned(Creature *pSumm)
    {
        if (pSumm && pSumm->GetEntry() == NPC_LITTLE_OOZE)   // Not directly summoned by SummonMgr
        {
            SummonMgr.AddSummonToList(pSumm->GetObjectGuid());
            pSumm->CastSpell(pSumm, SPELL_WEAK_RADIATING_OOZE, true);
        }
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void GetPointOnCircle(float rad, float ang, float &x, float &y)
    {
        x = rad * cos(ang);
        y = rad * sin(ang);
    }

    void JustDied(Unit* pKiller)
    {
        SummonMgr.UnsummonAll();
        m_BossEncounter = DONE;
        DoScriptText(SAY_DEATH1, m_creature);
        if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
        {
            Putricide->MonsterMoveWithSpeed(4356.7f, 3265.5f, 384.4f);
            DoScriptText(SAY_PUTRICIDE_DEATH2, Putricide);
        }
    }

    void ScriptMessage(Creature* pSender, uint32 data1, uint32 data2)
    {
        if (pSender && pSender->GetEntry() == NPC_LITTLE_OOZE)
        {
            if (Creature *BigOoze = SummonMgr.SummonCreatureAt(pSender, NPC_BIG_OOZE, TEMPSUMMON_CORPSE_DESPAWN))
            {
                BigOoze->CastSpell(BigOoze, SPELL_RADIATING_OOZE, true);
                BigOoze->SetInCombatWithZone();
                BigOoze->SetSpeedRate(MOVE_RUN, 0.5f);
            }
            SummonMgr.UnsummonCreature(pSender);
        }
        else
            ScriptEventInterface::ScriptMessage(pSender, data1, data2);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_OOZE_FLOOD:
                {
                    SummonMgr.UnsummonAllWithId(NPC_PUDDLE_STALKER);
                    if (OozeFloodPositionsUsed[0] && OozeFloodPositionsUsed[1] && OozeFloodPositionsUsed[2] && OozeFloodPositionsUsed[3])
                        OozeFloodPositionsUsed.reset();

                    int Position;
                    do
                    {
                        Position = urand(0, 3);
                    }
                    while (OozeFloodPositionsUsed[Position]);
                    OozeFloodPositionsUsed[Position] = true;

                    float Angle, x, y;
                    Angle = Position * M_PI/2;
                    for (int i = 0; i < 2; i++)
                    {
                        Angle += M_PI/6;
                        GetPointOnCircle(OOZE_FLOOD_CASTER_DISTANCE, Angle, x, y);
                        Creature *Caster = SummonMgr.SummonCreature(NPC_PUDDLE_STALKER, CenterPosition[0] + x, CenterPosition[1] + y, OOZE_FLOOD_CAST_HEIGHT);
                        if (Caster)
                        {
                            Caster->SetSplineFlags(SPLINEFLAG_UNKNOWN7); //Fly
                            GetPointOnCircle(OOZE_FLOOD_RECIVE_DISTANCE, Angle, x, y);
                            Creature *Target = SummonMgr.SummonCreature(NPC_PUDDLE_STALKER, CenterPosition[0] + x, CenterPosition[1] + y, FLOOR_HEIGHT);
                            if (Target)
                                Caster->CastSpell(Target, SPELL_OOZE_FLOOD, false);
                        }
                    }
                    if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
                        DoScriptText(urand(0, 1) ? SAY_PUTRICIDE_SLIME_FLOOD1 : SAY_PUTRICIDE_SLIME_FLOOD2, Putricide);
                    break;
                }
                case EVENT_MUTATED_INFECTION:
                {
                    DoCast(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0), SPELL_MUTATED_INFECTION);
                    int NextInfectionTime = TIMER_MUTATED_INFECTION - NumInfections*0.25*IN_MILLISECONDS;
                    if (NextInfectionTime < 4000)
                        NextInfectionTime = 4000;
                    Events.RescheduleEvent(EVENT_MUTATED_INFECTION, NextInfectionTime);
                    NumInfections++;
                    break;
                }
                case EVENT_SLIME_SPRAY: // not working.
                {
                    Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0);
                    if (Target)
                    {
                        Creature *SlimeTarget = SummonMgr.SummonCreatureAt(Target, NPC_OOZE_SPRAY_STALKER);
                        SlimeTarget->setFaction(Target->getFaction());
                        DoCast(SlimeTarget, SPELL_SLIME_SPRAY);
                        SlimeTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                    DoScriptText(SAY_SLIME_SPRAY, m_creature);
                    break;
                }
                case EVENT_VILE_GAS:
                    if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
                    {
                        Unit *Target = GetPlayerAtMinimumRange(14);
                        if (!Target)
                            Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0);
                        if (Target)
                        Putricide->CastSpell(Target, SPELL_VILE_GAS, false);
                    }
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_rotface_oozeAI : public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    EventManager Events;

    mob_rotface_oozeAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(m_creature->GetInstanceData()))
    {
    }

    void Reset()
    {
    }

    void JustSummoned(Creature *pSumm)
    {
        if (pSumm && pSumm->GetEntry() == NPC_STICKY_OOZE)
        {
            pSumm->setFaction(14);  // hostile
            pSumm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pSumm->CastSpell(pSumm, SPELL_STICKY_OOZE_DAMAGE_AURA, true);
            pSumm->ForcedDespawn(30*IN_MILLISECONDS);
        }
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_LITTLE_OOZE &&
            m_creature->GetVisibility() == VISIBILITY_ON && m_creature->IsWithinDistInMap(pWho, 10.0f))
        {
            if (m_creature->GetEntry() == NPC_BIG_OOZE)
            {
                m_creature->CastSpell(m_creature, SPELL_UNSTABLE_OOZE, true);
                if (Aura *UnstableOozeAura = m_creature->GetAura(SPELL_UNSTABLE_OOZE, EFFECT_INDEX_0))
                    if (UnstableOozeAura->GetStackAmount() >= 5)
                    {
                        DoCast(m_creature->getVictim(), SPELL_UNSTABLE_OOZE_EXPLOSION);
                        Events.ScheduleEvent(EVENT_DESPAWN_OOZE, TIMER_DESPAWN_OOZE);
                        if (Creature *Rotface = GET_CREATURE(TYPE_ROTFACE))
                            DoScriptText(SAY_UNSTABLE_OOZE_EXPLOSION, Rotface);
                    }
            }
            else
                BroadcastScriptMessageToEntry(m_creature, NPC_ROTFACE, 140.0f);
            static_cast<Creature*>(pWho)->ForcedDespawn();
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit *pWho)
    {
        SCHEDULE_EVENT(STICKY_OOZE);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_STICKY_OOZE:
                    DoCast(m_creature->getVictim(), SPELL_STICKY_OOZE);
                    break;
                case EVENT_DESPAWN_OOZE:
                    DoStartNoMovement(m_creature->getVictim());
                    m_creature->SetVisibility(VISIBILITY_OFF);
                    m_creature->ForcedDespawn(20*IN_MILLISECONDS);  // Damage does not appply if original caster is nonexistant
                    break;
                default:
                    break;
            }
    }
};

void AddSC_boss_rotface()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_rotface);
    REGISTER_SCRIPT(mob_rotface_ooze);
}
