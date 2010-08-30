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
SD%Complete: 80%
SDComment: Slime Spray not working from a target selection problem.
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"
#include "TemporarySummon.h"

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
    SPELL_UNSTABLE_OOZE_EXPLOSION_MISSILE = 69832,
    SPELL_UNSTABLE_OOZE             = 69558,

    // both oozes
    SPELL_STICKY_OOZE               = 69774,
    SPELL_STICKY_OOZE_DAMAGE_AURA   = 69776,

    // precious
    SPELL_DECIMATE                  = 71123,
    SPELL_MORTAL_WOUND              = 71127,
    SPELL_SUMMON_PLAYER             = 25104,
};

enum Npcs
{
    NPC_PUDDLE_STALKER              = 37013,    // Ooze flood creator/receiver
    NPC_STICKY_OOZE                 = 37006,    // Sticky Ooze damage NPC
    NPC_OOZE_SPRAY_STALKER          = 37986,    // Reciver of Slime Spray
    NPC_BIG_OOZE                    = 36899,
    NPC_LITTLE_OOZE                 = 36897,
    NPC_OOZE_EXPLOSION_STALKER      = 38107,
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
    EVENT_UNSUMMON,
    EVENT_BLOW_UP,

    EVENT_DECIMATE,
    EVENT_MORTAL_WOUND,
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
#define TIMER_DESPAWN_OOZE          4*IN_MILLISECONDS
#define TIMER_DESPAWN_STICKY_OOZE   30*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_rotfaceAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    int32 NumInfections;
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
        NumInfections = 0;
        OozeFloodPositionsUsed.reset();
        m_BossEncounter = NOT_STARTED;
        Events.Reset();
        if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
            Putricide->MonsterMoveWithSpeed(4356.7f, 3265.5f, 389.4f);
        //boss_icecrown_citadelAI::Reset();
    }

    void Aggro(Unit* pWho)
    {
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

    void JustDied(Unit* pKiller)
    {
        SummonMgr.UnsummonAll();
        m_BossEncounter = DONE;
        DoScriptText(SAY_DEATH1, m_creature);
        if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
        {
            Putricide->GetMotionMaster()->MoveTargetedHome();
            DoScriptText(SAY_PUTRICIDE_DEATH2, Putricide);
        }
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        if (pSender && pSender->GetEntry() == NPC_LITTLE_OOZE)
        {
            if (Creature *BigOoze = SummonMgr.SummonCreatureAt(pSender, NPC_BIG_OOZE, TEMPSUMMON_DEAD_DESPAWN))
            {
                BigOoze->CastSpell(BigOoze, SPELL_RADIATING_OOZE, true);
                BigOoze->SetInCombatWithZone();
                BigOoze->SetSpeedRate(MOVE_RUN, 0.5f);
            }
            if (pSender->GetTypeId() == TYPEID_UNIT)
                SummonMgr.UnsummonCreature(static_cast<Creature*>(pSender));
            return;
        }
        ScriptEventInterface::ScriptMessage(pSender, data1, data2);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
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
                        Angle += M_PI_F/6;
                        GetPointOnCircle(x, y, OOZE_FLOOD_CASTER_DISTANCE, Angle, CenterPosition[0], CenterPosition[1]);
                        Creature *Caster = SummonMgr.SummonCreature(NPC_PUDDLE_STALKER, x, y, OOZE_FLOOD_CAST_HEIGHT);
                        if (Caster)
                        {
                            Caster->SetSplineFlags(SPLINEFLAG_UNKNOWN7); //Fly
                            GetPointOnCircle(x, y, OOZE_FLOOD_RECIVE_DISTANCE, Angle, CenterPosition[0], CenterPosition[1]);
                            Creature *Target = SummonMgr.SummonCreature(NPC_PUDDLE_STALKER, x, y, FLOOR_HEIGHT);
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
                    int NextInfectionTime = TIMER_MUTATED_INFECTION - 25*NumInfections*IN_MILLISECONDS / 100;
                    if (NextInfectionTime < 6000)
                        NextInfectionTime = 6000;
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

struct MANGOS_DLL_DECL mob_rotface_oozeAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance *m_pInstance;
    bool m_bInUse;

    mob_rotface_oozeAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_bInUse(false)
    {
    }

    void Reset()
    {
        if (m_creature->isTemporarySummon())
            static_cast<TemporarySummon*>(m_creature)->UnSummon();
        else
            m_creature->ForcedDespawn();
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (m_bInUse)
            return;
        if (pWho && pWho->GetTypeId() == TYPEID_UNIT && m_creature->GetEntry() == NPC_LITTLE_OOZE &&
            m_creature->IsWithinDistInMap(pWho, 10.0f))
            // we are the little ooze here
            // we merge into big ooze (we dont make the big ooze "suck" us)
            switch(pWho->GetEntry())
            {
                case NPC_LITTLE_OOZE: // we are the little ooze and we see another (we merge)
                    m_bInUse = true;
                    BroadcastScriptMessageToEntry(m_creature, NPC_ROTFACE, 140.0f);
                    SendEventTo(static_cast<Creature*>(pWho), EVENT_UNSUMMON, 0);
                    break;
                case NPC_BIG_OOZE: // we are the little ooze and we see a big ooze (we want to join it!)
                    m_bInUse = true;
                    pWho->CastSpell(pWho, SPELL_UNSTABLE_OOZE, true);
                    Events.ScheduleEvent(EVENT_UNSUMMON, 0); // we are the small ooze, we despawn cause we just merged with the big ooze
                    if (Aura *UnstableOozeAura = pWho->GetAura(SPELL_UNSTABLE_OOZE, EFFECT_INDEX_0))
                    {
                        switch (UnstableOozeAura->GetStackAmount())
                            {
                            case 2:
                                pWho->MonsterTextEmote("Big Ooze grows more unstable!", 0, false);
                                break;
                            case 3:
                                pWho->MonsterTextEmote("Big Ooze grows volatile!", 0, false);
                                break;
                            case 4:
                                pWho->MonsterTextEmote("Big Ooze can barely maintain its form!", 0, false);
                                break;
                            }
                        if (UnstableOozeAura->GetStackAmount() >= 5)
                        {
                            SendEventTo(static_cast<Creature*>(pWho), EVENT_BLOW_UP, 0);
                            SendEventTo(static_cast<Creature*>(pWho), EVENT_DESPAWN_OOZE, TIMER_DESPAWN_OOZE);
                            if (Creature *Rotface = GET_CREATURE(TYPE_ROTFACE))
                                DoScriptText(SAY_UNSTABLE_OOZE_EXPLOSION, Rotface);
                        }
                    }
                    break;
                default:
                    break;
            }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit *pWho)
    {
        SCHEDULE_EVENT(STICKY_OOZE);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_pInstance->IsEncounterInProgress())
            Reset();

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
                    BroadcastScriptMessageToEntry(m_creature, NPC_OOZE_EXPLOSION_STALKER, 140.0f);
                    m_creature->RemoveAllAuras();
                    Events.CancelEvent(EVENT_STICKY_OOZE);
                    DoStartNoMovement(m_creature->getVictim());
                    SetCombatMovement(false);
                    m_creature->SetDisplayId(11686);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    // Damage does not appply if original caster is nonexistant
                    Events.ScheduleEvent(EVENT_UNSUMMON, 20*IN_MILLISECONDS);
                    break;
                case EVENT_UNSUMMON:
                    Reset();
                    break;
                case EVENT_BLOW_UP:
                    Events.CancelEvent(EVENT_STICKY_OOZE);
                    m_creature->RemoveAllAuras();
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_UNSTABLE_OOZE_EXPLOSION, false);
                    break;
                default:
                    break;
            }
    }
};

