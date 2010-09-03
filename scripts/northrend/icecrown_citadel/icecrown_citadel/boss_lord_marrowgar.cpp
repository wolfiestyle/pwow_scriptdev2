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
SDName: boss_lord_marrowgar
SD%Complete: 90%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_BERSERK               = 26662,
    SPELL_BONE_SLICE            = 69055,
    SPELL_BONE_SPIKE_GRAVEYARD  = 69057,
    SPELL_BONE_STORM            = 69076,
    SPELL_COLDFLAME_DAMAGE_AURA = 69145,
    SPELL_IMPALED               = 69065,
};

enum Summons
{
    NPC_COLDFLAME               = 36672,
    NPC_BONE_SPIKE              = 38711,
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
};

#define TIMER_BONE_SLICE            1*IN_MILLISECONDS
#define TIMER_COLDFLAME             4*IN_MILLISECONDS, 6*IN_MILLISECONDS
#define TIMER_COLDFLAME_MOVE        20
#define TIMER_BONE_STORM            50*IN_MILLISECONDS
#define TIMER_BONE_STORM_MOVE       5*IN_MILLISECONDS
#define TIMER_BONE_SPIKE_GRAVEYARD  20*IN_MILLISECONDS
#define TIMER_ENRAGE                10*MINUTE*IN_MILLISECONDS

#define COLDFLAME_START_DIST        6
#define COLDFLAME_STEP_DIST         6
#define COLDFLAME_TOTAL_DIST        42

struct MANGOS_DLL_DECL mob_bone_spikeAI: public Scripted_NoMovementAI, public ScriptMessageInterface
{
    uint64 m_TargetGuid;

    mob_bone_spikeAI(Creature *pCreature):
        Scripted_NoMovementAI(pCreature),
        m_TargetGuid(0)
    {
    }

    void Reset()
    {
        RemoveImpale();
    }

    void JustDied(Unit *pKiller)
    {
        RemoveImpale();
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_TargetGuid)
            return;
        Unit *pTarget = Unit::GetUnit(*m_creature, m_TargetGuid);

        if (pTarget && pTarget->isAlive())
            m_creature->SetTargetGUID(m_TargetGuid);
        else
            DespawnCreature(m_creature);
    }

    void ScriptMessage(WorldObject* target, uint32 event_id, uint32 /*data2*/)
    {
        if (!target || !target->isType(TYPEMASK_UNIT) || event_id != EVENT_BONE_SPIKE_GRAVEYARD)
            return;
        m_TargetGuid = target->GetGUID();
        DoCast(static_cast<Unit*>(target), SPELL_IMPALED, true);
    }

    void RemoveImpale()
    {
        if (!m_TargetGuid)
            return;
        if (Unit *pTarget = Unit::GetUnit(*m_creature, m_TargetGuid))
            pTarget->RemoveAurasDueToSpell(SPELL_IMPALED);
        m_TargetGuid = 0;
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
        m_bInBoneStorm = false;
        SummonMgr.UnsummonAll();
        ColdflameAttribs.clear();
        RemoveEncounterAuras(SPELL_IMPALED);
        boss_icecrown_citadelAI::Reset();
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
        m_BossEncounter = DONE;
    }

    void SpellHitTarget(Unit* victim, SpellEntry const* pSpell)
    {
        SpellEntry const *CompareSpell = GetSpellStore()->LookupEntry(SPELL_BONE_SPIKE_GRAVEYARD);
        if (CompareSpell && pSpell->SpellDifficultyId == CompareSpell->SpellDifficultyId)
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 2)) //do not attack tanks.
                if (Creature *pSumm = SummonMgr.SummonCreatureAt(pTarget, NPC_BONE_SPIKE, TEMPSUMMON_TIMED_DESPAWN, 5*MINUTE*IN_MILLISECONDS))
                    SendScriptMessageTo(pSumm, pTarget, EVENT_BONE_SPIKE_GRAVEYARD);
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
                        if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
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
