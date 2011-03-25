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
SDName: boss_lord_marrowgar
SD%Complete: 90%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "Vehicle.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_BERSERK               = 26662,
    SPELL_BONE_SLICE            = 69055,
    SPELL_BONE_SPIKE_GRAVEYARD  = 69057,
    SPELL_BONE_STORM            = 69076,
    SPELL_COLDFLAME_DAMAGE_AURA = 69145,
    SPELL_IMPALE_1              = 69062, // summon vehicle spell
    SPELL_IMPALE_2              = 72669, // summon vehicle spell
    SPELL_IMPALE_3              = 72670, // summon vehicle spell
    SPELL_IMPALED               = 69065, // vehicle applies this aura at its summoner

    SPDIFF_BONE_SPIKE_GRAVEYARD = 1822,
};

uint32 const ImpaleSpells[3] = {SPELL_IMPALE_1, SPELL_IMPALE_2, SPELL_IMPALE_3};

enum Summons
{
    NPC_COLDFLAME               = 36672,
};

enum Says
{
    SAY_INTRO                   = -1300400,
    SAY_AGGRO                   = -1300401,
    SAY_BONE_STORM              = -1300402,
    SAY_BONE_SPIKE1             = -1300403,
    SAY_BONE_SPIKE2             = -1300404,
    SAY_BONE_SPIKE3             = -1300405,
    SAY_KILLED_PLAYER1          = -1300406,
    SAY_KILLED_PLAYER2          = -1300407,
    SAY_ENRAGE                  = -1300408,
    SAY_DEATH                   = -1300409,
};

enum Events
{
    EVENT_COLDFLAME = 1,
    EVENT_BONE_SLICE,
    EVENT_COLDFLAME_MOVE,
    EVENT_BONE_STORM,
    EVENT_BONE_STORM_MOVE,
    EVENT_BONE_STORM_STOP,
    EVENT_BONE_SPIKE_GRAVEYARD,
    EVENT_ENRAGE,

    MESSAGE_DEAD,
    MESSAGE_SUMMONED,
};

#define TIMER_BONE_SLICE            1*IN_MILLISECONDS
#define TIMER_COLDFLAME             4*IN_MILLISECONDS, 6*IN_MILLISECONDS
#define TIMER_COLDFLAME_MOVE        20
#define TIMER_BONE_STORM            50*IN_MILLISECONDS
#define TIMER_BONE_STORM_MOVE       5*IN_MILLISECONDS
#define TIMER_BONE_SPIKE_GRAVEYARD  20*IN_MILLISECONDS
#define TIMER_ENRAGE                10*MINUTE*IN_MILLISECONDS
#define TIMER_BONED                 8*IN_MILLISECONDS

#define COLDFLAME_START_DIST        6
#define COLDFLAME_STEP_DIST         6
#define COLDFLAME_TOTAL_DIST        42

struct MANGOS_DLL_DECL mob_bone_spikeAI: public Scripted_NoMovementAI
{
    ScriptedInstance *m_pInstance;
    ObjectGuid m_Target;
    uint32 m_uiBonedTimer;

    mob_bone_spikeAI(Creature *pCreature):
        Scripted_NoMovementAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_uiBonedTimer(0)
    {
        BroadcastScriptMessageToEntry(pCreature, NPC_MARROWGAR, 100.0f, MESSAGE_SUMMONED);
    }

    void Reset()
    {
        RemoveImpale();
        m_Target.Clear();
    }

    void JustDied(Unit *pKiller)
    {
        BroadcastScriptMessageToEntry(m_creature, NPC_MARROWGAR, 100.0f, MESSAGE_DEAD);
        RemoveImpale();
    }