struct MANGOS_DLL_DECL mob_rotface_ooze_nmAI: public Scripted_NoMovementAI
{
    EventManager Events;
    ScriptedInstance* m_pInstance;

    mob_rotface_ooze_nmAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
    {
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->setFaction(FACTION_HOSTILE);
        pCreature->CastSpell(pCreature, SPELL_STICKY_OOZE_DAMAGE_AURA, true);
        pCreature->SetInCombatWithZone();
        Events.ScheduleEvent(EVENT_UNSUMMON, 30*IN_MILLISECONDS);
    }

    void Reset()
    {
        Events.Reset();
        if (m_creature->isTemporarySummon())
            static_cast<TemporarySummon*>(m_creature)->UnSummon();
        else
            m_creature->ForcedDespawn();
    }

    void Aggro() {}

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_pInstance->IsEncounterInProgress())
            Reset();

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_UNSUMMON:
                    Reset();
                    break;
            }
    }
};

struct MANGOS_DLL_DECL mob_rotface_ooze_explosion_stalkerAI: public Scripted_NoMovementAI, public ScriptMessageInterface
{
    EventManager Events;
    ObjectGuid BigOozeGUID;
    WorldLocation pos;

    mob_rotface_ooze_explosion_stalkerAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature)
    {
        pCreature->SetDisplayId(11686);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void ScriptMessage(WorldObject* pSender, uint32 /*data1*/, uint32 /*data2*/)
    {
        if (!pSender || pSender->GetEntry() != NPC_BIG_OOZE)
            return;
        BigOozeGUID = pSender->GetObjectGuid();
        m_creature->GetPosition(pos);
        Events.ScheduleEvent(EVENT_BLOW_UP, 0);
    }

    void Reset()
    {
        Events.Reset();
    }

    void Aggro() {}

    void UpdateAI(uint32 const uiDiff)
    {        
        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BLOW_UP:
                    if (BigOozeGUID.IsEmpty())
                        return;
                    Unit *BigOoze = m_creature->GetMap()->GetCreature(BigOozeGUID);
                    if (!BigOoze)
                        return;
                    BigOoze->CastSpell(pos.coord_x, pos.coord_y, pos.coord_z, SPELL_UNSTABLE_OOZE_EXPLOSION_MISSILE, true);
                    if (m_creature->isTemporarySummon())
                        static_cast<TemporarySummon*>(m_creature)->UnSummon();
                    else
                        m_creature->ForcedDespawn();
                    break;
            }
    }
};