    void PassengerBoarded(Unit* pSummoner, int8 seat, bool apply)
    {
        if (!pSummoner)
            return;

        if (apply)
        {
            m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+1.0f);
            if (m_Target.IsEmpty())
                m_Target = pSummoner->GetObjectGuid();
            m_creature->CastSpell(pSummoner, SPELL_IMPALED, true);
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_MARROWGAR) != IN_PROGRESS) // make them despawn after encounter is over or in fail-case
        {
            RemoveImpale();
            DespawnCreature(m_creature);
        }

        m_uiBonedTimer += uiDiff;

        // if we had a passenger, but he died, we remove the "Impaled" aura (death persistant)
        if (!m_Target.IsEmpty())
            if (Unit* summoner = m_creature->GetMap()->GetUnit(m_Target))
                if (!summoner->isAlive())
                {
                    // set achievement failed
                    if (m_pInstance)
                        m_pInstance->SetData(DATA_ACHIEVEMENT_CHECK_MARROWGAR, 1);
                    RemoveImpale();
                }

        if (m_uiBonedTimer >= TIMER_BONED && m_pInstance && m_pInstance->GetData(DATA_ACHIEVEMENT_CHECK_MARROWGAR) == 0) // just set it once
            m_pInstance->SetData(DATA_ACHIEVEMENT_CHECK_MARROWGAR, 1);
    }

    void RemoveImpale()
    {
        if (!m_Target.IsEmpty())
            if (Unit* summoner = m_creature->GetMap()->GetUnit(m_Target))
                summoner->RemoveAurasDueToSpell(SPELL_IMPALED);
    }
};

struct MANGOS_DLL_DECL boss_lord_marrowgarAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    bool m_bSaidBeginningStuff :1;
    bool m_bInBoneStorm :1;

    typedef std::list<std::pair<WorldLocation /*start point, direction*/, uint32 /*dist*/> > ColdFlameList;
    ColdFlameList ColdflameAttribs;

    boss_lord_marrowgarAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(m_creature),
        m_bSaidBeginningStuff(false),
        m_bInBoneStorm(false)
    {
    }

    void Reset()
    {
        if (GameObject* Door = GET_GAMEOBJECT(DATA_MARROWGAR_DOOR_ENTRANCE))
            Door->SetGoState(GO_STATE_ACTIVE);
        m_bInBoneStorm = false;
        SummonMgr.UnsummonAll();
        ColdflameAttribs.clear();
        RemoveEncounterAuras(SPELL_IMPALED);
        boss_icecrown_citadelAI::Reset();
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        // only the bone spikes should send messages to marrowgar
        if (data1 == MESSAGE_SUMMONED)
            SummonMgr.AddSummonToList(pSender->GetObjectGuid());
        else if (data1 == MESSAGE_DEAD)
            SummonMgr.RemoveSummonFromList(pSender->GetObjectGuid());
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!m_bSaidBeginningStuff && pWho && pWho->GetTypeId() == TYPEID_PLAYER && pWho->isTargetableForAttack())
        {
            m_bSaidBeginningStuff = true;
            DoScriptText(SAY_INTRO, m_creature);
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho)
    {
        if (InstanceProgressionCheck())
            return;
        if (GameObject* Door = GET_GAMEOBJECT(DATA_MARROWGAR_DOOR_ENTRANCE))
            Door->SetGoState(GO_STATE_READY);
        if (m_pInstance)
            m_pInstance->SetData(DATA_ACHIEVEMENT_CHECK_MARROWGAR, 0);  // reset achievement check on engage
        DoScriptText(SAY_AGGRO, m_creature);
        SCHEDULE_EVENT_R(COLDFLAME);
        SCHEDULE_EVENT(BONE_SPIKE_GRAVEYARD);
        SCHEDULE_EVENT(BONE_STORM);
        Events.ScheduleEvent(EVENT_BONE_SLICE, 10*IN_MILLISECONDS, TIMER_BONE_SLICE);
        SCHEDULE_EVENT(ENRAGE);
        m_BossEncounter = IN_PROGRESS;
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        RemoveEncounterAuras(SPELL_IMPALED);
        m_BossEncounter = DONE;
    }

    void SpellHit(Unit* pDoneBy, SpellEntry const* pSpell)
    {
        if (pSpell->SpellDifficultyId != SPDIFF_BONE_SPIKE_GRAVEYARD)
            return;

        //TODO: spell actually has AoE target with effect SPELL_EFFECT_SCRIPT_EFFECT
        for (int i = m_bIs10Man ? 1 : 3; i ; --i)
        {
            Unit *Target = NULL;
            do // prevent selection of same target for Bone Spike
            {
                Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 2); // dont get tanks
                if (!Target || m_creature->getAttackers().size() <= (m_bIs10Man ? 3 : 5))
                    break;
            }
            while (Target->HasAura(SPELL_IMPALED));
            if (!Target) // prevent exploiting
                Target = m_creature->getVictim();
            if (Target)
                Target->CastSpell(Target, ImpaleSpells[i], true);
        }
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BONE_SLICE:
                    if (!m_bInBoneStorm)
                        DoCast(m_creature->getVictim(), SPELL_BONE_SLICE);
                    break;
                case EVENT_COLDFLAME:
                {
                    WorldLocation StartPos;
                    m_creature->GetPosition(StartPos);
                    if (m_bInBoneStorm)
                    {
                        for (uint32 i = 0; i < 4; i++)
                        {
                            StartPos.orientation += i*(M_PI/2);
                            ColdflameAttribs.push_back(std::make_pair(StartPos, 0));
                        }
                    }
                    else
                    {
                        Unit *target;
                        do
                            target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                        while (target && target->IsWithinDist(m_creature, 5.0f) && target != m_creature->getVictim());

                        if (target)
                        {
                            StartPos.orientation = m_creature->GetAngle(target);
                            ColdflameAttribs.push_back(std::make_pair(StartPos, 0));
                        }
                    }
                    // (no break)
                }
                case EVENT_COLDFLAME_MOVE:
                {
                    for (ColdFlameList::iterator i = ColdflameAttribs.begin(); i != ColdflameAttribs.end(); )
                    {
                        if (i->second <= COLDFLAME_TOTAL_DIST)
                        {
                            float x, y;
                            i->second += COLDFLAME_STEP_DIST;
                            GetPointOnCircle(x, y, i->second + COLDFLAME_START_DIST, i->first.orientation);
                            Creature *flame = SummonMgr.SummonCreatureAt(i->first, NPC_COLDFLAME, TEMPSUMMON_TIMED_DESPAWN, m_bIsHeroic ? 8000 : 3000, x, y);
                            if (flame)
                            {
                                flame->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                flame->CastSpell(m_creature, SPELL_COLDFLAME_DAMAGE_AURA, false);
                            }
                            Events.ScheduleEvent(EVENT_COLDFLAME_MOVE, TIMER_COLDFLAME_MOVE);
                            ++i;
                        }
                        else
                            i = ColdflameAttribs.erase(i);
                    }
                    break;
                }
                case EVENT_BONE_SPIKE_GRAVEYARD:
                {
                    if (!m_bIsHeroic && m_bInBoneStorm)
                        break;
                    switch (urand(0, 2))
                    {
                        case 0: DoScriptText(SAY_BONE_SPIKE1, m_creature); break;
                        case 1: DoScriptText(SAY_BONE_SPIKE2, m_creature); break;
                        case 2: DoScriptText(SAY_BONE_SPIKE3, m_creature); break;
                    }
                    DoCast(m_creature, SPELL_BONE_SPIKE_GRAVEYARD);
                    break;
                }
                case EVENT_BONE_STORM:
                    DoScriptText(SAY_BONE_STORM, m_creature);
                    DoCast(m_creature, SPELL_BONE_STORM);
                    m_creature->SetSpeedRate(MOVE_WALK, 3.0f, true);
                    m_creature->SetSpeedRate(MOVE_RUN, 3.0f, true);
                    DoStartNoMovement(m_creature->getVictim());
                    m_bInBoneStorm = true;
                    for (uint32 i = 0; i < (m_bIsHeroic ? 4 : 3); i++)
                        Events.ScheduleEvent(EVENT_BONE_STORM_MOVE, 4000 + i*TIMER_BONE_STORM_MOVE);
                    Events.ScheduleEvent(EVENT_BONE_STORM_STOP, TIMER_BONE_STORM_MOVE*(m_bIsHeroic ? 5 : 4));
                    break;
                case EVENT_BONE_STORM_MOVE:
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        float x, y, z;
                        pTarget->GetPosition(x, y, z);
                        m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
                    }
                    break;
                }
                case EVENT_BONE_STORM_STOP:
                    m_creature->RemoveAurasDueToSpell(SPELL_BONE_STORM);
                    m_bInBoneStorm = false;
                    m_creature->SetSpeedRate(MOVE_WALK, 1.0f, true);
                    m_creature->SetSpeedRate(MOVE_RUN, 1.0f, true);
                    DoStartMovement(m_creature->getVictim());
                    Events.DelayEventsWithId(EVENT_BONE_SLICE, 10*IN_MILLISECONDS);
                    break;
                case EVENT_ENRAGE:  //you lose!
                    DoScriptText(SAY_ENRAGE, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                default:
                    break;
            }

        if (!m_bInBoneStorm) // prevent melee damage while in bonestorm
            DoMeleeAttackIfReady();
    }
};

void AddSC_boss_lord_marrowgar()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_lord_marrowgar);
    REGISTER_SCRIPT(mob_bone_spike);
}