struct MANGOS_DLL_DECL mob_precious_ICCAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    EventManager Events;

    mob_precious_ICCAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(m_creature->GetInstanceData()))
    {
    }

    void Reset()
    {
        Events.Reset();
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEventInRange(EVENT_DECIMATE,     10*IN_MILLISECONDS, 20*IN_MILLISECONDS, 20*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        Events.ScheduleEventInRange(EVENT_MORTAL_WOUND, 10*IN_MILLISECONDS, 15*IN_MILLISECONDS, 20*IN_MILLISECONDS, 30*IN_MILLISECONDS);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_DECIMATE:
                    DoCast(m_creature->getVictim(), SPELL_DECIMATE, false);
                    break;
                case EVENT_MORTAL_WOUND:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_WOUND);
                    break;
            }

        if (m_creature->GetDistance2d(m_creature->getVictim()) > 20.0f) // if our tank is too far, we summon it to us
            DoCast(m_creature->getVictim(), SPELL_SUMMON_PLAYER, true);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        if (Creature* pRotface = GET_CREATURE(TYPE_ROTFACE))
        {
            m_creature->PlayDirectSound(16993); // if rotface is too far away, we play the sound
            DoScriptText(SAY_PRECIOUS_DEATH, pRotface);
        }
    }

};

void AddSC_boss_rotface()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_rotface);
    REGISTER_SCRIPT(mob_rotface_ooze);
    REGISTER_SCRIPT(mob_rotface_ooze_nm);
    REGISTER_SCRIPT(mob_rotface_ooze_explosion_stalker);
    REGISTER_SCRIPT(mob_precious_ICC);
}
